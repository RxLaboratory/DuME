#include "global.h"


#ifdef QT_DEBUG
AppVersion DuMEVersion(VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,QString(VERSION_SUFFIX) + "_Development_Build");
#else
AppVersion DuMEVersion(VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_SUFFIX);
#endif

PresetManager *presetManager = new PresetManager();
FFmpeg *ffmpeg = new FFmpeg();

