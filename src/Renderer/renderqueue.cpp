#include "Renderer/renderqueue.h"

RenderQueue::RenderQueue(AfterEffects *afterEffects, QObject *parent ) : QObject(parent)
{
    setStatus( MediaUtils::Initializing );

    // === FFmpeg ===

    // The transcoder
    // Create the renderer
    _ffmpegRenderer = new FFmpegRenderer( ffmpeg->binary() );
    // Connections
    connect( ffmpeg, &FFmpeg::binaryChanged, _ffmpegRenderer, &FFmpegRenderer::setBinary ) ;
    connect( _ffmpegRenderer, &FFmpegRenderer::newLog, this, &RenderQueue::ffmpegLog ) ;
    connect( _ffmpegRenderer, &FFmpegRenderer::console, this, &RenderQueue::ffmpegConsole ) ;
    connect( _ffmpegRenderer, &FFmpegRenderer::statusChanged, this, &RenderQueue::ffmpegStatusChanged ) ;
    connect( _ffmpegRenderer, &FFmpegRenderer::progress, this, &RenderQueue::ffmpegProgress ) ;
    _ffmpegRenderer->setStopCommand("q\n");

    // === After Effects ===

    // The renderer
    _ae = afterEffects;
    // Create the renderer
    _aeRenderer = new AERenderer( _ae->binary() );
    // Connections
    connect( _ae, &AfterEffects::binaryChanged, _aeRenderer, &AERenderer::setBinary ) ;
    connect( _aeRenderer, &AERenderer::newLog, this, &RenderQueue::aeLog ) ;
    connect( _aeRenderer, &AERenderer::console, this, &RenderQueue::aeConsole ) ;
    connect( _aeRenderer, &AERenderer::statusChanged, this, &RenderQueue::aeStatusChanged ) ;
    connect( _aeRenderer, &AERenderer::progress, this, &RenderQueue::aeProgress ) ;

    // A timer to keep track of the rendering process
    timer = new QTimer( this );
    timer->setSingleShot(true);

    setStatus( MediaUtils::Waiting );
}

RenderQueue::~RenderQueue()
{
    stop(100);
    postRenderCleanUp();
}

void RenderQueue::setStatus(MediaUtils::RenderStatus st)
{
    if( st == _status) return;
    _status = st;
    emit statusChanged(_status);
}

void RenderQueue::ffmpegLog(QString message, LogUtils::LogType lt)
{
    message = "FFmpeg | " + message;
    emit newLog( message, lt );
}

void RenderQueue::ffmpegStatusChanged( MediaUtils::RenderStatus status )
{
    if ( MediaUtils::isBusy( status ) )
    {
        setStatus( MediaUtils::FFmpegEncoding );
        emit newLog("FFmpeg is running.");
    }
    else if ( status == MediaUtils::Finished )
    {
        emit newLog("FFmpeg Transcoding process successfully finished.");
        postRenderCleanUp( MediaUtils::Finished );
    }
    else if ( status == MediaUtils::Stopped )
    {
        emit newLog("FFmpeg transcoding has been stopped.");
        postRenderCleanUp( MediaUtils::Stopped );
    }
    else if ( status == MediaUtils::Error )
    {
        emit newLog("An unexpected FFmpeg error has occured.", LogUtils::Critical );
        postRenderCleanUp( MediaUtils::Error );
    }
}

void RenderQueue::ffmpegProgress()
{
    //Relay progress information
    _numFrames = _ffmpegRenderer->numFrames();
    _frameRate = _ffmpegRenderer->frameRate();
    _currentFrame = _ffmpegRenderer->currentFrame();
    _startTime = _ffmpegRenderer->startTime();
    _outputSize = _ffmpegRenderer->outputSize();
    _outputBitrate = _ffmpegRenderer->outputBitrate();
    _expectedSize = _ffmpegRenderer->expectedSize();
    _encodingSpeed = _ffmpegRenderer->encodingSpeed();
    _remainingTime = _ffmpegRenderer->timeRemaining();
    _elapsedTime = _ffmpegRenderer->elapsedTime();
    emit progress();
}

void RenderQueue::renderFFmpeg(QueueItem *item)
{
    setStatus( MediaUtils::Launching );
    //generate arguments
    QStringList arguments;
    arguments << "-loglevel" << "error" << "-stats" << "-y";

    //output checks
    MediaInfo *o = item->getOutputMedias()[0];
    FFPixFormat *outputPixFmt = nullptr;
    FFPixFormat::ColorSpace outputColorSpace = FFPixFormat::OTHER;

    if (o->hasVideo())
    {
        outputPixFmt = o->videoStreams()[0]->pixFormat();
        if (outputPixFmt->name() == "") outputPixFmt = o->videoStreams()[0]->defaultPixFormat();
        if (outputPixFmt->name() == "") outputPixFmt = o->defaultPixFormat();
        if (outputPixFmt->name() != "") outputColorSpace = outputPixFmt->colorSpace();
    }

    //input checks
    bool exrInput = false;
    MediaInfo *i = item->getInputMedias()[0];
    if (i->hasVideo()) exrInput = i->videoStreams()[0]->codec()->name() == "exr";

    //add inputs
    foreach(MediaInfo *input, item->getInputMedias())
    {
        QString inputFileName = input->fileName();
        //add custom options
        foreach(QStringList option,input->ffmpegOptions())
        {
            arguments << option[0];
            if (option.count() > 1)
            {
                if (option[1] != "") arguments << option[1];
            }
        }
        if (input->hasVideo())
        {
            VideoInfo *stream = input->videoStreams()[0];
            //add sequence options
            if (input->isSequence())
            {
                arguments << "-start_number" << QString::number(input->startNumber());
                if (stream->framerate() != 0.0) arguments << "-framerate" << QString::number(stream->framerate());
                else arguments << "-framerate" << "24";
                inputFileName = input->ffmpegSequenceName();
            }

            //add color management
            FFPixFormat::ColorSpace inputColorSpace = FFPixFormat::OTHER;
            FFPixFormat *inputPixFmt = stream->pixFormat();
            if (inputPixFmt->name() == "") inputPixFmt = stream->defaultPixFormat();
            if (inputPixFmt->name() == "") inputPixFmt = input->defaultPixFormat();
            inputColorSpace = inputPixFmt->colorSpace();

            bool convertToYUV = outputColorSpace == FFPixFormat::YUV && inputColorSpace != FFPixFormat::YUV  && input->hasVideo() ;

            if (stream->colorTRC()->name() != "" ) arguments << "-color_trc" << stream->colorTRC()->name();
            else if ( convertToYUV ) arguments << "-color_trc" << "bt709";
            if (stream->colorRange()->name() != "") arguments << "-color_range" << stream->colorRange()->name();
            else if ( convertToYUV ) arguments << "-color_range" << "tv";
            if (stream->colorPrimaries()->name() != "") arguments << "-color_primaries" << stream->colorPrimaries()->name();
            else if ( convertToYUV ) arguments << "-color_primaries" << "bt709";
            if (stream->colorSpace()->name() != "") arguments << "-colorspace" << stream->colorSpace()->name();
            else if ( convertToYUV ) arguments << "-colorspace" << "bt709";
        }

        //add input file
        arguments << "-i" << QDir::toNativeSeparators(inputFileName);
    }
    //add outputs
    foreach(MediaInfo *output, item->getOutputMedias())
    {
        //maps
        foreach (StreamReference map, output->maps())
        {
            int mediaId = map.mediaId();
            int streamId = map.streamId();
            if (mediaId >= 0 && streamId >= 0) arguments << "-map" << QString::number( mediaId ) + ":" + QString::number( streamId );
        }

        //muxer
        QString muxer = output->muxer()->name();
        if (output->muxer()->isSequence()) muxer = "image2";

        if (muxer != "") arguments << "-f" << muxer;

        //add custom options
        foreach(QStringList option,output->ffmpegOptions())
        {
            arguments << option[0];
            if (option.count() > 1)
            {
                if (option[1] != "") arguments << option[1];
            }
        }

        //video
        if (output->hasVideo())
        {
            VideoInfo *stream = output->videoStreams()[0];

            FFCodec *vc = stream->codec();
            if (vc->name() != "") arguments << "-c:v" << vc->name();
            else vc = output->defaultVideoCodec();

            if (vc->name() != "copy")
            {
                //bitrate
                int bitrate = int( stream->bitrate() );
                if (bitrate != 0) arguments << "-b:v" << QString::number(bitrate);

                //bitrate type
                if (vc->name() == "h264" && stream->bitrateType() == MediaUtils::BitrateType::CBR)
                {
                    arguments << "-minrate" << QString::number(bitrate);
                    arguments << "-maxrate" << QString::number(bitrate);
                    arguments << "-bufsize" << QString::number(bitrate*2);
                }

                //size
                int width = stream->width();
                int height = stream->height();
                //fix odd sizes (for h264)
                if (vc->name() == "h264" && width % 2 != 0)
                {
                    width--;
                    emit newLog("Adjusting width for h264 compatibility. New width: " + QString::number(width));
                }
                if (vc->name() == "h264" && height % 2 != 0)
                {
                    height--;
                    emit newLog("Adjusting height for h264 compatibility. New height: " + QString::number(height));
                }
                if (width != 0 && height != 0) arguments << "-s" << QString::number(width) + "x" + QString::number(height);

                //framerate
                if (stream->framerate() != 0.0) arguments << "-r" << QString::number(stream->framerate());

                //loop (gif)
                if (vc->name() == "gif")
                {
                    int loop = output->loop();
                    arguments << "-loop" << QString::number(loop);
                }

                //profile
                if (stream->profile()->name() != "") arguments << "-profile:v" << stream->profile()->name();

                //level
                if (stream->level() != "") arguments << "-level" << stream->level();

                //quality
                int quality = stream->quality();
                if ( quality >= 0 && vc->name() != "" )
                {
                    arguments << vc->qualityParam() << vc->qualityValue(quality);
                }

                //encoding speed
                int speed = stream->encodingSpeed();
                if (vc->name() == "h264" && speed >= 0) arguments << vc->speedParam() << vc->speedValue(speed);

                //fine tuning
                if (vc->name() == "h264" && stream->tuning()->name() != "")
                {
                    arguments << "-tune" << stream->tuning()->name();
                    if (stream->tuning()->name() == "zerolatency")
                    {
                        arguments << "-movflags" << "+faststart";
                    }
                }

                //start number (sequences)
                if (muxer == "image2")
                {
                    int startNumber = output->startNumber();
                    arguments << "-start_number" << QString::number(startNumber);
                }

                //pixel format
                QString pixFmt = stream->pixFormat()->name();
                //set default for h264 to yuv420 (ffmpeg generates 444 by default which is not standard)
                if (pixFmt == "" && vc->name() == "h264") pixFmt = "yuv420p";
                if (pixFmt == "" && muxer == "mp4") pixFmt = "yuv420p";
                if (pixFmt != "") arguments << "-pix_fmt" << pixFmt;

                //color
                //add color management
                if (stream->colorTRC()->name() != "") arguments << "-color_trc" << stream->colorTRC()->name();
                if (stream->colorRange()->name() != "") arguments << "-color_range" << stream->colorRange()->name();
                if (stream->colorTRC()->name() != "") arguments << "-color_primaries" << stream->colorPrimaries()->name();
                if (stream->colorSpace()->name() != "") arguments << "-colorspace" << stream->colorSpace()->name();

                //b-pyramids
                //set as none to h264: not really useful (only on very static footage), but has compatibility issues
                if (vc->name() == "h264") arguments << "-x264opts" << "b_pyramid=0";

                //unpremultiply
                bool unpremultiply = !stream->premultipliedAlpha();
                if (unpremultiply) arguments << "-vf" << "unpremultiply=inplace=1";

                //LUT for input EXR
                if (exrInput) arguments << "-vf" << "lutrgb=r=gammaval(0.416666667):g=gammaval(0.416666667):b=gammaval(0.416666667)";

                //Vendor
                //set to ap10 with prores for improved compatibility
                if (vc->name().indexOf("prores") >= 0) arguments << "-vendor" << "ap10";
            }
        }
        else
        {
             arguments << "-vn";
        }

        //audio
        if (output->hasAudio())
        {
            AudioInfo *stream = output->audioStreams()[0];

            QString acodec = stream->codec()->name();
            if (acodec == "")

            //codec
            if (acodec != "") arguments << "-c:a" << acodec;

            if (acodec != "copy")
            {
                //bitrate
                int bitrate = int( stream->bitrate() );
                if (bitrate != 0)
                {
                    arguments << "-b:a" << QString::number(stream->bitrate());
                }

                //sampling
                int sampling = stream->samplingRate();
                if (sampling != 0)
                {
                    arguments << "-ar" << QString::number(sampling);
                }

                //sample format
                QString sampleFormat = stream->sampleFormat()->name();
                if (sampleFormat != "")
                {
                    arguments << "-sample_fmt" << sampleFormat;
                }
            }
        }
        else
        {
            //no audio
            arguments << "-an";
        }

        //file
        QString outputPath = QDir::toNativeSeparators( output->fileName() );

        //if sequence, digits
        if (output->isSequence())
        {
            outputPath = QDir::toNativeSeparators( output->ffmpegSequenceName() );
        }

        arguments << outputPath;
    }

    emit newLog("Beginning new encoding\nUsing FFmpeg commands:\n" + arguments.join(" | "));

    //launch
    _ffmpegRenderer->setOutputFileName( item->getOutputMedias()[0]->fileName() );

    foreach (MediaInfo *m, item->getInputMedias())
    {
        if (m->hasVideo())
        {
            VideoInfo *stream = m->videoStreams()[0];
            _ffmpegRenderer->setNumFrames( int( m->duration() * stream->framerate() ) );
            _ffmpegRenderer->setFrameRate( stream->framerate() );
            break;
        }
    }

    _ffmpegRenderer->start( arguments );
}

void RenderQueue::aeLog(QString message, LogUtils::LogType lt)
{
    message = "After Effects | " + message;
    emit newLog( message, lt );
}

void RenderQueue::aeProgress()
{
    //Relay progress information
    _numFrames = _aeRenderer->numFrames();
    _frameRate = _aeRenderer->frameRate();
    _currentFrame = _aeRenderer->currentFrame();
    _startTime = _aeRenderer->startTime();
    _outputSize = _aeRenderer->outputSize();
    _outputBitrate = _aeRenderer->outputBitrate();
    _expectedSize = _aeRenderer->expectedSize();
    _encodingSpeed = _aeRenderer->encodingSpeed();
    _remainingTime = _aeRenderer->timeRemaining();
    _elapsedTime = _aeRenderer->elapsedTime();
    emit progress();
}

QTime RenderQueue::elapsedTime() const
{
    return _elapsedTime;
}

QTime RenderQueue::remainingTime() const
{
    return _remainingTime;
}

double RenderQueue::encodingSpeed() const
{
    return _encodingSpeed;
}

double RenderQueue::expectedSize() const
{
    return _expectedSize;
}

double RenderQueue::outputBitrate() const
{
    return _outputBitrate;
}

double RenderQueue::outputSize( ) const
{
    return _outputSize;
}

int RenderQueue::currentFrame() const
{
    return _currentFrame;
}

int RenderQueue::numFrames() const
{
    return _numFrames;
}

MediaUtils::RenderStatus RenderQueue::status() const
{
    return _status;
}

QueueItem *RenderQueue::currentItem()
{
    return _currentItem;
}

void RenderQueue::encode()
{
    if (_status == MediaUtils::FFmpegEncoding || _status == MediaUtils::AERendering  || _status == MediaUtils::BlenderRendering ) return;

    setStatus( MediaUtils::Launching );
    //launch first item
    encodeNextItem();
}

void RenderQueue::encode(QueueItem *item)
{
    _encodingQueue.clear();
    _encodingQueue << item;
    encode();
}

void RenderQueue::encode(QList<QueueItem*> list)
{
    _encodingQueue = list;
    encode();
}

void RenderQueue::encode(QList<MediaInfo *> inputs, QList<MediaInfo *> outputs)
{
    QueueItem *item = new QueueItem(inputs,outputs,this);
    _encodingQueue.clear();
    _encodingQueue << item;
    encode();
}

void RenderQueue::encode(MediaInfo *input, MediaInfo *output)
{
    QueueItem *item = new QueueItem(input,output,this);
    _encodingQueue.clear();
    _encodingQueue << item;
    encode();
}

int RenderQueue::addQueueItem(QueueItem *item)
{
    _encodingQueue << item;
    return _encodingQueue.count()-1;
}

void RenderQueue::removeQueueItem(int id)
{
    QueueItem *i = _encodingQueue.takeAt(id);
    i->deleteLater();
}

QueueItem *RenderQueue::takeQueueItem(int id)
{
    return _encodingQueue.takeAt(id);
}

void RenderQueue::clearQueue()
{
    while(_encodingQueue.count() > 0)
    {
        removeQueueItem(0);
    }
}

void RenderQueue::stop(int timeout)
{
    emit newLog( "Stopping queue" );

    if ( _status == MediaUtils::FFmpegEncoding )
    {
        _ffmpegRenderer->stop( timeout );
    }
    else if ( _status == MediaUtils::AERendering )
    {
        _aeRenderer->stop( timeout );
    }

    setStatus( MediaUtils::Waiting );

    emit newLog( "Queue stopped" );
}

void RenderQueue::postRenderCleanUp( MediaUtils::RenderStatus lastStatus )
{
    if (_status == MediaUtils::FFmpegEncoding || _status == MediaUtils::AERendering || _status == MediaUtils::BlenderRendering )
    {
        setStatus( MediaUtils::Cleaning );

        //restore ae templates
        if ( _status == MediaUtils::AERendering ) _ae->restoreOriginalTemplates();

        finishCurrentItem( lastStatus );

        encodeNextItem();
    }
    else
    {
        setStatus( lastStatus );
    }
}

void RenderQueue::encodeNextItem()
{
    if (_encodingQueue.count() == 0)
    {
        setStatus( MediaUtils::Waiting );
        return;
    }

    _currentItem = _encodingQueue.takeAt(0);
    //connect item status to queue status
    connect(this, SIGNAL(statusChanged(MediaUtils::RenderStatus)), _currentItem, SLOT(statusChanged(MediaUtils::RenderStatus)) );

    setStatus( MediaUtils::Launching );

    //Check if there are AEP to render
    foreach(MediaInfo *input, _currentItem->getInputMedias())
    {
        if (input->isAep())
        {
            //check if we need audio
            bool needAudio = false;
            foreach(MediaInfo *output, _currentItem->getOutputMedias())
            {
                if (output->hasAudio())
                {
                    needAudio = true;
                    break;
                }
            }

            renderAep(input, needAudio);
            return;
        }
    }

    //Now all aep are rendered, transcode with ffmpeg
    renderFFmpeg( _currentItem );
}

void RenderQueue::finishCurrentItem( MediaUtils::RenderStatus lastStatus )
{
    if (_currentItem == nullptr) return;
    //disconnect item status from queue status
    disconnect(this, nullptr, _currentItem, nullptr);
    _currentItem->setStatus( lastStatus );
    _currentItem->postRenderCleanUp();
    //move to history
    _encodingHistory << _currentItem;
    _currentItem = nullptr;
}

void RenderQueue::aeStatusChanged( MediaUtils::RenderStatus status )
{
    if ( MediaUtils::isBusy( status ) )
    {
        setStatus( MediaUtils::AERendering );
        emit newLog("After Effects is running.");
    }
    else if ( status == MediaUtils::Finished )
    {
        MediaInfo *input = _currentItem->getInputMedias()[0];

        emit newLog("After Effects Render process successfully finished");

        //encode rendered EXR
        if (!input->aeUseRQueue())
        {
            //set exr
            //get one file
            QString aeTempPath = input->cacheDir()->path();
            QDir aeTempDir(aeTempPath);
            QStringList filters("DuME_*.exr");
            QStringList files = aeTempDir.entryList(filters,QDir::Files | QDir::NoDotAndDotDot);

           //if nothing has been rendered, set to error and go on with next queue item
            if (files.count() == 0)
            {
                postRenderCleanUp( MediaUtils::Error );
                return;
            }

            //set file and launch
            double frameRate = input->videoStreams()[0]->framerate();
            input->update( QFileInfo(aeTempPath + "/" + files[0]));
            if (int( frameRate ) != 0) input->videoStreams()[0]->setFramerate(frameRate);

            //reInsert at first place in renderqueue
            _encodingQueue.insert(0,_currentItem);

            encodeNextItem();
        }
        else
        {
            emit newLog("After Effects Rendering process successfully finished.");
            postRenderCleanUp( MediaUtils::Finished );
        }
    }
    else if ( status == MediaUtils::Stopped )
    {
        emit newLog("After Effects rendering has been stopped.");
        postRenderCleanUp( MediaUtils::Stopped );
    }
    else if ( status == MediaUtils::Error )
    {
        emit newLog("An unexpected After Effects error has occured.", LogUtils::Critical);
        postRenderCleanUp( MediaUtils::Error );
    }
}

void RenderQueue::renderAep(MediaInfo *input, bool audio)
{
    QStringList arguments("-project");
    QStringList audioArguments;
    arguments <<  QDir::toNativeSeparators(input->fileName());

    QString tempPath = "";

    //if not using the existing render queue
    if (!input->aeUseRQueue())
    {
        //set the cache dir
        QTemporaryDir *aeTempDir = cacheManager->getAeTempDir();
        emit newLog( "After Effects temporary dir set to: " + QDir::toNativeSeparators(aeTempDir->path()) );
        input->setCacheDir(aeTempDir);

        //if a comp name is specified, render this comp
        if (input->aepCompName() != "") arguments << "-comp" << input->aepCompName();
        //else use the sepecified renderqueue item
        else if (input->aepRqindex() > 0) arguments << "-rqindex" << QString::number(input->aepRqindex());
        //or the first one if not specified
        else arguments << "-rqindex" << "1";

        //and finally, append arguments
        audioArguments = arguments;

        arguments << "-RStemplate" << "DuMultiMachine";
        arguments << "-OMtemplate" << "DuEXR";
        tempPath = QDir::toNativeSeparators(aeTempDir->path() + "/" + "DuME_[#####]");
        arguments << "-output" << tempPath;

        if (audio)
        {
            audioArguments << "-RStemplate" << "DuBest";
            audioArguments << "-OMtemplate" << "DuWAV";
            QString audioTempPath = QDir::toNativeSeparators(aeTempDir->path() + "/" + "DuME");
            audioArguments << "-output" << audioTempPath;
        }
        else
        {
            audioArguments.clear();
        }

        // Add our templates for rendering
        _ae->setDuMETemplates();
    }

    emit newLog("Beginning After Effects rendering\nUsing aerender commands:\n" + arguments.join(" | "));

    //adjust the number of threads
    //keep one available for exporting the audio
    int numThreads = input->aepNumThreads();
    if (audio && numThreads > 1) numThreads--;

    // video
    _aeRenderer->setNumFrames( int( input->duration() * input->videoStreams()[0]->framerate() ) );
    _aeRenderer->setFrameRate( input->videoStreams()[0]->framerate() );
    _aeRenderer->setOutputFileName( tempPath );
    _aeRenderer->start( arguments, numThreads );
    // audio
    if (audio) _aeRenderer->start( audioArguments );

    setStatus( MediaUtils::AERendering );
}
