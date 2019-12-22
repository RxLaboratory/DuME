#include "rendererqueue.h"

RendererQueue::RendererQueue( FFmpeg *ffmpeg, AERenderer *aeRenderer, QObject *parent ) : QObject(parent)
{
    setStatus( Initializing );

    // The FFmpeg transcoder
    _ffmpeg = ffmpeg;
    connect( _ffmpeg, SIGNAL( newLog(QString) ), this, SLOT( ffmpegLog(QString) ) );
    connect( _ffmpeg, SIGNAL( newError(QString) ), this, SLOT( ffmpegError(QString)) );

    // The After Effects renderer
    _aeRenderer = aeRenderer;
    connect( _aeRenderer, SIGNAL(finished()), this, SLOT( finishedAe() ) );

    // A timer to keep track of the rendering process
    timer = new QTimer(this);
    timer->setSingleShot(true);

    setStatus( Waiting );
}

RendererQueue::~RendererQueue()
{
    postRenderCleanUp();
}

void RendererQueue::postRenderCleanUp()
{
    setStatus(Cleaning);

    _currentItem->postRenderCleanUp();

    encodeNextItem();
}

void RendererQueue::setStatus(RendererQueue::Status st)
{
    _status = st;
    emit statusChanged(_status);
}

void RendererQueue::log(QString message)
{
    qInfo() << message;
    emit newLog( message );
}

void RendererQueue::error(QString message)
{
    qWarning() << message;
    emit newError( message );
}


void RendererQueue::ffmpegLog(QString message)
{
    message = "FFmpeg | " + message;
    log( message );
}

void RendererQueue::ffmpegError(QString message)
{
    message = "FFmpeg | " + message;
    error(message);

    setStatus( Error );
    _currentItem->setStatus(FFQueueItem::Error);
    _encodingHistory << _currentItem;
}

RendererQueue::Status RendererQueue::status() const
{
    return _status;
}

FFQueueItem *RendererQueue::getCurrentItem()
{
    return _currentItem;
}

void RendererQueue::encode()
{
    if (_status == FFmpegEncoding) return;
    setStatus(FFmpegEncoding);

    //launch first item
    encodeNextItem();
}

void RendererQueue::encode(FFQueueItem *item)
{
    _encodingQueue << item;
    encode();
}

void RendererQueue::encode(QList<FFQueueItem*> list)
{
    _encodingQueue.append(list);
    encode();
}

void RendererQueue::encode(MediaInfo *input, QList<MediaInfo *> outputs)
{
    FFQueueItem *item = new FFQueueItem(input,outputs,this);
    _encodingQueue << item;
    encode();
}

void RendererQueue::encode(MediaInfo *input, MediaInfo *output)
{
    FFQueueItem *item = new FFQueueItem(input,output,this);
    _encodingQueue << item;
    encode();
}

int RendererQueue::addQueueItem(FFQueueItem *item)
{
    _encodingQueue.append(item);
    return _encodingQueue.count()-1;
}

void RendererQueue::removeQueueItem(int id)
{
    FFQueueItem *i = _encodingQueue.takeAt(id);
    delete i;
}

FFQueueItem *RendererQueue::takeQueueItem(int id)
{
    return _encodingQueue.takeAt(id);
}

void RendererQueue::clearQueue()
{
    while(_encodingQueue.count() > 0)
    {
        removeQueueItem(0);
    }
}

void RendererQueue::stop(int timeout)
{
    log( "Stopping queue" );

    if ( _status == FFmpegEncoding )
    {
        _ffmpeg->stop( timeout );
    }

    setStatus(Waiting);

    log( "Queue stopped" );
}

void RendererQueue::finished()
{
    if (_status == FFmpegEncoding || _status == AERendering || _status == BlenderRendering )
    {
        _currentItem->setStatus(FFQueueItem::Finished);
        emit encodingFinished(_currentItem);
        //move to history
        _encodingHistory << _currentItem;

        setStatus(Cleaning);

        disconnect(timer, SIGNAL(timeout()), nullptr, nullptr);
        connect(timer,SIGNAL(timeout()), this, SLOT(postRenderCleanUp()));
        timer->start(3000);

        encodeNextItem();
    }
    else
    {
        setStatus(Waiting);
    }
}

void RendererQueue::encodeNextItem()
{
    if (_encodingQueue.count() == 0)
    {
        setStatus(Waiting);
        return;
    }

    _currentItem = _encodingQueue.takeAt(0);

    //Check if there are AEP to render
    foreach(MediaInfo *input,_currentItem->getInputMedias())
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

    //generate arguments
    QStringList arguments("-stats");
    arguments << "-y";

    //add inputs
    foreach(MediaInfo *input,_currentItem->getInputMedias())
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
        //add sequence options
        if (input->isImageSequence())
        {
            arguments << "-framerate" << QString::number(input->videoFramerate());
            arguments << "-start_number" << QString::number(input->startNumber());
            inputFileName = input->ffmpegSequenceName();
        }
        //add trc
        if (input->trc() != "")
        {
            arguments << "-apply_trc" << input->trc();
        }
        //add input file
        arguments << "-i" << QDir::toNativeSeparators(inputFileName);
    }
    //add outputs
    foreach(MediaInfo *output,_currentItem->getOutputMedias())
    {
        //muxer
        QString muxer = "";
        if (output->muxer() != nullptr)
        {
            muxer = output->muxer()->name();
            if (output->muxer()->isSequence()) muxer = "image2";
        }
        if (muxer != "")
        {
            arguments << "-f" << muxer;
        }

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
        QString codec = "";
        if (output->videoCodec() != nullptr) codec = output->videoCodec()->name();


        if (output->hasVideo())
        {
            //codec
            if (codec != "") arguments << "-vcodec" << codec;

            if (codec != "copy")
            {
                //bitrate
                int bitrate = output->videoBitrate();
                if (bitrate != 0)
                {
                    arguments << "-b:v" << QString::number(bitrate);

                }

                //size
                int width = output->videoWidth();
                int height = output->videoHeight();
                //fix odd sizes (for h264)
                if (codec == "h264" && width % 2 != 0)
                {
                    width--;
                    log("Adjusting width for h264 compatibility. New width: " + QString::number(width));
                }
                if (codec == "h264" && height % 2 != 0)
                {
                    height--;
                    log("Adjusting height for h264 compatibility. New height: " + QString::number(height));
                }
                if (width != 0 && height != 0)
                {
                    arguments << "-s" << QString::number(width) + "x" + QString::number(height);
                }

                //framerate
                double framerate = output->videoFramerate();
                if (framerate != 0.0)
                {
                    arguments << "-r" << QString::number(framerate);
                }

                //loop (gif)
                if (codec == "gif")
                {
                    int loop = output->loop();
                    arguments << "-loop" << QString::number(loop);
                }

                //profile
                int profile = output->videoProfile();
                if (profile > -1)
                {
                    arguments << "-profile" << QString::number(profile);
                }

                //quality (h264)
                int quality = output->videoQuality();
                if (codec == "h264" && quality > 0 )
                {
                    quality = 100-quality;
                    //adjust to CRF values
                    if (quality < 10)
                    {
                        //convert to range 0-15 // visually lossless
                        quality = quality*15/10;
                    }
                    else if (quality < 25)
                    {
                        //convert to range 15-21 // very good
                        quality = quality-10;
                        quality = quality*6/15;
                        quality = quality+15;
                    }
                    else if (quality < 50)
                    {
                        //convert to range 22-28 // good
                        quality = quality-25;
                        quality = quality*6/25;
                        quality = quality+21;
                    }
                    else if (quality < 75)
                    {
                        //convert to range 29-34 // bad
                        quality = quality-50;
                        quality = quality*6/25;
                        quality = quality+28;
                    }
                    else
                    {
                        //convert to range 35-51 // very bad
                        quality = quality-75;
                        quality = quality*17/25;
                        quality = quality+34;
                    }
                    arguments << "-crf" << QString::number(quality);
                }

                //start number (sequences)
                if (muxer == "image2")
                {
                    int startNumber = output->startNumber();
                    arguments << "-start_number" << QString::number(startNumber);
                }

                //pixel format
                QString pixFmt = "";
                if (output->pixFormat() != nullptr) pixFmt = output->pixFormat()->name();
                //set default for h264 to yuv420 (ffmpeg generates 444 by default which is not standard)
                if (pixFmt == "" && codec == "h264") pixFmt = "yuv420p";
                if (pixFmt != "") arguments << "-pix_fmt" << pixFmt;

                //b-pyramids
                //set as none to h264: not really useful (only on very static footage), but has compatibility issues
                if (codec == "h264") arguments << "-x264opts" << "b_pyramid=0";

                //unpremultiply
                bool unpremultiply = !output->premultipliedAlpha();
                if (unpremultiply) arguments << "-vf" << "unpremultiply=inplace=1";
            }
        }
        else
        {
            //no video
            arguments << "-vn";
        }

        //audio
        QString acodec = "";
        if (output->audioCodec() != nullptr) acodec = output->audioCodec()->name();

        if (output->hasAudio())
        {
            //codec
            if (acodec != "") arguments << "-acodec" << acodec;

            if (acodec != "copy")
            {
                //bitrate
                int bitrate = output->audioBitrate();
                if (bitrate != 0)
                {
                    arguments << "-b:a" << QString::number(output->audioBitrate());
                }

                //sampling
                int sampling = output->audioSamplingRate();
                if (sampling != 0)
                {
                    arguments << "-ar" << QString::number(sampling);
                }
            }
        }
        else
        {
            //no audio
            arguments << "-an";
        }

        //file
        QString outputPath = QDir::toNativeSeparators(output->fileName());

        //if sequence, digits
        if (output->muxer() != nullptr)
        {
            if (output->isImageSequence())
            {
                outputPath = output->ffmpegSequenceName();
            }
        }

        arguments << outputPath;
    }

    log("Beginning new encoding\nUsing FFmpeg commands:\n" + arguments.join(" | "));

    //launch
    _ffmpeg->start( arguments );
    _currentItem->setStatus(FFQueueItem::InProgress);
    emit  encodingStarted(_currentItem);
}

void RendererQueue::finishedAe()
{
    MediaInfo *input = _currentItem->getInputMedias()[0];

    //encode rendered EXR
    if (!input->aeUseRQueue())
    {
        log("After Effects Render process successfully finished");

        //set exr
        //get one file
        QString aeTempPath = input->cacheDir()->path();
        QDir aeTempDir(aeTempPath);
        QStringList filters("DuME_*.exr");
        QStringList files = aeTempDir.entryList(filters,QDir::Files | QDir::NoDotAndDotDot);

        //if nothing has been rendered, set to error and go on with next queue item
        if (files.count() == 0)
        {
            _currentItem->setStatus(FFQueueItem::AEError);
            emit encodingFinished(_currentItem);
            //move to history
            _encodingHistory << _currentItem;
            encodeNextItem();
            return;
        }

        //set file and launch
        QString prevTrc = input->trc();
        double frameRate = input->videoFramerate();
        input->updateInfo( _ffmpeg->analyseMedia(aeTempPath + "/" + files[0]));
        if (prevTrc == "") input->setTrc("gamma22");
        else input->setTrc(prevTrc);
        if (frameRate != 0) input->setVideoFramerate(frameRate);

        //reInsert at first place in renderqueue
        _encodingQueue.insert(0,_currentItem);

        encodeNextItem();
    }
    else
    {
        finished();
    }
}

void RendererQueue::renderAep(MediaInfo *input, bool audio)
{
    QStringList arguments("-project");
    QStringList audioArguments;
    arguments <<  QDir::toNativeSeparators(input->fileName());

    //if not using the existing render queue
    if (!input->aeUseRQueue())
    {
        //get the cache dir
        QTemporaryDir *aeTempDir = new QTemporaryDir( settings.value("aerender/cache","" ).toString());
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
        QString tempPath = QDir::toNativeSeparators(aeTempDir->path() + "/" + "DuME_[#####]");
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
    }

    log("Beginning After Effects rendering\nUsing aerender commands:\n" + arguments.join(" | "));

    //adjust the number of threads
    //keep one available for exporting the audio
    int numThreads = input->aepNumThreads();

    // video processes
    _aeRenderer->start( arguments, audioArguments, numThreads, input->aeUseRQueue() );


    _currentItem->setStatus(FFQueueItem::InProgress);

    setStatus(AERendering);
    emit encodingStarted(_currentItem);
}

MediaInfo *RendererQueue::loadPreset(QString json)
{
    MediaInfo *mediaInfo = nullptr;

    log("Loading preset");

    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

    //validate file
    if (!jsonDoc.isObject())
    {
        error("Invalid preset file");
        return mediaInfo;
    }
    QJsonObject mainObj = jsonDoc.object();
    if (mainObj.value("dume") == QJsonValue::Undefined)
    {
        error("Invalid preset file - Cannot find DuMe object");
        return mediaInfo;
    }

    //load and create mediaInfo
    QJsonObject mediaObj = mainObj.value("dume").toObject();
    QString version = mediaObj.value("version").toString();
    //TODO Check version
    log("Preset version: " + version);


    log("Getting media info...");
    mediaInfo = new MediaInfo();

    //muxer
    QJsonObject muxerObj = mediaObj.value("muxer").toObject();
    QString muxerName = muxerObj.value("name").toString();
    log("Muxer: " + muxerName);
    mediaInfo->setMuxer(_ffmpeg->getMuxer( muxerName ));
    mediaInfo->setLoop(mediaObj.value("loop").toInt());

    //video
    if (mediaObj.value("hasVideo").toBool())
    {
        mediaInfo->setVideo(true);
        QJsonObject videoObj = mediaObj.value("video").toObject();
        QString codecName = videoObj.value("codecName").toString();
        log("Video codec: " + codecName);
        if (codecName != "default")
        {
            mediaInfo->setVideoCodec(_ffmpeg->getVideoEncoder( codecName ));
        }
        mediaInfo->setVideoWidth(videoObj.value("width").toInt());
        mediaInfo->setVideoHeight(videoObj.value("height").toInt());
        mediaInfo->setVideoFramerate(videoObj.value("framerate").toDouble());
        mediaInfo->setVideoBitrate(videoObj.value("bitrate").toInt());
        mediaInfo->setVideoProfile(videoObj.value("profile").toInt());
        mediaInfo->setVideoQuality(videoObj.value("quality").toInt());
        mediaInfo->setStartNumber(videoObj.value("startNumber").toInt());
        if (!videoObj.value("premultipliedAlpha").isUndefined()) mediaInfo->setPremultipliedAlpha(videoObj.value("premultipliedAlpha").toBool());
        else mediaInfo->setPremultipliedAlpha(true);
        mediaInfo->setPixFormat(_ffmpeg->getPixFormat( videoObj.value("pixelFormat").toString()) );
    }

    //audio
    if (mediaObj.value("hasAudio").toBool())
    {
        mediaInfo->setAudio(true);
        QJsonObject audioObj = mediaObj.value("audio").toObject();
        QString codecName = audioObj.value("codecName").toString();
        log("Audio codec: " + codecName);
        if (codecName != "default")
        {
            mediaInfo->setAudioCodec( _ffmpeg->getAudioEncoder( codecName ) );
        }
        mediaInfo->setAudioSamplingRate(audioObj.value("sampling").toInt());
        mediaInfo->setAudioBitrate(audioObj.value("bitrate").toInt());
    }

    //options
    QJsonArray options = mediaObj.value("options").toArray();
    log("Custom options");
    foreach(QJsonValue option,options)
    {
        QJsonObject optionObj = option.toObject();
        QStringList opt(optionObj.value("name").toString());
        opt << optionObj.value("value").toString();
        mediaInfo->addFFmpegOption(opt);
    }

    log("DuME preset loaded");

    return mediaInfo;
}

MediaInfo *RendererQueue::loadPresetFromFile(QString jsonFileName)
{
    MediaInfo *mediaInfo = nullptr;
    QFile jsonFile(jsonFileName);
    log("Opening preset file: " + jsonFileName);
    if (jsonFile.open(QIODevice::ReadOnly))
    {
        log("File opened");
        mediaInfo = loadPreset(jsonFile.readAll());
        jsonFile.close();
    }
    return mediaInfo;
}
