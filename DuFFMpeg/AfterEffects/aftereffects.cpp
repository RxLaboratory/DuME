#include "aftereffects.h"

bool aeVersionSorter(AfterEffectsVersion *v1, AfterEffectsVersion *v2)
{
    if (v1->version() < v2->version()) return true;
    if (v1->version() > v2->version()) return false;

    if (v1->name().toLower() < v2->name().toLower()) return true;
    if (v1->name().toLower() > v2->name().toLower()) return false;

    return true;
}

AfterEffects::AfterEffects(QObject *parent) : QObject(parent)
{
    emit newLog("Initializing the After Effects renderer");

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

    setCurrentAERenderProcess();
}

QList<AfterEffectsVersion *> AfterEffects::versions() const
{
    return _versions;
}

void AfterEffects::setCurrentAERenderProcess()
{
    QString name = "Custom";

    if ( settings.value("aerender/useLatest",true).toBool() )
    {
        name = _versions.last()->name();
    }
    else
    {
        name = settings.value("aerender/version","Custom").toString();
    }

    setCurrentAERenderProcess( name );
}

void AfterEffects::setCurrentAERenderProcess(QString name)
{
    emit newLog("Selecting the After Effects renderer");

#if INIT_AE //used when developping to skip Ae loading

    if ( name == "Custom" )
    {
        // Create and add the custom process to the list
        AfterEffectsVersion *ae = new AfterEffectsVersion( settings.value("aerender/path","").toString() );
        if ( ae->isValid() )
        {
            _versions << ae;
            name = ae->name();
        }
        else
        {
            emit newLog("Invalid After Effects custom renderer at: " + settings.value("aerender/path","").toString());
            //revert to use latest
            settings.setValue( "aerender/useLatest", true );
            name = _versions.last()->name();
        }
    }

    for (int i = 0; i < _versions.count(); i++)
    {
        if (_versions[i]->name() == name)
        {
            settings.setValue( "aerender/version", _versions[i]->name() );
            settings.setValue("aerender/path", _versions[i]->path() );

            emit newLog("After Effects set to renderer: " + _versions[i]->name() );
            emit currentAeVersionChanged( _versions[i]->path() );
            _binary = _versions[i]->path();
            break;
        }
    }

#endif
}

QString AfterEffects::binary() const
{
    return _binary;
}

void AfterEffects::findAeVersions(QString dir)
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
#ifdef Q_OS_LINUX
        QString aerenderFile("");
#endif
        AfterEffectsVersion *ae = new AfterEffectsVersion( aerenderFile );
        if (ae->isValid()) _versions << ae;
    }
}
