#include "cachemanager.h"

CacheManager::CacheManager(QObject *parent) : QObject(parent)
{

}

void CacheManager::init()
{
    QSettings settings;
    QString defaultCachePath = QDir::tempPath() + "/" + qApp->applicationName() + "/";
    defaultCachePath = QDir::toNativeSeparators(defaultCachePath);

    QString currentCachePath = settings.value("cachePath", defaultCachePath).toString();

    qDebug() << "Cache located at " + currentCachePath;

    setRootCacheDir( currentCachePath, false );
}

QDir CacheManager::getRootCacheDir() const
{
    return _rootCacheDir;
}

void CacheManager::setRootCacheDir(QString path, bool purge)
{
    if (purge) purgeCache();

    //root
    _rootCacheDir = QDir(path);
    if (!_rootCacheDir.exists()) _rootCacheDir.mkpath(".");

    //aecache
    _aeCacheDir = QDir( _rootCacheDir.path() + "/aeCache" );
    if (!_aeCacheDir.exists()) _aeCacheDir.mkpath(".");

     QSettings settings;
     settings.setValue("cachePath", path);
}

void CacheManager::purgeCache()
{
    _aeCacheDir.removeRecursively();
}

QDir CacheManager::getAeCacheDir() const
{
    return _aeCacheDir;
}

QTemporaryDir *CacheManager::getAeTempDir()
{
    return new QTemporaryDir( _aeCacheDir.absolutePath() + "/DuME_Cache" );
}
