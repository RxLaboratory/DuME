#include "blockworkingspace.h"

BlockWorkingSpace::BlockWorkingSpace(MediaInfo *mediaInfo, QWidget *parent) :
    QWidget(parent)
{
    _mediaInfo = mediaInfo;

    setupUi(this);

    foreach(FFColorProfile *cp, FFmpeg::instance()->colorProfiles())
    {
        QString pName = cp->prettyName();
        if (cp->name() == "") pName = "None (color management deactivated)";
        profileBox->addItem( pName, cp->name());
    }
    profileBox->setCurrentData("input");

    connect(profileBox, SIGNAL(currentIndexChanged(int)), this, SLOT(profileBox_currentIndexChanged(int)));
    connect( _mediaInfo, SIGNAL( changed() ), this, SLOT( update() ) );
}

void BlockWorkingSpace::update()
{
    if (!_mediaInfo->hasVideo())
    {
        this->hide();
        return;
    }

    VideoInfo *stream = _mediaInfo->videoStreams().at(0);

    if (stream->isCopy())
    {
        this->hide();
        return;
    }

    this->show();

    QSignalBlocker b(profileBox);

    profileBox->setCurrentData( stream->workingSpace()->name() );
}

void BlockWorkingSpace::profileBox_currentIndexChanged(int /*index*/)
{
    _mediaInfo->setWorkingSpace( profileBox->currentData().toString() );
}
