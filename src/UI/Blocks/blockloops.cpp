#include "blockloops.h"

BlockLoops::BlockLoops(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);
}

void BlockLoops::activate(bool activate)
{
    _freezeUI = true;

    if (activate) _mediaInfo->setLoop( -1 );
    else _mediaInfo->setLoop( videoLoopsEdit->value() );

    _freezeUI = false;
}

void BlockLoops::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    if (_mediaInfo->videoStreams()[0]->isCopy())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    FFMuxer *m = _mediaInfo->muxer();
    if ( m == nullptr )
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    else if (m->name() != "gif")
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    emit blockEnabled(true);

    int l = _mediaInfo->loop();
    videoLoopsEdit->setValue( l );
    if (l == -1) videoLoopsEdit->setSuffix(" No loop");
    else if (l == 0) videoLoopsEdit->setSuffix(" Infinite");
    else if (l == 1) videoLoopsEdit->setSuffix(" loop");
    else videoLoopsEdit->setSuffix(" loops");

    _freezeUI = false;
}


void BlockLoops::on_videoLoopsEdit_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setLoop( arg1 );
}
