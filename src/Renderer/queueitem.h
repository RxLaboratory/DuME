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

    QList<MediaInfo*> getInputMedias();
    QList<MediaInfo*> getOutputMedias();
    int addInputMedia(MediaInfo *input);
    int addOutputMedia(MediaInfo *output);
    MediaInfo *takeInputMedia(int id);
    MediaInfo *takeInputMedia(QString fileName);
    MediaInfo *takeOutputMedia(int id);
    MediaInfo *takeOutputMedia(QString fileName);
    MediaUtils::Status getStatus();

public slots:
    /**
     * @brief setStatus Changes the status of the item
     * @param st The status
     */
    void setStatus( MediaUtils::Status st );
    void postRenderCleanUp();

signals:
    void statusChanged( MediaUtils::Status );

private slots:

private:
    QList<MediaInfo *> _inputMedias;
    QList<MediaInfo *> _outputMedias;
    MediaUtils::Status _status;
};

#endif // FFQUEUEITEM_H
