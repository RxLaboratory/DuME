#include "queueitem.h"

QueueItem::QueueItem(QObject *parent) : QObject(parent)
{
    _inputMedias = new MediaList(this);
    _outputMedias = new MediaList(this);
    _status = MediaUtils::Waiting;
}

QueueItem::QueueItem(MediaList *inputs, MediaList *outputs, QObject *parent) : QObject(parent)
{
    _inputMedias = inputs;
    _outputMedias = outputs;
    _status = MediaUtils::Waiting;
}

QueueItem::QueueItem(QList<MediaInfo *> inputs, QList<MediaInfo *> outputs, QObject *parent) : QObject(parent)
{
    _inputMedias = new MediaList(this);
    _outputMedias = new MediaList(this);
    foreach(MediaInfo *i, inputs)
    {
        addInputMedia(i);
    }
    foreach(MediaInfo *o, outputs)
    {
        addOutputMedia(o);
    }
    _status = MediaUtils::Waiting;
}

QueueItem::QueueItem(MediaInfo *input, QList<MediaInfo *> outputs, QObject *parent) : QObject(parent)
{
    _inputMedias = new MediaList(this);
    _outputMedias = new MediaList(this);
    addInputMedia(input);
    foreach(MediaInfo *o, outputs)
    {
        addOutputMedia(o);
    }
    _status = MediaUtils::Waiting;
}

QueueItem::QueueItem(MediaInfo *input, MediaInfo *output, QObject *parent) : QObject(parent)
{
    _inputMedias = new MediaList(this);
    _outputMedias = new MediaList(this);
    addInputMedia(input);
    addOutputMedia(output);
    _status = MediaUtils::Waiting;
}

QueueItem::~QueueItem()
{
    postRenderCleanUp();
}

QList<MediaInfo *> QueueItem::getInputMedias()
{
    return _inputMedias->medias();
}

MediaList *QueueItem::inputMedias()
{
    return _inputMedias;
}

QList<MediaInfo *> QueueItem::getOutputMedias()
{
    return _outputMedias->medias();
}

MediaList *QueueItem::outputMedias()
{
    return _outputMedias;
}

int QueueItem::addInputMedia(MediaInfo *input)
{
    return _inputMedias->addMedia(input);
}

int QueueItem::addOutputMedia(MediaInfo *output)
{
    return _outputMedias->addMedia(output);
}

MediaInfo *QueueItem::takeInputMedia(int id)
{
    return _inputMedias->takeMedia(id);
}

MediaInfo *QueueItem::takeInputMedia(QString fileName)
{
    for (int i = 0 ; i < _inputMedias->count() ; i++)
    {
        if (_inputMedias->media(i)->fileName() == fileName)
        {
            return _inputMedias->takeMedia(i);
        }
    }
    return nullptr;
}

MediaInfo *QueueItem::takeOutputMedia(int id)
{
    return _outputMedias->takeMedia(id);
}

MediaInfo *QueueItem::takeOutputMedia(QString fileName)
{
    for (int i = 0 ; i < _outputMedias->count() ; i++)
    {
        if (_outputMedias->media(i)->fileName() == fileName)
        {
            return _outputMedias->takeMedia(i);
        }
    }
    return nullptr;
}

MediaUtils::RenderStatus QueueItem::status()
{
    return _status;
}

void QueueItem::setStatus( MediaUtils::RenderStatus st )
{
    if(_status == st) return;
    _status = st;
    emit statusChanged( _status );
}

void QueueItem::postRenderCleanUp()
{
    foreach (MediaInfo *input, _inputMedias->medias())
    {
        //remove cache
        if (input->cacheDir() == nullptr) return;
        QTemporaryDir *cacheDir = input->cacheDir();
        cacheDir->remove();
        input->setCacheDir(nullptr);
        delete cacheDir;
    }
}
