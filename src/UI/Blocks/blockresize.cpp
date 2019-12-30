#include "blockresize.h"

BlockResize::BlockResize(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo, parent)
{
    setupUi(this);
}

void BlockResize::setActivated(bool activate)
{
    if (activate)
    {
        _mediaInfo->setVideoWidth( 0 );
        _mediaInfo->setVideoHeight( 0 );
    }
    else
    {
        _mediaInfo->setVideoWidth( videoWidthButton->value() );
        _mediaInfo->setVideoHeight( videoHeightButton->value() );
    }
}

void BlockResize::update()
{
    int w = _mediaInfo->videoWidth();
    int h = _mediaInfo->videoHeight();
    if (w > 0) videoWidthButton->setValue( w );
    if (h > 0) videoHeightButton->setValue( h );
    aspectRatio();
    checkSizes( );
}

void BlockResize::setWidth(int w)
{
    _freezeUI = true;
    videoWidthButton->setValue( w );
    aspectRatio();
    checkSizes( );
    _freezeUI = false;
}

void BlockResize::setHeight(int h)
{
    _freezeUI = true;
    videoHeightButton->setValue( h );
    aspectRatio();
    checkSizes( );
    _freezeUI = false;
}

void BlockResize::aspectRatio()
{
    double width = videoWidthButton->value();
    double height = videoHeightButton->value();
    double ratio =  width / height;
    //round it to 3 digits
    int roundedRatio = int(ratio*100+0.5);
    ratio = roundedRatio;
    ratio = ratio/100;
    aspectRatioLabel->setText(QString::number(ratio) + ":1");
}

void BlockResize::on_videoWidthButton_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoWidth( arg1 );
    checkSizes( );
}

void BlockResize::on_videoHeightButton_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoHeight( arg1 );
    checkSizes( );
}

void BlockResize::checkSizes( )
{
    int w = videoWidthButton->value();
    int h = videoHeightButton->value();
    if ( h % 2 != 0 || w % 2 != 0 )
    {
        FFCodec *c = _mediaInfo->videoCodec();
        if ( c == nullptr ) c = _mediaInfo->defaultVideoCodec();
        if ( c == nullptr ) return;
        if ( c->name() == "h264" )
        {
            emit status("h264 needs even numbers.");
        }
    }
    else
    {
        emit status( ""  );
    }
}
