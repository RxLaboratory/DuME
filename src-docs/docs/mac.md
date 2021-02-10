# Install DuME on Mac OS X

**You can [download DuME here](https://rainboxlab.org/tools/dume/).**

[TOC]

## DMG Archive

You can download a `.zip` archive whoc contains a `.dmg` archive you can install the usual way, along with the *After Effects* script. Open the `.dmg` archive and drop the DuME App in your applications folder. Read below to learn how to install the After Effects script.

## After Effects Script

If you intend to use *DuME* with *Adobe After Effects*, you can also install the *DuME Script for After Effects* and copy the content of the `ScriptUI Panels` folder from the zip to the `ScriptUI Panels` folder of *After Effects*.  
There are two ways to do this:

- Manually (if you have administrator privileges) by copying the `.jsx` file to  
`Applications\Adobe After Effects\Scripts\ScriptUI Panels`
- Automatically through the `File\Scripts\Install ScriptUI Panel...` meny entry in *After Effects*.

Restart After Effects when it's done, and DuME will be available in the *Windows* menu in *After Effects*.

## Build from source

You can [get the source code on Github](https://github.com/Rainbox-dev/DuME), and easily build *DuME* using *Qt* and the *Qt Creator*.

You'll also need to [download and install FFmpeg](https://ffmpeg.org/download.html#build-mac).