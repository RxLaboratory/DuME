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
    if ( f != 0.0) frameRateEdit->setValue( f );
}

void BlockFrameRate::setFrameRate(double f)
{
    frameRateEdit->setValue( f );
}

void BlockFrameRate::on_frameRateBox_activated(const QString &arg1)
{
    if (arg1 != "Custom")
    {
        QString num = frameRateBox->currentText().replace(" fps","");
        frameRateEdit->setValue(num.toDouble());
    }
}

void BlockFrameRate::on_frameRateEdit_valueChanged(double arg1)
{
    //look for corresponding value
    for (int i = 1 ; i < frameRateBox->count() ; i++)
    {
        QString num = frameRateBox->itemText(i).replace(" fps","");
        if ( int( num.toDouble()*100 ) == int( arg1*100 ) )
        {
            frameRateBox->setCurrentIndex(i);
            return;
        }
    }
    frameRateBox->setCurrentIndex(0);

    _mediaInfo->setVideoFramerate( arg1 );
}
