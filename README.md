# DuME - The Duduf Media Encoder
Rendering and transcoding medias.

## Dependencies

- Qt >= 5.9.8
- OpenImageIO >= 2.0
- FFmpeg executable binary >= 3.4

## Build

***WARNING* DuME is currently under heavy refactory, and the code as it is today won't compile.**  
This work should be finished in a few days.

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
