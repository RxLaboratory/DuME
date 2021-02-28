#ifndef BLOCKWORKINGSPACE_H
#define BLOCKWORKINGSPACE_H

#include "ui_blockworkingspace.h"

#include "Renderer/mediainfo.h"

class BlockWorkingSpace : public QWidget, private Ui::BlockWorkingSpace
{
    Q_OBJECT

public:
    explicit BlockWorkingSpace(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void update();
private slots:
    void profileBox_currentIndexChanged(int index);
private:
    MediaInfo *_mediaInfo;
};

#endif // BLOCKWORKINGSPACE_H
