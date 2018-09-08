#ifndef AERENDEROBJECT_H
#define AERENDEROBJECT_H

#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class AERenderObject : public QObject
{
    Q_OBJECT
public:
    AERenderObject(QString name = "", QString path = "", QObject *parent = nullptr);

    QString name() const;

    QString path() const;

    int mainVersion() const;

    int primaryVersion() const;

    int secondaryVersion() const;

    int buildNumber() const;

signals:

public slots:

private:
    QString _name;
    QString _path;
    int _mainVersion;
    int _primaryVersion;
    int _secondaryVersion;
    int _buildNumber;
};

#endif // AERENDEROBJECT_H
