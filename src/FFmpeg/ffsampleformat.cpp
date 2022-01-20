#include "ffsampleformat.h"

FFSampleFormat::FFSampleFormat(QString name, QString prettyName, int depth, QObject *parent):
    FFBaseObject(name, prettyName, parent)
{
    _depth = depth;
}

FFSampleFormat::FFSampleFormat(QString name, int depth, QObject *parent):
    FFBaseObject(name, parent)
{
    _depth = depth;
    //let's parse the name to get a pretty name
    QRegularExpression re = RegExUtils::getRegularExpression("ffmpeg sample format");
    QRegularExpressionMatch match = re.match(name);
    _prettyName = name;
    if (match.hasMatch())
    {
        _prettyName = "";
        if (match.captured(1) == "u") _prettyName += "Unsigned ";
        else if (match.captured(1) == "s") _prettyName += "Signed ";
        _prettyName += QString::number(depth) + "-bit ";
        if (match.captured(3) == "flt") _prettyName += "floating-point";
        else if (match.captured(4) == "dbl") _prettyName += "double";
        else _prettyName += "integer ";
        if (match.captured(5) == "p") _prettyName += ", planar";
    }
}


int FFSampleFormat::depth()
{
    return _depth;
}

FFSampleFormat *FFSampleFormat::getDefault(QObject *parent)
{
    return new FFSampleFormat( "", "Default", 0, parent );
}
