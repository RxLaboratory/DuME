#include "blockmapping.h"
#include "ui_blockmapping.h"

BlockMapping::BlockMapping(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) :
    BlockContentWidget(mediaInfo,inputMedias,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Mapping block";
#endif
    setupUi(this);

    _presets->addAction( actionAdd );
}

void BlockMapping::activate(bool activate)
{
    _freezeUI = true;
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
    _freezeUI = false;
}

void BlockMapping::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    qDeleteAll(_streamWidgets);
    _streamWidgets.clear();

    QList<StreamReference> maps = _mediaInfo->maps();

    foreach( StreamReference map, maps)
    {
        addStreamWidget(map.mediaId(), map.streamId());
    }

    _freezeUI = false;
}

void BlockMapping::changeStream(int index, int streamId)
{
    if (_freezeUI) return;
    _freezeUI = true;
    _mediaInfo->setMapStream(index, streamId);
    _freezeUI = false;
}

void BlockMapping::changeMedia(int index, int mediaId)
{
    if (_freezeUI) return;
    _freezeUI = true;
    _mediaInfo->setMapMedia(index, mediaId);
    _freezeUI = false;
}

void BlockMapping::addStreamWidget(int mediaId, int streamId)
{
    _freezeUI = true;
    StreamReferenceWidget *sw = new StreamReferenceWidget( _streamWidgets.count(), _inputMedias, this );
    sw->setMediaId( mediaId );
    sw->setStreamId( streamId );
    connect( sw, SIGNAL(removed(int)), this, SLOT (removeStreamWidget(int)));
    connect( sw, SIGNAL(mediaChanged(int, int)), this, SLOT(changeMedia(int, int)));
    connect( sw, SIGNAL(streamChanged(int, int)), this, SLOT(changeStream(int, int)));
    _streamWidgets << sw;
    mainLayout->addWidget(sw);
    _freezeUI = false;
}

void BlockMapping::removeStreamWidget(int id)
{
    _freezeUI = true;
    StreamReferenceWidget *sw = _streamWidgets.takeAt(id);
    mainLayout->removeWidget(sw);
    sw->deleteLater();
    _mediaInfo->removeMap(id);
    _freezeUI = false;
}

void BlockMapping::on_actionAdd_triggered()
{
    addStreamWidget(-1, -1);
    _freezeUI = true;
    _mediaInfo->addMap( -1, -1 );
    _freezeUI = false;
}
