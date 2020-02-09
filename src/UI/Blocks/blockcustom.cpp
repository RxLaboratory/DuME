#include "blockcustom.h"

BlockCustom::BlockCustom(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Custom block";
#endif
    setupUi(this);
}

BlockCustom::BlockCustom(MediaInfo *mediaInfo, QString p, QString v, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);
    paramEdit->setText( p );
    valueEdit->setText( v );
}

void BlockCustom::setParam(QString p)
{
    paramEdit->setText( p );
}

void BlockCustom::setValue(QString v)
{
    valueEdit->setText( v );
}

QString BlockCustom::param()
{
    return paramEdit->text();
}

QString BlockCustom::value()
{
    return valueEdit->text();
}
