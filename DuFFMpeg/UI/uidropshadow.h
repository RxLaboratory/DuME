#ifndef UIDROPSHADOW_H
#define UIDROPSHADOW_H

#include <QObject>
#include <QGraphicsDropShadowEffect>

class UIDropShadow : public QGraphicsDropShadowEffect
{
    Q_OBJECT
public:
    UIDropShadow(QObject *parent = Q_NULLPTR);
};

#endif // UIDROPSHADOW_H
