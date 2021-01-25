#include "blockaudiocodec.h"

BlockAudioCodec::BlockAudioCodec(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Audio Codec block";
#endif
    setType(Type::Audio);
    _freezeUI = true;

    setupUi(this);

    connect( FFmpeg::instance(), SIGNAL(binaryChanged(QString)), this, SLOT(listCodecs()));

    listCodecs();

    _freezeUI = false;
}

void BlockAudioCodec::activate(bool activate)
{
    if ( activate && audioCodecsFilterBox->currentIndex() != 0)
    {
        _mediaInfo->setAudioCodec( audioCodecsBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setAudioCodec( "" );
    }
}

void BlockAudioCodec::update()
{  
    listCodecs();

    FFCodec *c = _mediaInfo->audioStreams()[0]->codec();
    setCodec( c->name() );
}

void BlockAudioCodec::setDefaultCodec()
{
    bool freeze  =_freezeUI;
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

    _freezeUI = freeze;
}

void BlockAudioCodec::setCodec(QString name, bool tryWithoutFilter )
{
    bool freeze  =_freezeUI;
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
            _freezeUI = freeze;
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

    _freezeUI = freeze;
}

void BlockAudioCodec::listCodecs()
{
    // If FFmpeg is not available/not loaded correctly
    // Only the "copy" codec is available, we can't do anything with this block
    if (FFmpeg::instance()->audioEncoders().count() < 2)
    {
        activate(false);
        return;
    }

    bool freeze  =_freezeUI;
    _freezeUI = true;

    QString prevSelection = audioCodecsBox->currentData(Qt::UserRole).toString();

    audioCodecsBox->clear();

    foreach (FFCodec *c, FFmpeg::instance()->audioEncoders() )
    {
        if ( audioCodecsFilterBox->currentIndex() <= 1 || (audioCodecsFilterBox->currentIndex() == 3 && c->isLossless()) || (audioCodecsFilterBox->currentIndex() == 2 && c->isLossy()))
        {
            audioCodecsBox->addItem( c->prettyName(), c->name() );
        }
    }

    //try to reselect it
    setCodec ( prevSelection, false );

    _freezeUI = freeze;
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
