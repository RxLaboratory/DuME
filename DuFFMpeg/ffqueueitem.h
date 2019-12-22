#ifndef FFQUEUEITEM_H
#define FFQUEUEITEM_H

#include "ffobject.h"

#include "dumediainfo.h"

class FFQueueItem : public FFObject
{
        Q_OBJECT
public:
    FFQueueItem(QList<DuMediaInfo*> inputs,QList<DuMediaInfo*> outputs,QObject *parent = nullptr);
    FFQueueItem(DuMediaInfo *input,QList<DuMediaInfo*> outputs,QObject *parent = nullptr);
    FFQueueItem(DuMediaInfo *input,DuMediaInfo *output,QObject *parent = nullptr);
    ~FFQueueItem();

    /**
     * @brief The Status enum Used to describe the current status of the item
     */
    enum Status { Waiting, InProgress, Finished, Stopped, AEError, Error };
    Q_ENUM(Status)

    QList<DuMediaInfo*> getInputMedias();
    QList<DuMediaInfo*> getOutputMedias();
    int addInputMedia(DuMediaInfo *input);
    int addOutputMedia(DuMediaInfo *output);
    DuMediaInfo *removeInputMedia(int id);
    DuMediaInfo *removeInputMedia(QString fileName);
    DuMediaInfo *removeOutputMedia(int id);
    DuMediaInfo *removeOutputMedia(QString fileName);
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
    QList<DuMediaInfo *> _inputMedias;
    QList<DuMediaInfo *> _outputMedias;
    Status _status;
};

#endif // FFQUEUEITEM_H
