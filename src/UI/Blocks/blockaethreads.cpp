#include "blockaethreads.h"

BlockAEThreads::BlockAEThreads(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    _freezeUI = true;
    setupUi(this);
    threadsBox->setValue(QThread::idealThreadCount());
    _freezeUI = false;
}

void BlockAEThreads::activate(bool activate)
{
    _freezeUI = true;
    if (activate)
    {
        _mediaInfo->setAepNumThreads( threadsBox->value() );
    }
    else
    {
        _mediaInfo->setAepNumThreads( 1 );
    }
    _freezeUI = false;
}

void BlockAEThreads::update()
{
    _freezeUI = true;

    threadsBox->setValue( _mediaInfo->aepNumThreads() );

    _freezeUI = false;
}

void BlockAEThreads::on_threadsBox_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setAepNumThreads( arg1 );
}
