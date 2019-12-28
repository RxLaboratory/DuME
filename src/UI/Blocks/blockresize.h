#ifndef BLOCKRESIZE_H
#define BLOCKRESIZE_H

#include "ui_blockresize.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockResize : public UIBlockContent, private Ui::BlockResize
{
    Q_OBJECT

public:
    explicit BlockResize(MediaInfo *mediaInfo, QWidget *parent = nullptr);
};

#endif // BLOCKRESIZE_H
