#include "inputwidget.h"

InputWidget::InputWidget(FFmpeg *ff, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;
    mediaInfo = new FFMediaInfo("",this);
}

FFMediaInfo *InputWidget::getMediaInfo()
{
    return mediaInfo;
}

void InputWidget::on_inputBrowseButton_clicked()
{
    //get current file path
    QFileInfo fi(inputEdit->text());
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",fi.path());
    if (inputPath == "") return;

    //update UI
    inputEdit->setText(inputPath);

    mediaInfo->updateInfo(ffmpeg->getMediaInfoString(inputPath));

    //Text
    QString mediaInfoString = "Media information";

    mediaInfoString += "\n\nContainers: " + mediaInfo->getContainer().join(",");
    QTime duration(0,0,0);
    duration = duration.addSecs(mediaInfo->getDuration());
    mediaInfoString += "\nDuration: " + duration.toString("hh:mm:ss.zzz");
    double size = mediaInfo->getSize(FFMediaInfo::MB);
    int roundedSize = size*1000+0.5;
    size = roundedSize/1000;
    mediaInfoString += "\nSize: " + QString::number(size) + " MB";
    mediaInfoString += "\nContains video: ";
    if (mediaInfo->hasVideo()) mediaInfoString += "yes";
    else mediaInfoString += "no";
    mediaInfoString += "\nContains audio: ";
    if (mediaInfo->hasAudio()) mediaInfoString += "yes";
    else mediaInfoString += "no";
    mediaInfoString += "\nImage sequence: ";
    if (mediaInfo->isImageSequence()) mediaInfoString += "yes";
    else mediaInfoString += "no";

    mediaInfoString += "\n\nVideo codec: " + mediaInfo->getVideoCodec().getPrettyName();
    mediaInfoString += "\nResolution: " + QString::number(mediaInfo->getVideoWidth()) + "x" + QString::number(mediaInfo->getVideoHeight());
    mediaInfoString += "\nFramerate: " + QString::number(mediaInfo->getVideoFramerate()) + " fps";
    int bitrate = mediaInfo->getVideoBitrate(FFMediaInfo::Mbps);
    mediaInfoString += "\nBitrate: " + QString::number(bitrate) + " Mbps";

    mediaInfoString += "\n\nAudio codec: " + mediaInfo->getAudioCodec().getPrettyName();
    mediaInfoString += "\nSampling rate: " + QString::number(mediaInfo->getAudioSamplingRate()) + " Hz";
    int abitrate = mediaInfo->getAudioBitrate(FFMediaInfo::Kbps);
    mediaInfoString += "\nBitrate: " + QString::number(abitrate) + " Kbps";

    mediaInfoString += "\n\nFFmpeg analysis:\n" + mediaInfo->getFfmpegOutput();

    mediaInfosText->setText(mediaInfoString);

    /*//get media infos
    FFMediaInfo *input = ffmpeg->getMediaInfo(inputPath);
    //populate UI
    videoWidthButton->setValue(input->getVideoWidth());
    videoHeightButton->setValue(input->getVideoHeight());
    frameRateEdit->setValue(input->getVideoFramerate());
    int sampling = input->getAudioSamplingRate();
    if (sampling == 8000) samplingBox->setCurrentIndex(0);
    else if (sampling == 11025) samplingBox->setCurrentIndex(1);
    else if (sampling == 16000) samplingBox->setCurrentIndex(2);
    else if (sampling == 22050) samplingBox->setCurrentIndex(3);
    else if (sampling == 32000) samplingBox->setCurrentIndex(4);
    else if (sampling == 44100) samplingBox->setCurrentIndex(5);
    else if (sampling == 48000) samplingBox->setCurrentIndex(6);
    else if (sampling == 88200) samplingBox->setCurrentIndex(7);
    else if (sampling == 96000) samplingBox->setCurrentIndex(8);
    else samplingBox->setCurrentIndex(6);
    */
}
