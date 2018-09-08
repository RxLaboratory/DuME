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

public slots:
    void init();

private slots:
    void stdErrorAE();
    void stdOutputAE();
    void errorOccurredAE(QProcess::ProcessError e);
private:
    QList<AERenderObject *> _versions;
    QProcess *_aerender;
    QString _aeOutput;

    void findAeVersions(QString dir);
};

#endif // AERENDER_H
