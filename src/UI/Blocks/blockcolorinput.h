#ifndef BLOCKCOLORINPUT_H
#define BLOCKCOLORINPUT_H

#include "ui_blockcolorinput.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockColorInput : public BlockContentWidget, private Ui::BlockColorInput
{
    Q_OBJECT

public:
    explicit BlockColorInput(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void spaceBox_currentIndexChanged(int index);
    void primariesBox_currentIndexChanged(int index);
    void trcBox_currentIndexChanged(int index);
    void rangeBox_currentIndexChanged(int index);
    void presetTriggered();
};

#endif // BLOCKCOLORINPUT_H
