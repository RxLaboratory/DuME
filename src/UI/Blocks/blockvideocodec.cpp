#include "blockvideocodec.h"

BlockVideoCodec::BlockVideoCodec(FFmpeg *ffmpeg, MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo, parent)
{
    setupUi(this);

    _ffmpeg = ffmpeg;
    connect( _ffmpeg,SIGNAL(binaryChanged(QString)),this,SLOT(listCodecs()) );
    listCodecs();
}

void BlockVideoCodec::listCodecs()
{
    _freezeUI = true;

    QString prevSelection = videoCodecsBox->currentData(Qt::UserRole).toString();
    videoCodecsBox->clear();

    QList<FFCodec *> encoders = _ffmpeg->videoEncoders();
    if (encoders.count() == 0)
    {
        _freezeUI = false;
        return;
    }

    int videoFilter = videoCodecsFilterBox->currentIndex();

    foreach(FFCodec *encoder,encoders)
    {
        if (encoder->name() == "copy") continue;
        if (videoFilter <= 0 || (videoFilter == 1 && encoder->isLossy()) || (videoFilter == 2 && encoder->isLossless()) || (videoFilter == 3 && encoder->isIframe()))
        {
            videoCodecsBox->addItem( encoder->prettyName(),QVariant(encoder->name()) );
        }
    }

    setCodec( prevSelection );

    _freezeUI = false;
}

void BlockVideoCodec::setCodec(QString name)
{
    _freezeUI = true;

    if ( name == "" ) selectDefaultVideoCodec();

    for (int v = 0; v < videoCodecsBox->count() ; v++)
    {
        if (videoCodecsBox->itemData(v).toString() == name)
        {
            videoCodecsBox->setCurrentIndex(v);
            return;
        }
    }

    //Try again without filters
    videoCodecsFilterBox->setCurrentIndex(0);
    listCodecs();
    for (int v = 0; v < videoCodecsBox->count() ; v++)
    {
        if (videoCodecsBox->itemData(v).toString() == name)
        {
            videoCodecsBox->setCurrentIndex(v);
            return;
        }
    }

    _freezeUI = false;
}

void BlockVideoCodec::selectDefaultVideoCodec()
{
    _freezeUI = true;

    FFMuxer *_currentMuxer = _mediaInfo->muxer();

    if ( _currentMuxer == nullptr ) return;

    FFCodec *videoCodec = _ffmpeg->muxerDefaultCodec(_currentMuxer, FFCodec::Video);

    //Select Default Codec

    if ( videoCodec != nullptr )
    {
        setCodec( videoCodec->name() );
    }

    _freezeUI = false;
}

void BlockVideoCodec::setActivated(bool activate)
{
    if ( activate )
    {
        _mediaInfo->setVideoCodec( videoCodecsBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setVideoCodec( nullptr );
    }
}

void BlockVideoCodec::update()
{
    listCodecs();

    // set codec
    FFCodec *c = _mediaInfo->videoCodec();
    if (c == nullptr)
    {
        selectDefaultVideoCodec();
        return;
    }
    else
    {
        setCodec( c->name() );
    }
}

void BlockVideoCodec::on_videoCodecsFilterBox_currentIndexChanged(int index)
{
    if ( _freezeUI ) return;
    listCodecs();
}

void BlockVideoCodec::on_videoCodecsBox_currentIndexChanged(int index)
{
    if ( _freezeUI ) return;
    _mediaInfo->setVideoCodec( videoCodecsBox->itemData(index, Qt::UserRole).toString() );
}
