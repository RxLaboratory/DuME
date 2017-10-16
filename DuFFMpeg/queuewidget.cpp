#include "queuewidget.h"

QueueWidget::QueueWidget(FFmpeg *ff,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;

    inputWidget = new InputWidget(ffmpeg,this);
    outputWidget = new OutputWidget(ffmpeg,this);

    inputTab1->layout()->addWidget(inputWidget);
    outputTab1->layout()->addWidget(outputWidget);

    ffmpeg_init();

    connect(ffmpeg,SIGNAL(binaryChanged()),this,SLOT(ffmpeg_init()));


    //TODO Connect input infos to output
}

FFMediaInfo *QueueWidget::getInputMedia()
{
    return inputWidget->getMediaInfo();
}

FFMediaInfo *QueueWidget::getOutputMedia()
{
    return outputWidget->getMediaInfo();
}

void QueueWidget::ffmpeg_init()
{

}
