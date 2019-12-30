#include "blockaudiobitrate.h"

BlockAudioBitrate::BlockAudioBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
}

void BlockAudioBitrate::setActivated(bool activate)
{
    _freezeUI = true;

    if (activate)
    {
        _mediaInfo->setAudioBitrate( MediaUtils::convertToBps( audioBitRateEdit->value(), MediaUtils::kbps ));
    }
    else
    {
        _mediaInfo->setAudioBitrate( 0 );
    }

    _freezeUI = false;
}

void BlockAudioBitrate::update()
{
    audioBitRateEdit->setValue( _mediaInfo->audioBitrate() );
}

void BlockAudioBitrate::on_audioBitRateEdit_valueChanged(int arg1)
{
    if (arg1 == 0) audioBitRateEdit->setSuffix( " Auto" );
    else audioBitRateEdit->setSuffix( " kbps" );
    if (_freezeUI) return;
    _mediaInfo->setAudioBitrate( arg1 );
}
