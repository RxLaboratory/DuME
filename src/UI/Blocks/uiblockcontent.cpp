#include "uiblockcontent.h"

UIBlockContent::UIBlockContent(MediaInfo *mediaInfo, QWidget *parent) : QWidget(parent)
{
    _freezeUI = true;
    _activated = false;
    _mediaInfo = mediaInfo;
    _presets = new QMenu();

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( changed() ) );

    _freezeUI = false;
}

void UIBlockContent::setActivated(bool activate)
{
    _activated = activate;
}

void UIBlockContent::update()
{

}

void UIBlockContent::changed()
{
    _freezeUI = true;
    update();
    _freezeUI = false;
}

QMenu *UIBlockContent::getPresets() const
{
    return _presets;
}
