#include "cachemanager.h"

CacheManager::CacheManager(QObject *parent) : QObject(parent)
{

}

void CacheManager::scan()
{
    qint64 c = FileUtils::getDirSize(_rootCacheDir);
    if (c != _cacheSize)
    {
        _cacheSize = c;
        emit cacheSizeChanged(_cacheSize);
    }
}

qint64 CacheManager::cacheSize() const
{
    return _cacheSize;
}

CacheManager *CacheManager::instance()
{
    if (!_instance) _instance = new CacheManager();
    return _instance;
}

void CacheManager::init()
{
    QSettings settings;
    QString defaultCachePath = QDir::tempPath() + "/" + qApp->applicationName() + "/";
    defaultCachePath = QDir::toNativeSeparators(defaultCachePath);

    QString currentCachePath = settings.value("cachePath", defaultCachePath).toString();

    qDebug() << "Cache located at " + currentCachePath;

    setRootCacheDir( currentCachePath, false );

    _scanTimer = new QTimer(this);
    connect(_scanTimer, SIGNAL(timeout()), this, SLOT(scan()));
    _scanTimer->start(5000);
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
    _rootCacheDir.removeRecursively();
}

QDir CacheManager::aeCacheDir() const
{
    return _aeCacheDir;
}

QTemporaryDir *CacheManager::getAeTempDir()
{
    return new QTemporaryDir( _aeCacheDir.absolutePath() + "/DuME_Cache" );
}

CacheManager *CacheManager::_instance = nullptr;
