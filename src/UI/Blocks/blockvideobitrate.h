#ifndef BLOCKVIDEOBITRATE_H
#define BLOCKVIDEOBITRATE_H

#include "ui_blockvideobitrate.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockVideoBitrate : public UIBlockContent, private Ui::BlockVideoBitrate
{
    Q_OBJECT

public:
    explicit BlockVideoBitrate(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_videoBitrateButton_clicked(bool checked);
    void on_videoBitRateEdit_editingFinished();
    void on_videoQualityButton_clicked(bool checked);
    void on_videoQualitySlider_valueChanged(int value);
    void on_actionPerfect_95_triggered();
    void on_actionAuto_triggered();
    void on_actionGood_60_triggered();
    void on_actionProxy_30_triggered();
    void on_actionBad_20_triggered();
    void on_actionBlu_Ray_triggered();
    void on_actionDVD_triggered();
    void on_actionStreaming_12_Mbps_triggered();
};

#endif // BLOCKVIDEOBITRATE_H