#include "abstractrenderer.h"

AbstractRenderer::AbstractRenderer(QObject *parent) : QObject(parent)
{
    //set default values
    _currentFrame = 0;
    _numFrames = 0;
    _frameRate = 24;
    _startTime = QTime(0,0,0,0);
    _outputSize = 0;
    _outputBitrate = 0;
    _expectedSize = 0;
    _encodingSpeed = 0;
    _timeRemaining = QTime(0,0,0,0);
    _elapsedTime = QTime(0,0,0,0);

    _outputFileName = "";

    _binaryFileName = "";

    _stopCommand = "";
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

double AbstractRenderer::outputSize(MediaUtils::SizeUnit unit) const
{
    if (unit == MediaUtils::Bytes) return _outputSize;
    else if (unit == MediaUtils::KB) return _outputSize/1024;
    else if (unit == MediaUtils::MB) return _outputSize/1024/1024;
    else if (unit == MediaUtils::GB) return _outputSize/1024/1024/1024;

    return _outputSize;
}

double AbstractRenderer::outputBitrate(MediaUtils::BitrateUnit unit) const
{
    if (unit == MediaUtils::Bits) return _outputBitrate;
    else if (unit == MediaUtils::Kbps) return _outputBitrate/1000;
    else if (unit == MediaUtils::Mbps) return _outputBitrate/1000/1000;

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

void AbstractRenderer::start( QStringList arguments )
{
    emit newLog("Launching " + QString::number( _numThreads ) + " processes.");
    for (int i = 0; i < _numThreads; i++ )
    {
        launchProcess( arguments );
    }
    _startTime.start();
}

void AbstractRenderer::stop(int timeout)
{
    newLog("Sending the stop command");

    // send the stop command to everyone
    if ( _stopCommand != "")
    {
        foreach( QProcess *renderProcess, _renderProcesses )
        {
            if (renderProcess->state() == QProcess::NotRunning) continue;
            renderProcess->write( _stopCommand.toUtf8() );
        }
    }

    emit newLog("Stop command sent. Waiting for processes to shut down.");

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

void AbstractRenderer::processStdError()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process);
    QString log = "Process " + QString::number(id) + ": " + process->readAllStandardError();
    readyRead( log );
    emit newOutput( log );
}

void AbstractRenderer::processStdOutput()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process);
    QString log = "Process " + QString::number(id) + ": " + process->readAllStandardOutput();
    readyRead( log );
    emit newOutput( log );
}

void AbstractRenderer::processStarted()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process);

    emit newLog("Process " + QString::number( id ) + " started.");

    emit started();
}

void AbstractRenderer::processFinished()
{
    //check which processes have finished
    QList<int> finishedProcesses;
    QString debugFinishedProcesses = "Processes: ";
    for(int i = 0; i < _renderProcesses.count(); i++)
    {
        if( _renderProcesses[i]->state() == QProcess::NotRunning )
        {
            finishedProcesses << i;
            if (i > 0) debugFinishedProcesses += ", ";
            debugFinishedProcesses += QString::number(i);
        }
    }

    emit newLog( debugFinishedProcesses + " have finished." );

    //if all processes have finished
    if ( finishedProcesses.count() == _renderProcesses.count() )
    {
        //remove all processes
        while( _renderProcesses.count() > 0 )
        {
            QProcess *process = _renderProcesses.takeLast();
            process->deleteLater();
        }

        emit finished();
    }
}

void AbstractRenderer::processErrorOccurred(QProcess::ProcessError e)
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process);

    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start process " + QString::number( id ) + ".";
    }
    else if (e == QProcess::Crashed)
    {
        error = "Process (" + QString::number( id ) + ") just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "Process (" + QString::number( id ) + ") operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "Process (" + QString::number( id ) + ") write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read process (" + QString::number( id ) + ") output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown process (" + QString::number( id ) + ") error occured.";
    }

    emit newError( error );
}

void AbstractRenderer::killRenderProcesses()
{   
    while ( _renderProcesses.count() >= 0 )
    {
        QProcess *rp = _renderProcesses.takeLast();
        rp->kill();
        rp->deleteLater();
        emit newLog( "Killed process " + QString::number( _renderProcesses.count() + 1 ) );
    }
    emit newLog("Some processes did not stop correctly and had to be killed. The output file which may be corrupted.");
}

void AbstractRenderer::setEncodingSpeed(double encodingSpeed)
{
    _encodingSpeed = encodingSpeed;
}

void AbstractRenderer::setOutputBitrate(double outputBitrate)
{
    _outputBitrate = outputBitrate;
}

void AbstractRenderer::setOutputSize(double outputSize)
{
    _outputSize = outputSize;
}

void AbstractRenderer::setBinaryFileName(const QString &binaryFileName)
{
    _binaryFileName = binaryFileName;
}

void AbstractRenderer::setFrameRate(double frameRate)
{
    _frameRate = frameRate;
}

QTime AbstractRenderer::elapsedTime() const
{
    return _elapsedTime;
}

void AbstractRenderer::setNumFrames(int numFrames)
{
    _numFrames = numFrames;
}

void AbstractRenderer::setCurrentFrame(int currentFrame)
{
    _currentFrame = currentFrame;

    if (_currentFrame == 0)
    {
        _outputSize = 0;
        _outputBitrate = 0;
        _encodingSpeed = 0;
        _timeRemaining = QTime( 0, 0 );
        _elapsedTime = QTime( 0, 0 );
        emit progress();
        return;
    }

    //compute times

    QTime currentTime = QTime::currentTime();
    int elapsedSeconds = _startTime.secsTo( currentTime );
    int remainingSeconds = elapsedSeconds * _numFrames / _currentFrame;

    _elapsedTime = QTime( 0, 0 ).addSecs( elapsedSeconds );
    _timeRemaining = QTime( 0, 0 ).addSecs( remainingSeconds );

    //compute size

    //get file(s)
    //check if it's a sequence
    QRegularExpression regExDigits("{(#+)}");
    QRegularExpressionMatch regExDigitsMatch = regExDigits.match( _outputFileName );
    QFileInfo infoOutput( _outputFileName );
    // get all frames
    if ( regExDigitsMatch.hasMatch() )
    {
        QDir containingDir = infoOutput.dir();
        QString baseName = infoOutput.fileName();
        QFileInfoList files = containingDir.entryInfoList( QStringList( baseName.replace( regExDigits, "*") ), QDir::Files);
        _outputSize = 0;
        foreach( QFileInfo file, files)
        {
            _outputSize += file.size();
        }
    }
    //get video file
    else
    {
        _outputSize = infoOutput.size();
    }

    //compute bitrate

    double outputBits = _outputSize / 8;
    double renderedSeconds = _currentFrame / _frameRate;
    _outputBitrate = outputBits / renderedSeconds;

    //expected size
    _expectedSize = _outputSize * _numFrames / _currentFrame;

    //encoding speed
    double expectedDuration = elapsedSeconds + remainingSeconds;
    double videoDuration = _numFrames / _frameRate;
    _encodingSpeed = videoDuration / expectedDuration;
}

void AbstractRenderer::readyRead(QString output)
{
    emit newLog( output );
    emit progress();
}

void AbstractRenderer::launchProcess( QStringList arguments )
{
    //create process
    QProcess *renderer = new QProcess(this);
    connect( renderer, SIGNAL(readyReadStandardError()), this, SLOT(processStdError()));
    connect( renderer, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdOutput()));
    connect( renderer, SIGNAL(processStarted()), this, SLOT(processStarted()));
    connect( renderer, SIGNAL(processFinished(int)), this, SLOT(processFinished()));
    connect( renderer, SIGNAL(processErrorOccurred(QProcess::ProcessError)), this, SLOT(processErrorOccurred(QProcess::ProcessError)));

    //launch
    renderer->setProgram( _binaryFileName );
    renderer->setArguments( arguments );
    renderer->start(QIODevice::ReadWrite);

    //TODO check processor affinity?

    _renderProcesses << renderer;

    emit newLog("Launched process: " + QString::number( _renderProcesses.count() ));
}
