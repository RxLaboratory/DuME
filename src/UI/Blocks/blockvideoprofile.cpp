#include "blockvideoprofile.h"

BlockVideoProfile::BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
}

void BlockVideoProfile::activate(bool activate)
{
    _freezeUI = true;
    if (activate)
    {
        _mediaInfo->setVideoProfile( videoProfileBox->currentData().toString() );
        _mediaInfo->setVideoLevel( videoLevelBox->currentData().toString() );
    }
    else
    {
        _mediaInfo->setVideoProfile( nullptr );
        _mediaInfo->setVideoLevel( "" );
    }
    _freezeUI = false;
}

void BlockVideoProfile::update()
{
    if(_freezeUI) return;
    _freezeUI = true;

    //Update profiles list
    videoProfileBox->clear();
    videoLevelBox->clear();

    if (!_mediaInfo->hasVideo() || _mediaInfo->copyVideo())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    FFCodec *c = _mediaInfo->videoCodec();
    if ( c == nullptr ) c = _mediaInfo->defaultVideoCodec();
    if ( c == nullptr )
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    // add levels
    if ( c->name() == "h264" )
    {
        videoLevelBox->addItem("Default", "");
        videoLevelBox->addItem("3.0", "3.0");
        videoLevelBox->addItem("3.1", "3.1");
        videoLevelBox->addItem("4.0", "4.0");
        videoLevelBox->addItem("4.1", "4.1");
        videoLevelBox->addItem("4.2", "4.2");

        videoLevelBox->show();
    }
    else videoLevelBox->hide();

    // add profiles
    if ( c->profiles().count() > 0 )
    {
        videoProfileBox->addItem("Default", "");
        foreach( FFProfile *p, c->profiles() )
        {
            videoProfileBox->addItem( p->prettyName(), p->name() );
        }

        emit blockEnabled(true);
    }
    else
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    FFProfile *p = _mediaInfo->videoProfile();
    if ( p != nullptr )
    {
        for (int i = 0; i < videoProfileBox->count(); i++)
        {
            if ( p->name() == videoProfileBox->itemData(i).toString() )
            {
                videoProfileBox->setCurrentIndex(i);
                break;
            }
        }
    }
    else
    {
        videoProfileBox->setCurrentIndex( 0 );
    }


    for (int i = 0; i < videoLevelBox->count(); i++)
    {
        if ( _mediaInfo->videoLevel() == videoLevelBox->itemData(i).toString() )
        {
            videoLevelBox->setCurrentIndex(i);
            break;
        }
    }
    _freezeUI = false;
}

void BlockVideoProfile::on_videoProfileBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _freezeUI = true;
    _mediaInfo->setVideoProfile( videoProfileBox->itemData(index).toString() );
    _freezeUI = false;
}

void BlockVideoProfile::on_videoLevelBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _freezeUI = true;
    _mediaInfo->setVideoLevel( videoLevelBox->itemData(index).toString() );
    _freezeUI = false;
}
