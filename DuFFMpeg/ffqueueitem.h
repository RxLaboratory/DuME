#ifndef FFQUEUEITEM_H
#define FFQUEUEITEM_H

#include "ffobject.h"

#include "ffmediainfo.h"

class FFQueueItem : public FFObject
{
        Q_OBJECT
public:
    FFQueueItem(QList<FFMediaInfo*> inputs,QList<FFMediaInfo*> outputs,QObject *parent = nullptr);
    FFQueueItem(FFMediaInfo *input,QList<FFMediaInfo*> outputs,QObject *parent = nullptr);
    FFQueueItem(FFMediaInfo *input,FFMediaInfo *output,QObject *parent = nullptr);
    ~FFQueueItem();

    /**
     * @brief The Status enum Used to describe the current status of the item
     */
    enum Status { Waiting, InProgress, Finished, Stopped, AEError, Error };
    Q_ENUM(Status)

    QList<FFMediaInfo*> getInputMedias();
    QList<FFMediaInfo*> getOutputMedias();
    int addInputMedia(FFMediaInfo *input);
    int addOutputMedia(FFMediaInfo *output);
    FFMediaInfo *removeInputMedia(int id);
    FFMediaInfo *removeInputMedia(QString fileName);
    FFMediaInfo *removeOutputMedia(int id);
    FFMediaInfo *removeOutputMedia(QString fileName);
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
    QList<FFMediaInfo *> _inputMedias;
    QList<FFMediaInfo *> _outputMedias;
    Status _status;
};

#endif // FFQUEUEITEM_H
