#ifndef BLOCKAUDIOBITRATE_H
#define BLOCKAUDIOBITRATE_H

#include "ui_blockaudiobitrate.h"
#include "UI/Blocks/uiblockcontent.h"

class BlockAudioBitrate : public UIBlockContent, private Ui::BlockAudioBitrate
{
    Q_OBJECT

public:
    explicit BlockAudioBitrate(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void setActivated( bool activate );
    void update();
private slots:
    void on_audioBitRateEdit_valueChanged(int arg1);
};

#endif // BLOCKAUDIOBITRATE_H
