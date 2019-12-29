#include "blockvideoprofile.h"

BlockVideoProfile::BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
}

void BlockVideoProfile::setActivated(bool activate)
{
    if (activate)
    {
        _mediaInfo->setVideoProfile( videoProfileBox->currentIndex() );
    }
    else
    {
        _mediaInfo->setVideoProfile( -1 );
    }
}

void BlockVideoProfile::update()
{
    videoProfileBox->setCurrentIndex( _mediaInfo->videoProfile() );
}

void BlockVideoProfile::on_videoProfileBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _mediaInfo->setVideoProfile( index );
}
