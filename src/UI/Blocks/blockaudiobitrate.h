#ifndef BLOCKAUDIOBITRATE_H
#define BLOCKAUDIOBITRATE_H

#include "ui_blockaudiobitrate.h"
#include "UI/Blocks/blockcontentwidget.h"

class BlockAudioBitrate : public BlockContentWidget, private Ui::BlockAudioBitrate
{
    Q_OBJECT

public:
    explicit BlockAudioBitrate(MediaInfo *mediaInfo, QWidget *parent = nullptr);
public slots:
    void activate( bool blockEnabled );
    void update();
private slots:
    void on_audioBitRateEdit_editingFinished();
    void on_action128_triggered();
    void on_action196_triggered();
    void on_action256_triggered();
    void on_action320_triggered();
    void on_audioBitRateEdit_valueChanged(int arg1);
    void on_actionAuto_triggered();
};

#endif // BLOCKAUDIOBITRATE_H
