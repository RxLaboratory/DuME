#include "abstractrendererinfo.h"

AbstractRendererInfo::AbstractRendererInfo(QObject *parent) : QObject(parent)
{
    _output = "";
    _status = MediaUtils::Waiting;
    _process = new QProcess();
    _valid = true;
    _ignoreErrors = false;

    //Connect process
    connect(_process,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(_process,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(_process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));
}

QString AbstractRendererInfo::binary() const
{
    return _binary;
}

bool AbstractRendererInfo::setBinary(QString binary)
{
    bool fileExists = true;

#ifdef Q_OS_WIN
    fileExists = QFile(binary).exists();
#endif

    if(fileExists)
    {
        _binary = binary;
        _process->setProgram( binary );
        //check result
        qDebug() << "Renderer set to: " + binary;
        emit binaryChanged( binary );
        qDebug() << "Renderer is valid.";
        emit valid(true);
        _valid = true;
        qDebug() << "Renderer ready.";
        return true;
    }
    else
    {
        emit newLog("Renderer executable binary not found.", LogUtils::Warning );
        emit valid(false);
        _valid = false;
        return false;
    }
}

bool AbstractRendererInfo::runCommand(QString commands, int timeout, QIODevice::OpenModeFlag of, bool ignoreErrors)
{
    //detect arguments
    QRegularExpression re("(\"[^\"]*\"|[\\S]+)");
    QRegularExpressionMatchIterator i = re.globalMatch(commands);
    QStringList commandList;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString command = match.captured(1);
        command.replace("\"","");
        commandList << command;
    }
    return runCommand(commandList, timeout, of, ignoreErrors);
}

bool AbstractRendererInfo::runCommand(QStringList commands, int timeout, QIODevice::OpenModeFlag of, bool ignoreErrors)
{
    _output = "";
    _process->setArguments(commands);
    _ignoreErrors = ignoreErrors;
    _process->start(of);
    QCoreApplication::processEvents();
    return _process->waitForFinished(timeout);
}

void AbstractRendererInfo::log(QString l, LogUtils::LogType lt)
{
    emit newLog( l, lt );
}

void AbstractRendererInfo::stdError()
{
    QString output = _process->readAllStandardError();
    readyRead(output);
}

void AbstractRendererInfo::stdOutput()
{
    QString output = _process->readAllStandardOutput();
    readyRead(output);
}

void AbstractRendererInfo::errorOccurred(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start process.";
    }
    else if (e == QProcess::Crashed)
    {
        error = "Process just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "Process operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "Process write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read process output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown process error occured.";
    }

    if (_ignoreErrors) emit newLog( error, LogUtils::Warning );
    else emit newLog( error, LogUtils::Critical );
    _lastErrorMessage = error;
    _status = MediaUtils::Error;

    if (!_ignoreErrors)
    {
        _valid = false;
        emit valid(false);
    }
}

QString AbstractRendererInfo::lastErrorMessage() const
{
    return _lastErrorMessage;
}

void AbstractRendererInfo::readyRead(QString output)
{
    emit newLog( output, LogUtils::Debug );
    emit console( output );
    _output += output;
}

bool AbstractRendererInfo::isValid() const
{
    return _valid;
}

