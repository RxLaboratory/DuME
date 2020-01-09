#ifndef INPUTMEDIAS_H
#define INPUTMEDIAS_H

#include <QObject>

#include "Renderer/mediainfo.h"

class MediaList : public QObject
{
    Q_OBJECT
public:
    explicit MediaList(QObject *parent = nullptr);
    MediaList(QList<MediaInfo *> medias, QObject *parent = nullptr);
    MediaList(MediaInfo *media, QObject *parent = nullptr);
    QList<MediaInfo *> medias() const;
    MediaInfo *getMedia(int mediaId);
    int addMedia(MediaInfo *media);
    MediaInfo *takeMedia(int id);
    void removeMedia(int id);
    void removeMedia(MediaInfo *media);

signals:
    void newMedia(MediaInfo*);
    void mediaRemoved(MediaInfo*);

private:
    QList<MediaInfo *> _medias;

};

#endif // INPUTMEDIAS_H
