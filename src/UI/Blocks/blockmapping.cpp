#include "blockmapping.h"
#include "ui_blockmapping.h"

BlockMapping::BlockMapping(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) :
    UIBlockContent(mediaInfo,inputMedias,parent)
{
    setupUi(this);
}

void BlockMapping::setActivated(bool activate)
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

void BlockMapping::on_addButton_clicked()
{
    addStreamWidget();
    _mediaInfo->addMap( _streamWidgets.last()->mediaId(), _streamWidgets.last()->streamId() );
}

void BlockMapping::changeStream(int index, int streamId)
{
    _mediaInfo->maps()[index].setStreamId(streamId);
}

void BlockMapping::changeMedia(int index, int mediaId)
{
    _mediaInfo->maps()[index].setMediaId(mediaId);
}

void BlockMapping::addStreamWidget(int mediaId, int streamId)
{
    StreamReferenceWidget *sw = new StreamReferenceWidget( _streamWidgets.count(), _inputMedias, this );
    //TODO add medias
    //TODO connect medias added/removed
    sw->setMediaId( mediaId );
    sw->setStreamId( streamId );
    connect( sw, SIGNAL(removed(int)), this, SLOT (removeStreamWidget(int)));
    connect( sw, SIGNAL(mediaChanged(int, int)), this, SLOT(changeMedia(int, int)));
    connect( sw, SIGNAL(streamChanged(int, int)), this, SLOT(changeStream(int, int)));
    _streamWidgets << sw;
    mainLayout->addWidget(sw);
}

void BlockMapping::removeStreamWidget(int id)
{
    StreamReferenceWidget *sw = _streamWidgets.takeAt(id);
    mainLayout->removeWidget(sw);
    sw->deleteLater();
    _mediaInfo->removeMap(id);
}
