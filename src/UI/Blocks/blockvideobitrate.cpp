#include "blockvideobitrate.h"

BlockVideoBitrate::BlockVideoBitrate(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    _freezeUI = true;
    setupUi(this);
    _freezeUI = false;
}

void BlockVideoBitrate::setActivated(bool activate)
{
    _mediaInfo->setVideoBitrate( 0 );
    _mediaInfo->setVideoQuality( -1 );
}

void BlockVideoBitrate::update()
{
    _freezeUI = true;

    // bitrate
    int b = _mediaInfo->videoBitrate( );
    if ( b == 0)
    {
        videoBitRateEdit->setValue( 0 ) ;
        videoBitRateEdit->setEnabled( false );
        videoBitrateButton->setChecked( false );
    }
    else
    {
        videoBitRateEdit->setValue( MediaUtils::convertFromBps( b, MediaUtils::Mbps ) );
        videoBitRateEdit->setEnabled( true );
        videoBitrateButton->setChecked( true );
    }

    // quality
    int q = _mediaInfo->videoQuality();
    if (q == -1)
    {
        videoQualitySlider->setValue( 90 );
        qualityLabel->setText("Excellent");
        videoQualityButton->setChecked( false );
        videoQualityWidget->setEnabled(false);
    }
    else
    {
        videoQualityButton->setChecked( true );
        videoQualitySlider->setValue( q );
        videoQualityWidget->setEnabled( true );

        if (q >= 90)
        {
            qualityLabel->setText("(Visually) Lossless | " + QString::number(q) + "%");
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

    _freezeUI = false;
}

void BlockVideoBitrate::on_videoBitrateButton_clicked(bool checked)
{
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

void BlockVideoBitrate::on_videoBitRateEdit_valueChanged(double arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setVideoBitrate( arg1 );
}

void BlockVideoBitrate::on_videoQualityButton_clicked(bool checked)
{
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
