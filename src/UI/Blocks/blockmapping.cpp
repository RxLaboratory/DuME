#include "blockmapping.h"
#include "ui_blockmapping.h"

BlockMapping::BlockMapping(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) :
    BlockContentWidget(mediaInfo,inputMedias,parent)
{
    setupUi(this);

    _presets->addAction( actionAdd );
}

void BlockMapping::activate(bool activate)
{
    if (activate)
    {
        foreach( StreamReferenceWidget *sw, _streamWidgets )
        {
            _mediaInfo->addMap( sw->mediaId(), sw->streamId() );
        }
    }
    else
    {
        _mediaInfo->removeAllMaps();
    }
}

void BlockMapping::update()
{
    qDeleteAll(_streamWidgets);
    _streamWidgets.clear();

    QList<StreamReference> maps = _mediaInfo->maps();

    foreach( StreamReference map, maps)
    {
        addStreamWidget(map.mediaId(), map.streamId());
    }
}

void BlockMapping::changeStream(int index, int streamId)
{
    if (_freezeUI) return;
    bool freeze  =_freezeUI;
    _freezeUI = true;
    _mediaInfo->setMapStream(index, streamId);
    _freezeUI = freeze;
}

void BlockMapping::changeMedia(int index, int mediaId)
{
    if (_freezeUI) return;
    bool freeze  =_freezeUI;
    _freezeUI = true;
    _mediaInfo->setMapMedia(index, mediaId);
    _freezeUI = freeze;
}

void BlockMapping::addStreamWidget(int mediaId, int streamId)
{
    bool freeze  =_freezeUI;
    _freezeUI = true;
    StreamReferenceWidget *sw = new StreamReferenceWidget( _streamWidgets.count(), _inputMedias, this );
    sw->setMediaId( mediaId );
    sw->setStreamId( streamId );
    connect( sw, SIGNAL(removed(int)), this, SLOT (removeStreamWidget(int)));
    connect( sw, SIGNAL(mediaChanged(int, int)), this, SLOT(changeMedia(int, int)));
    connect( sw, SIGNAL(streamChanged(int, int)), this, SLOT(changeStream(int, int)));
    _streamWidgets << sw;
    mainLayout->addWidget(sw);
    _freezeUI = freeze;
}

void BlockMapping::removeStreamWidget(int id)
{
    bool freeze  =_freezeUI;
    _freezeUI = true;
    StreamReferenceWidget *sw = _streamWidgets.takeAt(id);
    mainLayout->removeWidget(sw);
    sw->deleteLater();
    _mediaInfo->removeMap(id);
    _freezeUI = freeze;
}

void BlockMapping::on_actionAdd_triggered()
{
    addStreamWidget(-1, -1);
    bool freeze  =_freezeUI;
    _freezeUI = true;
    _mediaInfo->addMap( -1, -1 );
    _freezeUI = freeze;
}
