#include "aerenderobject.h"

AERenderObject::AERenderObject(QString name, QString path, QObject *parent) : QObject(parent)
{
    _name = name;
    _path = path;
    _mainVersion = 0;
    _primaryVersion = 0;
    _secondaryVersion = 0;
    _buildNumber = 0;

    //get version from name
    QRegularExpression re(".* \\((\\d+)\\.(\\d+)(?:\\.(\\d+))?(?:x(\\d+))?\\)");
    //matchs the version numbers in these patterns:
    // XX (16.0x207)
    // XX (11.0.4x2)
    // XX (15.1.2)
    // XX (15.1)

    QRegularExpressionMatch match = re.match(name);
    if (match.hasMatch())
    {
        _mainVersion = match.captured(1).toInt();
        _primaryVersion = match.captured(2).toInt();
        _secondaryVersion = match.captured(3).toInt();
        _buildNumber = match.captured(4).toInt();
    }
}

QString AERenderObject::name() const
{
    return _name;
}

QString AERenderObject::path() const
{
    return _path;
}

int AERenderObject::mainVersion() const
{
    return _mainVersion;
}

int AERenderObject::primaryVersion() const
{
    return _primaryVersion;
}

int AERenderObject::secondaryVersion() const
{
    return _secondaryVersion;
}

int AERenderObject::buildNumber() const
{
    return _buildNumber;
}
