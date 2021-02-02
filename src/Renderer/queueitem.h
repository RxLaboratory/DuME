#ifndef FFQUEUEITEM_H
#define FFQUEUEITEM_H

#include <QObject>
#include "mediainfo.h"
#include "Renderer/medialist.h"

class QueueItem : public QObject
{
        Q_OBJECT
public:
    QueueItem(QObject *parent = nullptr);
    QueueItem(MediaList *inputs,MediaList *outputs,QObject *parent = nullptr);
    QueueItem(QList<MediaInfo*> inputs,QList<MediaInfo*> outputs,QObject *parent = nullptr);
    QueueItem(MediaInfo *input,QList<MediaInfo*> outputs,QObject *parent = nullptr);
    QueueItem(MediaInfo *input,MediaInfo *output,QObject *parent = nullptr);
    ~QueueItem();

    QList<MediaInfo*> getInputMedias();
    MediaList *inputMedias();
    QList<MediaInfo*> getOutputMedias();
    MediaList *outputMedias();
    int addInputMedia(MediaInfo *input);
    int addOutputMedia(MediaInfo *output);
    MediaInfo *takeInputMedia(int id);
    MediaInfo *takeInputMedia(QString fileName);
    MediaInfo *takeOutputMedia(int id);
    MediaInfo *takeOutputMedia(QString fileName);
    MediaUtils::RenderStatus status();

public slots:
    /**
     * @brief setStatus Changes the status of the item
     * @param st The status
     */
    void setStatus( MediaUtils::RenderStatus st );
    void postRenderCleanUp();

signals:
    void statusChanged( MediaUtils::RenderStatus );

private slots:

private:
    MediaList *_inputMedias;
    MediaList *_outputMedias;
    MediaUtils::RenderStatus _status;
};

#endif // FFQUEUEITEM_H
