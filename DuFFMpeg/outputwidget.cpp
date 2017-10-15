#include "outputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

OutputWidget::OutputWidget(FFmpeg *ff, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ffmpeg = ff;

    mediaInfo = new FFMediaInfo("",this);

    //populate sampling box
    samplingBox->addItem("8,000 Hz",QVariant(8000));
    samplingBox->addItem("11,025 Hz",QVariant(11025));
    samplingBox->addItem("16,000 Hz",QVariant(16000));
    samplingBox->addItem("22,050 Hz",QVariant(22050));
    samplingBox->addItem("32,000 Hz",QVariant(32000));
    samplingBox->addItem("44,100 Hz",QVariant(44100));
    samplingBox->addItem("48,000 Hz",QVariant(48000));
    samplingBox->addItem("88,200 Hz",QVariant(88200));
    samplingBox->addItem("96,000 Hz",QVariant(96000));

    ffmpeg_init();

    connect(ffmpeg,SIGNAL(binaryChanged()),this,SLOT(ffmpeg_init()));
}

FFMediaInfo *OutputWidget::getMediaInfo()
{
    mediaInfo->updateInfo("");

    mediaInfo->setFileName(outputEdit->text());
    if (!noVideoButton->isChecked())
    {
        mediaInfo->setVideo(true);
        if (videoCopyButton->isChecked()) mediaInfo->setVideoCodec(ffmpeg->getVideoEncoder("copy"));
        else
        {
            mediaInfo->setVideoCodec(ffmpeg->getVideoEncoder(videoCodecsBox->currentData().toString()));
            if (resizeButton->isChecked())
            {
                mediaInfo->setVideoWidth(videoWidthButton->value());
                mediaInfo->setVideoHeight(videoHeightButton->value());
            }
            if (frameRateButton->isChecked())
            {
                mediaInfo->setVideoFramerate(frameRateEdit->value());
            }
            mediaInfo->setVideoBitrate(videoBitRateEdit->value(),FFMediaInfo::Mbps);
        }
    }
    if (!noAudioButton->isChecked())
    {
        mediaInfo->setAudio(true);
        if (audioCopyButton->isChecked()) mediaInfo->setAudioCodec(ffmpeg->getAudioEncoder("copy"));
        else
        {
            mediaInfo->setAudioCodec(ffmpeg->getAudioEncoder(audioCodecsBox->currentData().toString()));
            if (samplingButton->isChecked())
            {
                mediaInfo->setAudioSamplingRate(samplingBox->currentData().toInt());
            }
            mediaInfo->setAudioBitrate(audioBitRateEdit->value(),FFMediaInfo::Kbps);
        }
    }

    return mediaInfo;
}

void OutputWidget::on_videoTranscodeButton_toggled(bool checked)
{
    videoTranscodeWidget->setEnabled(checked);
}

void OutputWidget::on_audioTranscodeButton_toggled(bool checked)
{
    audioTranscodeWidget->setEnabled(checked);
}

void OutputWidget::on_resizeButton_toggled(bool checked)
{
    videoWidthButton->setEnabled(checked);
    videoHeightButton->setEnabled(checked);
    widthLabel->setEnabled(checked);
    heightLabel->setEnabled(checked);
    aspectRatioLabel->setEnabled(checked);
}

void OutputWidget::on_frameRateButton_toggled(bool checked)
{
    frameRateBox->setEnabled(checked);
    frameRateEdit->setEnabled(checked);
}

void OutputWidget::on_samplingButton_toggled(bool checked)
{
    samplingBox->setEnabled(checked);
}

void OutputWidget::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file");
    if (outputPath == "") return;
    outputEdit->setText(outputPath);
}

void OutputWidget::on_frameRateBox_activated(const QString &arg1)
{
    if (arg1 != "Custom")
    {
        QString num = frameRateBox->currentText().replace(" fps","");
        frameRateEdit->setValue(num.toDouble());
    }
}

void OutputWidget::on_frameRateEdit_valueChanged(double arg1)
{
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

void OutputWidget::on_videoQualitySlider_valueChanged(int value)
{
    if (value >= 90)
    {
        qualityLabel->setText("(Visually) Lossless | " + QString::number(value) + "%");
    }
    else if (value >= 75)
    {
        qualityLabel->setText("Very good | " + QString::number(value) + "%");
    }
    else if (value >= 50)
    {
        qualityLabel->setText("Good | " + QString::number(value) + "%");
    }
    else if (value >= 25)
    {
        qualityLabel->setText("Bad | " + QString::number(value) + "%");
    }
    else
    {
        qualityLabel->setText("Very bad | " + QString::number(value) + "%");
    }
}

void OutputWidget::on_videoQualitySlider_sliderReleased()
{
    //Adjust bitrate
    //TODO Must depend on resolution (and bpc)
    //for now, values for full HD / Very good means at least 24Mbps (H.264 blu ray)
    //TODO adjust depending on codec
    int value = videoQualitySlider->value();
    double bitrate = value;
    bitrate = bitrate*24/75;
    videoBitRateEdit->setValue(bitrate);
}

void OutputWidget::on_videoBitRateEdit_valueChanged(double arg1)
{
    //Adjust quality
    //TODO Must depend on resolution (and bpc)
    //for now, values for full HD / Very good means at least 24Mbps (H.264 blu ray)
    //TODO adjust depending on codec
    int quality = arg1*75/24;
    videoQualitySlider->setValue(quality);
}

void OutputWidget::on_audioQualitySlider_sliderReleased()
{
    //Adjust bitrate
    //TODO adjust depending on codec
    int value = audioQualitySlider->value();
    int bitrate = value*320/100;
    audioBitRateEdit->setValue(bitrate);
}

void OutputWidget::on_audioQualitySlider_valueChanged(int value)
{
    if (value >= 90)
    {
        audioQualityLabel->setText("(Nearly) Lossless | " + QString::number(value) + "%");
    }
    else if (value >= 60)
    {
        audioQualityLabel->setText("Very good | " + QString::number(value) + "%");
    }
    else if (value >= 30)
    {
        audioQualityLabel->setText("Good | " + QString::number(value) + "%");
    }
    else if (value >= 17)
    {
        audioQualityLabel->setText("Bad | " + QString::number(value) + "%");
    }
    else
    {
        audioQualityLabel->setText("Very bad | " + QString::number(value) + "%");
    }
}

void OutputWidget::on_audioBitRateEdit_valueChanged(int arg1)
{
    //Adjust bitrate
    //TODO adjust depending on codec
    int quality = arg1*100/320;
    audioQualitySlider->setValue(quality);
}

void OutputWidget::on_videoWidthButton_valueChanged()
{
    aspectRatio();
}

void OutputWidget::on_videoHeightButton_valueChanged()
{
    aspectRatio();
}

void OutputWidget::aspectRatio()
{
    double width = videoWidthButton->value();
    double height = videoHeightButton->value();
    double ratio =  width / height;
    //round it to 3 digits
    int roundedRatio = ratio*100+0.5;
    ratio = roundedRatio;
    ratio = ratio/100;
    aspectRatioLabel->setText(QString::number(ratio) + ":1");
}

void OutputWidget::ffmpeg_init()
{
    //get codecs
    videoCodecsBox->clear();
    audioCodecsBox->clear();
    QList<FFCodec *> encoders = ffmpeg->getEncoders(true);

    int videoFilter = videoCodecsFilterBox->currentIndex();
    int audioFilter = audioCodecsFilterBox->currentIndex();

    foreach(FFCodec *encoder,encoders)
    {
        if (encoder->name() == "copy") continue;
        if (encoder->isVideo())
        {
            if (videoFilter <= 0 || (videoFilter == 1 && encoder->isLossy()) || (videoFilter == 2 && encoder->isLossless()) || (videoFilter == 3 && encoder->isIframe()))
            {
                videoCodecsBox->addItem(encoder->prettyName(),QVariant(encoder->name()));
            }
        }

        if (encoder->isAudio())
        {
            if (audioFilter <= 0 || (audioFilter == 1 && encoder->isLossy()) || (audioFilter == 2 && encoder->isLossless()))
            {
                audioCodecsBox->addItem(encoder->prettyName(),QVariant(encoder->name()));
            }
        }
    }
}

void OutputWidget::on_videoCodecsFilterBox_currentIndexChanged(const QString &arg1)
{
    ffmpeg_init();
}

void OutputWidget::on_audioCodecsFilterBox_currentIndexChanged(int index)
{
    ffmpeg_init();
}
