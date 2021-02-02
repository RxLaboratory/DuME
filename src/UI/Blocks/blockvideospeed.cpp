#include "blockvideospeed.h"

BlockVideoSpeed::BlockVideoSpeed(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setType(Type::Video);
    setupUi(this);
}

void BlockVideoSpeed::activate(bool activate)
{
    if (!activate)
    {
        _mediaInfo->setVideoSpeed(1.0);
    }
    else
    {
        _mediaInfo->setVideoSpeed(speedBox->value());
    }
}

void BlockVideoSpeed::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    speedBox->setValue(stream->speed());
}

void BlockVideoSpeed::on_speedBox_valueChanged(double arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoSpeed(arg1);
}
