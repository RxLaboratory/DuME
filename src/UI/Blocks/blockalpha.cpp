#include "blockalpha.h"

BlockAlpha::BlockAlpha(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setType(Type::Video);

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
        _mediaInfo->setPixFormat( "" );
        _mediaInfo->setPremultipliedAlpha( true );
    }
}

void BlockAlpha::update()
{
    if (!_mediaInfo->canHaveAlpha())
    {
        emit blockEnabled(false);
        return;
    }

    VideoInfo *stream = _mediaInfo->videoStreams()[0];

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

    qDebug() << "Alpha block updated";
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
