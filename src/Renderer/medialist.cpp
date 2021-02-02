#include "medialist.h"

MediaList::MediaList(QObject *parent) : QObject(parent)
{

}

MediaList::MediaList(QList<MediaInfo *> medias, QObject *parent) : QObject(parent)
{
    _medias = medias;
}

MediaList::MediaList(MediaInfo *media, QObject *parent) : QObject(parent)
{
    _medias << media;
}

QList<MediaInfo *> MediaList::medias() const
{
    return _medias;
}

MediaInfo *MediaList::media(int mediaId)
{
    foreach(MediaInfo *m, _medias)
    {
        if (m->id() == mediaId) return m;
    }
    return nullptr;
}

int MediaList::addMedia(MediaInfo *media)
{
    _medias << media;
    emit newMedia( media );
    emit changed();
    return _medias.count()-1;
}

MediaInfo *MediaList::takeMedia(int id)
{
    MediaInfo *media = _medias.takeAt(id);
    emit mediaRemoved(media);
    emit changed();
    return media;
}

void MediaList::removeMedia(int id)
{
    MediaInfo *media = _medias.takeAt(id);
    emit mediaRemoved(media);
    emit changed();
    media->deleteLater();
}

void MediaList::removeMedia(MediaInfo *media)
{
    _medias.removeOne(media);
    emit mediaRemoved(media);
    emit changed();
    media->deleteLater();
}

bool MediaList::hasVideo()
{
    foreach( MediaInfo *m, _medias)
    {
        if (m->hasVideo()) return true;
    }
    return false;
}

bool MediaList::hasAudio()
{
    foreach( MediaInfo *m, _medias)
    {
        if (m->hasAudio()) return true;
    }
    return false;
}

int MediaList::count()
{
    return _medias.count();
}
