#include "blockcontentwidget.h"

BlockContentWidget::BlockContentWidget(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) : QWidget(parent)
{
    qDebug() << "Creating: " + this->objectName();
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();
    _inputMedias = inputMedias;
    _type = Type::All;

    update();

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( changed() ) );

    _freezeUI = false;
}

BlockContentWidget::BlockContentWidget(MediaInfo *mediaInfo, QWidget *parent) : QWidget(parent)
{
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();
    _inputMedias = new MediaList();

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( changed() ) );

    _freezeUI = false;
}

void BlockContentWidget::activate(bool act)
{

}

void BlockContentWidget::update()
{

}

void BlockContentWidget::setActivated(bool act)
{
    qDebug() << "-> Activating: " + this->objectName();
    _activated = act;

    bool freeze = _freezeUI;
    _freezeUI = true;

    activate(act);

    if (act) qDebug() << "    - Activaed!";
    else qDebug() << "    - Deactivated";

    _freezeUI = freeze;
}

void BlockContentWidget::changed()
{
    qDebug() << "-> Updating: " + this->objectName();
    if (_freezeUI) return;
    bool freeze = _freezeUI;
    _freezeUI = true;

    if (_type == Type::Video)
    {
        if (!_mediaInfo->hasVideo())
        {
            emit blockEnabled(false);
            _freezeUI = freeze;
            return;
        }

        VideoInfo *stream = _mediaInfo->videoStreams().at(0);
        if (stream->isCopy())
        {
            emit blockEnabled(false);
            _freezeUI = freeze;
            return;
        }
    }
    else if (_type == Type::Audio)
    {
        if (!_mediaInfo->hasAudio())
        {
            emit blockEnabled(false);
            _freezeUI = freeze;
            return;
        }
        AudioInfo *stream = _mediaInfo->audioStreams()[0];
        if (stream->isCopy())
        {
            emit blockEnabled(false);
            _freezeUI = freeze;
            return;
        }
    }

    emit blockEnabled(true);

    update();

    qDebug() << "    - Updated";

    _freezeUI = freeze;
}

BlockContentWidget::Type BlockContentWidget::type() const
{
    return _type;
}

void BlockContentWidget::setType(const Type &type)
{
    _type = type;
}

QMenu *BlockContentWidget::getPresets() const
{
    return _presets;
}
