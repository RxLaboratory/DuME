#include "aerenderer.h"
#include <QtDebug>

//The unique instance is nullptr until instance() has been called once.
AERenderer *AERenderer::_instance = nullptr;

AERenderer *AERenderer::instance()
{
    if (!_instance) _instance = new AERenderer();
    return _instance;
}

AERenderer::AERenderer(QObject *parent) : AbstractRenderer(parent)
{
    _duration = 0;
    connect(AfterEffects::instance(), &AfterEffects::binaryChanged, this, &AbstractRenderer::setBinary);
    setBinary(AfterEffects::instance()->binary());
}

bool AERenderer::launchJob()
{
    qDebug() << "Checking if After Effects has something to do...";
    //Check if there are AEP to render
    foreach(MediaInfo *input, _job->getInputMedias())
    {
        if (input->isAep())
        {
            //check if we need audio
            bool needAudio = false;
            foreach(MediaInfo *output, _job->getOutputMedias())
            {
                if (output->hasAudio())
                {
                    needAudio = true;
                    break;
                }
            }
            renderAep(input, needAudio);
            return true;
        }
    }
    setStatus( MediaUtils::Waiting );
    return false;
}

void AERenderer::renderAep(MediaInfo *aep, bool audio)
{
    qDebug() << "Launching After Effects Job...";
    setStatus( MediaUtils::Launching );
    QStringList arguments("-project");
    QStringList audioArguments;
    arguments <<  QDir::toNativeSeparators(aep->fileName());

    QString tempPath = "";

    //if not using the existing render queue
    if (!aep->aeUseRQueue())
    {
        //set the cache dir
        QTemporaryDir *aeTempDir = CacheManager::instance()->getAeTempDir();
        emit newLog( "After Effects temporary dir set to: " + QDir::toNativeSeparators(aeTempDir->path()) );
        aep->setCacheDir(aeTempDir);

        //if a comp name is specified, render this comp
        if (aep->aepCompName() != "") arguments << "-comp" << aep->aepCompName();
        //else use the sepecified renderqueue item
        else if (aep->aepRqindex() > 0) arguments << "-rqindex" << QString::number(aep->aepRqindex());
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

        // (Try to) Add our templates for rendering // DISABLED FOR NOW as the Ae script (should) does that
        //_ae->setDuMETemplates();
    }

    emit newLog("Beginning After Effects rendering\nUsing aerender command:\n" + arguments.join(" | "));

    //adjust the number of threads
    //keep one available for exporting the audio
    int numThreads = aep->aepNumThreads();
    if (audio && numThreads > 1) numThreads--;

    // video
    QList<VideoInfo*> vStreams = aep->videoStreams();
    this->setNumFrames( int( aep->duration() * vStreams[0]->framerate() ) );
    this->setFrameRate( vStreams[0]->framerate() );
    this->setOutputFileName( tempPath );
    this->start( arguments, numThreads );
    // audio
    if (audio) this->start( audioArguments );

    setStatus( MediaUtils::AERendering );
}

void AERenderer::readyRead(QString output)
{
    emit console(output);

    QRegularExpression reProgress("PROGRESS:\\s*[\\d:]+\\s*\\((\\d+)\\)");
    QRegularExpressionMatch match = reProgress.match(output);
    if (match.hasMatch())
    {
        int currentFrame = match.captured(1).toInt();
        setCurrentFrame( currentFrame );
    }

    //Duration
    QRegularExpression reDuration("PROGRESS:\\s*Duration:\\s*(?:(?:(\\d):(\\d\\d):(\\d\\d):(\\d\\d))|(\\d+))");
    match = reDuration.match(output);
    if (match.hasMatch())
    {
        if (match.captured(5) == "")
        {
            int h = match.captured(1).toInt();
            int m = match.captured(2).toInt();
            int s = match.captured(3).toInt();
            int i = match.captured(4).toInt();

            if (_frameRate != 0)
            {
                _duration = i/_frameRate + s + m*60 + h*60*60;
                double n = _duration * _frameRate;
                _numFrames = int(n);
            }
            else
            {
                _duration = i/24 + s + m*60 + h*60*60;
                double n = _duration * 24;
                _numFrames = int(n);
            }

        }
        else
        {
            _numFrames = match.captured(5).toInt();
            if (_frameRate != 0) _duration = _numFrames / _frameRate;
            else _duration = _numFrames / 24;
        }
    }

    // Frame Rate
    QRegularExpression reFrameRate("PROGRESS:  Frame Rate: (\\d+,\\d\\d)");
    match = reFrameRate.match(output);
    if (match.hasMatch())
    {
        _frameRate = match.captured(1).toDouble();
    }

    emit progress();
}

