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

    //populate color transfer
    trcBox->addItem("BT.709", QVariant("bt709"));
    trcBox->addItem("Gamma", QVariant("gamma"));
    trcBox->addItem("BT.470 M (Gamma 2.2)", QVariant("gamma22"));
    trcBox->addItem("BT.470 BG (Gamma 2.8)", QVariant("gamma28"));
    trcBox->addItem("SMPTE 170 M", QVariant("smpte170m"));
    trcBox->addItem("SMPTE 240 M", QVariant("smpte240m"));
    trcBox->addItem("Linear", QVariant("linear"));
    trcBox->addItem("Log", QVariant("log"));
    trcBox->addItem("Log square root", QVariant("log_sqrt"));
    trcBox->addItem("IEC 61966-2-4", QVariant("iec61966_2_4"));
    trcBox->addItem("BT.1361", QVariant("bt1361"));
    trcBox->addItem("IEC 61966-2-1", QVariant("iec61966_2_1"));
    trcBox->addItem("bt2020_10bit", QVariant("bt2020_10bit"));
    trcBox->addItem("bt2020_12bit", QVariant("BT.2020 - 12 bit"));
    trcBox->addItem("smpte2084", QVariant("SMPTE ST 2084"));
    trcBox->addItem("smpte428_1", QVariant("SMPTE ST 428-1"));

    trcBox->setCurrentIndex(0);

    updateOptions();
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
            QStringList option(param);
            option << _customValueEdits[i]->text();
            _mediaInfo->addFFmpegOption(option);
        }
    }

    return _mediaInfo;
}

void InputWidget::openFile(QString file)
{
    QSettings settings;
    if (file == "") return;

    //Text
    QString mediaInfoString = "Media information";

    QFileInfo fileInfo(file);
    //update UI
    inputEdit->setText(QDir::toNativeSeparators(file));
    //keep in settings
    settings.setValue("input/path",QVariant(fileInfo.path()));

    if (fileInfo.suffix() == "aep" || fileInfo.suffix() == "aet" || fileInfo.suffix() == "aepx")
    {
        _mediaInfo->updateInfo("");
        _mediaInfo->setAep(true);
        _mediaInfo->setAepNumThreads(threadsBox->value());
        _mediaInfo->setFileName(file);

        if (fileInfo.suffix() == "aep") mediaInfoString += "\n\nAfter Effects project.";
        if (fileInfo.suffix() == "aet") mediaInfoString += "\n\nAfter Effects template.";
        if (fileInfo.suffix() == "aepx") mediaInfoString += "\n\nAfter Effects XML project.";
    }
    else
    {
        _mediaInfo->updateInfo(ffmpeg->getMediaInfoString(file));

        mediaInfoString += "\n\nContainers: " + _mediaInfo->extensions().join(",");

        if (_mediaInfo->duration() != 0.0)
        {
            QTime duration(0,0,0);
            duration = duration.addSecs(_mediaInfo->duration());
            mediaInfoString += "\nDuration: " + duration.toString("hh:mm:ss.zzz");
        }
        else if (_mediaInfo->isImageSequence())
        {
            mediaInfoString += "\nDuration: " + QString::number(_mediaInfo->frames().count()) + " frames";
            mediaInfoString += "\nStart Frame Number: " + QString::number(_mediaInfo->startNumber());
        }


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

        if (_mediaInfo->hasVideo())
        {
            mediaInfoString += "\n\nVideo codec: ";
            if(_mediaInfo->videoCodec() != nullptr)
            {
                mediaInfoString += _mediaInfo->videoCodec()->prettyName();
            }
            mediaInfoString += "\nResolution: " + QString::number(_mediaInfo->videoWidth()) + "x" + QString::number(_mediaInfo->videoHeight());
            mediaInfoString += "\nFramerate: " + QString::number(_mediaInfo->videoFramerate()) + " fps";
            int bitrate = _mediaInfo->videoBitrate(FFMediaInfo::Mbps);
            if (bitrate != 0) mediaInfoString += "\nBitrate: " + QString::number(bitrate) + " Mbps";
            else
            {
                bitrate = _mediaInfo->videoBitrate(FFMediaInfo::Kbps);
                if (bitrate != 0) mediaInfoString += "\nBitrate: " + QString::number(bitrate) + " kbps";
            }
            if (_mediaInfo->pixFormat() != nullptr)
            {
                mediaInfoString += "\nPixel Format: " + _mediaInfo->pixFormat()->name();
                if (_mediaInfo->pixFormat()->hasAlpha()) mediaInfoString += "\nAlpha: yes";
                else mediaInfoString += "\nAlpha: no";
            }
        }

        if (_mediaInfo->hasAudio())
        {
            mediaInfoString += "\n\nAudio codec: ";
            if(_mediaInfo->audioCodec() != nullptr)
            {
                mediaInfoString += _mediaInfo->audioCodec()->prettyName();
            }
            mediaInfoString += "\nSampling rate: " + QString::number(_mediaInfo->audioSamplingRate()) + " Hz";
            int abitrate = _mediaInfo->audioBitrate(FFMediaInfo::Kbps);
            if (abitrate != 0) mediaInfoString += "\nBitrate: " + QString::number(abitrate) + " kbps";
        }

        //mediaInfoString += "\n\nFFmpeg analysis:\n" + _mediaInfo->ffmpegOutput();
    }

    mediaInfosText->setText(mediaInfoString);

    updateOptions();

    emit newMediaLoaded(_mediaInfo);
}

void InputWidget::openFile(QUrl file)
{
    openFile(file.toLocalFile());
}

void InputWidget::on_inputBrowseButton_clicked()
{
    QSettings settings;
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",settings.value("input/path",QVariant("")).toString());
    if (inputPath == "") return;
    openFile(inputPath);
}

void InputWidget::on_inputEdit_editingFinished()
{
    //check if file exists, try to read url
    QUrl test(inputEdit->text());
    if (!test.isEmpty())
    {
        if (test.isValid())
        {
            inputEdit->setText(test.toLocalFile());
        }
    }

    openFile(inputEdit->text());
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
    customParamsLayout->insertRow(customParamsLayout->rowCount(),customParam,customValue);
    _customParamEdits << customParam;
    _customValueEdits << customValue;
}

void InputWidget::on_frameRateButton_toggled(bool checked)
{
    frameRateBox->setEnabled(checked);
    frameRateEdit->setEnabled(checked);
}

void InputWidget::on_trcButton_toggled(bool checked)
{
    trcBox->setEnabled(checked);
    if (checked)
    {
        _mediaInfo->setTrc(trcBox->currentData().toString());
    }
    else
    {
        _mediaInfo->setTrc("");
    }
    emit newMediaLoaded(_mediaInfo);
}

void InputWidget::on_frameRateBox_activated(const QString &arg1)
{
    if (arg1 != "Custom")
    {
        QString num = frameRateBox->currentText().replace(" fps","");
        frameRateEdit->setValue(num.toDouble());
    }
}

void InputWidget::on_frameRateEdit_valueChanged(double arg1)
{
    //update mediainfo
    _mediaInfo->setVideoFramerate(frameRateEdit->value());
    emit newMediaLoaded(_mediaInfo);
    //look for corresponding value
    for (int i = 1 ; i < frameRateBox->count() ; i++)
    {
        QString num = frameRateBox->itemText(i).replace(" fps","");
        if (num.toDouble() == arg1)
        {
            frameRateBox->setCurrentIndex(i);
            return;
        }
    }
    frameRateBox->setCurrentIndex(0);

}

void InputWidget::on_trcBox_currentIndexChanged(int index)
{
    if (trcButton->isChecked())
    {
        _mediaInfo->setTrc(trcBox->itemData(index).toString());
    }
    emit newMediaLoaded(_mediaInfo);
}

void InputWidget::on_compButton_clicked()
{
    compEdit->setEnabled(true);
    rqindexButton->setChecked(false);
    compButton->setChecked(true);
    rqindexBox->setEnabled(false);
    _mediaInfo->setAepCompName(compEdit->text());
}

void InputWidget::on_compEdit_textEdited(const QString &arg1)
{
    _mediaInfo->setAepCompName(arg1);
}

void InputWidget::on_rqindexButton_clicked()
{
    rqindexBox->setEnabled(true);
    compEdit->setEnabled(false);
    compButton->setChecked(false);
    rqindexButton->setChecked(true);
    _mediaInfo->setAepRqindex(rqindexBox->value());
    _mediaInfo->setAepCompName("");
}

void InputWidget::on_rqindexBox_valueChanged(int arg1)
{
    _mediaInfo->setAepRqindex(arg1);
    _mediaInfo->setAepCompName("");
}

void InputWidget::on_threadsButton_toggled(bool checked)
{
    threadsBox->setEnabled(checked);
}

void InputWidget::updateOptions()
{
    //frame rate
    frameRateButton->hide();
    frameRateBox->hide();
    frameRateEdit->hide();
    //Aep
    aeLabel->hide();
    compButton->hide();
    compEdit->hide();
    threadsBox->hide();
    threadsButton->hide();
    rqindexButton->hide();
    rqindexBox->hide();

    if (_mediaInfo->isImageSequence())
    {
        frameRateButton->show();
        frameRateBox->show();
        frameRateEdit->show();
    }

    if (_mediaInfo->isAep())
    {
        aeLabel->show();
        compButton->show();
        compEdit->show();
        threadsBox->show();
        threadsButton->show();
        rqindexButton->show();
        rqindexBox->show();
        rqindexButton->setChecked(true);
        compButton->setChecked(false);
        rqindexBox->setEnabled(true);
        compEdit->setEnabled(false);
        threadsBox->setValue(QThread::idealThreadCount());
    }

    //uncheck what is hidden
    if (frameRateButton->isHidden()) frameRateButton->setChecked(false);
}



