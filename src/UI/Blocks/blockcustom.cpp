#include "blockcustom.h"

BlockCustom::BlockCustom(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

BlockCustom::BlockCustom(QString p, QString v, QWidget *parent) :
    QWidget(parent)
{
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
