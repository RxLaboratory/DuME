#include "aftereffects.h"

bool aeVersionSorter(AfterEffectsVersion *v1, AfterEffectsVersion *v2)
{
    if (v1->version() < v2->version()) return true;
    if (v1->version() > v2->version()) return false;

    if (v1->name().toLower() < v2->name().toLower()) return true;
    if (v1->name().toLower() > v2->name().toLower()) return false;

    return true;
}

AfterEffects::AfterEffects(QObject *parent) : AbstractRendererInfo(parent)
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

    setBinary();
}

QList<AfterEffectsVersion *> AfterEffects::versions() const
{
    return _versions;
}

bool AfterEffects::setBinary()
{
    QString name = "Custom";

    if ( settings.value("aerender/useLatest",true).toBool() )
    {
        name = "Latest";
    }
    else
    {
        name = settings.value("aerender/version","Custom").toString();
    }

    return setBinary( name );
}

bool AfterEffects::setBinary(QString name)
{
    emit newLog("Selecting the After Effects renderer");

    _currentVersion = nullptr;

#if INIT_AE //used when developping to skip Ae loading

    if ( name == "Custom" )
    {
        // Create and add the custom process to the list
        AfterEffectsVersion *ae = new AfterEffectsVersion( settings.value("aerender/path","").toString() );
        if ( ae->isValid() )
        {
            _versions << ae;
            name = ae->name();
            _useLatest = false;
        }
        else
        {
            emit newLog( "Invalid After Effects custom renderer at: " + settings.value("aerender/path","").toString(), LogUtils::Warning );
            //revert to use latest
            name = "Latest";
        }
    }

    if ( name == "Latest" && _versions.count() > 0 )
    {
        _useLatest = true;
        settings.setValue( "aerender/useLatest", true );
        name = _versions.last()->name();
        settings.setValue( "aerender/version", "Latest" );
    }
    else
    {
        settings.setValue( "aerender/useLatest", false );
        settings.setValue( "aerender/version", name);
    }

    for (int i = 0; i < _versions.count(); i++)
    {
        if (_versions[i]->name() == name)
        {
            settings.setValue("aerender/path", _versions[i]->path() );

            if ( AbstractRendererInfo::setBinary( _versions[i]->path() ) )
            {
                emit newLog("After Effects set to renderer: " + _versions[i]->name() );
                _currentName = name;
                _currentVersion = _versions[i];
                return true;
            }
        }
    }

    emit newLog( "After Effects not found on this system. After Effects rendering disabled.", LogUtils::Warning );
    return false;

#endif
}

bool AfterEffects::setDuMETemplates()
{
    if ( _currentVersion != nullptr ) return _currentVersion->setDuMETemplates();
    else return false;
}

void AfterEffects::restoreOriginalTemplates()
{
    if ( _currentVersion != nullptr ) _currentVersion->restoreOriginalTemplates();
}

bool AfterEffects::useLatest() const
{
    return _useLatest;
}

QString AfterEffects::currentName() const
{
    return _currentName;
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
