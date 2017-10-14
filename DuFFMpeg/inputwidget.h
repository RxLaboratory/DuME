#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "ui_inputwidget.h"

#include <QFileDialog>

#include "ffmpeg.h"

class InputWidget : public QWidget, private Ui::InputWidget
{
    Q_OBJECT

public:
    explicit InputWidget(FFmpeg *ff,QWidget *parent = 0);
    FFMediaInfo *getMediaInfo();

private slots:
    void on_inputBrowseButton_clicked();

private:
    FFmpeg *ffmpeg;
    FFMediaInfo *mediaInfo;
};

#endif // INPUTWIDGET_H
