#include "uiblockcontent.h"

UIBlockContent::UIBlockContent(MediaInfo *mediaInfo, QWidget *parent) : QWidget(parent)
{
    _activated = false;
    _mediaInfo = mediaInfo;

    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( update() ) );
}

void UIBlockContent::setActivated(bool activate)
{
    _activated = activate;
}

void UIBlockContent::update()
{

}
