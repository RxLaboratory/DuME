#ifndef FFSAMPLEFORMAT_H
#define FFSAMPLEFORMAT_H

#include <QObject>

#include "FFmpeg/ffbaseobject.h"
#include "duqf-utils/utils.h"

#include <QRegularExpression>

class FFSampleFormat : public FFBaseObject
{
public:
    FFSampleFormat(QString name, QString prettyName, int depth, QObject *parent = nullptr);
    FFSampleFormat(QString name, int depth, QObject *parent = nullptr);
    int depth();

    static FFSampleFormat *getDefault(QObject *parent = nullptr);
private:
    int _depth;
};

#endif // FFSAMPLEFORMAT_H
