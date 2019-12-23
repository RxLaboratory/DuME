#ifndef FFBASEOBJECT_H
#define FFBASEOBJECT_H

#include <QObject>
#include <QString>


class FFBaseObject : public QObject
{
    Q_OBJECT
public:
    FFBaseObject(QString name, QString prettyName = "", QObject *parent = nullptr);

    QString name();
    QString prettyName();

    void setName(const QString &name);
    void setPrettyName(const QString &prettyName);

private:
    QString _name;
    QString _prettyName;
};

#endif // FFBASEOBJECT_H
