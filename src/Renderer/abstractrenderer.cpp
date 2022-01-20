#include "abstractrenderer.h"
#include <QtDebug>

AbstractRenderer::AbstractRenderer(QObject *parent) : QObject(parent)
{
    setStatus( MediaUtils::Initializing );

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

    _output = "";
    _timer = QElapsedTimer();
}

int AbstractRenderer::currentFrame() const
{
    return _currentFrame;
}

QTime AbstractRenderer::startTime() const
{
    return _startTime;
}

double AbstractRenderer::outputSize( ) const
{
    return _outputSize;
}

double AbstractRenderer::outputBitrate( ) const
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

void AbstractRenderer::start( QStringList arguments, int numThreads )
{
    setStatus( MediaUtils::Launching );

    _timer.start();

    qDebug().noquote() << "Launching " + QString::number( numThreads ) + " processes.";
    for (int i = 0; i < numThreads; i++ )
    {
        launchProcess( arguments );
    }
    _startTime = QTime::currentTime();

    setStatus( MediaUtils::Encoding );
}

void AbstractRenderer::stop(int timeout)
{
    qDebug().noquote() << "Sending the stop command";

    setStatus( MediaUtils::Cleaning );

    // send the stop command to everyone
    if ( _stopCommand != "")
    {
        foreach( QProcess *renderProcess, _renderProcesses )
        {
            if (renderProcess->state() == QProcess::NotRunning) continue;
            renderProcess->write( _stopCommand.toUtf8() );
        }
    }

   qDebug().noquote() << "Stop command sent. Waiting for processes to shut down.";

    // wait for timeout and kill all remaining processes
    QTimer::singleShot(timeout, this, SLOT( killRenderProcesses()) );
}

void AbstractRenderer::processStdError()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    QString log = process->readAllStandardError();
    processOutput( log );
}

void AbstractRenderer::processStdOutput()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    QString log = process->readAllStandardOutput();
    processOutput( log );
}

void AbstractRenderer::processStarted()
{
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process) + 1;

    qDebug().noquote() << "Process " + QString::number( id ) + " started.";
}

void AbstractRenderer::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    processOutput("\n", true);
    // Get the process
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process);

    if (exitStatus == QProcess::NormalExit)
    {
        qDebug().noquote() << "Process " + QString::number(id + 1) + " has exited with code " + QString::number(exitCode) + ".";
    }
    if (exitStatus == QProcess::CrashExit)
    {
        qDebug().noquote() << "Process " + QString::number(id + 1) + " has crashed with code " + QString::number(exitCode) + ". Some output files may be corrupted";
    }

    _renderProcesses.removeAt(id);
    process->deleteLater();

    //if all processes have finished
    if ( _renderProcesses.count() == 0 )
    {
        disconnect(this, &AbstractRenderer::statusChanged, _job, &QueueItem::setStatus);
        setStatus( MediaUtils::Finished );
        if (_job->status() != MediaUtils::Error) _job->setStatus(MediaUtils::Finished);

    }
}

void AbstractRenderer::processErrorOccurred(QProcess::ProcessError e)
{
    processOutput("\n", true);
    QProcess* process = qobject_cast<QProcess*>(sender());
    int id = _renderProcesses.indexOf(process) + 1;

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

    emit newLog( error, DuQFLog::Warning );

    // Check if all processes have stopped
    QList<int> finishedProcesses;
    for(int i = 0; i < _renderProcesses.count(); i++)
    {
        if( _renderProcesses[i]->state() == QProcess::NotRunning )
        {
            finishedProcesses << i;
        }
    }

    //if all processes have finished
    if ( finishedProcesses.count() == _renderProcesses.count() )
    {
        //remove all processes
        while( _renderProcesses.count() > 0 )
        {
            QProcess *process = _renderProcesses.takeLast();
            process->deleteLater();
        }

        setStatus( MediaUtils::Error );
    }
}

void AbstractRenderer::killRenderProcesses()
{   
    bool killed = false;
    while ( _renderProcesses.count() > 0 )
    {
        QProcess *rp = _renderProcesses.takeLast();
        if (rp->state() != QProcess::NotRunning)
        {
            rp->kill();
            qDebug().noquote() << "Killed process " + QString::number( _renderProcesses.count() + 1 ) ;
            killed = true;
        }
        rp->deleteLater();
    }
    if (killed) emit newLog("Some processes did not stop correctly and had to be killed. The output file may be corrupted.");

    setStatus( MediaUtils::Stopped );
}

void AbstractRenderer::processOutputLine()
{
    _output = _output.trimmed();
    qDebug() << _output;
    if (_output != "") readyRead(_output);
    _output = "";
}

void AbstractRenderer::processOutput(QString output, bool ignoreTimer)
{
    _outputBuffer += output;
    if (!_timer.hasExpired(100) && !ignoreTimer) return;

    QStringList outputs = _outputBuffer.split(QRegularExpression("[\\n\\r]"));

    if (outputs.count() > 1)
    {
        for (int i = 0 ; i < outputs.count() -1 ; i++)
        {
            if (i == 0) _output += outputs.at(i);
            else _output = outputs.at(i);
            processOutputLine();
        }
        // last one
        if (_outputBuffer.endsWith("\n") || _outputBuffer.endsWith("\r")) _outputBuffer = outputs.last() + "\n";
    }

    _output += _outputBuffer;

    if (_output.endsWith("\n") || _output.endsWith("\r"))
    {
        processOutputLine();
    }

    _outputBuffer = "";
}

bool AbstractRenderer::launchJob()
{
    return false;
}

MediaUtils::RenderStatus AbstractRenderer::status() const
{
    return _status;
}

void AbstractRenderer::setBinary(const QString &binaryFileName)
{
    _binaryFileName = binaryFileName;
    setStatus( MediaUtils:: Waiting );
}

void AbstractRenderer::setStatus(MediaUtils::RenderStatus status)
{
    if (status == _status) return;
    _status = status;
    emit statusChanged( _status );
}

bool AbstractRenderer::render(QueueItem *job)
{
    _job = job;
    connect(this, &AbstractRenderer::statusChanged, _job, &QueueItem::setStatus);
    setStatus( MediaUtils:: Launching );
    return launchJob();
}

double AbstractRenderer::expectedSize() const
{
    return _expectedSize;
}

int AbstractRenderer::numFrames() const
{
    return _numFrames;
}

double AbstractRenderer::frameRate() const
{
    return _frameRate;
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

void AbstractRenderer::setCurrentFrame(int currentFrame, double size, double bitrate, double speed)
{
    _currentFrame = currentFrame;

    qDebug() << "Progress: " + QString::number( _currentFrame );

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
    int remainingSeconds = (elapsedSeconds * _numFrames / _currentFrame)-elapsedSeconds;

    _elapsedTime = QTime( 0, 0 ).addSecs( elapsedSeconds );
    _timeRemaining = QTime( 0, 0 ).addSecs( remainingSeconds );

    //compute size
    if (size == 0)
    {
        //get file(s)
        //check if it's a sequence
        QRegularExpression regExDigits("({#+})");
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
    }
    else
    {
        _outputSize = size;
    }


    //compute bitrate
    if (bitrate == 0)
    {
        double outputBits = _outputSize / 8;
        double renderedSeconds = _currentFrame / _frameRate;
        _outputBitrate = outputBits / renderedSeconds;
    }
    else
    {
        _outputBitrate = bitrate;
    }


    //expected size
    _expectedSize = _outputSize * _numFrames / _currentFrame;

    //encoding speed
    if (speed == 0)
    {
        double expectedDuration = elapsedSeconds + remainingSeconds;
        double videoDuration = _numFrames / _frameRate;
        _encodingSpeed = videoDuration / expectedDuration;
    }
    else
    {
        _encodingSpeed = speed;
    }
}

void AbstractRenderer::readyRead(QString output)
{
    //emit console( output );
    //emit progress();
}

void AbstractRenderer::launchProcess( QStringList arguments )
{
    //create process
    QProcess *renderer = new QProcess(this);
    connect( renderer, SIGNAL(readyReadStandardError()), this, SLOT(processStdError()));
    connect( renderer, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdOutput()));
    connect( renderer, SIGNAL(started()), this, SLOT(processStarted()));
    connect( renderer, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect( renderer, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(processErrorOccurred(QProcess::ProcessError)));

    //launch

    //TODO check processor affinity?
    _renderProcesses << renderer;
    ProcessUtils::runProcess( renderer, _binaryFileName, arguments);

    qDebug().noquote() << "Launched process: " + QString::number( _renderProcesses.count() );
}
