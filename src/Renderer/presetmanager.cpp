#include "presetmanager.h"

#include <QtDebug>

PresetManager::PresetManager(QObject *parent) : QObject(parent)
{
    QSettings settings;
    load();
    _resetDefaultPreset = ":/presets/MP4 - Standard";
    _defaultUserPresetPath = QDir::homePath() + "/DuME Presets/";
    _defaultPreset = QFileInfo( settings.value("presets/default", _resetDefaultPreset).toString() );
    _instance = this;
}

PresetManager *PresetManager::instance()
{
    if (!_instance) _instance = new PresetManager();
    return _instance;
}

void PresetManager::load()
{
    QSettings settings;
    //list internal
    _internalPresets.clear();
    foreach(QFileInfo preset, QDir(":/presets/").entryInfoList(QDir::Files))
    {
        _internalPresets << Preset(preset);
    }

    //list users
    _userPresets.clear();

    _userPresetsPath = settings.value("presets/path", "" ).toString();
    if (_userPresetsPath == "") _userPresetsPath = _defaultUserPresetPath;

    QStringList filters("*.meprst");
    filters << "*.json" << "*.dffp";
    foreach (QFileInfo preset, QDir(_userPresetsPath).entryInfoList(filters,QDir::Files))
    {
        _userPresets << Preset(preset);
    }
}

QList<Preset> PresetManager::internalPresets() const
{
    return _internalPresets;
}

QList<Preset> PresetManager::userPresets() const
{
    return _userPresets;
}

QList<Preset> PresetManager::presets() const
{
    QList<Preset> all(_internalPresets);
    all.append(_userPresets);
    return all;
}

Preset PresetManager::defaultPreset() const
{
    return _defaultPreset;
}

void PresetManager::setDefaultPreset(const Preset &defaultPreset)
{
    QSettings settings;
    _defaultPreset = defaultPreset;
    settings.setValue("presets/default", _defaultPreset.file().absoluteFilePath() );
}

void PresetManager::resetDefaultPreset()
{
    Preset p = Preset(QFileInfo( _resetDefaultPreset ));
    setDefaultPreset(p);
}

QString PresetManager::userPresetsPath()
{
    return _userPresetsPath;
}

PresetManager *PresetManager::_instance = nullptr;
