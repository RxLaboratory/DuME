# Install DuME on linux

!!! note
    For now, DuME on linux is only availabe in 64bit (on intel/amd systems but not arm) and will not work on 32bit systems (unless you build it from source by yourself).

**You can [download DuME here](https://rainboxlab.org/tools/dume/).**

[TOC]

## Debian and Ubuntu

If you're using Debian or Ubuntu, in most cases you can just download and install the *.deb* package the usual way. There are two different packages:

- ***dume*** is the default choice which installs *DuME* and gets the dependencies (*FFmpeg* and *Qt-5*) from the Debian/Ubuntu repository, as usual. This should work well on all recent systems.

On older LTS systems (like *Ubuntu Bionic Beaver* (18.04)), the version of FFmpeg may be inferior than v4.0 needed by *DuME*. In this case you can get and install the *dume-static* build.

- ***dume-static*** includes a static build of FFmpeg v4.3, and should work with all and older versions of *Debian*/*Ubuntu*, but in this case ffmpeg will not be updated if there are new updates available until DuME updates are available too.

!!! note
    DuME depends on the package qt5-default in version 5.9.5 or more recent. If it's not available in your package manager, use the AppImage instead.  
    For *Ubuntu*, these libraries are available since *Bionic Beaver* (18.04).

## AppImage for all distributions

An easy way to use DuME is to download the AppImage file.

Make the file executable, either with your file explorer or the command line (replace `x.x.x` by the actual version):  
`chmod u=rwx DuME-x.x.x-x86_64.AppImage`

All dependencies are included in the *AppImage*, so everything should work out of the box.

Then you can launch it by double clicking on the AppImage file or with the command line:  
`./DuME-x.x.x-x86_64.AppImage`

## Build from source

You can [get the source code on Github](https://github.com/Rainbox-dev/DuME), and easily build DuME using Qt and the Qt Creator.

You'll also need to [download and install FFmpeg](https://ffmpeg.org/download.html#build-linux). Most of the time, it should be available in your distribution package manager. For example, in Debian or Ubuntu:  
`sudo apt-get install ffmpeg`
