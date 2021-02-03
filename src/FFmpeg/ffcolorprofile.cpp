#include "ffcolorprofile.h"

FFColorProfile::FFColorProfile(QString name, QString prettyName, FFColorItem *primaries, FFColorItem *trc, FFColorItem *space, FFColorItem *range, QObject *parent):
    FFBaseObject(name, prettyName, parent)
{
    _primaries = primaries;
    _trc = trc;
    _space = space;
    _range = range;
}

FFColorItem *FFColorProfile::primaries() const
{
    return _primaries;
}

FFColorItem *FFColorProfile::trc() const
{
    return _trc;
}

FFColorItem *FFColorProfile::space() const
{
    return _space;
}

FFColorItem *FFColorProfile::range() const
{
    return _range;
}
