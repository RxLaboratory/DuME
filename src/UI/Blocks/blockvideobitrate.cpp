#include "blockvideobitrate.h"

BlockVideoBitrate::BlockVideoBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);

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

    if (activate)
    {
        if (videoBitrateButton->isChecked())
        {
            _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( videoBitRateEdit->value(), MediaUtils::Mbps ) );
        }
        else
        {
            _mediaInfo->setVideoBitrate( 0 );
        }

        if (videoQualityButton->isChecked())
        {
            _mediaInfo->setVideoQuality( videoQualitySlider->value() );
        }
        else
        {
            _mediaInfo->setVideoQuality( -1 );
        }
    }
    else
    {
        _mediaInfo->setVideoBitrate( 0 );
        _mediaInfo->setVideoQuality( -1 );
    }

    _freezeUI = false;
}

void BlockVideoBitrate::update()
{
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

    actionPerfect_95->setVisible( useQuality );
    actionAuto->setVisible( useQuality );
    actionGood_60->setVisible( useQuality );
    actionProxy_30->setVisible( useQuality );
    actionBad_20->setVisible( useQuality );
    actionBlu_Ray->setVisible( useBitrate );
    actionStreaming_12_Mbps->setVisible( useBitrate );
    actionDVD->setVisible( useBitrate );

    //Display
    if ( useBitrate && useQuality )
    {
        videoBitRateEdit->show();
        videoBitrateButton->show();
        videoBitrateButton->setEnabled( true );

        videoQualityButton->show();
        videoQualityWidget->show();
        videoQualityButton->setEnabled( true );
    }
    else if (useQuality)
    {
        videoBitRateEdit->hide();
        videoBitrateButton->hide();

        videoQualityButton->show();
        videoQualityWidget->show();
        videoQualityButton->setEnabled( false );
    }
    else if (useBitrate)
    {
        videoBitRateEdit->show();
        videoBitrateButton->show();
        videoBitrateButton->setEnabled( false );

        videoQualityButton->hide();
        videoQualityWidget->hide();
    }
    else
    {
        videoBitRateEdit->hide();
        videoBitrateButton->hide();
        videoQualityButton->hide();
        videoQualityWidget->hide();
        return;
    }

    // bitrate
    if ( useBitrate )
    {
        int b = stream->bitrate( );
        videoBitRateEdit->setEnabled( true );
        if ( b == 0 )
        {
            videoBitRateEdit->setValue( 0 ) ;
            videoBitRateEdit->setSuffix( " Auto" );
            if (useQuality)
            {
                videoBitRateEdit->setEnabled( false );
                videoBitrateButton->setChecked( false );
            }
        }
        else
        {
            videoBitRateEdit->setValue( MediaUtils::convertFromBps( b, MediaUtils::Mbps ) );
            videoBitRateEdit->setSuffix( " Mbps" );
            videoBitrateButton->setChecked( true );
        }
    }

    if ( useQuality )
    {
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

            if (q == 100)
            {
                qualityLabel->setText("Lossless (if possible) | 100%");
            }
            else if (q >= 90)
            {
                qualityLabel->setText("Perfect | " + QString::number(q) + "%");
            }
            else if (q >= 75)
            {
                qualityLabel->setText("Very good | " + QString::number(q) + "%");
            }
            else if (q >= 50)
            {
                qualityLabel->setText("Good | " + QString::number(q) + "%");
            }
            else if (q >= 25)
            {
                qualityLabel->setText("Bad | " + QString::number(q) + "%");
            }
            else
            {
                qualityLabel->setText("Very bad | " + QString::number(q) + "%");
            }
        }
    }

    _freezeUI = false;
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

void BlockVideoBitrate::on_videoQualitySlider_valueChanged(int value)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoQuality( value );
}

void BlockVideoBitrate::on_actionPerfect_95_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 95 );
}

void BlockVideoBitrate::on_actionAuto_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( -1 );
}

void BlockVideoBitrate::on_actionGood_60_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 60 );
}

void BlockVideoBitrate::on_actionProxy_30_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 30 );
}

void BlockVideoBitrate::on_actionBad_20_triggered()
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( 20 );
}

void BlockVideoBitrate::on_actionBlu_Ray_triggered()
{
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 25, MediaUtils::Mbps ) );
    _mediaInfo->setVideoQuality( -1 );
}

void BlockVideoBitrate::on_actionDVD_triggered()
{
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 7, MediaUtils::Mbps ) );
    _mediaInfo->setVideoQuality( -1 );
}

void BlockVideoBitrate::on_actionStreaming_12_Mbps_triggered()
{
    _mediaInfo->setVideoBitrate( MediaUtils::convertToBps( 12, MediaUtils::Mbps ) );
    _mediaInfo->setVideoQuality( -1 );
}
