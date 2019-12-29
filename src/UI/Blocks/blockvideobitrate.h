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
    void on_videoBitRateEdit_valueChanged(double arg1);
    void on_videoQualityButton_clicked(bool checked);

    void on_videoQualitySlider_valueChanged(int value);

private:
    bool _freezeUI;
};

#endif // BLOCKVIDEOBITRATE_H
