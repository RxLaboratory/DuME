#ifndef BLOCKCROP_H
#define BLOCKCROP_H

#include "ui_blockcrop.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockCrop : public BlockContentWidget, private Ui::BlockCrop
{
    Q_OBJECT

public:
    explicit BlockCrop(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
    void setMaximum(int w, int h);
private slots:
    void setCropMargins();
    void setCropSize();
    void on_setMarginsButton_toggled(bool checked);
    void on_widthSpinBox_valueChanged(int arg1);
    void on_heightSpinBox_valueChanged(int arg1);

    void on_actionHD1080_triggered();
    void on_action2KFlat_triggered();
    void on_action2KScope_triggered();
    void on_actionDSM2K_triggered();
    void on_action4KFlat_triggered();
    void on_action4KScope_triggered();
    void on_actionDSM4K_triggered();
    void on_action8KUHD_triggered();
    void on_action4KUHD_triggered();
    void on_actionHDSquare_triggered();
    void on_actionSmallSquare_triggered();
private:
    void setSize(int width, int height);
};

#endif // BLOCKCROP_H
