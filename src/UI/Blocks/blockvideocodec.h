#ifndef BLOCKVIDEOCODEC_H
#define BLOCKVIDEOCODEC_H

#include "global.h"

#include "ui_blockvideocodec.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockVideoCodec : public BlockContentWidget, private Ui::BlockVideoCodec
{
    Q_OBJECT

public:
    explicit BlockVideoCodec(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void listCodecs();
    void on_videoCodecsFilterBox_currentIndexChanged(int index);
    void on_videoCodecsBox_currentIndexChanged(int index);
private:
    void setDefaultCodec();
    void setCodec(QString name, bool tryWithoutFilter = true );
};

#endif // BLOCKVIDEOCODEC_H
