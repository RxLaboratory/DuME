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
    //Update profiles list
    videoProfileBox->clear();

    FFCodec *c = _mediaInfo->videoCodec();
    if ( c == nullptr ) c = _mediaInfo->defaultVideoCodec();
    if ( c == nullptr ) return;

    if ( c->name() == "prores" )
    {
        videoProfileBox->addItem("Proxy", 0);
        videoProfileBox->addItem("LT", 1);
        videoProfileBox->addItem("Normal", 2);
        videoProfileBox->addItem("HQ", 3);
    }

    videoProfileBox->setCurrentIndex( _mediaInfo->videoProfile() );
}

void BlockVideoProfile::on_videoProfileBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _mediaInfo->setVideoProfile( index );
}
