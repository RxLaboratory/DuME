#ifndef BLOCKAUDIOSAMPLEFORMAT_H
#define BLOCKAUDIOSAMPLEFORMAT_H

#include "ui_blockaudiosampleformat.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockAudioSampleFormat : public BlockContentWidget, private Ui::BlockAudioSampleFormat
{
    Q_OBJECT

public:
    explicit BlockAudioSampleFormat(MediaInfo *mediaInfo,QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_samplingBox_currentIndexChanged(int index);
};

#endif // BLOCKAUDIOSAMPLEFORMAT_H
