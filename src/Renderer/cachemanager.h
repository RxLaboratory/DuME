#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include "duqf-utils/utils.h"

#include <QObject>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>
#include <QtDebug>
#include <QTimer>

class CacheManager : public QObject
{
    Q_OBJECT
public:
    static CacheManager *instance();
    QDir getRootCacheDir()  const;
    void init();
    QDir aeCacheDir() const;
    QTemporaryDir *getAeTempDir();
    qint64 cacheSize() const;

public slots:
    void setRootCacheDir(QString path, bool purge = true);
    void purgeCache();

signals:
    void cacheSizeChanged(int);

private:
    //private constructor, this is a singleton
    explicit CacheManager(QObject *parent = nullptr);
    /**
     * @brief A timer to regularly scan the cache (and know its size)
     */
    QTimer *_scanTimer;
    QDir _rootCacheDir;
    QDir _aeCacheDir;
    qint64 _cacheSize;

private slots:
    void scan();
protected:
    static CacheManager *_instance;
};

#endif // CACHEMANAGER_H
