#ifndef BLOCKFRAMERATE_H
#define BLOCKFRAMERATE_H

#include "ui_blockframerate.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockFrameRate : public UIBlockContent, private Ui::BlockFrameRate
{
    Q_OBJECT

public:
    explicit BlockFrameRate(MediaInfo *mediaInfo, QWidget *parent = nullptr);

public slots:
    void setActivated( bool activate );
    void update();
    void setFrameRate( double f );
private slots:
    void on_frameRateEdit_valueChanged(double arg1);
    void on_action8_fps_triggered();
    void on_action12_fps_triggered();
    void on_action15_fps_triggered();
    void on_action23_976_fps_triggered();
    void on_action24_fps_triggered();
    void on_action25_fps_triggered();
    void on_action29_97_fps_triggered();
    void on_action30_fps_triggered();
    void on_action50_fps_triggered();
    void on_action59_94_fps_triggered();
    void on_action60_fps_triggered();
    void on_action120_fps_triggered();
};

#endif // BLOCKFRAMERATE_H
