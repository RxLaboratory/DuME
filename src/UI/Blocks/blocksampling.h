#ifndef BLOCKSAMPLING_H
#define BLOCKSAMPLING_H

#include "ui_blocksampling.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockSampling : public BlockContentWidget, private Ui::BlockSampling
{
    Q_OBJECT

public:
    explicit BlockSampling(MediaInfo *mediaInfo,QWidget *parent = nullptr);
    void setSampling(int s);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_samplingBox_currentIndexChanged(int index);
};

#endif // BLOCKSAMPLING_H
