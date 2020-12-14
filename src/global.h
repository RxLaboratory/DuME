#ifndef GLOBAL_H
#define GLOBAL_H

#include "Renderer/presetmanager.h"
#include "FFmpeg/ffmpeg.h"
#include "Renderer/cachemanager.h"

// Presets
extern PresetManager *presetManager;
// FFmpeg Infos
extern FFmpeg *ffmpeg;
// Cache Manager
extern CacheManager *cacheManager;

#endif // GLOBAL_H
