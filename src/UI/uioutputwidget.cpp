#include "uioutputwidget.h"
#include "uidropshadow.h"

#include <QGraphicsDropShadowEffect>

#ifdef QT_DEBUG
#include <QtDebug>
#endif

UIOutputWidget::UIOutputWidget(FFmpeg *ff, int id, QWidget *parent) :
    QWidget(parent)
{
    _freezeUI = true;
    _loadingPreset = false;
    setupUi(this);

    //add some nice shadows
    resizeWidget->setGraphicsEffect(new UIDropShadow);
    frameRateWidget->setGraphicsEffect(new UIDropShadow);
    mainVideoCodecWidget->setGraphicsEffect(new UIDropShadow);
    sequenceWidget->setGraphicsEffect(new UIDropShadow);
    componentsWidget->setGraphicsEffect(new UIDropShadow);

    samplingWidget->setGraphicsEffect(new UIDropShadow);
    mainAudioCodecWidget->setGraphicsEffect(new UIDropShadow);

    outputTabs->setCurrentIndex(0);

    _ffmpeg = ff;
    _index = id;
    _mediaInfo = new MediaInfo(_ffmpeg, this);
    _currentMuxer = nullptr;
    _currentAudioCodec = nullptr;
    _currentVideoCodec = nullptr;
    _inputHasAlpha = false;

    //populate sampling box
    //TODO Get from ffmpeg
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

    init();

    ffmpeg_init();

    connect(_ffmpeg,SIGNAL(binaryChanged()),this,SLOT(ffmpeg_init()));

    _freezeUI = false;

    //Set defaults

    on_presetsFilterBox_activated(0);
    //hide/show supported/unsupported options
    updateAudioVideoOptions();
}

void UIOutputWidget::init()
{
    //main
    formatsFilterBox->setCurrentText("");
    formatsBox->setCurrentIndex(0);

    //video
    videoTranscodeButton->setChecked(true);
    resizeButton->setChecked(false);
    frameRateButton->setChecked(false);
    videoCodecButton->setChecked(false);
    videoBitrateButton->setChecked(false);
    videoLoopsButton->setChecked(false);
    videoProfileButton->setChecked(false);
    videoQualityButton->setChecked(false);

    //audio
    audioTranscodeButton->setChecked(true);
    samplingButton->setChecked(false);
    audioCodecButton->setChecked(false);
    audioBitrateButton->setChecked(false);

    //params
    qDeleteAll(_customParamEdits);
    qDeleteAll(_customValueEdits);
    _customParamEdits.clear();
    _customValueEdits.clear();
}

MediaInfo *UIOutputWidget::getMediaInfo()
{
    _mediaInfo->reInit( true );

    _mediaInfo->setFileName(outputEdit->text());

    //VIDEO
    if (!noVideoButton->isChecked())
    {
        _mediaInfo->setVideo(true);
        if (videoCopyButton->isChecked()) _mediaInfo->setVideoCodec( _ffmpeg->videoEncoder("copy") );
        else
        {
            _mediaInfo->setVideoCodec( _ffmpeg->videoEncoder(videoCodecsBox->currentData().toString()) );
            if (resizeButton->isChecked())
            {
                _mediaInfo->setVideoWidth(videoWidthButton->value());
                _mediaInfo->setVideoHeight(videoHeightButton->value());
            }
            if (frameRateButton->isChecked())
            {
                _mediaInfo->setVideoFramerate(frameRateEdit->value());
            }
            if (videoBitrateButton->isChecked())
            {
                _mediaInfo->setVideoBitrate(videoBitRateEdit->value(), MediaUtils::Mbps);
            }
            if (videoQualityButton->isChecked())
            {
                _mediaInfo->setVideoQuality(videoQualitySlider->value());
            }
            if (videoProfileButton->isChecked())
            {
                _mediaInfo->setVideoProfile(videoProfileBox->currentData().toInt());
            }
            if (startNumberButton->isChecked())
            {
                _mediaInfo->setStartNumber(startNumberEdit->value());
            }
            if (pixFmtButton->isChecked() || (alphaButton->isChecked() && alphaButton->isVisible()) )
            {
                _mediaInfo->setPixFormat(_ffmpeg->pixFormat(pixFmtBox->currentData().toString()));
            }
            if (!unmultButton->isChecked())
            {
                _mediaInfo->setPremultipliedAlpha(true);
            }
            else
            {
                _mediaInfo->setPremultipliedAlpha(false);
            }
        }
    }

    //AUDIO
    if (!noAudioButton->isChecked())
    {
        _mediaInfo->setAudio(true);
        if (audioCopyButton->isChecked()) _mediaInfo->setAudioCodec(_ffmpeg->audioEncoder("copy"));
        else
        {
            _mediaInfo->setAudioCodec(_ffmpeg->audioEncoder(audioCodecsBox->currentData().toString()));
            if (samplingButton->isChecked())
            {
                _mediaInfo->setAudioSamplingRate(samplingBox->currentData().toInt());
            }
            if (audioBitrateButton->isChecked()) _mediaInfo->setAudioBitrate(audioBitRateEdit->value(),MediaUtils::Kbps);
        }
    }

    //MUXER
    FFMuxer *muxer = _ffmpeg->muxer(formatsBox->currentData().toString());
    _mediaInfo->setMuxer(muxer);

    //LOOP
    if (videoLoopsButton->isChecked()) _mediaInfo->setLoop(videoLoopsEdit->value());

    //CUSTOM
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

void UIOutputWidget::setMediaInfo(MediaInfo *mediaInfo)
{
    if (mediaInfo == nullptr) return;

    init();

    delete _mediaInfo;
    _mediaInfo = mediaInfo;
    mediaInfo->setParent(this);


    //MUXER
    FFMuxer *muxer = _mediaInfo->muxer();
    if (muxer != nullptr)
    {
        for (int i = 0;i < formatsBox->count() ; i++)
        {
            if (formatsBox->itemData(i).toString() == muxer->name())
            {
                formatsBox->setCurrentIndex(i);
                break;
            }
        }
    }
    //loop
    int loop = _mediaInfo->loop();
    videoLoopsEdit->setValue(loop);
    if (loop > -1) videoLoopsButton->setChecked(true);

    //VIDEO
    if (_mediaInfo->hasVideo())
    {
        _currentVideoCodec = _mediaInfo->videoCodec();
        if (_currentVideoCodec != nullptr)
        {
            if (_currentVideoCodec->name() == "copy") videoCopyButton->setChecked(true);
            else
            {
                videoTranscodeButton->setChecked(true);
                videoCodecButton->setChecked(true);
                for(int i = 0 ; i  < videoCodecsBox->count() ; i++)
                {
                    if (videoCodecsBox->itemData(i).toString() == _currentVideoCodec->name())
                    {
                        videoCodecsBox->setCurrentIndex(i);
                        break;
                    }
                }
                //set pixFmt
                FFPixFormat *pf = _mediaInfo->pixFormat();
                if (pf != nullptr)
                {
                    pixFmtButton->setChecked(true);
                    for(int i = 0 ; i  < pixFmtBox->count() ; i++)
                    {
                        if (pixFmtBox->itemData(i).toString() == pf->name())
                        {
                            pixFmtBox->setCurrentIndex(i);
                            break;
                        }
                    }
                }
            }
        }
        int width = _mediaInfo->videoWidth();
        int height = _mediaInfo->videoHeight();
        if (width != 0 && height != 0)
        {
            resizeButton->setChecked(true);
            videoWidthButton->setValue(width);
            videoHeightButton->setValue(height);
        }
        double framerate = _mediaInfo->videoFramerate();
        if (framerate != 0.0)
        {
            frameRateButton->setChecked(true);
            frameRateEdit->setValue(framerate);
        }
        double bitrate = _mediaInfo->videoBitrate(MediaUtils::Mbps);
        if (bitrate != 0.0)
        {
            videoBitrateButton->setChecked(true);
            videoBitRateEdit->setValue(bitrate);
        }
        int quality = _mediaInfo->videoQuality();
        if (quality > -1)
        {
            videoQualityButton->setChecked(true);
            videoQualitySlider->setValue(quality);
        }
        int profile = _mediaInfo->videoProfile();
        if (profile > -1)
        {
            videoProfileButton->setChecked(true);
            videoProfileBox->setCurrentIndex(profile);
        }
        unmultButton->setChecked(!_mediaInfo->premultipliedAlpha());

    }
    else
    {
        noVideoButton->setChecked(true);
    }


    //AUDIO
    if (_mediaInfo->hasAudio())
    {
        _currentAudioCodec = _mediaInfo->audioCodec();
        if (_currentAudioCodec != nullptr)
        {
            if (_currentAudioCodec->name() == "copy") audioCopyButton->setChecked(true);
            else
            {
                audioTranscodeButton->setChecked(true);
                audioCodecButton->setChecked(true);
                for(int i = 0 ; i  < audioCodecsBox->count() ; i++)
                {
                    if (audioCodecsBox->itemData(i).toString() == _currentAudioCodec->name())
                    {
                        audioCodecsBox->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
        int sampling = _mediaInfo->audioSamplingRate();
        if (sampling != 0)
        {
            samplingButton->setChecked(true);
            for(int i = 0 ; i < samplingBox->count() ; i++)
            {
                if (samplingBox->itemData(i).toInt() == sampling)
                {
                    samplingBox->setCurrentIndex(i);
                    break;
                }
            }
        }
        double bitrate = _mediaInfo->audioBitrate(MediaUtils::Kbps);
        if (bitrate != 0.0)
        {
            audioBitrateButton->setChecked(true);
            audioBitRateEdit->setValue(int( bitrate ));
        }
    }
    else
    {
        noAudioButton->setChecked(true);
    }


    //CUSTOM
    foreach (QStringList option , _mediaInfo->ffmpegOptions())
    {
        QString name = option[0];
        QString value = "";
        if (option.count() > 0) value = option[1];
        addNewParam(name,value);
    }

}

QString UIOutputWidget::getOutputPath()
{
    return outputEdit->text();
}

void UIOutputWidget::on_videoTranscodeButton_toggled( )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    updateAudioVideoOptions();
}

void UIOutputWidget::on_audioTranscodeButton_toggled( )
{
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    updateAudioVideoOptions();
}

void UIOutputWidget::on_resizeButton_toggled(bool checked)
{
    videoWidthButton->setEnabled(checked);
    videoHeightButton->setEnabled(checked);
    widthLabel->setEnabled(checked);
    heightLabel->setEnabled(checked);
    aspectRatioLabel->setEnabled(checked);
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_frameRateButton_toggled(bool checked)
{
    frameRateBox->setEnabled(checked);
    frameRateEdit->setEnabled(checked);
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_samplingButton_toggled(bool checked)
{
    samplingBox->setEnabled(checked);
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_outputBrowseButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(this,"Output file",outputEdit->text());
    if (outputPath == "") return;
    updateOutputExtension(outputPath);
}

void UIOutputWidget::on_frameRateBox_activated(const QString &arg1)
{
    if (arg1 != "Custom")
    {
        QString num = frameRateBox->currentText().replace(" fps","");
        frameRateEdit->setValue(num.toDouble());
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_frameRateEdit_valueChanged(double arg1)
{
    //look for corresponding value
    for (int i = 1 ; i < frameRateBox->count() ; i++)
    {
        QString num = frameRateBox->itemText(i).replace(" fps","");
        if ( int( num.toDouble()*100 ) == int( arg1*100 ) )
        {
            frameRateBox->setCurrentIndex(i);
            return;
        }
    }
    frameRateBox->setCurrentIndex(0);
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_videoQualitySlider_valueChanged(int value)
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
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_videoWidthButton_valueChanged(int val)
{
    aspectRatio();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);

    //warnings depending on codec
    QString codec = videoCodecsBox->currentData().toString();
    if (codec == "h264" && val % 2 != 0) emit newLog("WARNING: h264 only accepts width with an even number of pixels");
}

void UIOutputWidget::on_videoHeightButton_valueChanged(int val)
{
    aspectRatio();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);

    //warnings depending on codec
    QString codec = videoCodecsBox->currentData().toString();
    if (codec == "h264" && val % 2 != 0) emit newLog("WARNING: h264 only accepts height with an even number of pixels");
}

void UIOutputWidget::on_videoCodecsBox_currentIndexChanged( )
{
    updateAudioVideoOptions();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
    _currentVideoCodec = _ffmpeg->videoEncoder(videoCodecsBox->currentData().toString());
    pixFmtFilterBox->setCurrentIndex(0);

    //check if there is alpha in one of the pixel formats to display button
    int numAlpha = 0;
    int numNoAlpha = 0;

    if (_currentVideoCodec == nullptr) return;

    QList<FFPixFormat *> pixFmts = _currentVideoCodec->pixFormats();
    if (pixFmts.count() == 0) return;

    foreach(FFPixFormat *pf, _currentVideoCodec->pixFormats())
    {
        if (pf->hasAlpha()) numAlpha++;
        else numNoAlpha++;
        if (numAlpha > 0 && numNoAlpha > 0) break;
    }
    if (numAlpha > 0 && numNoAlpha > 0) alphaButton->show();
    else alphaButton->hide();

    ffmpeg_loadPixFmts(true);
}

void UIOutputWidget::on_audioCodecsBox_currentIndexChanged()
{
    _currentAudioCodec = _ffmpeg->audioEncoder(audioCodecsBox->currentData().toString());
}

void UIOutputWidget::on_videoProfileButton_toggled(bool checked)
{
    if (checked)
    {
        videoProfileBox->setEnabled(true);
    }
    else
    {
        videoProfileBox->setEnabled(false);
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_videoLoopsButton_toggled(bool checked)
{
    if (checked)
    {
        videoLoopsEdit->setValue(0);
        videoLoopsEdit->setEnabled(true);
    }
    else
    {
        videoLoopsEdit->setValue(-1);
        videoLoopsEdit->setEnabled(false);
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_videoLoopsEdit_valueChanged(int arg1)
{
    if (arg1 == -1) videoLoopsEdit->setSuffix(" No loop");
    else if (arg1 == 0) videoLoopsEdit->setSuffix(" Infinite");
    else videoLoopsEdit->setSuffix(" loops");
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_startNumberButton_clicked(bool checked)
{
    if (checked)
    {
        startNumberEdit->setValue(0);
        startNumberEdit->setEnabled(true);
    }
    else
    {
        startNumberEdit->setValue(0);
        startNumberEdit->setEnabled(false);
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_videoCodecsFilterBox_currentIndexChanged()
{
    ffmpeg_loadCodecs();
}

void UIOutputWidget::on_pixFmtFilterBox_currentIndexChanged()
{
    ffmpeg_loadPixFmts();
}

void UIOutputWidget::on_audioCodecsFilterBox_currentIndexChanged()
{
    ffmpeg_loadCodecs();
}

void UIOutputWidget::on_addParam_clicked()
{
    addNewParam();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_formatsBox_currentIndexChanged(int index)
{
    if (index == -1) return;
    _currentMuxer = _ffmpeg->muxer(formatsBox->currentData().toString());

    if (_freezeUI) return;
    _freezeUI = true;

    selectDefaultVideoCodec();

    selectDefaultAudioCodec();

    //UI

    //video
    if (_currentMuxer->isVideo())
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



    _freezeUI = false;

    updateOutputExtension(outputEdit->text());
    updateAudioVideoOptions();
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_formatsFilterBox_currentIndexChanged()
{
    ffmpeg_loadMuxers();
}

void UIOutputWidget::on_videoCodecButton_toggled(bool checked)
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

void UIOutputWidget::on_pixFmtButton_toggled(bool checked)
{
    if (checked)
    {
        pixFmtFilterBox->setItemText(0,"All bit depths");
        pixFmtWidget->setEnabled(true);
    }
    else
    {
        pixFmtFilterBox->setCurrentIndex(0);
        ffmpeg_loadPixFmts();
        pixFmtFilterBox->setItemText(0,"Default");
        pixFmtWidget->setEnabled(false);
    }
}

void UIOutputWidget::on_videoBitrateButton_toggled(bool checked)
{
    if (checked)
    {
        videoQualityButton->setChecked(false);
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
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_videoQualityButton_toggled(bool checked)
{
    if (checked)
    {
        videoBitrateButton->setChecked(false);
        videoQualityWidget->setEnabled(true);
        on_videoQualitySlider_valueChanged(videoQualitySlider->value());
    }
    else
    {
        videoQualityWidget->setEnabled(false);
        qualityLabel->setText("Excellent");
    }
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_audioCodecButton_toggled(bool checked)
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
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_audioBitrateButton_toggled(bool checked)
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
    if (!_loadingPreset) presetsBox->setCurrentIndex(0);
}

void UIOutputWidget::on_presetsBox_currentIndexChanged(int index)
{
    if (index == 0) return;
    if (_freezeUI) return;
    _freezeUI = true;

    //save
    if (index == presetsBox->count()-1)
    {
        presetsBox->setCurrentIndex(0);

        //ask for file
        QString saveFileName = QFileDialog::getSaveFileName(this,"Save output preset",settings.value("presets/path",QDir::homePath() + "/DuME Presets/").toString(),"DuME preset (*.dffp *.json);;All Files (*.*)");
        if (saveFileName == "")
        {
            _freezeUI = false;
            return;
        }
        //update infos
        getMediaInfo();
        //export
        _mediaInfo->exportPreset(saveFileName);
        //add to box
        _freezeUI = false;
        loadPresets();
    }
    //load
    else if (index == presetsBox->count()-2)
    {
        presetsBox->setCurrentIndex(0);

        QString openFileName = QFileDialog::getOpenFileName(this,"Load output preset",settings.value("presets/path",QDir::homePath() + "/DuME Presets/").toString(),"DuME preset (*.dffp *.json);;All Files (*.*)");
        if (openFileName == "")
        {
            _freezeUI = false;
            return;
        }
        _loadingPreset = true;
        _freezeUI = false;
        //load
        _mediaInfo->loadPreset(openFileName);
        _loadingPreset = false;
    }
    //load preset
    else
    {
        _loadingPreset = true;
        //load
        _freezeUI = false;
        //set filters to all
        formatsFilterBox->setCurrentIndex(0);
        videoCodecsFilterBox->setCurrentIndex(0);
        audioCodecsFilterBox->setCurrentIndex(0);

        _mediaInfo->loadPreset(presetsBox->currentData().toString());

        _loadingPreset = false;
    }

    _freezeUI = false;
}

void UIOutputWidget::on_presetsFilterBox_activated()
{
    loadPresets(QSettings().value("presets/path","").toString());
}

void UIOutputWidget::on_alphaButton_toggled(bool checked)
{
    ffmpeg_loadPixFmts(true);
    if (_inputHasAlpha && checked)
    {
        unmultButton->show();
    }
    else
    {
        unmultButton->hide();
    }
}

void UIOutputWidget::aspectRatio()
{
    double width = videoWidthButton->value();
    double height = videoHeightButton->value();
    double ratio =  width / height;
    //round it to 3 digits
    int roundedRatio = int(ratio*100+0.5);
    ratio = roundedRatio;
    ratio = ratio/100;
    aspectRatioLabel->setText(QString::number(ratio) + ":1");
}

void UIOutputWidget::updateOutputExtension(QString outputPath)
{
    if (outputPath == "") return;
    QFileInfo output(outputPath);

    //remove existing {#####}
    QString outputName = output.completeBaseName();
    outputName = outputName.replace(QRegularExpression("_?{#+}"),"");

    QString newExt = "";

    if (_currentMuxer != nullptr)
    {
        //add {#####}
        if (_currentMuxer->isSequence())
        {
            newExt = "_{#####}";
        }

        if (_currentMuxer->extensions().count() > 0)
        {
            newExt += "." + _currentMuxer->extensions()[0];
        }

    }
    QString num = "";
    if (_index > 1) num = "_" + QString::number(_index);
    outputName.replace(QRegularExpression(num + "$"),"");
    outputPath = output.path() + "/" + outputName + num + newExt;
    //check if file exists
    if (QFile(outputPath).exists())
    {
        outputPath = output.path() + "/" + outputName + "_transcoded" + num + newExt;
    }

    outputEdit->setText(QDir::toNativeSeparators(outputPath));
}

void UIOutputWidget::selectDefaultVideoCodec()
{
    if (_currentMuxer == nullptr) return;

    FFCodec *videoCodec = _ffmpeg->muxerDefaultCodec(_currentMuxer, FFCodec::Video);

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

void UIOutputWidget::selectDefaultAudioCodec()
{
    if (_currentMuxer == nullptr) return;

    FFCodec *audioCodec = _ffmpeg->muxerDefaultCodec(_currentMuxer, FFCodec::Audio);

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
}

void UIOutputWidget::selectDefaultPixFmt()
{
    if (_currentVideoCodec == nullptr) return;

    FFPixFormat *pixFmt = _currentVideoCodec->defaultPixFormat();

    //Select Default Pix Format

    bool ok = false;
    if (pixFmt != nullptr)
    {
        for (int i = 0; i < pixFmtBox->count() ; i++)
        {
            if (pixFmtBox->itemData(i).toString() == pixFmt->name())
            {
                pixFmtBox->setCurrentIndex(i);
                ok = true;
                break;
            }
        }
        if (!ok)
        {
            int bpc = pixFmt->bitsPerComponent();
            for (int i = 0; i < pixFmtBox->count() ; i++)
            {
                if (pixFmtBox->itemText(i).indexOf("@" + QString::number(bpc) + "bpc") > 0)
                {
                    pixFmtBox->setCurrentIndex(i);
                    break;
                }
            }

        }
    }
}

void UIOutputWidget::updateAudioVideoOptions()
{
    //VIDEO
    mainVideoCodecWidget->hide();
    resizeWidget->hide();
    frameRateWidget->hide();
    sequenceWidget->hide();
    componentsWidget->hide();
    //AUDIO
    samplingWidget->hide();
    mainAudioCodecWidget->hide();

    //hide all details

    //VIDEO
    //codec
    videoCodecButton->hide();
    videoCodecWidget->hide();
    //bitrate
    videoBitrateButton->hide();
    videoBitRateEdit->hide();
    //quality
    videoQualityButton->hide();
    videoQualityWidget->hide();
    //profile
    videoProfileButton->hide();
    videoProfileBox->hide();

    //loop
    videoLoopsButton->hide();
    videoLoopsEdit->hide();
    //start number
    startNumberButton->hide();
    startNumberEdit->hide();

    //frame rate
    frameRateButton->hide();
    frameRateBox->hide();
    frameRateEdit->hide();

    //unmult
    unmultButton->hide();

    //AUDIO
    //codec
    audioCodecButton->hide();
    audioCodecWidget->hide();
    //bitrate
    audioBitrateButton->hide();
    audioBitRateEdit->hide();

    if (videoTranscodeButton->isChecked())
    {
        resizeWidget->show();

        //show/hide codec depending on muxer
        FFMuxer *muxer = _ffmpeg->muxer(formatsBox->currentData().toString());
        if (muxer != nullptr)
        {
            if (!muxer->isSequence() && muxer->name() != "gif")
            {
                mainVideoCodecWidget->show();
                videoCodecButton->show();
                videoCodecWidget->show();
                videoBitrateButton->show();
                videoBitRateEdit->show();
            }

            if (muxer->isSequence())
            {
                sequenceWidget->show();
                startNumberButton->show();
                startNumberEdit->show();
            }
            else
            {
                frameRateWidget->show();
                frameRateBox->show();
                frameRateButton->show();
                frameRateEdit->show();
            }
        }

        FFCodec *codec = _ffmpeg->videoEncoder(videoCodecsBox->currentData().toString());
        if (codec != nullptr)
        {
            //prores
            if (codec->name() == "prores")
            {
                mainVideoCodecWidget->show();
                videoProfileBox->clear();
                videoProfileBox->addItem("Proxy",0);
                videoProfileBox->addItem("LT",1);
                videoProfileBox->addItem("Normal",2);
                videoProfileBox->addItem("HQ",3);
                videoProfileButton->show();
                videoProfileBox->show();
            }
            //h264
            else if (codec->name() == "h264")
            {
                mainVideoCodecWidget->show();
                videoQualityButton->show();
                videoQualityWidget->show();
            }
            //gif
            else if (codec->name() == "gif")
            {
                sequenceWidget->show();
                videoLoopsButton->show();
                videoLoopsEdit->show();
            }
        }
    }

    if (audioTranscodeButton->isChecked())
    {
        //show/hide bitrate depending on muxer
        FFMuxer *muxer = _ffmpeg->muxer(formatsBox->currentData().toString());
        if (muxer != nullptr)
        {
            if (!muxer->isSequence() && muxer->name() != "gif")
            {
                mainAudioCodecWidget->show();
                samplingWidget->show();
                audioCodecButton->show();
                audioCodecWidget->show();
                audioBitrateButton->show();
                audioBitRateEdit->show();
            }
        }
    }

    //uncheck what is hidden
    if (videoCodecButton->isHidden()) videoCodecButton->setChecked(false);
    if (videoBitrateButton->isHidden()) videoBitrateButton->setChecked(false);
    if (videoQualityButton->isHidden()) videoQualityButton->setChecked(false);
    if (videoProfileButton->isHidden()) videoProfileButton->setChecked(false);
    if (videoLoopsButton->isHidden()) videoLoopsButton->setChecked(false);
    if (audioCodecButton->isHidden()) audioCodecButton->setChecked(false);
    if (audioBitrateButton->isHidden()) audioBitrateButton->setChecked(false);
    if (frameRateButton->isHidden()) frameRateButton->setChecked(false);
    if (startNumberButton->isHidden()) startNumberButton->setChecked(false);
    if (pixFmtButton->isHidden()) pixFmtButton->setChecked(false);
    if (unmultButton->isHidden()) unmultButton->setChecked(false);
}

void UIOutputWidget::addNewParam(QString name, QString value)
{
    //add a param and a value
    QLineEdit *customParam = new QLineEdit(this);
    customParam->setPlaceholderText("-param");
    customParam->setText(name);
    customParam->setMinimumWidth(100);
    customParam->setMaximumWidth(100);
    //the value edit
    QLineEdit *customValue = new QLineEdit(this);
    customValue->setPlaceholderText("Value");
    customValue->setText(value);
    //add to layout and lists
    customOptionsLayout->insertRow(1,customParam,customValue);
    _customParamEdits << customParam;
    _customValueEdits << customValue;
}

void UIOutputWidget::ffmpeg_init()
{
    _freezeUI = true;
    _currentMuxer = nullptr;
    _mediaInfo->reInit( false );
    ffmpeg_loadCodecs();
    ffmpeg_loadMuxers();
    _freezeUI = false;
}

void UIOutputWidget::ffmpeg_loadCodecs()
{
    _freezeUI = true;
    //get codecs and muxers
    videoCodecsBox->clear();
    audioCodecsBox->clear();
    QList<FFCodec *> encoders = _ffmpeg->encoders();
    if (encoders.count() == 0)
    {
        _freezeUI = false;
        return;
    }

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
    selectDefaultAudioCodec();
    _freezeUI = false;
}

void UIOutputWidget::ffmpeg_loadMuxers()
{
    _freezeUI = true;

    formatsBox->clear();

    QList<FFMuxer *> muxers = _ffmpeg->muxers();
    if (muxers.count() == 0) return;

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

void UIOutputWidget::ffmpeg_loadPixFmts(bool init)
{
    _freezeUI = true;

    //get pixFmts
    pixFmtBox->clear();
    if (_currentVideoCodec == nullptr)
    {
        _freezeUI = false;
        return;
    }
    QList<FFPixFormat *> pixFmts = _currentVideoCodec->pixFormats();
    if (pixFmts.count() == 0)
    {
        _freezeUI = false;
        return;
    }

    QStringList filters = QStringList();

    foreach (FFPixFormat *pixFmt, pixFmts)
    {

        QString filter = QString::number(pixFmt->bitsPerPixel()) + "bits (" + QString::number(pixFmt->numComponents()) + " channels @" + QString::number(pixFmt->bitsPerComponent()) + "bpc)" ;
        bool addToList = pixFmt->prettyName().indexOf(pixFmtFilterBox->currentText()) > 0 || pixFmtFilterBox->currentIndex() == 0;

        //check if there is alpha
        if (alphaButton->isVisible())
        {
            if (pixFmt->hasAlpha() && alphaButton->isChecked())
            {
                //add pix fmt in list
                if (addToList) pixFmtBox->addItem(pixFmt->prettyName(),QVariant(pixFmt->name()));
                if (!filters.contains(filter)) filters << filter;
            }
            else if (!pixFmt->hasAlpha() && !alphaButton->isChecked())
            {
                //add pix fmt in list
                if (addToList) pixFmtBox->addItem(pixFmt->prettyName(),QVariant(pixFmt->name()));
                if (!filters.contains(filter)) filters << filter;
            }
        }
        else
        {
            if (addToList) pixFmtBox->addItem(pixFmt->prettyName(),QVariant(pixFmt->name()));
            if (!filters.contains(filter)) filters << filter;
        }
    }

    if (init)
    {
        filters.sort();
        //remove old
        pixFmtFilterBox->clear();
        pixFmtFilterBox->addItem("All bit depths",QVariant(0));
        foreach(QString filter,filters)
        {
            pixFmtFilterBox->addItem(filter);
        }
    }

    selectDefaultPixFmt();
    _freezeUI = false;
}

void UIOutputWidget::newInputMedia(MediaInfo *input)
{
    //set output fileName
    QFileInfo inputFile(input->fileName());
    QString outputPath = inputFile.path() + "/" + inputFile.completeBaseName();
    updateOutputExtension(outputPath);

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

    if (input->pixFormat() != nullptr)
    {
        if (input->pixFormat()->hasAlpha())
        {
            _inputHasAlpha = true;
            if (alphaButton->isChecked()) unmultButton->show();
            else unmultButton->hide();
        }
        else
        {
            _inputHasAlpha = false;
            unmultButton->hide();
        }
    }
    else
    {
        _inputHasAlpha = false;
        unmultButton->hide();
    }

    if (input->isAep())
    {
        _inputHasAlpha = true;
        if (alphaButton->isChecked()) unmultButton->show();
        else unmultButton->hide();
    }

    //If ae render queue
    if (input->isAep() && input->aeUseRQueue())
    {
        this->hide();
    }
    else
    {
        this->show();
    }
}

void UIOutputWidget::loadPresets()
{
    if (_freezeUI) return;
    _freezeUI = true;

    presetsBox->clear();
    int index = presetsFilterBox->currentIndex();

    QStringList presets;
    //list internal
    if (index == 0 || index == 1)
    {
        foreach(QString preset, QDir(":/presets/").entryList(QDir::Files))
        {
            presets << ":/presets/" + preset;
        }
    }
    //list users
    if (index == 0 || index == 2)
    {
        QString userPresetsPath = settings.value("presets/path",QDir::homePath() + "/DuFFmpeg Presets/").toString();
        QStringList filters("*.dffp");
        filters << "*.json";
        foreach (QString preset, QDir(userPresetsPath).entryList(filters,QDir::Files))
        {
            presets << userPresetsPath + "/" + preset;
        }
    }

    presets.sort();
    //add custom
    presetsBox->addItem("Custom");
    foreach(QString preset,presets)
    {
        presetsBox->addItem(QFileInfo(preset).completeBaseName(),preset);
    }
    //add load ans save
    presetsBox->addItem("Load...");
    presetsBox->addItem("Save as...");

    _freezeUI = false;
}
