#include "blockaudiobitrate.h"

BlockAudioBitrate::BlockAudioBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);

    _presets->addAction( actionAuto );
    _presets->addAction( action128 );
    _presets->addAction( action196 );
    _presets->addAction( action256 );
    _presets->addAction( action320 );
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
    audioBitRateEdit->setValue( MediaUtils::convertFromBps( _mediaInfo->audioBitrate(), MediaUtils::kbps ) );
}

void BlockAudioBitrate::on_audioBitRateEdit_valueChanged(int arg1)
{
    if (audioBitRateEdit->value() == 0) audioBitRateEdit->setSuffix( " Auto" );
    else audioBitRateEdit->setSuffix( " kbps" );
}

void BlockAudioBitrate::on_audioBitRateEdit_editingFinished()
{
    if (_freezeUI) return;
    _mediaInfo->setAudioBitrate( MediaUtils::convertToBps( audioBitRateEdit->value(), MediaUtils::kbps) );
}

void BlockAudioBitrate::on_action128_triggered()
{
    _mediaInfo->setAudioBitrate( MediaUtils::convertToBps( 128, MediaUtils::kbps ) );
}

void BlockAudioBitrate::on_action196_triggered()
{
    _mediaInfo->setAudioBitrate( MediaUtils::convertToBps(196, MediaUtils::kbps) );
}

void BlockAudioBitrate::on_action256_triggered()
{
    _mediaInfo->setAudioBitrate( MediaUtils::convertToBps(256, MediaUtils::kbps) );
}

void BlockAudioBitrate::on_action320_triggered()
{
    _mediaInfo->setAudioBitrate( MediaUtils::convertToBps(320, MediaUtils::kbps) );
}

void BlockAudioBitrate::on_actionAuto_triggered()
{
    _mediaInfo->setAudioBitrate( 0 );
}
