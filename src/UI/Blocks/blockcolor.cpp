#include "blockcolor.h"

BlockColor::BlockColor(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    _freezeUI = true;
    setupUi(this);

    //pouplate boxes
    primariesBox->addItem("Auto","");
    primariesBox->addItem("film","Film");
    primariesBox->addItem("BT.709 / RGB","bt709");
    primariesBox->addItem("BT.470 M","bt470m");
    primariesBox->addItem("BT.470 BG","bt470bg");
    primariesBox->addItem("BT.2020","bt2020");
    primariesBox->addItem("SMPTE 170 M","smpte170m");
    primariesBox->addItem("SMPTE 240 M","smpte240m");
    primariesBox->addItem("SMPTE 428","smpte428");
    primariesBox->addItem("SMPTE 428-1","smpte428_1");
    primariesBox->addItem("SMPTE 431","smpte431");
    primariesBox->addItem("SMPTE 432","smpte432");
    primariesBox->addItem("JEDEC P22","jedec-p22");

    trcBox->addItem("Auto","");
    trcBox->addItem("sRGB","iec61966_2_1");
    trcBox->addItem("Extended-gamut YCC (xvYCC)","iec61966_2_4");
    trcBox->addItem("Linear","linear");
    trcBox->addItem("Log","log100");
    trcBox->addItem("Log square root","log_sqrt");
    trcBox->addItem("BT.709","bt709");
    trcBox->addItem("BT.470 M","gamma22");
    trcBox->addItem("BT.470 BG","gamma28");
    trcBox->addItem("BT.1361","bt1361");
    trcBox->addItem("BT.1361 E","bt1361e");
    trcBox->addItem("BT.2020 - 10 bit","bt2020_10");
    trcBox->addItem("BT.2020 - 12 bit","bt2020_12");
    trcBox->addItem("SMPTE 170 M","smpte170m");
    trcBox->addItem("SMPTE 240 M","smpte240m");
    trcBox->addItem("SMPTE 428","smpte428");
    trcBox->addItem("SMPTE 428-1","smpte428_1");
    trcBox->addItem("SMPTE 2084","smpte2084");

    spaceBox->addItem("Auto", "");
    spaceBox->addItem("RGB","rgb");
    spaceBox->addItem("BT.709","bt709");
    spaceBox->addItem("BT.470 BG","bt470bg");
    spaceBox->addItem("BT.2020 NCL","bt2020_ncl");
    spaceBox->addItem("BT.2020 CL","bt2020_cl");
    spaceBox->addItem("SMPTE 2085","smpte2085");
    spaceBox->addItem("SMPTE 170 M","smpte170m");
    spaceBox->addItem("SMPTE 240 M","smpte240m");
    spaceBox->addItem("YCGCO","ycgco");
    spaceBox->addItem("YCOCG","ycocg");
    spaceBox->addItem("FCC","fcc");

    rangeBox->addItem("Auto","");
    rangeBox->addItem("Limited","tv");
    rangeBox->addItem("Full","pc");

    _presets->addAction( actionsRGB );
    _presets->addAction( actionBT709 );
    _presets->addAction( actionUHD10 );
    _presets->addAction( actionUHD12 );

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
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo() || _mediaInfo->copyVideo())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    emit blockEnabled(true);

    for (int i = 0; i < trcBox->count(); i++)
    {
        if (trcBox->itemData(i) == _mediaInfo->colorTRC())
        {
            trcBox->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < spaceBox->count(); i++)
    {
        if (spaceBox->itemData(i) == _mediaInfo->colorSpace())
        {
            spaceBox->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < rangeBox->count(); i++)
    {
        if (rangeBox->itemData(i) == _mediaInfo->colorRange())
        {
            rangeBox->setCurrentIndex(i);
            break;
        }
    }
    for (int i = 0; i < primariesBox->count(); i++)
    {
        if (primariesBox->itemData(i) == _mediaInfo->colorPrimaries())
        {
            primariesBox->setCurrentIndex(i);
            break;
        }
    }
    _freezeUI = false;
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
