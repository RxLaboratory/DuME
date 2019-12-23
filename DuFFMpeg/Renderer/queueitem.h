#ifndef FFQUEUEITEM_H
#define FFQUEUEITEM_H

#include <QObject>
#include "mediainfo.h"

class QueueItem : public QObject
{
        Q_OBJECT
public:
    QueueItem(QList<MediaInfo*> inputs,QList<MediaInfo*> outputs,QObject *parent = nullptr);
    QueueItem(MediaInfo *input,QList<MediaInfo*> outputs,QObject *parent = nullptr);
    QueueItem(MediaInfo *input,MediaInfo *output,QObject *parent = nullptr);
    ~QueueItem();

    /**
     * @brief The Status enum Used to describe the current status of the item
     */
    enum Status { Waiting, InProgress, Finished, Stopped, AEError, Error };
    Q_ENUM(Status)

    QList<MediaInfo*> getInputMedias();
    QList<MediaInfo*> getOutputMedias();
    int addInputMedia(MediaInfo *input);
    int addOutputMedia(MediaInfo *output);
    MediaInfo *removeInputMedia(int id);
    MediaInfo *removeInputMedia(QString fileName);
    MediaInfo *removeOutputMedia(int id);
    MediaInfo *removeOutputMedia(QString fileName);
    Status getStatus();

public slots:
    /**
     * @brief setStatus Changes the status of the item
     * @param st The status
     */
    void setStatus(Status st);
    void postRenderCleanUp();

signals:
    void encodingStarted();
    void encodingStopped();
    void encodingFinished();
    void queued();
    void statusChanged(Status);

private slots:

private:
    QList<MediaInfo *> _inputMedias;
    QList<MediaInfo *> _outputMedias;
    Status _status;
};

#endif // FFQUEUEITEM_H
