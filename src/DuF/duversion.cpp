#include "duversion.h"

DuVersion::DuVersion(int major, int minor, int micro, QString suffix):
    QVersionNumber(major, minor, micro)
{
    _suffix = suffix;
}

DuVersion::DuVersion(int major, int minor, int micro):
    QVersionNumber(major, minor, micro)
{
    _suffix = "";
}

DuVersion::DuVersion(int major, int minor):
    QVersionNumber(major, minor)
{
    _suffix = "";
}

DuVersion::DuVersion(int major):
    QVersionNumber(major)
{
    _suffix = "";
}

QString DuVersion::getString()
{
    QString v = this->toString();
    if (_suffix != "") v = v + "-" + _suffix;
    return v;
}

QString DuVersion::suffix() const
{
    return _suffix;
}

void DuVersion::setSuffix(const QString &suffix)
{
    _suffix = suffix;
}
