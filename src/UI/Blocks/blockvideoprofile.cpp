#include "blockvideoprofile.h"

BlockVideoProfile::BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Video Profile block";
#endif
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
        _mediaInfo->setVideoProfile( "" );
        _mediaInfo->setVideoLevel( "" );
    }
    _freezeUI = false;
}

void BlockVideoProfile::update()
{
    qDebug() << "Update Video Profile Block";
    if(_freezeUI) return;
    _freezeUI = true;

    //Update profiles list
    videoProfileBox->clear();
    videoLevelBox->clear();

    if (!_mediaInfo->hasVideo())
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

    FFProfile *p = stream->profile();
    videoProfileBox->setCurrentData( p->name() );

    videoLevelBox->setCurrentData( stream->level() );

    _freezeUI = false;
    qDebug() << "Video Profile Block updated";
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
