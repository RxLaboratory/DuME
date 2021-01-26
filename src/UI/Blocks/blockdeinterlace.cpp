#include "blockdeinterlace.h"

BlockDeinterlace::BlockDeinterlace(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Deinterlace block";
#endif
    _freezeUI = true;

    setType(Type::Video);

    setupUi(this);

    deinterlaceBox->addItem("Auto");
    deinterlaceBox->addItem("Top field first");
    deinterlaceBox->addItem("Bottom field first");
    deinterlaceBox->setEnabled(false);

    _freezeUI = false;
}

void BlockDeinterlace::activate(bool blockEnabled)
{
    if (blockEnabled)
    {
        _mediaInfo->setDeinterlace(deinterlaceButton->isChecked());
        setDeinterlaceParity(deinterlaceBox->currentIndex());
    }
    else
    {
        _mediaInfo->setDeinterlace( false );
        _mediaInfo->setDeinterlaceParity(MediaUtils::AutoParity);
    }
}

void BlockDeinterlace::update()
{
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    deinterlaceButton->setChecked( stream->deinterlace() );
    if ( stream->deinterlaceParity() == MediaUtils::AutoParity) deinterlaceBox->setCurrentIndex(0);
    else if ( stream->deinterlaceParity() == MediaUtils::TopFieldFirst) deinterlaceBox->setCurrentIndex(1);
    else if ( stream->deinterlaceParity() == MediaUtils::BottomFieldFirst) deinterlaceBox->setCurrentIndex(2);
}

void BlockDeinterlace::on_deinterlaceButton_clicked(bool checked)
{
    deinterlaceBox->setEnabled(checked);
    if (_freezeUI) return;
    _mediaInfo->setDeinterlace(checked);
}

void BlockDeinterlace::setDeinterlaceParity(int index)
{
    if (index == 1) _mediaInfo->setDeinterlaceParity(MediaUtils::TopFieldFirst);
    else if (index == 2) _mediaInfo->setDeinterlaceParity(MediaUtils::BottomFieldFirst);
    else _mediaInfo->setDeinterlaceParity(MediaUtils::AutoParity);
}

void BlockDeinterlace::on_deinterlaceBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    setDeinterlaceParity(index);
}
