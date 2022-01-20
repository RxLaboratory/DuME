#ifndef PRESET_H
#define PRESET_H

#include <QObject>
#include <QFileInfo>

class Preset : public QObject
{
public:
    Preset(QObject *parent = nullptr);
    Preset(QFileInfo file, QObject *parent = nullptr);
    Preset(Preset &&other);
    Preset(const Preset &other);
    ~Preset();

    Preset &operator=(Preset &&other);
    Preset &operator=(const Preset &other);
    bool operator==(Preset other);


    QString name() const;
    QFileInfo file() const;

private:
    QString _name;
    QFileInfo _file;
};

#endif // PRESET_H
