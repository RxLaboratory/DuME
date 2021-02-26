#include "fflut.h"

FFLut::FFLut(QString name, QString prettyName, QString inputProfile, QString outputProfile, Type type, QObject *parent):
        FFBaseObject(name, prettyName, parent)
{
    _inputProfile = inputProfile;
    _outputProfile = outputProfile;
    _type = type;
}

FFLut::Type FFLut::type() const
{
    return _type;
}

void FFLut::setType(const Type &type)
{
    _type = type;
}

QString FFLut::outputProfile() const
{
    return _outputProfile;
}

void FFLut::setOutputProfile(const QString &outputProfile)
{
    _outputProfile = outputProfile;
}

QString FFLut::inputProfile() const
{
    return _inputProfile;
}

void FFLut::setInputProfile(const QString &inputProfile)
{
    _inputProfile = inputProfile;
}

QString FFLut::extract()
{
    //extract LUT
    CacheManager *cm = CacheManager::instance();
    QDir d = cm->getRootCacheDir();
    if (!_name.startsWith(":/")) return _name;
    QFileInfo sourceInfo(_name);
    QString destinationLutPath = d.absolutePath() + "/" + sourceInfo.fileName();
    QFile::copy(_name, destinationLutPath);
    return destinationLutPath;
}
