#include "streamreferencewidget.h"

StreamReferenceWidget::StreamReferenceWidget(int id, MediaList *inputMedias, QWidget *parent) :
    QWidget(parent)
{
    _freezeUI = true;
    setupUi(this);
    _id = id;
    _freezeUI = false;
    _inputMedias = inputMedias;
    //populate
    foreach(MediaInfo *m, _inputMedias->medias())
    {
        addMedia(m);
    }
    connect( _inputMedias, SIGNAL(newMedia(MediaInfo*)), this, SLOT(addMedia(MediaInfo*)));
    connect( _inputMedias, SIGNAL(mediaRemoved(MediaInfo*)), this, SLOT(removeMedia(MediaInfo*)));
}

void StreamReferenceWidget::setStreamId(int id)
{
    _freezeUI = true;
    if (id == -1)
    {
        streamBox->setCurrentIndex(-1);
        _freezeUI = false;
        return;
    }

    for (int i = 0; i < streamBox->count(); i++)
    {
        if ( streamBox->itemData(i).toInt() == id)
        {
            streamBox->setCurrentIndex(i);
            _freezeUI = false;
            return;
        }
    }
}

void StreamReferenceWidget::setMediaId(int id)
{
    _freezeUI = true;
    if (id == -1)
    {
        mediaBox->setCurrentIndex(-1);
        updateStreamList();
        _freezeUI = false;
        return;
    }


    for (int i = 0; i < mediaBox->count(); i++)
    {
        if ( mediaBox->itemData(i).toInt() == id)
        {
            mediaBox->setCurrentIndex(i);
            updateStreamList();
            _freezeUI = false;
            return;
        }
    }
}

int StreamReferenceWidget::mediaId()
{
    return mediaBox->currentData().toInt();
}

int StreamReferenceWidget::streamId()
{
    return streamBox->currentData().toInt();
}

void StreamReferenceWidget::addMedia(MediaInfo *media)
{
    mediaBox->addItem( QFileInfo(media->fileName()).fileName(), media->id() );
    connect( media, SIGNAL(changed()), this, SLOT(mediaChanged()));
}

void StreamReferenceWidget::removeMedia(MediaInfo *media)
{
    for (int i = 0; i < mediaBox->count(); i++)
    {
        if (media->id() == mediaBox->itemData(i).toInt() )
        {
            mediaBox->removeItem(i);
            break;
        }
    }
}

void StreamReferenceWidget::on_mediaBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    updateStreamList();
    emit mediaChanged(_id, mediaBox->itemData(index).toInt());
}

void StreamReferenceWidget::on_removeButton_clicked()
{
    emit removed(_id);
}

void StreamReferenceWidget::updateStreamList()
{
    streamBox->clear();
    if (mediaBox->currentIndex() == -1) return;

    MediaInfo *m = _inputMedias->getMedia( mediaBox->currentData().toInt() );

    QList<VideoInfo *> vs = m->videoStreams();
    for (int i = 0; i < vs.count(); i++)
    {
        VideoInfo *s = vs[i];
        streamBox->addItem( "Video #" + QString::number(s->id()) + " (" + s->language()->prettyName() + ")", s->id());
    }

    QList<AudioInfo *> as = m->audioStreams();
    for (int i = 0; i < as.count(); i++)
    {
        AudioInfo *s = as[i];
        streamBox->addItem( "Audio #" + QString::number(s->id()) + " (" + s->language()->prettyName() + ")", s->id());
    }
}

void StreamReferenceWidget::on_streamBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    emit streamChanged( _id, streamBox->itemData(index).toInt());
}

void StreamReferenceWidget::mediaChanged()
{
    MediaInfo *media = static_cast<MediaInfo *>(sender());
    for (int i = 0; i < mediaBox->count(); i++)
    {
        if (mediaBox->itemData(i).toInt() == media->id())
        {
            mediaBox->setItemText(i, QFileInfo(media->fileName()).fileName() );
            updateStreamList();
            return;
        }
    }
}
