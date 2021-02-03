#ifndef FFCOLORPROFILE_H
#define FFCOLORPROFILE_H

#include <QObject>

#include "FFmpeg/ffcoloritem.h"


class FFColorProfile : public FFBaseObject
{
    Q_OBJECT
public:
    FFColorProfile(QString name, QString prettyName, FFColorItem *primaries, FFColorItem *trc, FFColorItem *space, FFColorItem *range, QObject *parent = nullptr);
    FFColorItem *primaries() const;
    FFColorItem *trc() const;
    FFColorItem *space() const;
    FFColorItem *range() const;
private:
    FFColorItem *_primaries;
    FFColorItem *_trc;
    FFColorItem *_space;
    FFColorItem *_range;
};

#endif // FFCOLORPROFILE_H
