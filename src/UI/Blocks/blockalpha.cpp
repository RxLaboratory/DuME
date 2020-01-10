#include "blockalpha.h"

BlockAlpha::BlockAlpha(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
}

void BlockAlpha::activate(bool activate)
{
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
}

void BlockAlpha::update()
{
    _freezeUI = true;

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
