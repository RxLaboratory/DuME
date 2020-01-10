#include "uiblockcontent.h"

UIBlockContent::UIBlockContent(MediaInfo *mediaInfo, MediaList *inputMedias, QWidget *parent) : QWidget(parent)
{
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();
    _inputMedias = inputMedias;

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( changed() ) );

    _freezeUI = false;
}

UIBlockContent::UIBlockContent(MediaInfo *mediaInfo, QWidget *parent) : QWidget(parent)
{
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();
    _inputMedias = new MediaList();

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( changed() ) );

    _freezeUI = false;
}

void UIBlockContent::activate(bool act)
{
    _activated = act;
}

void UIBlockContent::update()
{

}

void UIBlockContent::changed()
{
    update();
}

QMenu *UIBlockContent::getPresets() const
{
    return _presets;
}
