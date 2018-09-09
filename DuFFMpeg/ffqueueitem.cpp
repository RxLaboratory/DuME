#include "ffqueueitem.h"

FFQueueItem::FFQueueItem(QList<FFMediaInfo *> inputs, QList<FFMediaInfo *> outputs, QObject *parent) : FFObject(parent)
{
    _inputMedias = inputs;
    _outputMedias = outputs;
    _status = Waiting;
    emit queued();
}

FFQueueItem::FFQueueItem(FFMediaInfo *input, QList<FFMediaInfo *> outputs, QObject *parent) : FFObject(parent)
{
    _inputMedias << input;
    _outputMedias = outputs;
    _status = Waiting;
}

FFQueueItem::FFQueueItem(FFMediaInfo *input, FFMediaInfo *output, QObject *parent) : FFObject(parent)
{
    _inputMedias << input;
    _outputMedias << output;
    _status = Waiting;
}

QList<FFMediaInfo *> FFQueueItem::getInputMedias()
{
    return _inputMedias;
}

QList<FFMediaInfo *> FFQueueItem::getOutputMedias()
{
    return _outputMedias;
}

int FFQueueItem::addInputMedia(FFMediaInfo *input)
{
    _inputMedias << input;
    return _inputMedias.count() - 1;
}

int FFQueueItem::addOutputMedia(FFMediaInfo *output)
{
    _outputMedias << output;
    return _outputMedias.count() -1;
}

FFMediaInfo *FFQueueItem::removeInputMedia(int id)
{
    return _inputMedias.takeAt(id);
}

FFMediaInfo *FFQueueItem::removeInputMedia(QString fileName)
{
    for (int i = 0 ; i < _inputMedias.count() ; i++)
    {
        if (_inputMedias[i]->fileName() == fileName)
        {
            return _inputMedias.takeAt(i);
        }
    }
}

FFMediaInfo *FFQueueItem::removeOutputMedia(int id)
{
    return _outputMedias.takeAt(id);
}

FFMediaInfo *FFQueueItem::removeOutputMedia(QString fileName)
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
