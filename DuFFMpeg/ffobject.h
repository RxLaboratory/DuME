#define DUFFMPEG_VERSION "0.0.1-Alpha1"

#ifndef FFOBJECT_H
#define FFOBJECT_H

#include <QObject>

class FFObject : public QObject
{
    Q_OBJECT
public:
    explicit FFObject(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FFOBJECT_H
