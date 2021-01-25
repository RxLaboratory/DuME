#include "blockloops.h"

BlockLoops::BlockLoops(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Loops block";
#endif
    setType(Type::Video);
    setupUi(this);
}

void BlockLoops::activate(bool activate)
{
    if (activate) _mediaInfo->setLoop( -1 );
    else _mediaInfo->setLoop( videoLoopsEdit->value() );
}

void BlockLoops::update()
{
    FFMuxer *m = _mediaInfo->muxer();
    if ( m->name() == "" )
    {
        emit blockEnabled(false);
        return;
    }
    else if (m->name() != "gif")
    {
        emit blockEnabled(false);
        return;
    }

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
