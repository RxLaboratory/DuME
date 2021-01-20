#include "blockcontentwidget.h"

BlockContentWidget::BlockContentWidget(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) : QWidget(parent)
{
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();
    _inputMedias = inputMedias;
    _type = Type::All;

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( changed() ) );

    update();

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
    _activated = act;
}

void BlockContentWidget::update()
{

}

void BlockContentWidget::changed()
{
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

        VideoInfo *stream = _mediaInfo->videoStreams()[0];
        if (stream->isCopy())
        {
            emit blockEnabled(false);
            _freezeUI = freeze;
            return;
        }

        emit blockEnabled(true);
    }



    update();

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
