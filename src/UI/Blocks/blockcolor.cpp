#include "blockcolor.h"

BlockColor::BlockColor(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Color block";
#endif
    setType(Type::Video);
    _freezeUI = true;
    setupUi(this);

    //pouplate boxes
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

    _freezeUI = false;
}

void BlockColor::activate(bool activate)
{
    _freezeUI = true;

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

    _freezeUI = false;
}

void BlockColor::update()
{
    qDebug() << "Update Color Block";
    if (_freezeUI) return;
    bool freeze = _freezeUI;
    _freezeUI = true;

    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    trcBox->setCurrentData( stream->colorTRC()->name());
    spaceBox->setCurrentData( stream->colorSpace()->name());
    rangeBox->setCurrentData( stream->colorRange()->name());
    primariesBox->setCurrentData( stream->colorPrimaries()->name());

    _freezeUI = freeze;
    qDebug() << "Color block updated";
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

void BlockColor::on_actionsRGB_triggered()
{
    _mediaInfo->setColorPrimaries("bt709");
    _mediaInfo->setColorTRC("iec61966_2_1");
    _mediaInfo->setColorSpace("rgb");
    _mediaInfo->setColorRange("pc");
}

void BlockColor::on_actionBT709_triggered()
{
    _mediaInfo->setColorPrimaries("bt709");
    _mediaInfo->setColorTRC("bt709");
    _mediaInfo->setColorSpace("bt709");
    _mediaInfo->setColorRange("tv");
}

void BlockColor::on_actionUHD10_triggered()
{
    _mediaInfo->setColorPrimaries("bt2020");
    _mediaInfo->setColorTRC("bt2020_10");
    _mediaInfo->setColorSpace("bt2020_cl");
    _mediaInfo->setColorRange("pc");
}

void BlockColor::on_actionUHD12_triggered()
{
    _mediaInfo->setColorPrimaries("bt2020");
    _mediaInfo->setColorTRC("bt2020_12");
    _mediaInfo->setColorSpace("bt2020_cl");
    _mediaInfo->setColorRange("pc");
}
