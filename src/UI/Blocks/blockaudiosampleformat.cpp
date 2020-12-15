#include "UI/Blocks/blockaudiosampleformat.h"

BlockAudioSampleFormat::BlockAudioSampleFormat(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Audio Sample Format block";
#endif
    setupUi(this);

    _freezeUI = true;
    //populate sampling box
    foreach (FFSampleFormat *f, ffmpeg->sampleFormats() )
    {
        samplingBox->addItem(f->prettyName(), f->name());
    }
    _freezeUI = false;
}

void BlockAudioSampleFormat::activate(bool blockEnabled)
{
    _freezeUI = true;

    if (blockEnabled)
    {
        _mediaInfo->setAudioSampleFormat( samplingBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setAudioSampleFormat( "" );
    }

    _freezeUI = false;
}

void BlockAudioSampleFormat::update()
{
    qDebug() << "Update Audio Sample Format Block";
    if (_freezeUI) return;

    if (!_mediaInfo->hasAudio())
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

    if (stream->sampleFormat()->name() != "")
    {
        samplingBox->setCurrentData(stream->sampleFormat()->name());
    }
    else
    {
        samplingBox->setCurrentData("s16p");
    }

    _freezeUI = false;
}

void BlockAudioSampleFormat::on_samplingBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setAudioSampleFormat( samplingBox->itemData(index, Qt::UserRole).toString() );
}
