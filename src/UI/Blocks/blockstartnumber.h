#ifndef BLOCKSTARTNUMBER_H
#define BLOCKSTARTNUMBER_H

#include "ui_blockstartnumber.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockStartNumber : public BlockContentWidget, private Ui::BlockStartNumber
{
    Q_OBJECT

public:
    explicit BlockStartNumber(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_startNumberEdit_valueChanged(int arg1);
};

#endif // BLOCKSTARTNUMBER_H
