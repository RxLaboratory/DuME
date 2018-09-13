#include "aerender.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

AERender::AERender(QObject *parent) : QObject(parent)
{

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

AERenderObject *AERender::getAERenderObject(QString aeRenderFileName)
{
    foreach(AERenderObject *ae, _versions)
    {
        if (ae->path() == aeRenderFileName)
        {
            return ae;
        }
    }

    AERenderObject *ae = new AERenderObject(aeRenderFileName);
    ae->init();
    if (ae->isValid())
    {
        _versions << ae;
        //sort
        std::sort(_versions.begin(),_versions.end(),aeVersionSorter);
    }

    return ae;
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
        QString aerenderFile(path.absoluteFilePath() + "/Support Files/aerender.exe");
#endif
#ifdef Q_OS_MAC
        QString aerenderFile(path.absoluteFilePath() + "/aerender");
#endif

        AERenderObject *ae = new AERenderObject(aerenderFile);
        ae->init();
        if (ae->isValid()) _versions << ae;
    }
}
