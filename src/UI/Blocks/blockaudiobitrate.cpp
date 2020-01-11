#include "blockaudiobitrate.h"

BlockAudioBitrate::BlockAudioBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);

    _presets->addAction( actionAuto );
    _presets->addAction( action128 );
    _presets->addAction( action196 );
    _presets->addAction( action256 );
    _presets->addAction( action320 );
}

void BlockAudioBitrate::activate(bool activate)
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
    if (_freezeUI) return;
     _freezeUI = true;

    if (!_mediaInfo->hasAudio() )
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

    audioBitRateEdit->setValue( MediaUtils::convertFromBps( stream->bitrate(), MediaUtils::kbps ) );

    _freezeUI = false;
}

void BlockAudioBitrate::on_audioBitRateEdit_valueChanged(int arg1)
{
    if (arg1 == 0) audioBitRateEdit->setSuffix( " Auto" );
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
