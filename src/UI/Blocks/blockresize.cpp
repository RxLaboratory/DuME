#include "blockresize.h"

BlockResize::BlockResize(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo, parent)
{
    setupUi(this);

    _presets->addAction( actionKeepRatio );
    _presets->addSeparator();
    _presets->addAction( actionHD720 );
    _presets->addAction( actionHD1080 );
    _presets->addAction( action2KFlat );
    _presets->addAction( action2KScope );
    _presets->addAction( actionDSM2K );
    _presets->addAction( action4KFlat );
    _presets->addAction( action4KScope );
    _presets->addAction( actionDSM4K );
}

void BlockResize::setActivated(bool activate)
{
    if (!activate)
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
    videoWidthButton->setValue( w );
    videoHeightButton->setValue( h );
    checkSizes( );
}

void BlockResize::setWidth(int w)
{
    _freezeUI = true;
    videoWidthButton->setValue( w );
    checkSizes( );
    _freezeUI = false;
}

void BlockResize::setHeight(int h)
{
    _freezeUI = true;
    videoHeightButton->setValue( h );
    checkSizes( );
    _freezeUI = false;
}

QString BlockResize::aspectRatio()
{
    double width = videoWidthButton->value();
    double height = videoHeightButton->value();
    _currentRatio =  width / height;
    //round it to 3 digits
    int roundedRatio = int(_currentRatio*100+0.5);
    float ratio = roundedRatio;
    ratio = ratio/100;
    return QString::number(ratio) + ":1";
}

void BlockResize::on_videoWidthButton_editingFinished( )
{
    if (_freezeUI) return;
    double w = videoWidthButton->value();
    double h = videoHeightButton->value();
    if ( actionKeepRatio->isChecked() )
    {
        h = w / _currentRatio;
    }
    _mediaInfo->setVideoWidth( w );
    _mediaInfo->setVideoHeight( h );
}

void BlockResize::on_videoHeightButton_editingFinished()
{
    if (_freezeUI) return;
    double h = videoHeightButton->value();
    double w = videoWidthButton->value();
    if ( actionKeepRatio->isChecked() )
    {
        w = h * _currentRatio;
    }
    _mediaInfo->setVideoWidth( w );
    _mediaInfo->setVideoHeight( h );
}

void BlockResize::checkSizes( )
{
    int w = videoWidthButton->value();
    int h = videoHeightButton->value();
    emit status( aspectRatio() );

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
}

void BlockResize::setSize( int w, int h)
{
    if ( actionKeepRatio->isChecked() )
    {
        double wi = w;
        double he = h;
        double ratio = wi / he;

        if ( _currentRatio > ratio )
        {
            h = wi / _currentRatio ;
        }
        else
        {
            w = he * _currentRatio ;
        }
    }

    _mediaInfo->setVideoWidth( w );
    _mediaInfo->setVideoHeight( h );
}

void BlockResize::on_actionHD720_triggered()
{
    setSize( 1280, 720 );
}

void BlockResize::on_actionHD1080_triggered()
{
    setSize( 1920, 1080 );
}

void BlockResize::on_action2KFlat_triggered()
{
    setSize( 1998, 1080 );
}

void BlockResize::on_action2KScope_triggered()
{
    setSize( 2048, 858 );
}

void BlockResize::on_actionDSM2K_triggered()
{
    setSize( 2048, 1080 );
}

void BlockResize::on_action4KFlat_triggered()
{
    setSize( 3996, 2160 );
}

void BlockResize::on_action4KScope_triggered()
{
    setSize( 4096, 1716 );
}

void BlockResize::on_actionDSM4K_triggered()
{
    setSize( 4096, 2160 );
}



