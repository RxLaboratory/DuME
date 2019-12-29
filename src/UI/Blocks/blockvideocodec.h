#ifndef BLOCKVIDEOCODEC_H
#define BLOCKVIDEOCODEC_H

#include "ui_blockvideocodec.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockVideoCodec : public UIBlockContent, private Ui::BlockVideoCodec
{
    Q_OBJECT

public:
    explicit BlockVideoCodec(FFmpeg *ffmpeg, MediaInfo *mediaInfo, QWidget *parent = nullptr);
    void setDefaultVideoCodec();
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void listCodecs();
    void on_videoCodecsFilterBox_currentIndexChanged(int index);
    void on_videoCodecsBox_currentIndexChanged(int index);

private:
    FFmpeg *_ffmpeg;
    void setCodec(QString name);
};

#endif // BLOCKVIDEOCODEC_H
