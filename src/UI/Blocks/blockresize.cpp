#include "blockresize.h"

BlockResize::BlockResize(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create resize block";
#endif
    setType(Type::Video);
    setupUi(this);

    _freezeUI = true;

    _presets->addAction( actionKeepRatio );
    _presets->addSeparator();
    _presets->addAction( actionAdaptRatio );
    _presets->addAction( actionHD720 );
    _presets->addAction( actionHD1080 );
    _presets->addAction( action2KFlat );
    _presets->addAction( action2KScope );
    _presets->addAction( actionDSM2K );
    _presets->addAction( action4KUHD );
    _presets->addAction( action4KFlat );
    _presets->addAction( action4KScope );
    _presets->addAction( actionDSM4K );
    _presets->addAction( action8KUHD );

    foreach(FFBaseObject *a, FFmpeg::instance()->resizeAlgorithms())
    {
        algorithmBox->addItem(a->prettyName(), a->name());
    }

    _freezeUI = false;
}

void BlockResize::activate(bool activate)
{
    if (!activate)
    {
        _mediaInfo->setWidth( 0 );
        _mediaInfo->setHeight( 0 );
    }
    else
    {
        _mediaInfo->setWidth( videoWidthButton->value() );
        _mediaInfo->setHeight( videoHeightButton->value() );
        _mediaInfo->setResizeMode( modeBox->currentText() );
        _mediaInfo->setResizeAlgorithm( algorithmBox->currentData( ).toString() );
    }
}

void BlockResize::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    int w = stream->width();
    int h = stream->height();
    videoWidthButton->setValue( w );
    videoHeightButton->setValue( h );
    if ( w == 0 ) videoWidthButton->setSuffix( " Same as input" );
    else videoWidthButton->setSuffix( " px" );
    if ( h == 0 ) videoHeightButton->setSuffix( " Same as input" );
    else videoHeightButton->setSuffix( " px" );

    modeBox->setCurrentText(MediaUtils::ResizeModeToString(stream->resizeMode()));

    algorithmBox->setCurrentData( stream->resizeAlgorithm()->name());

    checkSizes( );
}

void BlockResize::setWidth(int w)
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    videoWidthButton->setValue( w );
    checkSizes( );

    _freezeUI = freeze;
}

void BlockResize::setHeight(int h)
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    videoHeightButton->setValue( h );
    checkSizes( );

    _freezeUI = freeze;
}

QString BlockResize::aspectRatio()
{
    double width = videoWidthButton->value();
    double height = videoHeightButton->value();
    if ( height == 0.0 )
    {
        _currentRatio = -1;
        return "";
    }
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
    if ( actionKeepRatio->isChecked() && _currentRatio != -1 )
    {
        h = w / _currentRatio;
    }
    _mediaInfo->setWidth( w );
    _mediaInfo->setHeight( h );
}

void BlockResize::on_videoHeightButton_editingFinished()
{
    if (_freezeUI) return;
    double h = videoHeightButton->value();
    double w = videoWidthButton->value();
    if ( actionKeepRatio->isChecked() && _currentRatio != -1 )
    {
        w = h * _currentRatio;
    }
    _mediaInfo->setWidth( w );
    _mediaInfo->setHeight( h );
}

void BlockResize::on_modeBox_currentIndexChanged(const QString &arg1)
{
    if(_freezeUI) return;
    _mediaInfo->setResizeMode(arg1);
}

void BlockResize::on_algorithmBox_currentIndexChanged(int /*index*/)
{
    if(_freezeUI) return;
    _mediaInfo->setResizeAlgorithm( algorithmBox->currentData().toString());
}

void BlockResize::checkSizes( )
{
    int w = videoWidthButton->value();
    int h = videoHeightButton->value();
    emit status( aspectRatio() );

    if (( h % 2 != 0 || w % 2 != 0 ) && _mediaInfo->hasVideo() )
    {
        FFCodec *c = _mediaInfo->videoStreams()[0]->codec();
        if ( c->name() == "") c = _mediaInfo->defaultVideoCodec();
        if ( c->name() == "h264" )
        {
            emit status("h264 needs even numbers.");
        }
    }
}

void BlockResize::setSize( int w, int h)
{
    if ( actionAdaptRatio->isChecked() && _currentRatio != -1)
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

    _mediaInfo->setWidth( w );
    _mediaInfo->setHeight( h );
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

void BlockResize::on_action4KUHD_triggered()
{
    setSize( 3840, 2160 );
}

void BlockResize::on_action8KUHD_triggered()
{
    setSize( 7680, 4320 );
}


