#include "blockcontentwidget.h"

BlockContentWidget::BlockContentWidget(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) : QWidget(parent)
{
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();
    _inputMedias = inputMedias;

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
    update();
}

QMenu *BlockContentWidget::getPresets() const
{
    return _presets;
}
