#include "inputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

InputWidget::InputWidget(FFmpeg *ff, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;
    _mediaInfo = new FFMediaInfo("",this);
}

FFMediaInfo *InputWidget::getMediaInfo()
{
    //update custom params before returning
    _mediaInfo->clearFFmpegOptions();

    for (int i = 0 ; i < _customParamEdits.count() ; i++)
    {
        QString param = _customParamEdits[i]->text();
        if (param != "")
        {
            _mediaInfo->addFFmpegOption(param);
            _mediaInfo->addFFmpegOption(_customValueEdits[0]->text());
        }
    }

    return _mediaInfo;
}

void InputWidget::on_inputBrowseButton_clicked()
{
    //get current file path
    QFileInfo fi(inputEdit->text());
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",fi.path());
    if (inputPath == "") return;

    //update UI
    inputEdit->setText(inputPath);

    _mediaInfo->updateInfo(ffmpeg->getMediaInfoString(inputPath));

    //Text
    QString mediaInfoString = "Media information";

    mediaInfoString += "\n\nContainers: " + _mediaInfo->getContainer().join(",");

    QTime duration(0,0,0);
    duration = duration.addSecs(_mediaInfo->getDuration());
    mediaInfoString += "\nDuration: " + duration.toString("hh:mm:ss.zzz");

    double size = _mediaInfo->getSize(FFMediaInfo::MB);
    int roundedSize = size*1000+0.5;
    size = roundedSize/1000;
    mediaInfoString += "\nSize: " + QString::number(size) + " MB";

    mediaInfoString += "\nContains video: ";
    if (_mediaInfo->hasVideo()) mediaInfoString += "yes";
    else mediaInfoString += "no";
    mediaInfoString += "\nContains audio: ";
    if (_mediaInfo->hasAudio()) mediaInfoString += "yes";
    else mediaInfoString += "no";
    mediaInfoString += "\nImage sequence: ";
    if (_mediaInfo->isImageSequence()) mediaInfoString += "yes";
    else mediaInfoString += "no";

    mediaInfoString += "\n\nVideo codec: ";
    if(_mediaInfo->getVideoCodec() != nullptr)
    {
        mediaInfoString += _mediaInfo->getVideoCodec()->prettyName();
    }
    mediaInfoString += "\nResolution: " + QString::number(_mediaInfo->getVideoWidth()) + "x" + QString::number(_mediaInfo->getVideoHeight());
    mediaInfoString += "\nFramerate: " + QString::number(_mediaInfo->getVideoFramerate()) + " fps";
    int bitrate = _mediaInfo->getVideoBitrate(FFMediaInfo::Mbps);
    mediaInfoString += "\nBitrate: " + QString::number(bitrate) + " Mbps";

    mediaInfoString += "\n\nAudio codec: ";
    if(_mediaInfo->getAudioCodec() != nullptr)
    {
        mediaInfoString += _mediaInfo->getAudioCodec()->prettyName();
    }
    mediaInfoString += "\nSampling rate: " + QString::number(_mediaInfo->getAudioSamplingRate()) + " Hz";
    int abitrate = _mediaInfo->getAudioBitrate(FFMediaInfo::Kbps);
    mediaInfoString += "\nBitrate: " + QString::number(abitrate) + " Kbps";
    mediaInfoString += "\n\nFFmpeg analysis:\n" + _mediaInfo->getFfmpegOutput();

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

void InputWidget::on_addParamButton_clicked()
{
    //add a param and a value
    QLineEdit *customParam = new QLineEdit(this);
    customParam->setPlaceholderText("-param");
    customParam->setMinimumWidth(100);
    customParam->setMaximumWidth(100);
    //the value edit
    QLineEdit *customValue = new QLineEdit(this);
    customValue->setPlaceholderText("Value");
    //add to layout and lists
    customParamsLayout->insertRow(customParamsLayout->rowCount()-1,customParam,customValue);
    _customParamEdits << customParam;
    _customValueEdits << customValue;
}
