#include "aerenderprocess.h"

#ifdef QT_DEBUG
 #include <QtDebug>
#endif

AERenderProcess::AERenderProcess(QString path, QObject *parent) : QObject(parent)
{
    _aerender = new QProcess(this);
    _aeOutput = "";

    connect(_aerender,SIGNAL(readyReadStandardError()),this,SLOT(stdError()));
    connect(_aerender,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutput()));
    connect(_aerender,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));

    _name = "";
    _path = path;
    _version = QVersionNumber();

    _isValid = false;

    init();
}

AERenderProcess::~AERenderProcess()
{
    restoreOriginalTemplates();
}

void AERenderProcess::stdError()
{
    _aeOutput += _aerender->readAllStandardError();
}

QVersionNumber AERenderProcess::version() const
{
    return _version;
}

void AERenderProcess::findDataPath()
{
    //try to find templates (the Ae data dir)
    QStringList dataPaths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

#ifdef Q_OS_WIN

    //search the right one
    foreach(QString dataPath, dataPaths)
    {
        dataPath = dataPath.replace("/Duduf/DuME","");

        emit debugLog( "Searching Ae Data in: " + dataPath );

        QDir test(dataPath);
        if (QDir(test.absolutePath() + "/Adobe/After Effects").exists())
        {
            _dataPath = test.absolutePath() + "/Adobe/After Effects";
            break;
        }
    }

    //if found, append version
    if (_dataPath != "")
    {
        _dataPath += "/" + QString::number( _version.majorVersion() ) + "." + QString::number( _version.minorVersion() );
    }

#endif

    //TODO MAC OS

    emit newDebug( "AE Data Location: " + _dataPath );
}

QString AERenderProcess::dataPath() const
{
    return _dataPath;
}

void AERenderProcess::init()
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

        emit newDebug( "Found Ae version: " + version );

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
            emit newDebug( "Found Ae version number: " + match.captured(1) );

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
                QVector<int> v;
                //major - minor - micro - build
                v << match.captured(1).toInt() << match.captured(2).toInt() << match.captured(3).toInt() << match.captured(4).toInt();
                _version = QVersionNumber( v );

                findDataPath();
            }
        }
    }
}

bool AERenderProcess::setDuMETemplates()
{
    //load render settings and output modules

    //if found, replace templates
    if (_dataPath == "") return false;

    QDir aeDataDir(_dataPath);
    if (!aeDataDir.exists()) return false;


    // if CS6 and before (<= 11.0), prefs file
    if (_version.majorVersion() <= 11)
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
                FileUtils::copy(":/after-effects/render" + QString::number( _version.majorVersion() ), origPath);
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

void AERenderProcess::restoreOriginalTemplates()
{
    //if found, replace templates
    if (_dataPath == "") return;

    QDir aeDataDir(_dataPath);
    if (!aeDataDir.exists()) return;

    // if CS6 and before (<= 11.0), prefs file
    if (_version.majorVersion() <= 11)
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
            FileUtils::remove(txtPath);

            //rename original file
            FileUtils::move(bakPath, txtPath);
        }
    }
}

void AERenderProcess::started()
{
    _aeOutput = "";
}

QString AERenderProcess::aeOutput() const
{
    return _aeOutput;
}

bool AERenderProcess::isValid() const
{
    return _isValid;
}

void AERenderProcess::stdOutput()
{
    _aeOutput += _aerender->readAllStandardOutput();
}

void AERenderProcess::errorOccurred(QProcess::ProcessError e)
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

    emit newDebug( error );
}

void AERenderProcess::readyReadAE(QString output)
{
    emit newDebug("AERender output: " + output);

    _aeOutput = _aeOutput + output;
/*
    //parse

    //get current input
    MediaInfo *input = _currentItem->getInputMedias().at(0);

    //Duration
    QRegularExpression reDuration("PROGRESS:  Duration: (\\d):(\\d\\d):(\\d\\d):(\\d\\d)");
    QRegularExpressionMatch match = reDuration.match(output);
    if (match.hasMatch())
    {
        int h = match.captured(1).toInt();
        int m = match.captured(2).toInt();
        int s = match.captured(3).toInt();
        int i = match.captured(4).toInt();

        double duration = h*60*60 + m*60 + s + i/input->videoFramerate();

        input->setDurationH(h);
        input->setDurationM(m);
        input->setDurationS(s);
        input->setDurationF(i);
        input->setDuration(duration);
    }

    QRegularExpression reFrameRate("PROGRESS:  Frame Rate: (\\d+,\\d\\d)");
    match = reFrameRate.match(output);
    if (match.hasMatch())
    {
        double fr = match.captured(1).toDouble();

        input->setVideoFramerate(fr);

        int h = input->durationH();
        int m = input->durationM();
        double s = input->durationS();
        int i = input->durationF();

        double duration = h*60*60 + m*60 + s + i/input->videoFramerate();
        input->setDuration(duration);
    }

    QRegularExpression reProgress("PROGRESS:  \\d:\\d\\d:\\d\\d:\\d\\d \\((\\d+)\\)");
    match = reProgress.match(output);
    if (match.hasMatch())
    {
        _currentFrame = match.captured(1).toInt();
        //time remaining
        //get current input duration
        //gets the current item duration
        double duration = input->duration() * input->videoFramerate();
        if (duration > 0)
        {
            if (_currentFrame > 0)
            {
                int elapsed = _startTime.elapsed() / 1000;
                double remaining = elapsed*duration/_currentFrame - elapsed;
                _timeRemaining = QTime(0,0,0).addMSecs(remaining*1000);
            }
        }
    }

    //TODO get size and bitrate


    emit progress();
*/
}


QString AERenderProcess::name() const
{
    return _name;
}

QString AERenderProcess::path() const
{
    return QDir::toNativeSeparators(_path);
}
