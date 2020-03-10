#include "blockaudiocodec.h"

BlockAudioCodec::BlockAudioCodec(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Audio Codec block";
#endif
    _freezeUI = true;

    setupUi(this);

    connect( ffmpeg, SIGNAL(binaryChanged(QString)), this, SLOT(listCodecs()));

    listCodecs();

    _freezeUI = false;
}

void BlockAudioCodec::activate(bool activate)
{
    _freezeUI = true;

    if ( activate && audioCodecsFilterBox->currentIndex() != 0)
    {
        _mediaInfo->setAudioCodec( audioCodecsBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setAudioCodec( "" );
    }

    _freezeUI = false;
}

void BlockAudioCodec::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasAudio())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    AudioInfo *stream = _mediaInfo->audioStreams()[0];
    if (stream->isCopy())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    emit blockEnabled(true);

    listCodecs();
    _freezeUI = true;

    FFCodec *c = _mediaInfo->audioStreams()[0]->codec();
    setCodec( c->name() );

    _freezeUI = false;
}

void BlockAudioCodec::setDefaultCodec()
{
    _freezeUI = true;

    audioCodecsFilterBox->setCurrentIndex( 0 );
    listCodecs();

    FFCodec *c = _mediaInfo->defaultAudioCodec();
    if (c->name() != "")
    {
        setCodec( c->name(), false );
    }
    else
    {
        audioCodecsBox->setCurrentIndex( -1 );
    }

    audioCodecsBox->setEnabled( false );

    _freezeUI = false;
}

void BlockAudioCodec::setCodec(QString name, bool tryWithoutFilter )
{
    _freezeUI = true;

    if (name == "")
    {
        setDefaultCodec();
        return;
    }

    for (int i = 0; i < audioCodecsBox->count(); i++)
    {
        if ( audioCodecsBox->itemData(i, Qt::UserRole).toString() == name)
        {
            audioCodecsBox->setCurrentIndex( i );
            _freezeUI = false;
            return;
        }
    }

    //try without filter
    if ( tryWithoutFilter )
    {
        audioCodecsFilterBox->setCurrentIndex( 1 );
        listCodecs();
        setCodec( name, false );
    }
    else // set none
    {
        audioCodecsBox->setCurrentIndex( -1 );
    }

    _freezeUI = false;
}

void BlockAudioCodec::listCodecs()
{
    // If FFmpeg is not available/not loaded correctly
    // Only the "copy" codec is available, we can't do anything with this block
    if (ffmpeg->audioEncoders().count() < 2)
    {
        activate(false);
        return;
    }

    _freezeUI = true;
    QString prevSelection = audioCodecsBox->currentData(Qt::UserRole).toString();

    audioCodecsBox->clear();

    foreach (FFCodec *c, ffmpeg->audioEncoders() )
    {
        if ( audioCodecsFilterBox->currentIndex() <= 1 || (audioCodecsFilterBox->currentIndex() == 3 && c->isLossless()) || (audioCodecsFilterBox->currentIndex() == 2 && c->isLossy()))
        {
            audioCodecsBox->addItem( c->prettyName(), c->name() );
        }
    }

    //try to reselect it
    setCodec ( prevSelection, false );

    _freezeUI = false;
}

void BlockAudioCodec::on_audioCodecsBox_currentIndexChanged(int index)
{
    if (_freezeUI ) return;
    _mediaInfo->setAudioCodec( audioCodecsBox->itemData(index, Qt::UserRole).toString() );
}

void BlockAudioCodec::on_audioCodecsFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI ) return;
    if (index == 0)
    {
        _mediaInfo->setAudioCodec( "" );
    }
    else
    {
        listCodecs();
        audioCodecsBox->setEnabled( true );
    }
}
