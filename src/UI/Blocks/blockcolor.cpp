#include "blockcolor.h"

BlockColor::BlockColor(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setType(Type::Video);
    _freezeUI = true;
    setupUi(this);

    //pouplate boxes
    FFmpeg *ffmpeg = FFmpeg::instance();
    foreach(FFColorItem *c, ffmpeg->colorPrimaries())
    {
        primariesBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFColorItem *c, ffmpeg->colorTRCs())
    {
        trcBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFColorItem *c, ffmpeg->colorSpaces())
    {
        spaceBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFColorItem *c, ffmpeg->colorRanges())
    {
        rangeBox->addItem( c->prettyName(), c->name() );
    }
    modeBox->addItem("Convert colors & Embed profile", "ConvertEmbed");
    modeBox->addItem("Convert colors only", "Convert");
    modeBox->addItem("Set Metadata only", "Embed");
    //create actions
    foreach(FFColorProfile *cp, ffmpeg->colorProfiles())
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
    if(_freezeUI) return;
    _mediaInfo->setColorConversionMode( modeBox->currentData().toString()  );
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
