#include "aerenderobject.h"

#ifdef QT_DEBUG
 #include <QtDebug>
#endif

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

AERenderObject::~AERenderObject()
{
    restoreOriginalTemplates();
}

void AERenderObject::stdErrorAE()
{
    _aeOutput += _aerender->readAllStandardError();
}

void AERenderObject::findDataPath()
{
    //try to find templates (the Ae data dir)
    QStringList dataPaths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

#ifdef Q_OS_WIN
    //search the right one
    foreach(QString dataPath, dataPaths)
    {
        dataPath = dataPath.replace("/Duduf/DuFFmpeg","");

#ifdef QT_DEBUG
qDebug() << "Searching Ae Data in: " + dataPath;
#endif

        QDir test(dataPath);
        if (QDir(test.absolutePath() + "/Adobe/After Effects").exists())
        {
            _dataPath = test.absolutePath() + "/Adobe/After Effects";
            break;
        }
    }

    //if found, append version
    if (_dataPath != "") _dataPath += "/" + QString::number(_mainVersion) + "." + QString::number(_primaryVersion);
#else

#endif

#ifdef QT_DEBUG
qDebug() << "AE Data Location: " + _dataPath;
#endif
}

QString AERenderObject::dataPath() const
{
    return _dataPath;
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

                findDataPath();
            }
        }
    }
}

bool AERenderObject::setDuFFmpegTemplates()
{
    //load render settings and output modules

    //if found, replace templates
    if (_dataPath == "") return false;

    QDir aeDataDir(_dataPath);
    if (!aeDataDir.exists()) return false;


    //TODO if CS6 and before (<= 11.0), prefs file
    if (_mainVersion <= 11)
    {
        QStringList settingsFilter("*-x64*");
        QStringList settingsFilePaths = aeDataDir.entryList(settingsFilter);

#ifdef QT_DEBUG
qDebug() << settingsFilePaths;
#endif

        //write our own settings
        foreach(QString settingsFilePath, settingsFilePaths)
        {
            if (!settingsFilePath.endsWith(".bak"))
            {
                QString bakPath = _dataPath + "/" + settingsFilePath + ".bak";
                QString origPath = _dataPath + "/" + settingsFilePath;

                //rename original file
                FileUtils::move(origPath,bakPath);

                //copy our own
                FileUtils::copy(":/after-effects/cs6-prefs", origPath);
            }
        }
    }
    //if higher than CS6
    else
    {
        QStringList settingsFilter("*-indep-render*");
        QStringList outputFilter("*-indep-output*");

        QStringList settingsFilePaths = aeDataDir.entryList(settingsFilter);
        QStringList outputFilePaths = aeDataDir.entryList(outputFilter);

#ifdef QT_DEBUG
qDebug() << settingsFilePaths;
qDebug() << outputFilePaths;
#endif

        //write our own settings
        foreach(QString settingsFilePath, settingsFilePaths)
        {
            if (!settingsFilePath.endsWith(".bak"))
            {
                QString bakPath = _dataPath + "/" + settingsFilePath + ".bak";
                QString origPath = _dataPath + "/" + settingsFilePath;

                //rename original file
                FileUtils::move(origPath,bakPath);

                //copy our own
                FileUtils::copy(":/after-effects/render" + QString::number(_mainVersion), origPath);
            }
        }

        //write our own modules
        foreach(QString outputFilePath, outputFilePaths)
        {
            if (!outputFilePath.endsWith(".bak"))
            {
                QString bakPath = _dataPath + "/" + outputFilePath + ".bak";
                QString origPath = _dataPath + "/" + outputFilePath;

                //rename original file
                FileUtils::move(origPath,bakPath);

                //copy our own
                FileUtils::copy(":/after-effects/output", origPath);
            }
        }

    }
    return true;
}

void AERenderObject::restoreOriginalTemplates()
{
    //if found, replace templates
    if (_dataPath == "") return;

    QDir aeDataDir(_dataPath);
    if (!aeDataDir.exists()) return;

    //TODO if CS6 and before (<= 11.0), prefs file
    if (_mainVersion <= 11)
    {
        QStringList settingsFilter("*-x64*");
        QStringList settingsFilePaths = aeDataDir.entryList(settingsFilter);

        //remove our own settings
        foreach(QString settingsFilePath, settingsFilePaths)
        {
            QString bakPath = _dataPath + "/" + settingsFilePath;
            QString txtPath = _dataPath + "/" + settingsFilePath.replace(".bak","");

#ifdef QT_DEBUG
qDebug() << "Restoring " + bakPath;
#endif

            //remove our own
            FileUtils::remove(txtPath);

            //rename original file
            FileUtils::move(bakPath, txtPath);
        }
    }
    //if higher than CS6
    else
    {
        QStringList settingsFilter("*-indep-render*.bak");
        QStringList outputFilter("*-indep-output*.bak");

        QStringList settingsFilePaths = aeDataDir.entryList(settingsFilter);
        QStringList outputFilePaths = aeDataDir.entryList(outputFilter);

        //remove our own settings
        foreach(QString settingsFilePath, settingsFilePaths)
        {
            QString bakPath = _dataPath + "/" + settingsFilePath;
            QString txtPath = _dataPath + "/" + settingsFilePath.replace(".bak","");

#ifdef QT_DEBUG
qDebug() << "Restoring " + bakPath;
#endif

            //remove our own
            FileUtils::remove(txtPath);

            //rename original file
            FileUtils::move(bakPath, txtPath);
        }

        //remove our own modules
        foreach(QString outputFilePath, outputFilePaths)
        {
            QString bakPath = _dataPath + "/" + outputFilePath;
            QString txtPath = _dataPath + "/" + outputFilePath.replace(".bak","");

#ifdef QT_DEBUG
qDebug() << "Restoring " + bakPath;
#endif

            //remove our own
            QFile newOutputFile(txtPath);
            newOutputFile.setPermissions(QFile::ReadOther | QFile::WriteOther);
            newOutputFile.remove();

            //rename original file
            QFile outputFile(bakPath);
            outputFile.setPermissions(QFile::ReadOther | QFile::WriteOther);
            outputFile.rename(txtPath);
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
    return QDir::toNativeSeparators(_path);
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
