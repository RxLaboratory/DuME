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
    _setTemplates = true;
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
    emit newLog( "Launching After Effects Job...", LogUtils::Debug);
    setStatus( MediaUtils::Launching );
    QStringList arguments("-project");
    QStringList audioArguments;
    arguments <<  QDir::toNativeSeparators(aep->fileName());

    QString tempPath = "";

    qDebug() << "Here's the project: " + QDir::toNativeSeparators(aep->fileName());

    //We don't want to trigger changes in the queue now that we're rendering
    QSignalBlocker b(aep);

    //if not using the existing render queue
    if (!aep->aeUseRQueue())
    {
        qDebug() << "We're not using Ae render queue, let's build the command.";
        //set the cache dir
        QTemporaryDir *aeTempDir = CacheManager::instance()->getAeTempDir();
        qDebug() << "After Effects temporary dir set to: " + QDir::toNativeSeparators(aeTempDir->path());
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

        // (Try to) Add our templates for rendering
        if (_setTemplates) AfterEffects::instance()->setDuMETemplates();
    }

    qDebug() << "Beginning After Effects rendering\nUsing aerender command:\n" + arguments.join(" | ");

    //adjust the number of threads
    //keep one available for exporting the audio
    int numThreads = aep->aepNumThreads();
    if (audio && numThreads > 1) numThreads--;

    qDebug() << "Using " + QString::number(numThreads) + " threads.";

    // video
    QList<VideoInfo*> vStreams = aep->videoStreams();
    this->setNumFrames( int( aep->duration() * vStreams[0]->framerate() ) );
    this->setFrameRate( vStreams[0]->framerate() );
    this->setOutputFileName( tempPath );

    qDebug() << "Starting...";

    this->start( arguments, numThreads );
    // audio
    if (audio) this->start( audioArguments );

    setStatus( MediaUtils::AERendering );

    qDebug() << "Launched!";
}

bool AERenderer::isUsingTemplates() const
{
    return _setTemplates;
}

void AERenderer::setUseTemplates(bool setTemplates)
{
    _setTemplates = setTemplates;
}

void AERenderer::readyRead(QString output)
{
    emit console(output);
    
    QRegularExpression reProgress("PROGRESS:\\s*[\\d:]+\\s*\\((\\d+)\\)");
    QRegularExpressionMatch match = reProgress.match(output);
    if (match.hasMatch())
    {
        //int currentFrame = match.captured(1).toInt();
        //Lets get progress from the number of files
        QFileInfo outputFile(this->outputFileName());
        QDir outputDir = outputFile.dir();
        QFileInfoList files = outputDir.entryInfoList(QStringList("*.exr"),QDir::Files);
        setCurrentFrame( files.count() );
        //render has started, let's restore original templates
        AfterEffects::instance()->restoreOriginalTemplates();
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

