#include "blockalpha.h"

BlockAlpha::BlockAlpha(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
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
        _mediaInfo->setPixFormat( nullptr );
        _mediaInfo->setPremultipliedAlpha( true );
    }
    _freezeUI = false;
}

void BlockAlpha::update()
{
    if (_freezeUI) return;
    _freezeUI = true;

    if (!_mediaInfo->hasVideo() || _mediaInfo->copyVideo() || !_mediaInfo->canHaveAlpha())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    emit blockEnabled(true);

    //has alpha
    alphaButton->setChecked( _mediaInfo->hasAlpha() );
    if ( alphaButton->isChecked() )
    {
        unmultButton->setChecked( !_mediaInfo->premultipliedAlpha() );
        unmultButton->setEnabled( true );
    }
    else
    {
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
