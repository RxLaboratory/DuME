#ifndef STREAMREFERENCEWIDGET_H
#define STREAMREFERENCEWIDGET_H

#include <QWidget>
#include <QFileInfo>

#include "ui_streamreferencewidget.h"

#include "Renderer/mediainfo.h"
#include "Renderer/medialist.h"


class StreamReferenceWidget : public QWidget, Ui::StreamReferenceWidget
{
    Q_OBJECT

public:
    explicit StreamReferenceWidget(int id, MediaList *inputMedias, QWidget *parent = nullptr);
    void setStreamId(int id);
    void setMediaId(int id);
    int mediaId();
    int streamId();

signals:
    void removed(int);
    void mediaChanged(int,int);
    void streamChanged(int,int);

public slots:
    void addMedia(MediaInfo *media);

private slots:
    void removeMedia(MediaInfo *media);
    void on_mediaBox_currentIndexChanged(int index);
    void on_removeButton_clicked();
    void on_streamBox_currentIndexChanged(int index);
    void mediaChanged();

private:
    int _id;
    bool _freezeUI;
    void updateStreamList();
    MediaList *_inputMedias;
};

#endif // STREAMREFERENCEWIDGET_H
