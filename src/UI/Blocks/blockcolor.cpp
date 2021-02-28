#include "blockcolor.h"

BlockColor::BlockColor(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setType(Type::Video);
    _freezeUI = true;
    setupUi(this);

    modeBox->addItem("Convert colors & Embed profile", "ConvertEmbed");
    modeBox->addItem("Convert colors only", "Convert");
    modeBox->addItem("Set Metadata only", "Embed");

    //pouplate boxes
    populateItems();

    //create actions
    foreach(FFColorProfile *cp, FFmpeg::instance()->colorProfiles())
    {
        QAction *a = new QAction(cp->prettyName());
        a->setData(cp->name());
        connect(a, SIGNAL(triggered()), this, SLOT(presetTriggered()));
        _presets->addAction(a);
    }

    _freezeUI = false;
}

void BlockColor::activate(bool activate)
{
    if (activate)
    {
        _mediaInfo->setColorTRC( trcBox->currentData().toString() );
        _mediaInfo->setColorSpace( spaceBox->currentData().toString() );
        _mediaInfo->setColorRange( rangeBox->currentData().toString() );
        _mediaInfo->setColorPrimaries( primariesBox->currentData().toString() );
    }
    else
    {
        _mediaInfo->setColorTRC( "" );
        _mediaInfo->setColorSpace( "" );
        _mediaInfo->setColorRange( "" );
        _mediaInfo->setColorPrimaries( "" );
    }
}

void BlockColor::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    trcBox->setCurrentData( stream->colorTRC()->name());
    spaceBox->setCurrentData( stream->colorSpace()->name());
    primariesBox->setCurrentData( stream->colorPrimaries()->name());
    if (stream->pixFormat()->colorSpace() == FFPixFormat::YUV)
    {
        rangeBox->show();
        rangeLabel->show();
        rangeBox->setCurrentData( stream->colorRange()->name());
    }
    else
    {
        rangeBox->setCurrentIndex(0);
        rangeBox->hide();
        rangeLabel->hide();
    }

}

void BlockColor::on_modeBox_currentIndexChanged(int /*index*/)
{
    populateItems();
    if(_freezeUI) return;
    _mediaInfo->setColorConversionMode( modeBox->currentData().toString()  );
}

void BlockColor::populateItems()
{
    FFmpeg *ffmpeg = FFmpeg::instance();

    bool convert = modeBox->currentIndex() == 0 || modeBox->currentIndex() == 1;

    QSignalBlocker b(primariesBox);
    QSignalBlocker b1(trcBox);
    QSignalBlocker b2(spaceBox);
    QSignalBlocker b3(rangeBox);

    primariesBox->clear();
    trcBox->clear();
    spaceBox->clear();
    rangeBox->clear();

    foreach(FFColorItem *c, ffmpeg->colorPrimaries())
    {
        if (!convert || c->isOutput()) primariesBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFColorItem *c, ffmpeg->colorTRCs())
    {
        if (!convert || c->isOutput()) trcBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFColorItem *c, ffmpeg->colorSpaces())
    {
        if (!convert || c->isOutput()) spaceBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFColorItem *c, ffmpeg->colorRanges())
    {
        if (!convert || c->isOutput()) rangeBox->addItem( c->prettyName(), c->name() );
    }
}

void BlockColor::on_spaceBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _mediaInfo->setColorSpace( spaceBox->itemData(index).toString() );
}

void BlockColor::on_primariesBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _mediaInfo->setColorPrimaries( primariesBox->itemData(index).toString() );
}

void BlockColor::on_trcBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _mediaInfo->setColorTRC( trcBox->itemData(index).toString() );
}

void BlockColor::on_rangeBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _mediaInfo->setColorRange( rangeBox->itemData(index).toString() );
}

void BlockColor::presetTriggered()
{
    QAction *a = static_cast<QAction *>(sender());
    _mediaInfo->setColorProfile( a->data().toString() );
}
