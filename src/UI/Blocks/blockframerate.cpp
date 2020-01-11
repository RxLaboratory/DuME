#include "blockframerate.h"

BlockFrameRate::BlockFrameRate(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setupUi(this);

    _presets->addAction( action8_fps );
    _presets->addAction( action12_fps );
    _presets->addAction( action15_fps );
    _presets->addAction( action23_976_fps );
    _presets->addAction( action24_fps );
    _presets->addAction( action25_fps );
    _presets->addAction( action29_97_fps );
    _presets->addAction( action30_fps );
    _presets->addAction( action50_fps );
    _presets->addAction( action59_94_fps );
    _presets->addAction( action60_fps );
    _presets->addAction( action120_fps );
}

void BlockFrameRate::activate(bool activate)
{
    _freezeUI = true;

    if (activate)
    {
        _mediaInfo->setFramerate( frameRateEdit->value() );
    }
    else
    {
        _mediaInfo->setFramerate( 0 );
    }

    _freezeUI = false;
}

void BlockFrameRate::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    if (stream->isCopy())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    emit blockEnabled(true);

    double f = stream->framerate();
    if ( f != 0.0)
    {
        frameRateEdit->setValue( f );
    }

    _freezeUI = false;
}

void BlockFrameRate::setFrameRate(double f)
{
    _freezeUI = true;
    frameRateEdit->setValue( f );
    _freezeUI = false;
}

void BlockFrameRate::on_frameRateEdit_valueChanged(double arg1)
{
    _mediaInfo->setFramerate( arg1 );
}

void BlockFrameRate::on_action8_fps_triggered()
{
    _mediaInfo->setFramerate( 8 );
}

void BlockFrameRate::on_action12_fps_triggered()
{
    _mediaInfo->setFramerate( 12 );
}

void BlockFrameRate::on_action15_fps_triggered()
{
    _mediaInfo->setFramerate( 15 );
}

void BlockFrameRate::on_action23_976_fps_triggered()
{
    _mediaInfo->setFramerate( 23.976 );
}

void BlockFrameRate::on_action24_fps_triggered()
{
    _mediaInfo->setFramerate( 24 );
}

void BlockFrameRate::on_action25_fps_triggered()
{
    _mediaInfo->setFramerate( 25 );
}

void BlockFrameRate::on_action29_97_fps_triggered()
{
    _mediaInfo->setFramerate( 29.97 );
}

void BlockFrameRate::on_action30_fps_triggered()
{
    _mediaInfo->setFramerate( 30 );
}

void BlockFrameRate::on_action50_fps_triggered()
{
    _mediaInfo->setFramerate( 50 );
}

void BlockFrameRate::on_action59_94_fps_triggered()
{
    _mediaInfo->setFramerate( 59.94 );
}

void BlockFrameRate::on_action60_fps_triggered()
{
    _mediaInfo->setFramerate( 60 );
}

void BlockFrameRate::on_action120_fps_triggered()
{
    _mediaInfo->setFramerate( 120 );
}
