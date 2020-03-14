#ifndef GLOBAL_H
#define GLOBAL_H

#include "UI/duapplicationutils.h"
#include "Renderer/presetmanager.h"
#include "FFmpeg/ffmpeg.h"

// App version
extern AppVersion DuMEVersion;
// Application
extern InitApplication *initApplication;
// Presets
extern PresetManager *presetManager;
// FFmpeg Infos
extern FFmpeg *ffmpeg;

#endif // GLOBAL_H
