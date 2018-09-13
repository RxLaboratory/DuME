#ifndef AERENDER_H
#define AERENDER_H

#include <QObject>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QProcess>
#include <algorithm>

#include "aerenderobject.h"

class AERender : public QObject
{
    Q_OBJECT

public:
    AERender(QObject *parent = nullptr);

    QList<AERenderObject *> versions() const;
    AERenderObject *getAERenderObject(QString aeRenderFileName);

public slots:
    void init();

private slots:

private:
    QList<AERenderObject *> _versions;

    void findAeVersions(QString dir);
};

#endif // AERENDER_H
