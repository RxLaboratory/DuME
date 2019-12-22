#include "abstractrenderer.h"

AbstractRenderer::AbstractRenderer(QObject *parent) : QObject(parent)
{
    //set default values
    _currentFrame = 0;
    _startTime = QTime(0,0,0,0);
    _outputSize = 0;
    _outputBitrate = 0;
    _encodingSpeed = 0;
    _timeRemaining = QTime(0,0,0,0);

    _outputFileName = "";
    _outputPath = "";

    _binaryFileName = "";

    _stopCommand = "";
    _pauseCommand = "";
    _numThreads = 1;
}

int AbstractRenderer::currentFrame() const
{
    return _currentFrame;
}

QTime AbstractRenderer::startTime() const
{
    return _startTime;
}

double AbstractRenderer::outputSize() const
{
    return _outputSize;
}

int AbstractRenderer::outputBitrate() const
{
    return _outputBitrate;
}

double AbstractRenderer::encodingSpeed() const
{
    return _encodingSpeed;
}

QTime AbstractRenderer::timeRemaining() const
{
    return _timeRemaining;
}

QString AbstractRenderer::outputFileName() const
{
    return _outputFileName;
}

void AbstractRenderer::setOutputFileName(const QString &outputFileName)
{
    _outputFileName = outputFileName;
}

void AbstractRenderer::setStopCommand(const QString &stopCommand)
{
    _stopCommand = stopCommand;
}

void AbstractRenderer::setPauseCommand(const QString &pauseCommand)
{
    _pauseCommand = pauseCommand;
}

void AbstractRenderer::start( QStringList arguments )
{
    for (int i = 0; i < _numThreads; i++ )
    {
        launchProcess( arguments );
    }
}

void AbstractRenderer::stop(int timeout)
{
    // send the stop command to everyone
    if ( _stopCommand != "")
    {
        foreach( QProcess *renderProcess, _renderProcesses )
        {
            if (renderProcess->state() == QProcess::NotRunning) continue;
            renderProcess->write( _stopCommand.toUtf8() );
        }
    }

    // wait for timeout and kill all remaining processes
    QTimer::singleShot(timeout, this, SLOT( killRenderProcesses()) );
}

int AbstractRenderer::numThreads() const
{
    return _numThreads;
}

void AbstractRenderer::setNumThreads(int numThreads)
{
    _numThreads = numThreads;
}

void AbstractRenderer::killRenderProcesses()
{
    while ( _renderProcesses.count() >= 0 )
    {
        QProcess *rp = _renderProcesses.takeFirst();
        rp->kill();
        delete rp;
    }
}

void AbstractRenderer::setCurrentFrame(int currentFrame)
{
    _currentFrame = currentFrame;
}

void AbstractRenderer::launchProcess( QStringList arguments )
{
    //create process
    QProcess *renderer = new QProcess(this);
    connect( renderer, SIGNAL(readyReadStandardError()), this, SLOT(stdError()));
    connect( renderer, SIGNAL(readyReadStandardOutput()), this, SLOT(stdOutput()));
    connect( renderer, SIGNAL(started()), this, SLOT(started()));
    connect( renderer, SIGNAL(finished(int)), this, SLOT(finished()));
    connect( renderer, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(errorOccurred(QProcess::ProcessError)));

    //launch
    renderer->setProgram( _binaryFileName );
    renderer->setArguments( arguments );
    renderer->start(QIODevice::ReadWrite);

    //TODO check processor affinity?

    _renderProcesses << renderer;
}
