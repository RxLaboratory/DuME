#ifndef FFCOLORITEM_H
#define FFCOLORITEM_H

#include "ffbaseobject.h"

class FFColorItem : public FFBaseObject
{
public:
    enum ScaleFilter{ ZScale, Colorspace, Gamma, LUT, None };
    Q_ENUM(ScaleFilter)

    enum Ability { Input = 1 << 0,
                   Output = 1 << 1
                 };
    Q_DECLARE_FLAGS(Abilities, Ability)

    FFColorItem(QString name, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QString outputScaleName, ScaleFilter scaleFilter, QObject *parent = nullptr);

    bool isInput() const;
    bool isOutput() const;
    void setInput(bool i = true);
    void setOutput(bool o = true);

    ScaleFilter selectOutputFilter(FFColorItem *inputItem);

    ScaleFilter defaultScaleFilter() const;
    void setDefaultScaleFilter(const ScaleFilter &scaleFilter);

    QString outputZScaleName() const;
    void setOutputZScaleName(const QString &scaleName);

    QString inputZScaleName() const;
    void setInputZScaleName(const QString &inputScaleName);

    QString outputCSScaleName() const;
    void setOutputCSScaleName(const QString &outputCSScaleName);

    QString inputCSScaleName() const;
    void setInputCSScaleName(const QString &inputCSScaleName);

    QString outputGScaleName() const;
    void setOutputGScaleName(const QString &outputGScaleName);

    QString inputGScaleName() const;
    void setInputGScaleName(const QString &inputGScaleName);

    QString outputLScaleName() const;
    void setOutputLScaleName(const QString &outputLScaleName);

    QString inputLScaleName() const;
    void setInputLScaleName(const QString &inputLScaleName);

    QString metadataName() const;
    void setMetadataName(const QString &metadataName);

protected:
    QString _outputZScaleName;
    QString _inputZScaleName;
    QString _outputCSScaleName;
    QString _inputCSScaleName;
    QString _outputGScaleName;
    QString _inputGScaleName;
    QString _outputLScaleName;
    QString _inputLScaleName;
    ScaleFilter _defaultScaleFilter;
    Abilities _abilities;
    QString _metadataName;
};

#endif // FFCOLORITEM_H
