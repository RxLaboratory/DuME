#include "blockstartnumber.h"

BlockStartNumber::BlockStartNumber(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Start Number block";
#endif
    setupUi(this);
}

void BlockStartNumber::activate(bool activate)
{
    _freezeUI = true;

    if (activate)
    {
        _mediaInfo->setStartNumber( startNumberEdit->value() );
    }
    else
    {
        _mediaInfo->setStartNumber( 0 );
    }

    _freezeUI = false;
}

void BlockStartNumber::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->isSequence())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    emit blockEnabled(true);


    startNumberEdit->setValue( _mediaInfo->startNumber() );

    _freezeUI = false;
}

void BlockStartNumber::on_startNumberEdit_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setStartNumber( arg1 );
}
