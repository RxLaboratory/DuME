#include "aerender.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

AERender::AERender(QObject *parent) : QObject(parent)
{
    _aerender = new QProcess(this);
    _aeOutput = "";

    connect(_aerender,SIGNAL(readyReadStandardError()),this,SLOT(stdErrorAE()));
    connect(_aerender,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOutputAE()));
    connect(_aerender,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurredAE(QProcess::ProcessError)));

}

QList<AERenderObject *> AERender::versions() const
{
    return _versions;
}

bool aeVersionSorter(AERenderObject *v1, AERenderObject *v2)
{
    if (v1->mainVersion() < v2->mainVersion()) return true;
    if (v1->mainVersion() > v2->mainVersion()) return false;

    if (v1->primaryVersion() < v2->primaryVersion()) return true;
    if (v1->primaryVersion() > v2->primaryVersion()) return false;

    if (v1->secondaryVersion() < v2->secondaryVersion()) return true;
    if (v1->secondaryVersion() > v2->secondaryVersion()) return false;

    if (v1->buildNumber() < v2->buildNumber()) return true;
    if (v1->buildNumber() > v2->buildNumber()) return false;

    if (v1->name().toLower() < v2->name().toLower()) return true;
    if (v1->name().toLower() > v2->name().toLower()) return false;

    return true;
}

void AERender::init()
{
    //get aerender paths
#ifdef Q_OS_WIN
    //Search all drives
    QFileInfoList drives = QDir::drives();
    foreach(QFileInfo drive, drives)
    {
        findAeVersions(drive.absoluteFilePath() + "Program Files\\Adobe\\");
        findAeVersions(drive.absoluteFilePath() + "Program Files (x86)\\Adobe\\");
    }

#endif
#ifdef Q_OS_MAC
    findAeVersions("/Applications");
#endif

    //sort
    std::sort(_versions.begin(),_versions.end(),aeVersionSorter);
}

void AERender::stdErrorAE()
{
    _aeOutput += _aerender->readAllStandardError();
}

void AERender::stdOutputAE()
{
    _aeOutput += _aerender->readAllStandardOutput();
}

void AERender::errorOccurredAE(QProcess::ProcessError e)
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

void AERender::findAeVersions(QString dir)
{
    QStringList filters("*After Effects *");

    QDir adobeDir(dir);

    adobeDir.setNameFilters(filters);
    adobeDir.setFilter(QDir::Dirs);

    QFileInfoList versionPaths = adobeDir.entryInfoList();

    foreach(QFileInfo path,versionPaths)
    {
#ifdef Q_OS_WIN
        QFile aerenderFile(path.absoluteFilePath() + "/Support Files/aerender.exe");
#endif
#ifdef Q_OS_MAC
        QFile aerenderFile(path.absoluteFilePath() + "/aerender");
#endif

        if (aerenderFile.exists())
        {
            //get version
            QRegularExpression reVersion(".*After Effects (.+)",QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match = reVersion.match(path.absoluteFilePath());
            QString version = "";
            if (match.hasMatch())
            {
                version = match.captured(1);
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
                version += " (" + match.captured(1) + ")";
            }

            AERenderObject *ae = new AERenderObject(version,aerenderFile.fileName());
            _versions << ae;
        }
    }
}
