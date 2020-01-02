#ifndef BLOCKEXR_H
#define BLOCKEXR_H

#include "ui_blockexr.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockEXR : public UIBlockContent, private Ui::BlockEXR
{
    Q_OBJECT

public:
    explicit BlockEXR(MediaInfo *mediaInfo, QWidget *parent = nullptr);
};

#endif // BLOCKEXR_H
