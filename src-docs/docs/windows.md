# Install DuME on Windows

!!! note
    For now, DuME on Windows is only availabe on intel/amd 64bit platforms and will not work on 32bit systems (unless you build it from source by yourself) nor on *arm* systems.

**You can [download DuME here](https://rainboxlab.org/tools/dume/).**

!!! note
    For now, only a zip archive is available and there is no installer provided. *DuME* is a portable application and you can just unzip the files on a flash drive if you'd like to keep it with you.

[TOC]

## ZIP Archive

You can download the `.zip` archive and just unzip **all** the files on your computer. Double click on DuME.exe to launch the application, or create a shortcut.

## After Effects Script

If you intend to use *DuME* with *Adobe After Effects*, you can also install the *DuME Script for After Effects* and copy the content of the `ScriptUI Panels` folder from the zip to the `ScriptUI Panels` folder of *After Effects*.  
There are two ways to do this:

- Manually (if you have administrator privileges) by copying the `.jsx` file to  
`C:\Program Files\Adobe\Adobe After Effects\Support Files\Scripts\ScriptUI Panels`
- Automatically through the `File\Scripts\Install ScriptUI Panel...` meny entry in *After Effects*.

Restart After Effects when it's done, and DuME will be available in the *Windows* menu in *After Effects*.

## Build from source

You can [get the source code on Github](https://github.com/Rainbox-dev/DuME), and easily build DuME using Qt and the Qt Creator.

You'll also need to [download and install FFmpeg](https://ffmpeg.org/download.html#build-windows).

To use LUT tools, you will also need [OpenColorIO](http://opencolorio.org) tools. The easiest way to get them on windows is to use [vcpkg](https://github.com/Microsoft/vcpkg) and install them with `vcpkg install opencolorio-tools`.

![META](authors:Nicolas "Duduf" Dufresne;license:GNU-FDL;copyright:2019-2021;updated:2021/03/07)