# DuME - The Duduf Media Encoder
Rendering and transcoding medias.

📖 A **comprehensive user documentation** is [available here](https://dume-docs.rainboxlab.org).

🎥 You can **download the latest public version [here](https://rainboxlab.org/tools/dume/)**.

📣 You can also **help the developper** (Nicolas "Duduf" Dufresne) **and access Beta versions** on [Patreon](https://patreon.com/duduf).

## Community

🚀 [Join us and have a chat](http://chat.rainboxlab.org) to get started!

🤗 We'd be happy to welcome you on the [forum](https://forum.rainboxlab.org) too.

## Dependencies

- Qt >= 5.14.0
- ( OpenImageIO >= 2.0 ) (not implemented yet)
- FFmpeg executable binary >= 4.0

## Build

First, get and install the dependencies:  
[Install Qt](https://www.qt.io/download-open-source) along with Qt Creator,  
Then install the [OpenImageIO](https://github.com/OpenImageIO/oiio) library for development,  
And finally, get an [FFmpeg](http://ffmpeg.org) executable binary to transcode medias with DuME.

Then, you can just open and build the project with Qt Creator. The project and the source code are in the [src](https://github.com/Rainbox-dev/DuME/tree/master/src) sub-directory.

### Linux

- Qt: use the [online installer for Qt](https://www.qt.io/download-open-source).
- OpenImageIO is probably available in your distribution packages.  
With Aptitude (Ubuntu / Debian): `sudo apt-get install libimageio-dev`.  
Otherwise, you will have to [build it from source](https://github.com/OpenImageIO/oiio/blob/master/INSTALL.md).
- FFmpeg is also probably available in your distribution packages.  
With Aptitude (Ubuntu / Debian): `sudo apt-get install ffmpeg`.

### Windows

- Qt: use the [online installer for Qt](https://www.qt.io/download-open-source).
- OpenImageIO: TODO
- FFmpeg can be downloaded [here](https://ffmpeg.zeranoe.com/builds/). In order to let DuME use it automatically, you can copy `ffmpeg.exe` in the same directory where you build DuME.

### Mac OS X

- Qt: use the [online installer for Qt](https://www.qt.io/download-open-source).
- OpenImageIO: TODO
- FFmpeg can be downloaded [here](https://evermeet.cx/ffmpeg/). In order to let DuME use it automatically, you can copy `ffmpeg` in the the application package once you've built it, in `Contents/MacOS`.
