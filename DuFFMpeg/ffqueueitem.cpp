#include "ffqueueitem.h"

FFQueueItem::FFQueueItem(QList<FFMediaInfo *> inputs, QList<FFMediaInfo *> outputs, QObject *parent)
{
    inputMedias = inputs;
    outputMedias = outputs;
    status = Waiting;
    emit queued();
}

FFQueueItem::FFQueueItem(FFMediaInfo *input, QList<FFMediaInfo *> outputs, QObject *parent)
{
    inputMedias << input;
    outputMedias = outputs;
    status = Waiting;
}

FFQueueItem::FFQueueItem(FFMediaInfo *input, FFMediaInfo *output, QObject *parent)
{
    inputMedias << input;
    outputMedias << output;
    status = Waiting;
}

QList<FFMediaInfo *> FFQueueItem::getInputMedias()
{
    return inputMedias;
}

QList<FFMediaInfo *> FFQueueItem::getOutputMedias()
{
    return outputMedias;
}

int FFQueueItem::addInputMedia(FFMediaInfo *input)
{
    inputMedias << input;
    return inputMedias.count() - 1;
}

int FFQueueItem::addOutputMedia(FFMediaInfo *output)
{
    outputMedias << output;
    return outputMedias.count() -1;
}

FFMediaInfo *FFQueueItem::removeInputMedia(int id)
{
    return inputMedias.takeAt(id);
}

FFMediaInfo *FFQueueItem::removeInputMedia(QString fileName)
{
    for (int i = 0 ; i < inputMedias.count() ; i++)
    {
        if (inputMedias[i]->getFileName() == fileName)
        {
            return inputMedias.takeAt(i);
        }
    }
}

FFMediaInfo *FFQueueItem::removeOutputMedia(int id)
{
    return outputMedias.takeAt(id);
}

FFMediaInfo *FFQueueItem::removeOutputMedia(QString fileName)
{
    for (int i = 0 ; i < outputMedias.count() ; i++)
    {
        if (outputMedias[i]->getFileName() == fileName)
        {
            return outputMedias.takeAt(i);
        }
    }
}

FFQueueItem::Status FFQueueItem::getStatus()
{
    return status;
}

void FFQueueItem::setStatus(Status st)
{
    if(status == st) return;
    status = st;
    emit statusChanged(status);
    if (status == InProgress) emit encodingStarted();
    else if (status == Finished) emit encodingFinished();
    else if (status == Stopped) emit encodingStopped();
    else if (status == Waiting) emit queued();
}
