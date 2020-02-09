#include "blockexr.h"

BlockEXR::BlockEXR(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create EXR block";
#endif
    setupUi(this);
}
