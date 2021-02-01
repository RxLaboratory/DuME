#include "blocktimerange.h"

BlockTimeRange::BlockTimeRange(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo, parent)
{
    setupUi(this);
    QRegularExpression re("(?:\\d*:)*\\d+\\.?\\d*");
    QRegularExpressionValidator *v = new QRegularExpressionValidator(re);
    inEdit->setValidator(v);
    outEdit->setValidator(v);
}

void BlockTimeRange::activate(bool blockEnabled)
{
    if (!blockEnabled)
    {
        _mediaInfo->setInPoint(0.0);
        _mediaInfo->setOutPoint(0.0);
    }
    else
    {
        _mediaInfo->setInPoint( inEdit->text() );
        _mediaInfo->setOutPoint( outEdit->text() );
    }
}

void BlockTimeRange::update()
{
    inEdit->setText( MediaUtils::durationToTimecode( _mediaInfo->inPoint() ));
    outEdit->setText( MediaUtils::durationToTimecode( _mediaInfo->outPoint() ));
}

void BlockTimeRange::on_inEdit_editingFinished()
{
    if (_freezeUI) return;
    _mediaInfo->setInPoint( inEdit->text() );
}

void BlockTimeRange::on_outEdit_editingFinished()
{
    if (_freezeUI) return;
    _mediaInfo->setOutPoint( outEdit->text() );
}
