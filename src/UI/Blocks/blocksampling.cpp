#include "blocksampling.h"

BlockSampling::BlockSampling(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    _freezeUI = true;
    setupUi(this);

    //populate sampling box
    //TODO Get from ffmpeg
    samplingBox->addItem("8,000 Hz", 8000);
    samplingBox->addItem("11,025 Hz", 11025);
    samplingBox->addItem("16,000 Hz", 16000);
    samplingBox->addItem("22,050 Hz", 22050);
    samplingBox->addItem("32,000 Hz", 32000);
    samplingBox->addItem("44,100 Hz", 44100);
    samplingBox->addItem("48,000 Hz", 48000);
    samplingBox->addItem("88,200 Hz", 88200);
    samplingBox->addItem("96,000 Hz", 96000);
    samplingBox->setCurrentIndex(6);

    _freezeUI = false;
}

void BlockSampling::setSampling(int s)
{
    _freezeUI = true;

    for (int i = 0; i < samplingBox->count(); i++)
    {
        if (samplingBox->itemData(i, Qt::UserRole).toInt() == s)
        {
            samplingBox->setCurrentIndex(i);
            break;
        }
    }

    _freezeUI = false;
}

void BlockSampling::setActivated(bool activate)
{
    if (activate)
    {
        _mediaInfo->setAudioSamplingRate( samplingBox->currentData(Qt::UserRole).toInt() );
    }
    else
    {
        _mediaInfo->setAudioSamplingRate( 0 );
    }
}

void BlockSampling::update()
{
    _freezeUI = true;

    for (int i = 0; i < samplingBox->count(); i++)
    {
        if (samplingBox->itemData(i, Qt::UserRole).toInt() == _mediaInfo->audioSamplingRate())
        {
            samplingBox->setCurrentIndex(i);
            return;
        }
    }
    _freezeUI = false;
}

void BlockSampling::on_samplingBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setAudioSamplingRate( samplingBox->itemData(index, Qt::UserRole).toInt() );
}
