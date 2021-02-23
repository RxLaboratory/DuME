#ifndef FFLUT_H
#define FFLUT_H

#include "ffbaseobject.h"

class FFLut : public FFBaseObject
{
public:
    enum Type{ OneD, ThreeD };
    Q_ENUM(Type)

    FFLut(QString name, QString prettyName, QString inputTrc = "", QString outputTrc = "", QObject *parent = nullptr);

    Type type() const;
    void setType(const Type &type);

    QString outputTrc() const;
    void setOutputTrc(const QString &outputTrc);

    QString inputTrc() const;
    void setInputTrc(const QString &inputTrc);

    QString outputPrimaries() const;
    void setOutputPrimaries(const QString &outputPrimaries);

    QString inputPrimaries() const;
    void setInputPrimaries(const QString &inputPrimaries);

private:
    QString _inputPrimaries;
    QString _outputPrimaries;
    QString _inputTrc;
    QString _outputTrc;
    Type _type;
};

#endif // FFLUT_H
