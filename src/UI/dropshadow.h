#ifndef UIDROPSHADOW_H
#define UIDROPSHADOW_H

#include <QObject>
#include <QGraphicsDropShadowEffect>

class DropShadow : public QGraphicsDropShadowEffect
{
    Q_OBJECT
public:
    DropShadow(QObject *parent = Q_NULLPTR);
};

#endif // UIDROPSHADOW_H
