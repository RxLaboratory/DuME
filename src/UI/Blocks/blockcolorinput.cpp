#include "blockcolorinput.h"

BlockColorInput::BlockColorInput(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setType(Type::Video);

    setupUi(this);

    //pouplate boxes
    FFmpeg *ffmpeg = FFmpeg::instance();
    foreach(FFBaseObject *c, ffmpeg->colorPrimaries())
    {
        primariesBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFBaseObject *c, ffmpeg->colorTRCs())
    {
        trcBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFBaseObject *c, ffmpeg->colorSpaces())
    {
        spaceBox->addItem( c->prettyName(), c->name() );
    }
    foreach(FFBaseObject *c, ffmpeg->colorRanges())
    {
        rangeBox->addItem( c->prettyName(), c->name() );
    }
    //create actions
    foreach(FFColorProfile *cp, ffmpeg->colorProfiles())
    {
        QAction *a = new QAction(cp->prettyName());
        a->setData(cp->name());
        connect(a, SIGNAL(triggered()), this, SLOT(presetTriggered()));
        _presets->addAction(a);
    }

    //connections
    connect(spaceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(spaceBox_currentIndexChanged(int)));
    connect(rangeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(rangeBox_currentIndexChanged(int)));
    connect(trcBox, SIGNAL(currentIndexChanged(int)), this, SLOT(trcBox_currentIndexChanged(int)));
    connect(primariesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(primariesBox_currentIndexChanged(int)));
}

void BlockColorInput::activate(bool activate)
{
    //Performance: Do not emit too many signals!
    QSignalBlocker b(_mediaInfo);

    if (activate)
    {
        _mediaInfo->setColorTRC( trcBox->currentData().toString() );
        _mediaInfo->setColorSpace( spaceBox->currentData().toString() );
        _mediaInfo->setColorRange( rangeBox->currentData().toString() );
        b.unblock();
        _mediaInfo->setColorPrimaries( primariesBox->currentData().toString() );
    }
    else
    {
        _mediaInfo->setColorTRC( "" );
        _mediaInfo->setColorSpace( "" );
        _mediaInfo->setColorRange( "" );
        b.unblock();
        _mediaInfo->setColorPrimaries( "" );
    }
}

void BlockColorInput::update()
{
    QSignalBlocker b1(rangeBox);
    QSignalBlocker b2(trcBox);
    QSignalBlocker b3(spaceBox);
    QSignalBlocker b4(primariesBox);

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

void BlockColorInput::spaceBox_currentIndexChanged(int index)
{
    _mediaInfo->setColorSpace( spaceBox->itemData(index).toString() );
}

void BlockColorInput::primariesBox_currentIndexChanged(int index)
{
    _mediaInfo->setColorPrimaries( primariesBox->itemData(index).toString() );
}

void BlockColorInput::trcBox_currentIndexChanged(int index)
{
    _mediaInfo->setColorTRC( trcBox->itemData(index).toString() );
}

void BlockColorInput::rangeBox_currentIndexChanged(int index)
{
    _mediaInfo->setColorRange( rangeBox->itemData(index).toString() );
}

void BlockColorInput::presetTriggered()
{
    QAction *a = static_cast<QAction *>(sender());
    _mediaInfo->setColorProfile( a->data().toString() );
}
