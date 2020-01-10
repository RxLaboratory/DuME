#include "blockaudiocodec.h"

BlockAudioCodec::BlockAudioCodec(FFmpeg *ffmpeg, MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    _freezeUI = true;

    setupUi(this);

    _ffmpeg = ffmpeg;
    connect( _ffmpeg, SIGNAL(binaryChanged(QString)), this, SLOT(listCodecs()));

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
        _mediaInfo->setAudioCodec( nullptr );
    }

    _freezeUI = false;
}

void BlockAudioCodec::update()
{
    listCodecs();
    _freezeUI = true;

    FFCodec *c = _mediaInfo->audioCodec();
    if ( c == nullptr )
    {
        setDefaultCodec();
    }
    else
    {
        setCodec( c->name() );
    }

    _freezeUI = false;
}

void BlockAudioCodec::setDefaultCodec()
{
    _freezeUI = true;

    audioCodecsFilterBox->setCurrentIndex( 0 );
    listCodecs();

    FFCodec *c = _mediaInfo->defaultAudioCodec();
    if (c != nullptr)
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
    _freezeUI = true;
    QString prevSelection = audioCodecsBox->currentData(Qt::UserRole).toString();

    audioCodecsBox->clear();

    foreach (FFCodec *c, _ffmpeg->audioEncoders() )
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
        _mediaInfo->setAudioCodec( nullptr );
    }
    else
    {
        listCodecs();
        audioCodecsBox->setEnabled( true );
    }
}
