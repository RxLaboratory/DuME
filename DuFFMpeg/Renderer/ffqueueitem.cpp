#include "ffqueueitem.h"

FFQueueItem::FFQueueItem(QList<MediaInfo *> inputs, QList<MediaInfo *> outputs, QObject *parent) : FFObject(parent)
{
    _inputMedias = inputs;
    _outputMedias = outputs;
    _status = Waiting;
    emit queued();
}

FFQueueItem::FFQueueItem(MediaInfo *input, QList<MediaInfo *> outputs, QObject *parent) : FFObject(parent)
{
    _inputMedias << input;
    _outputMedias = outputs;
    _status = Waiting;
}

FFQueueItem::FFQueueItem(MediaInfo *input, MediaInfo *output, QObject *parent) : FFObject(parent)
{
    _inputMedias << input;
    _outputMedias << output;
    _status = Waiting;
}

FFQueueItem::~FFQueueItem()
{
    postRenderCleanUp();
}

QList<MediaInfo *> FFQueueItem::getInputMedias()
{
    return _inputMedias;
}

QList<MediaInfo *> FFQueueItem::getOutputMedias()
{
    return _outputMedias;
}

int FFQueueItem::addInputMedia(MediaInfo *input)
{
    _inputMedias << input;
    return _inputMedias.count() - 1;
}

int FFQueueItem::addOutputMedia(MediaInfo *output)
{
    _outputMedias << output;
    return _outputMedias.count() -1;
}

MediaInfo *FFQueueItem::removeInputMedia(int id)
{
    return _inputMedias.takeAt(id);
}

MediaInfo *FFQueueItem::removeInputMedia(QString fileName)
{
    for (int i = 0 ; i < _inputMedias.count() ; i++)
    {
        if (_inputMedias[i]->fileName() == fileName)
        {
            return _inputMedias.takeAt(i);
        }
    }
}

MediaInfo *FFQueueItem::removeOutputMedia(int id)
{
    return _outputMedias.takeAt(id);
}

MediaInfo *FFQueueItem::removeOutputMedia(QString fileName)
{
    for (int i = 0 ; i < _outputMedias.count() ; i++)
    {
        if (_outputMedias[i]->fileName() == fileName)
        {
            return _outputMedias.takeAt(i);
        }
    }
}

FFQueueItem::Status FFQueueItem::getStatus()
{
    return _status;
}

void FFQueueItem::setStatus(Status st)
{
    if(_status == st) return;
    _status = st;
    emit statusChanged(_status);
    if (_status == InProgress) emit encodingStarted();
    else if (_status == Finished) emit encodingFinished();
    else if (_status == Stopped || _status == Error || _status == AEError) emit encodingStopped();
    else if (_status == Waiting) emit queued();
}

void FFQueueItem::postRenderCleanUp()
{
    //remove ae cache
    MediaInfo *input = _inputMedias[0];
    if (input->cacheDir() == nullptr) return;
    QTemporaryDir *aeTempDir = input->cacheDir();
    aeTempDir->remove();
    input->setCacheDir(nullptr);
    delete aeTempDir;
}
