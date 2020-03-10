#include "blockvideocodec.h"

BlockVideoCodec::BlockVideoCodec(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create video codec block";
#endif

    _freezeUI = true;

    setupUi(this);

    connect( ffmpeg,SIGNAL(binaryChanged(QString)),this,SLOT(listCodecs()) );

    listCodecs();

    _freezeUI = false;
}

void BlockVideoCodec::listCodecs()
{
    _freezeUI = true;
    QString prevSelection = videoCodecsBox->currentData(Qt::UserRole).toString();
    videoCodecsBox->clear();

    int videoFilter = videoCodecsFilterBox->currentIndex();

    foreach(FFCodec *encoder, ffmpeg->videoEncoders())
    {
        if (encoder->name() == "copy") continue;
        if (videoFilter <= 1 || (videoFilter == 2 && encoder->isLossy()) || (videoFilter == 3 && encoder->isLossless()) || (videoFilter == 4 && encoder->isIframe()))
        {
            videoCodecsBox->addItem( encoder->prettyName(), encoder->name() );
        }
    }

    //try to reselect it
    setCodec( prevSelection, false );

    _freezeUI = false;
}

void BlockVideoCodec::setCodec(QString name, bool tryWithoutFilter)
{
    _freezeUI = true;

    if (videoCodecsFilterBox->currentIndex() == 0) videoCodecsFilterBox->setCurrentIndex(1);

    for (int v = 0; v < videoCodecsBox->count() ; v++)
    {
        if (videoCodecsBox->itemData(v, Qt::UserRole).toString() == name)
        {
            videoCodecsBox->setCurrentIndex(v);
            _freezeUI = false;
            return;
        }
    }

    //try without filter
    if ( tryWithoutFilter )
    {
        videoCodecsFilterBox->setCurrentIndex( 1 );
        listCodecs();
        setCodec( name, false );
    }
    else // set default
    {
        videoCodecsBox->setCurrentIndex( -1 );
    }

    _freezeUI = false;
}

void BlockVideoCodec::setDefaultCodec()
{
    _freezeUI = true;

    videoCodecsFilterBox->setCurrentIndex( 1 );
    listCodecs();

    _freezeUI = true;

    FFCodec *videoCodec = _mediaInfo->defaultVideoCodec();

    //Select Default Codec

    if ( videoCodec->name() != "" )
    {
        setCodec( videoCodec->name(), false );
        _freezeUI = true;
    }
    else
    {
        videoCodecsBox->setCurrentIndex( -1 );
    }

    videoCodecsBox->setEnabled( false );
    videoCodecsFilterBox->setCurrentIndex( 0 );

    _freezeUI = false;
}

void BlockVideoCodec::activate(bool activate)
{
    _freezeUI = true;

    if ( activate && videoCodecsFilterBox->currentIndex() != 0 )
    {
        _mediaInfo->setVideoCodec( videoCodecsBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setVideoCodec( "" );
    }

    _freezeUI = false;
}

void BlockVideoCodec::update()
{
    qDebug() << "Update Video Codec Block";
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo() || _mediaInfo->isSequence())
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

    // set codec
    FFCodec *c = stream->codec();
    if ( c->name() == "" ) setDefaultCodec();
    else setCodec( c->name() );


    emit blockEnabled(true);

    _freezeUI = false;
    qDebug() << "Video Codec Block updated";
}

void BlockVideoCodec::on_videoCodecsFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI ) return;
    if (index == 0)
    {
        _mediaInfo->setVideoCodec( "" );
    }
    else
    {
        listCodecs();
        videoCodecsBox->setEnabled( true );
    }
}

void BlockVideoCodec::on_videoCodecsBox_currentIndexChanged(int index)
{
    if ( _freezeUI ) return;
    qDebug() << "Video Codec Block changed codec to " + videoCodecsBox->currentText();
    _mediaInfo->setVideoCodec( videoCodecsBox->itemData(index, Qt::UserRole).toString() );
}
