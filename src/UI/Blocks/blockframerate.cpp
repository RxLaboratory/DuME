#include "blockframerate.h"

BlockFrameRate::BlockFrameRate(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo, parent)
{
    setupUi(this);
}

void BlockFrameRate::setActivated(bool activate)
{
    if (activate)
    {
        _mediaInfo->setVideoFramerate( frameRateEdit->value() );
    }
    else
    {
        _mediaInfo->setVideoFramerate( 0 );
    }
}

void BlockFrameRate::update()
{
    double f = _mediaInfo->videoFramerate();
    if ( f != 0.0)
    {
        frameRateEdit->setValue( f );
        frameRateBox->setCurrentText( QString::number( f ) + " fps" );
    }
}

void BlockFrameRate::setFrameRate(double f)
{
    _freezeUI = true;
    frameRateEdit->setValue( f );
    _freezeUI = false;
}

void BlockFrameRate::on_frameRateBox_activated(const QString &arg1)
{
    if ( _freezeUI ) return;
    if (arg1 == "Custom") return;

    QString num = frameRateBox->currentText().replace(" fps","");
    _mediaInfo->setVideoFramerate( num.toDouble() );
}

void BlockFrameRate::on_frameRateEdit_valueChanged(double arg1)
{
    _mediaInfo->setVideoFramerate( arg1 );
}
