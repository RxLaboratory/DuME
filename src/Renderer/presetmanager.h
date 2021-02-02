#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QCoreApplication>

#include "preset.h"

class PresetManager : public QObject
{
    Q_OBJECT
public:
    static PresetManager *instance();

    void load();
    QList<Preset> internalPresets() const;
    QList<Preset> userPresets() const;
    QList<Preset> presets() const;

    Preset defaultPreset() const;
    void setDefaultPreset(const Preset &defaultPreset);
    void resetDefaultPreset();

    QString userPresetsPath();

signals:
    void changed();

private:
    // Private constructor, this is a singleton
    explicit PresetManager(QObject *parent = nullptr);
    QList<Preset> _internalPresets;
    QList<Preset> _userPresets;
    Preset _defaultPreset;
    QString _resetDefaultPreset;
    QString _defaultUserPresetPath;
    QString _userPresetsPath;
protected:
    static PresetManager *_instance;
};

#endif // PRESETMANAGER_H
