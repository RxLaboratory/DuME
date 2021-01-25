#include "UI/Blocks/blockaudiosampleformat.h"

BlockAudioSampleFormat::BlockAudioSampleFormat(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Audio Sample Format block";
#endif
    setType(Type::Audio);
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
    if (blockEnabled)
    {
        _mediaInfo->setAudioSampleFormat( samplingBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setAudioSampleFormat( "" );
    }
}

void BlockAudioSampleFormat::update()
{
    AudioInfo *stream = _mediaInfo->audioStreams()[0];

    if (stream->sampleFormat()->name() != "")
    {
        samplingBox->setCurrentData(stream->sampleFormat()->name());
    }
    else
    {
        samplingBox->setCurrentData("s16p");
    }
}

void BlockAudioSampleFormat::on_samplingBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setAudioSampleFormat( samplingBox->itemData(index, Qt::UserRole).toString() );
}
