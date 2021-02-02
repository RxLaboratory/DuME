#include "blockvideobitrate.h"

BlockVideoBitrate::BlockVideoBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setType(Type::Video);
    setupUi(this);

    bitrateTypeBox->addItem("VBR");
    bitrateTypeBox->addItem("CBR");

    _presets->addAction( actionPerfect_95 );
    _presets->addAction( actionAuto );
    _presets->addAction( actionGood_60 );
    _presets->addAction( actionProxy_30 );
    _presets->addAction( actionBad_20 );
    _presets->addAction( actionBlu_Ray );
    _presets->addAction( actionStreaming_12_Mbps );
    _presets->addAction( actionDVD );

    qualitySlider = new SliderSpinBox();
    qualitySlider->setSuffix("%");
    qualityLayout->insertWidget(0,qualitySlider);
    speedSlider = new SliderSpinBox();
    speedSlider->showValue(false);
    speedLayout->addWidget(speedSlider);

    connect(qualitySlider, &SliderSpinBox::sliderMoved, this, &BlockVideoBitrate::on_videoQualitySlider_sliderMoved);
    connect(speedSlider, &SliderSpinBox::sliderMoved, this, &BlockVideoBitrate::on_speedSlider_sliderMoved);
}

void BlockVideoBitrate::activate(bool activate)
{
    if (activate && videoBitrateButton->isChecked()) _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( videoBitRateEdit->value(), MediaUtils::Mbps ) );
    else _mediaInfo->setVideoBitrate( 0 );

    if (activate) _mediaInfo->setVideoBitrateType( bitrateTypeBox->currentText() );
    else _mediaInfo->setVideoBitrateType( "VBR" );

    if (activate && videoQualityButton->isChecked()) _mediaInfo->setVideoQuality( qualitySlider->value() );
    else _mediaInfo->setVideoQuality( -1 );

    if (activate && speedButton->isChecked()) _mediaInfo->setVideoEncodingSpeed( speedSlider->value());
    else _mediaInfo->setVideoEncodingSpeed( -1 );

    if (activate) _mediaInfo->setVideoTuning( tuneBox->currentData().toString() );
    else _mediaInfo->setVideoTuning( "" );
}

void BlockVideoBitrate::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];

    FFCodec *c = stream->codec();
    if ( c->name() == "" ) c = _mediaInfo->defaultVideoCodec();

    FFMuxer *m = _mediaInfo->muxer();
    if (m->name() == "gif")
    {
        emit blockEnabled(false);
        return;
    }

    bool useQuality = c->useQuality();
    bool useBitrate = !m->isSequence();
    bool useSpeed = c->useSpeed();
    bool useTuning = c->useTuning();
    bool useType = c->useBitrateType();

    actionPerfect_95->setVisible( useQuality );
    actionAuto->setVisible( useQuality );
    actionGood_60->setVisible( useQuality );
    actionProxy_30->setVisible( useQuality );
    actionBad_20->setVisible( useQuality );
    actionBlu_Ray->setVisible( useBitrate );
    actionStreaming_12_Mbps->setVisible( useBitrate );
    actionDVD->setVisible( useBitrate );

    // bitrate
    if ( useBitrate )
    {
        bitrateWidget->show();
        videoBitrateButton->show();
        int b = stream->bitrate( );
        videoBitRateEdit->setEnabled( true );
        if ( b == 0 )
        {
            videoBitRateEdit->setValue( 0 ) ;
            videoBitRateEdit->setSuffix( " Auto" );
            if (useQuality)
            {
                bitrateWidget->setEnabled( false );
                videoBitrateButton->setChecked( false );
            }
        }
        else
        {
            videoBitRateEdit->setValue( MediaUtils::convertFromBps( b, MediaUtils::Mbps ) );
            videoBitRateEdit->setSuffix( " Mbps" );
            videoBitrateButton->setChecked( true );
            bitrateWidget->setEnabled( true );
        }

        if (useType)
        {
            bitrateTypeBox->show();
            if (stream->bitrateType() == MediaUtils::BitrateType::VBR ) bitrateTypeBox->setCurrentText("VBR");
            else bitrateTypeBox->setCurrentText("CBR");
        }
        else
        {
            bitrateTypeBox->hide();
        }
    }
    else
    {
        bitrateWidget->hide();
        videoBitrateButton->hide();
    }

    // quality
    if ( useQuality )
    {
        videoQualityButton->show();
        videoQualityWidget->show();
        // quality
        int q = stream->quality();
        if (q == -1)
        {
            qualitySlider->setValue( 90 );
            qualitySlider->setPrefix("Excellent");
            qualitySlider->showValue(false);
            videoQualityButton->setChecked( false );
            videoQualityWidget->setEnabled( false );
        }
        else
        {
            videoQualityButton->setChecked( true );
            qualitySlider->setValue( q );
            qualitySlider->showValue(true);
            videoQualityWidget->setEnabled( true );

            if (q == 100) qualitySlider->setPrefix("Lossless (if possible) | ");
            else if (q >= 90) qualitySlider->setPrefix("Perfect | ");
            else if (q >= 75) qualitySlider->setPrefix("Good | ");
            else if (q >= 50) qualitySlider->setPrefix("Fair | ");
            else if (q >= 25) qualitySlider->setPrefix("Bad | ");
            else qualitySlider->setPrefix("Very bad | ");
        }
    }
    else
    {
        videoQualityButton->hide();
        videoQualityWidget->hide();
    }

    // speed
    if ( useSpeed )
    {
        int s = stream->encodingSpeed();
        speedButton->setVisible( true );
        speedWidget->setVisible( true );
        if (s == -1 )
        {
            speedWidget->setEnabled( false );
            speedSlider->setPrefix("Default - Medium");
            speedButton->setChecked( false );
            speedSlider->setValue( 35 );
        }
        else
        {
            speedWidget->setEnabled( true );
            speedButton->setChecked( true );
            if (s >= 90) speedSlider->setPrefix("Ultra fast");
            else if ( s >= 80 ) speedSlider->setPrefix("Super fast");
            else if ( s >= 70 ) speedSlider->setPrefix("Very fast");
            else if ( s >= 60 ) speedSlider->setPrefix("Faster");
            else if ( s >= 50 ) speedSlider->setPrefix("Fast");
            else if ( s >= 40 ) speedSlider->setPrefix("Medium");
            else if ( s >= 30 ) speedSlider->setPrefix("Slow");
            else if ( s >= 20 ) speedSlider->setPrefix("Slower");
            else speedSlider->setPrefix("Very Slow");
            speedSlider->setValue( s );
        }
    }
    else
    {
        speedButton->setVisible( false );
        speedWidget->setVisible( false );
    }

    // tuning
    if (useTuning)
    {
        tuneBox->clear();
        foreach (FFBaseObject *t, c->tunings())
        {
            tuneBox->addItem(t->prettyName(), t->name());
        }
        tuneBox->setCurrentIndex(0);
        tuneBox->setCurrentData(stream->tuning()->name());
        tuneBox->setVisible(true);
        tuneLabel->setVisible(true);
    }
    else
    {
        tuneBox->setVisible(false);
        tuneLabel->setVisible(false);
        tuneBox->setCurrentData("");
    }

    // lossless && intra
    if (c->name() == "h265" || c->name() == "libx265" || c->name() == "hevc" || c->name() == "h264" || c->name() == "libx264")
    {
        losslessButton->show();
        intraButton->show();
    }
    else
    {
        losslessButton->hide();
        intraButton->hide();
    }
}

void BlockVideoBitrate::on_videoBitrateButton_clicked(bool checked)
{
    if (_freezeUI) return;
    if (checked)
    {
        _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 24, MediaUtils::Mbps ) );
        _mediaInfo->setVideoQuality( -1 );
    }
    else
    {
        _mediaInfo->setVideoBitrate( 0 );
    }
}

void BlockVideoBitrate::on_videoBitRateEdit_editingFinished()
{
    if (_freezeUI) return;
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( videoBitRateEdit->value(), MediaUtils::Mbps ) );
}

void BlockVideoBitrate::on_videoQualityButton_clicked(bool checked)
{
    if (_freezeUI) return;
    if (checked)
    {
        _mediaInfo->setVideoQuality( 90 );
        _mediaInfo->setVideoBitrate( 0 );
    }
    else
    {
        _mediaInfo->setVideoQuality( -1 );
    }
}

void BlockVideoBitrate::on_videoQualitySlider_sliderMoved(int value)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoQuality( value );
}

void BlockVideoBitrate::on_speedButton_clicked(bool checked)
{
    if (_freezeUI) return;
    if (checked)
    {
        _mediaInfo->setVideoEncodingSpeed( 35 );
    }
    else
    {
        _mediaInfo->setVideoEncodingSpeed( -1 );
    }
}

void BlockVideoBitrate::on_speedSlider_sliderMoved(int value)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoEncodingSpeed( value );
}

void BlockVideoBitrate::on_tuneBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoTuning( tuneBox->itemData(index).toString());
}

void BlockVideoBitrate::on_bitrateTypeBox_currentIndexChanged(const QString &arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoBitrateType(arg1);
}

void BlockVideoBitrate::on_actionPerfect_95_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 95 );
    _mediaInfo->setVideoEncodingSpeed( 25 );
    _mediaInfo->setVideoTuning("");
}

void BlockVideoBitrate::on_actionAuto_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( -1 );
    _mediaInfo->setVideoEncodingSpeed( -1 );
    _mediaInfo->setVideoTuning("");
}

void BlockVideoBitrate::on_actionGood_60_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 60 );
    _mediaInfo->setVideoEncodingSpeed( -1 );
    _mediaInfo->setVideoTuning("");
}

void BlockVideoBitrate::on_actionProxy_30_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 30 );
    _mediaInfo->setVideoTuning("fastdecode");
    _mediaInfo->setVideoEncodingSpeed( -1 );
}

void BlockVideoBitrate::on_actionBad_20_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 20 );
    _mediaInfo->setVideoEncodingSpeed( 95 );
    _mediaInfo->setVideoTuning("");
}

void BlockVideoBitrate::on_actionBlu_Ray_triggered()
{
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 25, MediaUtils::Mbps ) );
    _mediaInfo->setVideoQuality( -1 );
    _mediaInfo->setVideoEncodingSpeed( -1 );
    _mediaInfo->setVideoTuning("");
}

void BlockVideoBitrate::on_actionDVD_triggered()
{
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 7, MediaUtils::Mbps ) );
    _mediaInfo->setVideoQuality( -1 );
    _mediaInfo->setVideoEncodingSpeed( -1 );
    _mediaInfo->setVideoTuning("");
}

void BlockVideoBitrate::on_actionStreaming_12_Mbps_triggered()
{
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 12, MediaUtils::Mbps ) );
    _mediaInfo->setVideoQuality( -1 );
    _mediaInfo->setVideoTuning("zerolatency");
    _mediaInfo->setVideoEncodingSpeed( -1 );
}

void BlockVideoBitrate::on_losslessButton_clicked(bool checked)
{
    qualitySlider->setEnabled(!checked);
    if (_freezeUI) return;
    if (checked) _mediaInfo->setVideoQuality( 100 );
    else _mediaInfo->setVideoQuality( qualitySlider->value() );
    _mediaInfo->setLossless( checked );
}

void BlockVideoBitrate::on_intraButton_clicked(bool checked)
{
    if (_freezeUI) return;
    _mediaInfo->setIntra(checked);
}
