#ifndef GLOBAL_H
#define GLOBAL_H

#include "DuF/duapplication.h"
#include "DuF/duversion.h"
#include "Renderer/presetmanager.h"
#include "FFmpeg/ffmpeg.h"
#include "Renderer/cachemanager.h"

// App version
extern DuVersion DuMEVersion;
// Application
extern InitApplication *initApplication;
// Presets
extern PresetManager *presetManager;
// FFmpeg Infos
extern FFmpeg *ffmpeg;
// Cache Manager
extern CacheManager *cacheManager;

#endif // GLOBAL_H
