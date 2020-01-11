#include "dropshadow.h"

DropShadow::DropShadow(QObject *parent) :
    QGraphicsDropShadowEffect(parent)
{
    setBlurRadius(20);
    setXOffset(2);
    setYOffset(2);
    setColor(QColor(0, 0, 0, 70));
}
