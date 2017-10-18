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

    mediaInfoString += "\n\nContainers: " + _mediaInfo->container().join(",");

    QTime duration(0,0,0);
    duration = duration.addSecs(_mediaInfo->duration());
    mediaInfoString += "\nDuration: " + duration.toString("hh:mm:ss.zzz");

    double size = _mediaInfo->size(FFMediaInfo::MB);
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
    if(_mediaInfo->videoCodec() != nullptr)
    {
        mediaInfoString += _mediaInfo->videoCodec()->prettyName();
    }
    mediaInfoString += "\nResolution: " + QString::number(_mediaInfo->videoWidth()) + "x" + QString::number(_mediaInfo->videoHeight());
    mediaInfoString += "\nFramerate: " + QString::number(_mediaInfo->videoFramerate()) + " fps";
    int bitrate = _mediaInfo->videoBitrate(FFMediaInfo::Mbps);
    mediaInfoString += "\nBitrate: " + QString::number(bitrate) + " Mbps";

    mediaInfoString += "\n\nAudio codec: ";
    if(_mediaInfo->audioCodec() != nullptr)
    {
        mediaInfoString += _mediaInfo->audioCodec()->prettyName();
    }
    mediaInfoString += "\nSampling rate: " + QString::number(_mediaInfo->audioSamplingRate()) + " Hz";
    int abitrate = _mediaInfo->audioBitrate(FFMediaInfo::Kbps);
    mediaInfoString += "\nBitrate: " + QString::number(abitrate) + " Kbps";
    mediaInfoString += "\n\nFFmpeg analysis:\n" + _mediaInfo->ffmpegOutput();

    mediaInfosText->setText(mediaInfoString);

    emit newMediaLoaded(_mediaInfo);
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
