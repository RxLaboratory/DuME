[TOC]

# *After Effects*: Installation

If *Adobe After Effects* is installed on the system, *DuME* is able to render and encode *After Effects* Compositions.

Before these compositions can be rendered, some presets need to be created inside *After Effects*. This operation has to be done only once for each version of *After Effects* you want to use.  
A small script is provided with *DuME* to make this first installation easy.

## Using the provided script

1- Launch the version of After Effects you want to use with *DuME*.
2- Run the script via `File/Scripts/Run Script File...`. The script is located in the *After Effects* folder where you have installed *DuME*.

## Manually

If for some reason this script does not work or you cannot use it, follow these steps in the rendere queue:

1- Select the "Lossless" output module. Set its format to `OpenEXR Sequence`, and the channels to RGB+Alpha. Leave all other options to their default values. Save this new module as "DuEXR".
2- Select the "Lossless" output module. Set its format to `WAV`, and the audio options to `96,000Hz / 32 bit / Stereo`. Save this new module as "DuWAV".
3- Select the "Best" render settings. and check the "Skip existing files" box. Save these new settings as "DuMultiMachine".
4- Select the "Best" render settings. Save these settings as "DuBest".
