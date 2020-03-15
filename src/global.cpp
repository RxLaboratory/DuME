#include "global.h"

#ifdef QT_DEBUG
DuVersion DuMEVersion(VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,QString(VERSION_SUFFIX) + "_Development_Build");
#else
DuVersion DuMEVersion(VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_SUFFIX);
#endif

InitApplication *initAppication = new InitApplication(DuMEVersion.getString());
PresetManager *presetManager = new PresetManager();
FFmpeg *ffmpeg = new FFmpeg();
