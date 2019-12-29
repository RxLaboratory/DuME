#include "blockloops.h"

BlockLoops::BlockLoops(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
}

void BlockLoops::setActivated(bool activate)
{
    if (activate) _mediaInfo->setLoop( -1 );
    else _mediaInfo->setLoop( videoLoopsEdit->value() );
}

void BlockLoops::update()
{
    int l = _mediaInfo->loop();
    videoLoopsEdit->setValue( l );
    if (l == -1) videoLoopsEdit->setSuffix(" No loop");
    else if (l == 0) videoLoopsEdit->setSuffix(" Infinite");
    else if (l == 1) videoLoopsEdit->setSuffix(" loop");
    else videoLoopsEdit->setSuffix(" loops");
}


void BlockLoops::on_videoLoopsEdit_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setLoop( arg1 );
}
