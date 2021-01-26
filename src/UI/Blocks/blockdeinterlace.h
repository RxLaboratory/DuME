#ifndef BLOCKDEINTERLACE_H
#define BLOCKDEINTERLACE_H

#include "ui_blockdeinterlace.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockDeinterlace : public BlockContentWidget, private Ui::BlockDeinterlace
{
    Q_OBJECT

public:
    explicit BlockDeinterlace(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_deinterlaceButton_clicked(bool checked);
    void on_deinterlaceBox_currentIndexChanged(int index);

private:
    void setDeinterlaceParity(int index);
};

#endif // BLOCKDEINTERLACE_H
