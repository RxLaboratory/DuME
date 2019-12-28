#include "uiblockcontent.h"

UIBlockContent::UIBlockContent(MediaInfo *mediaInfo, QWidget *parent) : QWidget(parent)
{
    _activated = false;
    _mediaInfo = mediaInfo;
}

void UIBlockContent::setActivated(bool activate)
{
    _activated = activate;
}
