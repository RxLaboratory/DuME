#ifndef BLOCKLOOPS_H
#define BLOCKLOOPS_H

#include "ui_blockloops.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockLoops : public UIBlockContent, private Ui::BlockLoops
{
    Q_OBJECT

public:
    explicit BlockLoops(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_videoLoopsEdit_valueChanged(int arg1);
};

#endif // BLOCKLOOPS_H
