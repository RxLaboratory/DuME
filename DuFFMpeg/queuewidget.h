#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include "ui_queuewidget.h"

#include "inputwidget.h"
#include "outputwidget.h"

class QueueWidget : public QWidget, private Ui::QueueWidget
{
    Q_OBJECT

public:
    explicit QueueWidget(FFmpeg *ff, QWidget *parent = 0);
    FFMediaInfo *getInputMedia();
    FFMediaInfo *getOutputMedia();

public slots:
    void ffmpeg_init();

private:
    InputWidget *inputWidget;
    OutputWidget *outputWidget;
    FFmpeg *ffmpeg;
};

#endif // QUEUEWIDGET_H
