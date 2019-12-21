#ifndef MAGICK_H
#define MAGICK_H


#include <QObject>
#include <Magick++.h>

using namespace Magick;

class DuMagick : public QObject
{
    Q_OBJECT
public:
    explicit DuMagick(QObject *parent = nullptr);

signals:

};

#endif // MAGICK_H
