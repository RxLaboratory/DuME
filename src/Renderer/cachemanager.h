#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QTemporaryDir>
#include <QFile>
#include <QtDebug>

class CacheManager : public QObject
{
    Q_OBJECT
public:
    explicit CacheManager(QObject *parent = nullptr);
    QDir getRootCacheDir()  const;
    void init();
    QDir getAeCacheDir() const;
    QTemporaryDir *getAeTempDir();

public slots:
    void setRootCacheDir(QString path, bool purge = true);
    void purgeCache();

signals:

private:
    QDir _rootCacheDir;
    QDir _aeCacheDir;
};

#endif // CACHEMANAGER_H
