#ifndef GLOBAL_H
#define GLOBAL_H

#include "version.h"
#include "Renderer/presetmanager.h"
#include "FFmpeg/ffmpeg.h"
#include "UI/rainboxui.h"

// App version
extern AppVersion DuMEVersion;
// Presets
extern PresetManager *presetManager;
// FFmpeg Infos
extern FFmpeg *ffmpeg;

#endif // GLOBAL_H
