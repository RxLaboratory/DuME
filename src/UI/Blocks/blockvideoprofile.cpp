#include "blockvideoprofile.h"

BlockVideoProfile::BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Video Profile block";
#endif
    setType(Type::Video);
    setupUi(this);
}

void BlockVideoProfile::activate(bool activate)
{
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
}

void BlockVideoProfile::update()
{
    //Update profiles list
    videoProfileBox->clear();
    videoLevelBox->clear();

    VideoInfo *stream = _mediaInfo->videoStreams()[0];

    FFCodec *c = stream->codec();
    if ( c->name() == "" ) c = _mediaInfo->defaultVideoCodec();

    // add levels
    if ( c->useLevel() )
    {
        videoLevelBox->clear();
        foreach( FFBaseObject *l, c->levels() )
        {
            videoLevelBox->addItem( l->prettyName(), l->name() );
        }
        videoLevelBox->show();
    }
    else videoLevelBox->hide();

    // add profiles
    if ( c->useProfile() )
    {
        videoProfileBox->clear();
        foreach( FFBaseObject *p, c->profiles() )
        {
            videoProfileBox->addItem( p->prettyName(), p->name() );
        }
    }
    else
    {
        emit blockEnabled(false);
        return;
    }

    FFBaseObject *p = stream->profile();
    videoProfileBox->setCurrentData( p->name() );

    videoLevelBox->setCurrentData( stream->level() );
}

void BlockVideoProfile::on_videoProfileBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    bool freeze = _freezeUI;
    _freezeUI = true;
    _mediaInfo->setVideoProfile( videoProfileBox->itemData(index).toString() );
    _freezeUI = freeze;
}

void BlockVideoProfile::on_videoLevelBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    bool freeze = _freezeUI;
    _freezeUI = true;
    _mediaInfo->setVideoLevel( videoLevelBox->itemData(index).toString() );
    _freezeUI = freeze;
}
