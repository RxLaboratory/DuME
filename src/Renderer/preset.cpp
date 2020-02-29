#include "preset.h"

Preset::Preset(QObject *parent): QObject(parent)
{
    _name = "";
    _file = QFileInfo();
}

Preset::Preset(QFileInfo file, QObject *parent): QObject(parent)
{
    _name = file.completeBaseName();
    _file = file;
}

Preset::Preset(Preset &&other)
{
    _name = other.name();
    _file = other.file();
    setParent(other.parent());
}

Preset::Preset(const Preset &other)
{
    _name = other.name();
    _file = other.file();
    setParent(other.parent());
}

Preset::~Preset()
{

}

Preset &Preset::operator=(Preset &&other)
{
    _name = other.name();
    _file = other.file();
    setParent(other.parent());
    return *this;
}

Preset &Preset::operator=(const Preset &other)
{
    _name = other.name();
    _file = other.file();
    setParent(other.parent());
    return *this;
}

bool Preset::operator==(Preset other)
{
    return _file.absoluteFilePath() == other.file().absoluteFilePath();
}

QString Preset::name() const
{
    return _name;
}

QFileInfo Preset::file() const
{
    return _file;
}
