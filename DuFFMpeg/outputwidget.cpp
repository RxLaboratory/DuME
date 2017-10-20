#include "outputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

OutputWidget::OutputWidget(FFmpeg *ff, QWidget *parent) :
    QWidget(parent)
{
    _freezeUI = true;
    setupUi(this);

    outputTabs->setCurrentIndex(0);

    _ffmpeg = ff;
    _mediaInfo = new FFMediaInfo("",this);
    _currentMuxer = nullptr;

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
    samplingBox->setCurrentIndex(6);

    //hide/show supported/unsupported options
    videoOptionsUpdate();

    ffmpeg_init();

    connect(_ffmpeg,SIGNAL(binaryChanged()),this,SLOT(ffmpeg_init()));

    _freezeUI = false;
}

FFMediaInfo *OutputWidget::getMediaInfo()
{
    _mediaInfo->updateInfo("");

    _mediaInfo->setFileName(outputEdit->text());

    //VIDEO
    if (!noVideoButton->isChecked())
    {
        _mediaInfo->setVideo(true);
        if (videoCopyButton->isChecked()) _mediaInfo->setVideoCodec(_ffmpeg->getVideoEncoder("copy"));
        else
        {
            if (videoCodecButton->isChecked()) _mediaInfo->setVideoCodec(_ffmpeg->getVideoEncoder(videoCodecsBox->currentData().toString()));
            if (resizeButton->isChecked())
            {
                _mediaInfo->setVideoWidth(videoWidthButton->value());
                _mediaInfo->setVideoHeight(videoHeightButton->value());
            }
            if (frameRateButton->isChecked())
            {
                _mediaInfo->setVideoFramerate(frameRateEdit->value());
            }
            if (videoBitrateButton->isChecked()) _mediaInfo->setVideoBitrate(videoBitRateEdit->value(),FFMediaInfo::Mbps);
        }
    }

    //AUDIO
    if (!noAudioButton->isChecked())
    {
        _mediaInfo->setAudio(true);
        if (audioCopyButton->isChecked()) _mediaInfo->setAudioCodec(_ffmpeg->getAudioEncoder("copy"));
        else
        {
            _mediaInfo->setAudioCodec(_ffmpeg->getAudioEncoder(audioCodecsBox->currentData().toString()));
            if (samplingButton->isChecked())
            {
                _mediaInfo->setAudioSamplingRate(samplingBox->currentData().toInt());
            }
            _mediaInfo->setAudioBitrate(audioBitRateEdit->value(),FFMediaInfo::Kbps);
        }
    }

    //CUSTOM
    for (int i = 0 ; i < _customVideoParamEdits.count() ; i++)
    {
        QString param = _customVideoParamEdits[i]->text();
        if (param != "")
        {
            _mediaInfo->addFFmpegOption(param);
            _mediaInfo->addFFmpegOption(_customVideoValueEdits[0]->text());
        }
    }
    for (int i = 0 ; i < _customAudioParamEdits.count() ; i++)
    {
        QString param = _customAudioParamEdits[i]->text();
        if (param != "")
        {
            _mediaInfo->addFFmpegOption(param);
            _mediaInfo->addFFmpegOption(_customAudioValueEdits[0]->text());
        }
    }

    return _mediaInfo;
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
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file",outputEdit->text());
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

void OutputWidget::on_videoWidthButton_valueChanged(int val)
{
    aspectRatio();
}

void OutputWidget::on_videoHeightButton_valueChanged(int val)
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

void OutputWidget::updateOutputExtension()
{
    if (outputEdit->text() == "") return;
    QFileInfo output(outputEdit->text());
    QString newExt = "";
    if (_currentMuxer != nullptr)
    {
        if (_currentMuxer->extensions().count() > 0)
        {
            newExt = "." + _currentMuxer->extensions()[0];
        }
    }
    QString outputPath = output.path() + "/" + output.completeBaseName() + newExt;
    //check if file exists
    if (QFile(outputPath).exists())
    {
        outputPath = output.path() + "/" + output.completeBaseName() + "_transcoded" + newExt;
    }

    outputEdit->setText(QDir::toNativeSeparators(outputPath));
}

void OutputWidget::selectDefaultVideoCodec()
{
    if (_currentMuxer == nullptr) return;

    FFCodec *videoCodec = _ffmpeg->getMuxerDefaultCodec(_currentMuxer, FFCodec::Video);

    //Select Default Codec

    if (videoCodec != nullptr)
    {
        for (int v = 0; v < videoCodecsBox->count() ; v++)
        {
            if (videoCodecsBox->itemData(v).toString() == videoCodec->name())
            {
                videoCodecsBox->setCurrentIndex(v);
            }
        }
    }
}

void OutputWidget::videoOptionsUpdate()
{
    //TODO show/hide depending on codec

    //quality
    videoQualityButton->setChecked(false);
    videoQualityButton->hide();
    videoQualityWidget->hide();
}

void OutputWidget::ffmpeg_init()
{
    _freezeUI = true;
    ffmpeg_loadCodecs();
    ffmpeg_loadMuxers();
    _freezeUI = false;
}

void OutputWidget::ffmpeg_loadCodecs()
{
    _freezeUI = true;
    //get codecs and muxers
    videoCodecsBox->clear();
    audioCodecsBox->clear();
    QList<FFCodec *> encoders = _ffmpeg->getEncoders();

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

    selectDefaultVideoCodec();
    _freezeUI = false;
}

void OutputWidget::ffmpeg_loadMuxers()
{
    _freezeUI = true;

    formatsBox->clear();

    QList<FFMuxer *> muxers = _ffmpeg->getMuxers();

    int formatsFilter = formatsFilterBox->currentIndex();

    foreach(FFMuxer *muxer,muxers)
    {
        //skip muxers without extension
        if ((formatsFilter != 0 && formatsFilter != 5) && muxer->extensions().count() == 0) continue;
        else if (formatsFilter == 5 && muxer->extensions().count() == 0) formatsBox->addItem(muxer->prettyName(),QVariant(muxer->name()));

        if (formatsFilter == 0 ||
                (formatsFilter == 1 && muxer->isAudio() && muxer->isVideo() )  ||
                (formatsFilter == 2 && muxer->isSequence() ) ||
                (formatsFilter == 3 && muxer->isAudio() && !muxer->isVideo() )  ||
                (formatsFilter == 4 && muxer->isVideo() && !muxer->isAudio() ))
        {
            formatsBox->addItem("." + muxer->extensions().join(", .") + " | " + muxer->prettyName(),QVariant(muxer->name()));
        }
    }
    _freezeUI = false;
    on_formatsBox_currentIndexChanged(formatsBox->currentIndex());
}

void OutputWidget::newInputMedia(FFMediaInfo *input)
{
    //set output fileName
    QFileInfo inputFile(input->fileName());
    QString outputPath = inputFile.path() + "/" + inputFile.completeBaseName();
    outputEdit->setText(outputPath);
    updateOutputExtension();

    //if not checked, update fields
    if (!resizeButton->isChecked())
    {
        videoWidthButton->setValue(input->videoWidth());
        videoHeightButton->setValue(input->videoHeight());
    }
    if (!frameRateButton->isChecked())
    {
        frameRateEdit->setValue(input->videoFramerate());
    }
    if (!samplingButton->isChecked())
    {
        int sampling = input->audioSamplingRate();
        if (sampling == 8000) samplingBox->setCurrentIndex(0);
        else if (sampling == 11025) samplingBox->setCurrentIndex(1);
        else if (sampling == 16000) samplingBox->setCurrentIndex(2);
        else if (sampling == 22050) samplingBox->setCurrentIndex(3);
        else if (sampling == 32000) samplingBox->setCurrentIndex(4);
        else if (sampling == 44100) samplingBox->setCurrentIndex(5);
        else if (sampling == 48000) samplingBox->setCurrentIndex(6);
        else if (sampling == 88200) samplingBox->setCurrentIndex(7);
        else if (sampling == 96000) samplingBox->setCurrentIndex(8);
    }
}

void OutputWidget::on_videoCodecsFilterBox_currentIndexChanged(int index)
{
    ffmpeg_loadCodecs();
}

void OutputWidget::on_audioCodecsFilterBox_currentIndexChanged(int index)
{
    ffmpeg_loadCodecs();
}

void OutputWidget::on_addVideoParam_clicked()
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
    QFormLayout *layout = (QFormLayout*)videoTranscodeWidget->layout();
    layout->insertRow(layout->rowCount()-2,customParam,customValue);
    _customVideoParamEdits << customParam;
    _customVideoValueEdits << customValue;
}

void OutputWidget::on_addAudioParam_clicked()
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
    QFormLayout *layout = (QFormLayout*)audioTranscodeWidget->layout();
    layout->insertRow(layout->rowCount()-1,customParam,customValue);
    _customAudioParamEdits << customParam;
    _customAudioValueEdits << customValue;

}

void OutputWidget::on_formatsBox_currentIndexChanged(int index)
{
    _currentMuxer = _ffmpeg->getMuxer(formatsBox->currentData().toString());

    if (_freezeUI) return;
    _freezeUI = true;

    selectDefaultVideoCodec();

    FFCodec *audioCodec = _ffmpeg->getMuxerDefaultCodec(_currentMuxer, FFCodec::Audio);

    if (audioCodec != nullptr)
    {
        for (int a = 0; a < audioCodecsBox->count() ; a++)
        {
            if (audioCodecsBox->itemData(a).toString() == audioCodec->name())
            {
                audioCodecsBox->setCurrentIndex(a);
            }
        }
    }

    //UI

    //video
    if (_currentMuxer->isVideo() || _currentMuxer->isSequence())
    {
        if (noVideoButton->isChecked()) videoTranscodeButton->setChecked(true);
        videoTranscodeButton->setEnabled(true);
        videoCopyButton->setEnabled(true);
    }
    else
    {
        noVideoButton->setChecked(true);
        videoTranscodeButton->setEnabled(false);
        videoCopyButton->setEnabled(false);
    }
    //audio
    if (_currentMuxer->isAudio())
    {
        if (noAudioButton->isChecked()) audioTranscodeButton->setChecked(true);
        audioTranscodeButton->setEnabled(true);
        audioCopyButton->setEnabled(true);
    }
    else
    {
        noAudioButton->setChecked(true);
        audioTranscodeButton->setEnabled(false);
        audioCopyButton->setEnabled(false);
    }

    updateOutputExtension();

    _freezeUI = false;
}

void OutputWidget::on_formatsFilterBox_currentIndexChanged(int index)
{
    ffmpeg_loadMuxers();
}

void OutputWidget::on_videoCodecButton_clicked(bool checked)
{
    if (checked)
    {
        videoCodecsFilterBox->setItemText(0,"All codecs");
        videoCodecWidget->setEnabled(true);
    }
    else
    {
        videoCodecsFilterBox->setCurrentIndex(0);
        ffmpeg_loadCodecs();
        videoCodecsFilterBox->setItemText(0,"Default");
        videoCodecWidget->setEnabled(false);
    }
}

void OutputWidget::on_videoBitrateButton_clicked(bool checked)
{
    if (checked)
    {
        videoQualityButton->setChecked(false);
        on_videoQualityButton_clicked(false);
        videoBitRateEdit->setValue(24.0);
        videoBitRateEdit->setSuffix(" Mbps");
        videoBitRateEdit->setEnabled(true);
    }
    else
    {
        videoBitRateEdit->setValue(0.0);
        videoBitRateEdit->setSuffix(" Auto");
        videoBitRateEdit->setEnabled(false);
    }
}

void OutputWidget::on_videoQualityButton_clicked(bool checked)
{
    if (checked)
    {
        videoBitrateButton->setChecked(false);
        on_videoBitrateButton_clicked(false);
        videoQualityWidget->setEnabled(true);
        on_videoQualitySlider_valueChanged(videoQualitySlider->value());
    }
    else
    {
        videoQualityWidget->setEnabled(false);
        qualityLabel->setText("Excellent");
    }
}

void OutputWidget::on_audioCodecButton_clicked(bool checked)
{
    if (checked)
    {
        audioCodecsFilterBox->setItemText(0,"All codecs");
        audioCodecWidget->setEnabled(true);
    }
    else
    {
        audioCodecsFilterBox->setCurrentIndex(0);
        ffmpeg_loadCodecs();
        audioCodecsFilterBox->setItemText(0,"Default");
        audioCodecWidget->setEnabled(false);
    }
}

void OutputWidget::on_audioBitrateButton_clicked(bool checked)
{
    if (checked)
    {
        audioBitRateEdit->setValue(320);
        audioBitRateEdit->setSuffix(" Kbps");
        audioBitRateEdit->setEnabled(true);
    }
    else
    {
        audioBitRateEdit->setValue(0);
        audioBitRateEdit->setSuffix(" Auto");
        audioBitRateEdit->setEnabled(false);
    }
}
