#include "queuewidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

QueueWidget::QueueWidget(FFmpeg *ff,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;

    inputWidget = new InputWidget(ffmpeg,this);
    outputWidget = new OutputWidget(ffmpeg,this);

    inputTab1->layout()->addWidget(inputWidget);
    outputTab1->layout()->addWidget(outputWidget);

    //splitter sizes
    _settings.beginGroup("queuesplitter");
    QList<int>sizes;
    sizes << _settings.value("inputsize",100).toInt();
    sizes << _settings.value("outputsize",100).toInt();
    splitter->setSizes(sizes);
    _settings.endGroup();

    ffmpeg_init();

    connect(ffmpeg,SIGNAL(binaryChanged()),this,SLOT(ffmpeg_init()));

    connect(inputWidget,SIGNAL(newMediaLoaded(FFMediaInfo*)),outputWidget,SLOT(newInputMedia(FFMediaInfo*)));
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

void QueueWidget::saveSettings()
{
    _settings.beginGroup("queuesplitter");
    _settings.setValue("inputsize",splitter->sizes()[0]);
    _settings.setValue("outputsize",splitter->sizes()[1]);
    _settings.endGroup();
}
