#include "blockvideocodec.h"

BlockVideoCodec::BlockVideoCodec(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setType(Type::Video);

    _freezeUI = true;

    setupUi(this);

    connect( FFmpeg::instance(),SIGNAL(binaryChanged(QString)),this,SLOT(listCodecs()) );

    listCodecs();

    videoCodecsBox->setEnabled(videoCodecsFilterBox->currentIndex() != 0);

    _freezeUI = false;
}

void BlockVideoCodec::listCodecs()
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    QString prevSelection = videoCodecsBox->currentData(Qt::UserRole).toString();
    videoCodecsBox->clear();

    int videoFilter = videoCodecsFilterBox->currentIndex();

    foreach(FFCodec *encoder, FFmpeg::instance()->videoEncoders())
    {
        if (encoder->name() == "copy") continue;
        if (videoFilter <= 1 || (videoFilter == 2 && encoder->isLossy()) || (videoFilter == 3 && encoder->isLossless()) || (videoFilter == 4 && encoder->isIframe()))
        {
            videoCodecsBox->addItem( encoder->prettyName(), encoder->name() );
        }
    }

    //try to reselect it
    setCodec( prevSelection, false );

    _freezeUI = freeze;
}

void BlockVideoCodec::setCodec(QString name, bool tryWithoutFilter)
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    videoCodecsBox->setCurrentData(name);
    if (videoCodecsBox->currentIndex() >= 0)
    {
        _freezeUI = freeze;
        return;
    }

    //try without filter
    if ( tryWithoutFilter )
    {
        videoCodecsFilterBox->setCurrentIndex( 1 );
        listCodecs();
        setCodec( name, false );
    }
    else
    {
        videoCodecsBox->setCurrentIndex( -1 );
    }

    _freezeUI = freeze;
}

void BlockVideoCodec::setDefaultCodec()
{
    bool freeze = _freezeUI;
    _freezeUI = true;

    videoCodecsFilterBox->setCurrentIndex( 1 );
    listCodecs();

    FFCodec *videoCodec = _mediaInfo->defaultVideoCodec();

    //Select Default Codec

    if ( videoCodec->name() != "" )
    {
        setCodec( videoCodec->name(), false );
    }
    else
    {
        videoCodecsBox->setCurrentIndex( -1 );
    }

    videoCodecsBox->setEnabled( false );
    videoCodecsFilterBox->setCurrentIndex( 0 );

    _freezeUI = freeze;
}

void BlockVideoCodec::activate(bool activate)
{
    if ( activate && videoCodecsFilterBox->currentIndex() != 0 )
    {
        _mediaInfo->setVideoCodec( videoCodecsBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setVideoCodec( "" );
    }
}

void BlockVideoCodec::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];

    // set codec
    FFCodec *c = stream->codec();
    if ( c->name() == "" ) setDefaultCodec();
    else setCodec( c->name() );
}

void BlockVideoCodec::on_videoCodecsFilterBox_currentIndexChanged(int index)
{
    videoCodecsBox->setEnabled(index != 0);
    if (_freezeUI ) return;
    if (index == 0)
    {
        _mediaInfo->setVideoCodec( "" );
    }
    else
    {
        listCodecs();
    }
}

void BlockVideoCodec::on_videoCodecsBox_currentIndexChanged(int index)
{
    if ( _freezeUI ) return;
    _mediaInfo->setVideoCodec( videoCodecsBox->itemData(index, Qt::UserRole).toString() );
}
