#include "queueitem.h"

QueueItem::QueueItem(QList<MediaInfo *> inputs, QList<MediaInfo *> outputs, QObject *parent) : QObject(parent)
{
    _inputMedias = inputs;
    _outputMedias = outputs;
    _status = MediaUtils::Waiting;
    emit queued();
}

QueueItem::QueueItem(MediaInfo *input, QList<MediaInfo *> outputs, QObject *parent) : QObject(parent)
{
    _inputMedias << input;
    _outputMedias = outputs;
    _status = MediaUtils::Waiting;
}

QueueItem::QueueItem(MediaInfo *input, MediaInfo *output, QObject *parent) : QObject(parent)
{
    _inputMedias << input;
    _outputMedias << output;
    _status = MediaUtils::Waiting;
}

QueueItem::~QueueItem()
{
    postRenderCleanUp();
}

QList<MediaInfo *> QueueItem::getInputMedias()
{
    return _inputMedias;
}

QList<MediaInfo *> QueueItem::getOutputMedias()
{
    return _outputMedias;
}

int QueueItem::addInputMedia(MediaInfo *input)
{
    _inputMedias << input;
    return _inputMedias.count() - 1;
}

int QueueItem::addOutputMedia(MediaInfo *output)
{
    _outputMedias << output;
    return _outputMedias.count() -1;
}

MediaInfo *QueueItem::takeInputMedia(int id)
{
    return _inputMedias.takeAt(id);
}

MediaInfo *QueueItem::takeInputMedia(QString fileName)
{
    for (int i = 0 ; i < _inputMedias.count() ; i++)
    {
        if (_inputMedias[i]->fileName() == fileName)
        {
            return _inputMedias.takeAt(i);
        }
    }
    return nullptr;
}

MediaInfo *QueueItem::takeOutputMedia(int id)
{
    return _outputMedias.takeAt(id);
}

MediaInfo *QueueItem::takeOutputMedia(QString fileName)
{
    for (int i = 0 ; i < _outputMedias.count() ; i++)
    {
        if (_outputMedias[i]->fileName() == fileName)
        {
            return _outputMedias.takeAt(i);
        }
    }
    return nullptr;
}

MediaUtils::Status QueueItem::getStatus()
{
    return _status;
}

void QueueItem::setStatus( MediaUtils::Status st )
{
    if(_status == st) return;
    _status = st;
    emit statusChanged( _status );
    if (_status == MediaUtils::FFmpegEncoding || _status == MediaUtils::AERendering || _status == MediaUtils::BlenderRendering ) emit encodingStarted();
    else if (_status == MediaUtils::Finished ) emit encodingFinished();
    else if (_status == MediaUtils::Stopped || _status == MediaUtils::Error ) emit encodingStopped();
    else if (_status == MediaUtils::Waiting ) emit queued();
}

void QueueItem::postRenderCleanUp()
{
    //remove ae cache
    MediaInfo *input = _inputMedias[0];
    if (input->cacheDir() == nullptr) return;
    QTemporaryDir *aeTempDir = input->cacheDir();
    aeTempDir->remove();
    input->setCacheDir(nullptr);
    delete aeTempDir;
}