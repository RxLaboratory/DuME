# DuME - The Duduf Media Encoder
Rendering and transcoding medias.

## Dependencies

- Qt >= 5.9.8
- OpenImageIO >= 1.7
- FFmpeg executable binary >= 3.4

## Build

The easiest way to build DuME is to [install Qt](https://www.qt.io/download-open-source) along with Qt Creator, then install the [OpenImageIO](https://github.com/OpenImageIO/oiio) library for development.

Then, you can just open and build the project with Qt Creator.

You will need an [FFmpeg](http://ffmpeg.org) executable binary to transcode medias with DuME.

### Linux

- Qt: use the [online installer for Qt](https://www.qt.io/download-open-source).
- OpenImageIO is probably available in your distribution packages. With Aptitude (Ubuntu / Debian): `sudo apt-get install libimageio-dev`. If not, you will have to [build it from source](https://github.com/OpenImageIO/oiio/blob/master/INSTALL.md).
- FFmpeg is also probably available in your distribution packages. With Aptitude (Ubuntu / Debian): `sudo apt-get install ffmpeg`.

### Windows

- Qt: use the [online installer for Qt](https://www.qt.io/download-open-source).
- OpenImageIO: TODO
- FFmpeg can be downloaded [here](https://ffmpeg.zeranoe.com/builds/).