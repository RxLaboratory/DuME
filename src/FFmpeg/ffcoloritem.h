#ifndef FFCOLORITEM_H
#define FFCOLORITEM_H

#include "ffbaseobject.h"

class FFColorItem : public FFBaseObject
{
public:
    enum ScaleFilter{ ZScale, Colorspace, Scale };
    Q_ENUM(ScaleFilter)

    FFColorItem(QString name, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QString scaleName, QObject *parent = nullptr);
    FFColorItem(QString name, QString prettyName, QString scaleName, ScaleFilter scaleFilter, QObject *parent = nullptr);

    QString scaleName() const;
    void setScaleName(const QString &scaleName);

    ScaleFilter scaleFilter() const;
    void setScaleFilter(const ScaleFilter &scaleFilter);

protected:
    QString _scaleName;
    ScaleFilter _scaleFilter;
};

#endif // FFCOLORITEM_H
