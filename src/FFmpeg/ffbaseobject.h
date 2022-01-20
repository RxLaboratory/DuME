#ifndef FFBASEOBJECT_H
#define FFBASEOBJECT_H

#include <QObject>
#include <QString>
#include <QJsonObject>


class FFBaseObject : public QObject
{
public:
    FFBaseObject(QString name, QObject *parent = nullptr);
    FFBaseObject(QString name, QString prettyName, QObject *parent = nullptr);

    QString name() const;
    QString prettyName() const;

    void setName(const QString &name);
    void setPrettyName(const QString &prettyName);

    QJsonObject toJson();

protected:
    QString _name;
    QString _prettyName;

};

#endif // FFBASEOBJECT_H
