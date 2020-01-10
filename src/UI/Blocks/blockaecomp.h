#ifndef BLOCKAECOMP_H
#define BLOCKAECOMP_H

#include "ui_blockaecomp.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockAEComp : public UIBlockContent, private Ui::BlockAEComp
{
    Q_OBJECT

public:
    explicit BlockAEComp(MediaInfo *mediaInfo,QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_compButton_clicked(bool checked);
    void on_rqindexButton_clicked(bool checked);
    void on_aeRenderQueueButton_clicked(bool checked);
    void on_compEdit_editingFinished();
    void on_rqindexBox_valueChanged(int arg1);
};

#endif // BLOCKAECOMP_H
