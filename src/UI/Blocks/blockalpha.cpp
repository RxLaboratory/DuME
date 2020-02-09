#include "blockalpha.h"

BlockAlpha::BlockAlpha(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Alpha block";
#endif
    setupUi(this);
}

void BlockAlpha::activate(bool activate)
{
    _freezeUI = true;
    if (activate)
    {
        _mediaInfo->setAlpha( alphaButton->isChecked() );
        _mediaInfo->setPremultipliedAlpha( !unmultButton->isChecked() );
    }
    else
    {
        _mediaInfo->setPixFormat( "" );
        _mediaInfo->setPremultipliedAlpha( true );
    }
    _freezeUI = false;
}

void BlockAlpha::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    VideoInfo *stream = _mediaInfo->videoStreams()[0];

    if (stream->isCopy())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    if (!_mediaInfo->canHaveAlpha())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    emit blockEnabled(true);

    //has alpha
    if ( _mediaInfo->hasAlpha() )
    {
        alphaButton->setChecked( true );
        unmultButton->setChecked( !stream->premultipliedAlpha() );
        unmultButton->setEnabled( true );
    }
    else
    {
        alphaButton->setChecked( false );
        unmultButton->setChecked( false );
        unmultButton->setEnabled( false );
    }
    _freezeUI = false;
}

void BlockAlpha::on_alphaButton_clicked(bool checked)
{
    if (_freezeUI) return;
    _mediaInfo->setAlpha( checked );
    if (!checked) _mediaInfo->setPremultipliedAlpha( true );
}

void BlockAlpha::on_unmultButton_clicked(bool checked)
{
    if (_freezeUI) return;
    _mediaInfo->setPremultipliedAlpha( !checked );
}
