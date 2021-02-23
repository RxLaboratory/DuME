#include "Renderer/renderqueue.h"

// Constructed when instance() is called for the first time.
RenderQueue *RenderQueue::_instance = nullptr;

RenderQueue *RenderQueue::instance()
{
    if (!_instance) _instance = new RenderQueue();
    return _instance;
}

RenderQueue::RenderQueue(QObject *parent ) : QObject(parent)
{
    setStatus( MediaUtils::Initializing );

    // === FFmpeg ===

    // The transcoder
    // Create the renderer
    _ffmpegRenderer = FFmpegRenderer::instance();
    _ffmpegRenderer->setBinary( FFmpeg::instance()->binary() );
    // Connections
    connect( FFmpeg::instance(), &FFmpeg::binaryChanged, _ffmpegRenderer, &FFmpegRenderer::setBinary ) ;
    connect( _ffmpegRenderer, &FFmpegRenderer::statusChanged, this, &RenderQueue::ffmpegStatusChanged ) ;
    connect( _ffmpegRenderer, &FFmpegRenderer::progress, this, &RenderQueue::ffmpegProgress ) ;
    _ffmpegRenderer->setStopCommand("q\n");

    // === After Effects ===

    // Create the renderer
    _aeRenderer = AERenderer::instance();
    // Connections
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

void RenderQueue::ffmpegStatusChanged( MediaUtils::RenderStatus status )
{
    if ( MediaUtils::isBusy( status ) )
    {
        setStatus( MediaUtils::FFmpegEncoding );
        emit newLog("FFmpeg is running.");
    }
    else if ( status == MediaUtils::Finished )
    {
        emit newLog("FFmpeg Transcoding process finished.");
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
    clearQueue();
    _encodingQueue << item;
    encode();
}

int RenderQueue::addQueueItem(QueueItem *item)
{
    clearQueue();
    _encodingQueue << item;
    return _encodingQueue.count()-1;
}

void RenderQueue::deleteQueueItem(int id)
{
    QueueItem *i = _encodingQueue.takeAt(id);
    i->deleteLater();
}

QueueItem *RenderQueue::takeQueueItem(int id)
{
    return _encodingQueue.takeAt(id);
}

void RenderQueue::deleteQueue()
{
    while(_encodingQueue.count() > 0)
    {
        deleteQueueItem(0);
    }
}

void RenderQueue::clearQueue()
{
    _encodingQueue.clear();
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

        //restore ae templates TODO run more tests for this
        if ( _status == MediaUtils::AERendering && AERenderer::instance()->isUsingTemplates() ) _ae->restoreOriginalTemplates();

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

    setStatus( MediaUtils::Launching );

    //Check if there are AEP to render
    if (_aeRenderer->render( _currentItem ) ) return;

    //Now all aep are rendered, transcode with ffmpeg
    _ffmpegRenderer->render( _currentItem );
}

void RenderQueue::finishCurrentItem( MediaUtils::RenderStatus lastStatus )
{
    if (_currentItem == nullptr) return;
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
            //Remove Temp AEP
            if (settings.value("aerender/removeAep", true).toBool())
            {
                QFileInfo aep(input->fileName());
                QDir aepFolder = aep.dir();
                if (aepFolder.dirName() == "DuME aep") aepFolder.removeRecursively();
            }

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
            //frames
            double frameRate = input->videoStreams()[0]->framerate();
            //block signals: we don't want to change any output parameter connected to the input
            QSignalBlocker b(input);
            input->update( QFileInfo(aeTempPath + "/" + files[0]));
            if (int( frameRate ) != 0) input->videoStreams()[0]->setFramerate(frameRate);
            //add audio
            QFileInfo audioFile(aeTempPath + "/DuME.wav");
            if (audioFile.exists())
            {
                MediaInfo *audio = new MediaInfo(audioFile, this);
                _currentItem->addInputMedia(audio);
            }

            //reInsert at first place in renderqueue
            _encodingQueue.insert(0,_currentItem);
            //and go
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

