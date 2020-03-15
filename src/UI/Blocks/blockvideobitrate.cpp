#include "blockvideobitrate.h"

BlockVideoBitrate::BlockVideoBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create video bitrate/quality block";
#endif
    setupUi(this);

    foreach (FFBaseObject *t, ffmpeg->tunings())
    {
        tuneBox->addItem(t->prettyName(), t->name());
    }
    tuneBox->setCurrentIndex(0);

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
}

void BlockVideoBitrate::activate(bool activate)
{
    _freezeUI = true;

    if (activate && videoBitrateButton->isChecked()) _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( videoBitRateEdit->value(), MediaUtils::Mbps ) );
    else _mediaInfo->setVideoBitrate( 0 );

    if (activate) _mediaInfo->setVideoBitrateType( bitrateTypeBox->currentText() );
    else _mediaInfo->setVideoBitrateType( "VBR" );

    if (activate && videoQualityButton->isChecked()) _mediaInfo->setVideoQuality( videoQualitySlider->value() );
    else _mediaInfo->setVideoQuality( -1 );

    if (activate && speedButton->isChecked()) _mediaInfo->setVideoEncodingSpeed( speedSlider->value());
    else _mediaInfo->setVideoEncodingSpeed( -1 );

    if (activate) _mediaInfo->setVideoTuning( tuneBox->currentData().toString() );
    else _mediaInfo->setVideoTuning( "" );

    _freezeUI = false;
}

void BlockVideoBitrate::update()
{
    qDebug() << "Update Video Bitrate Block";
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo() || _mediaInfo->isSequence())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    if (stream->isCopy())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    FFCodec *c = stream->codec();
    if ( c->name() == "" ) c = _mediaInfo->defaultVideoCodec();

    FFMuxer *m = _mediaInfo->muxer();
    if (m->name() == "gif")
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    emit blockEnabled(true);

    bool useQuality = c->qualityParam() != "";
    bool useBitrate = !m->isSequence();
    bool useSpeed = c->name() == "h264";
    bool useTuning = c->name() == "h264";
    bool useType = c->name() == "h264";

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
            videoQualitySlider->setValue( 90 );
            qualityLabel->setText("Excellent");
            videoQualityButton->setChecked( false );
            videoQualityWidget->setEnabled( false );
        }
        else
        {
            videoQualityButton->setChecked( true );
            videoQualitySlider->setValue( q );
            videoQualityWidget->setEnabled( true );

            if (q == 100) qualityLabel->setText("Lossless (if possible) | 100%");
            else if (q >= 90) qualityLabel->setText("Perfect | " + QString::number(q) + "%");
            else if (q >= 75) qualityLabel->setText("Very good | " + QString::number(q) + "%");
            else if (q >= 50) qualityLabel->setText("Good | " + QString::number(q) + "%");
            else if (q >= 25) qualityLabel->setText("Bad | " + QString::number(q) + "%");
            else qualityLabel->setText("Very bad | " + QString::number(q) + "%");
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
            speedLabel->setText("Default - Medium");
            speedButton->setChecked( false );
            speedSlider->setValue( 35 );
        }
        else
        {
            speedWidget->setEnabled( true );
            speedButton->setChecked( true );
            if (s >= 90) speedLabel->setText("Ultra fast");
            else if ( s >= 80 ) speedLabel->setText("Super fast");
            else if ( s >= 70 ) speedLabel->setText("Very fast");
            else if ( s >= 60 ) speedLabel->setText("Faster");
            else if ( s >= 50 ) speedLabel->setText("Fast");
            else if ( s >= 40 ) speedLabel->setText("Medium");
            else if ( s >= 30 ) speedLabel->setText("Slow");
            else if ( s >= 20 ) speedLabel->setText("Slower");
            else speedLabel->setText("Very Slow");
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

    _freezeUI = false;
    qDebug() << "Video Bitrate Block updated";
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
