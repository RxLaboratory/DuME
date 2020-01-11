#ifndef BLOCKEXR_H
#define BLOCKEXR_H

#include "ui_blockexr.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockEXR : public BlockContentWidget, private Ui::BlockEXR
{
    Q_OBJECT

public:
    explicit BlockEXR(MediaInfo *mediaInfo, QWidget *parent = nullptr);
};

#endif // BLOCKEXR_H
