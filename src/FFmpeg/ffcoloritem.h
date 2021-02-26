#ifndef FFCOLORITEM_H
#define FFCOLORITEM_H

#include "ffbaseobject.h"

class FFColorItem : public FFBaseObject
{
public:
    enum ScaleFilter{ ZScale, Colorspace, Scale, Gamma, LUT };
    Q_ENUM(ScaleFilter)

    enum Ability { Input = 1 << 0,
                   Output = 1 << 1
                 };
    Q_DECLARE_FLAGS(Abilities, Ability)

    FFColorItem(QString name, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QString outputScaleName, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QString outputScaleName, ScaleFilter scaleFilter, QObject *parent = nullptr);

    bool isInput() const;
    bool isOutput() const;
    void setInput(bool i = true);
    void setOutput(bool o = true);

    QString outputScaleName() const;
    void setOutputScaleName(const QString &scaleName);

    ScaleFilter scaleFilter() const;
    void setScaleFilter(const ScaleFilter &scaleFilter);

    QString metadataName() const;
    void setMetadataName(const QString &metadataName);

    QString inputScaleName() const;
    void setInputScaleName(const QString &inputScaleName);

protected:
    QString _outputScaleName;
    QString _inputScaleName;
    ScaleFilter _scaleFilter;
    Abilities _abilities;
    QString _metadataName;
};

#endif // FFCOLORITEM_H
