#ifndef FFLUT_H
#define FFLUT_H

#include "ffbaseobject.h"
#include "Renderer/cachemanager.h"

class FFLut : public FFBaseObject
{
public:
    enum Type{ OneD, ThreeD };
    Q_ENUM(Type)

    FFLut(QString name, QString prettyName, QString inputProfile = "", QString outputProfile = "", Type type = ThreeD, QObject *parent = nullptr);

    Type type() const;
    void setType(const Type &type);

    QString outputProfile() const;
    void setOutputProfile(const QString &outputProfile);

    QString inputProfile() const;
    void setInputProfile(const QString &inputProfile);

    QString extract();

private:
    QString _inputProfile;
    QString _outputProfile;
    Type _type;
};

#endif // FFLUT_H
