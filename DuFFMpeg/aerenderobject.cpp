#include "aerenderobject.h"

AERenderObject::AERenderObject(QString path, QObject *parent) : QObject(parent)
{
    _aerender = new QProcess(this);
    _aeOutput = "";

    connect(_aerender,SIGNAL(readyReadStandardError()),this,SLOT(stdErrorAE()));
    connect(_aerender,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutputAE()));
    connect(_aerender,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurredAE(QProcess::ProcessError)));

    _name = "";
    _path = path;
    _mainVersion = 0;
    _primaryVersion = 0;
    _secondaryVersion = 0;
    _buildNumber = 0;

    _isValid = false;
}

void AERenderObject::stdErrorAE()
{
    _aeOutput += _aerender->readAllStandardError();
}

void AERenderObject::init()
{
    QFile aerenderFile(_path);


    if (aerenderFile.exists())
    {
        _isValid = true;
        //get version
        QRegularExpression reVersion(".*After Effects ([^\\/\\\\]+)",QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = reVersion.match(_path);
        QString version = "";
        if (match.hasMatch())
        {
            _name = match.captured(1);
        }
#ifdef QT_DEBUG
qDebug() << "Found Ae version: " + version;
#endif
        //get real version number from aerender
        _aeOutput = "";

        _aerender->setProgram(aerenderFile.fileName());
        _aerender->setArguments(QStringList("-help"));
        _aerender->start();
        _aerender->waitForFinished(3000);

        QRegularExpression reRealVersion(".*aerender version ([\\d.x]+)",QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);
        match = reRealVersion.match(_aeOutput);

        if (match.hasMatch())
        {
    #ifdef QT_DEBUG
    qDebug() << "Found Ae version number: " + match.captured(1);
    #endif
            _name += " (" + match.captured(1) + ")";

            //get version from name
            QRegularExpression re(".* \\((\\d+)\\.(\\d+)(?:\\.(\\d+))?(?:x(\\d+))?\\)");
            //matchs the version numbers in these patterns:
            // XX (16.0x207)
            // XX (11.0.4x2)
            // XX (15.1.2)
            // XX (15.1)

            QRegularExpressionMatch match = re.match(_name);
            if (match.hasMatch())
            {
                _mainVersion = match.captured(1).toInt();
                _primaryVersion = match.captured(2).toInt();
                _secondaryVersion = match.captured(3).toInt();
                _buildNumber = match.captured(4).toInt();
            }
        }
    }
}

bool AERenderObject::isValid() const
{
    return _isValid;
}

void AERenderObject::stdOutputAE()
{
    _aeOutput += _aerender->readAllStandardOutput();
}

void AERenderObject::errorOccurredAE(QProcess::ProcessError e)
{
    QString error;
    if (e == QProcess::FailedToStart)
    {
        error = "Failed to start AERender.";
    }
    else if (e == QProcess::Crashed)
    {
        error = "AERender just crashed.";
    }
    else if (e == QProcess::Timedout)
    {
        error = "AERender operation timed out.";
    }
    else if (e == QProcess::WriteError)
    {
        error = "AERender write Error.";
    }
    else if (e == QProcess::ReadError)
    {
        error = "Cannot read AERender output.";
    }
    else if (e == QProcess::UnknownError)
    {
        error = "An unknown AERender error occured.";
    }
#ifdef QT_DEBUG
    qDebug() << error;
#endif
}

QString AERenderObject::name() const
{
    return _name;
}

QString AERenderObject::path() const
{
    return _path;
}

int AERenderObject::mainVersion() const
{
    return _mainVersion;
}

int AERenderObject::primaryVersion() const
{
    return _primaryVersion;
}

int AERenderObject::secondaryVersion() const
{
    return _secondaryVersion;
}

int AERenderObject::buildNumber() const
{
    return _buildNumber;
}
