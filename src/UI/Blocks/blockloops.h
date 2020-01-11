#ifndef BLOCKLOOPS_H
#define BLOCKLOOPS_H

#include "ui_blockloops.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockLoops : public BlockContentWidget, private Ui::BlockLoops
{
    Q_OBJECT

public:
    explicit BlockLoops(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_videoLoopsEdit_valueChanged(int arg1);
};

#endif // BLOCKLOOPS_H
