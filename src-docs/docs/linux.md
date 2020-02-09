# Install DuME on linux

!!! note
    For now, DuME on linux is only availabe in 64bit and will not work on 32bit systems (unless you build it from source by yourself).

## Debian and Ubuntu

If you're using Debian or Ubuntu, you can download the `.deb` package and install it the usual way.

!!! note
    DuME depends on the package qt5-default in version 5.9.5 or more recent. If it's not available in your package manager, use the AppImage instead.  
    For Ubuntu, these libraries are available since Bionic Beaver (18.04).

## AppImage for all distributions

An easy way to use DuME is to download the AppImage file.

Make the file executable, either with your file explorer or the command line (replace `x.x.x` by the actual version):  
`chmod u=rwx DuME-x.x.x-x86_64.AppImage`

You'll also need to [download and install FFmpeg](https://ffmpeg.org/download.html#build-linux). Most of the time, it should be available in your distribution package manager. For example, in Debian or Ubuntu:  
`sudo apt-get install ffmpeg`

Then you can launch it by double clicking on the AppImage file or with the command line:  
`./DuME-x.x.x-x86_64.AppImage`

## Build from source

You can [get the source code on Github](https://github.com/Rainbox-dev/DuME), and easily build DuME using Qt and the Qt Creator.

You'll also need to [download and install FFmpeg](https://ffmpeg.org/download.html#build-linux). Most of the time, it should be available in your distribution package manager. For example, in Debian or Ubuntu:  
`sudo apt-get install ffmpeg`
