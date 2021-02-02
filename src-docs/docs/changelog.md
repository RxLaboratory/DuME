# Changelog

This is the list of what has changed since the first public version of *DuFFmpeg*.

[TOC]

## 0.2.0

### New Features

- **JPEG** support.
- [**h265 / HEVC**](h264-5.md) support.
- **Lossless** and **Intra-Frame** h264 / AVC support.
- **Crop** videos.
- [**Apply a 1D or 3D LUT**](lut.md).
- **Deinterlace** videos.
- [**Change speed**](speed.md).
- Added a [**Motion interpolation**](speed.md) filter.
- Added new [**resize**](resize.md) options and algorithms.
- Select **input time range**.
- [Command Line](cli-options.md) improvements:
    - Ability to load a custom preset file.
    - Added the `--minimize` option.
- New **presets**.

### Improvements

- Several UI adjustments.
- Better display of the information for output formats.
- Restricted the ability to change the color range to YUV pixel formats only.
- Improved settings UI (moved cache options to their own section, hide After Effects on Linux...)
- Improved codec profile management for h.264 / AVC.
- Improved custom parameters management.
- Code refactoring and improvements to improve performance and stability, and prepare the development of the render queue.

### Fixes

- Fixed crash on quit.
- Automatically sets or reverts to default codec when needed.
- The closest pixel format is now set when switching the output with or without alpha.
- Fixed the pixel format list getting back to *Default* when changed.

## 0.1.5

### New Features

- Ability to set the audio sample format
- Webm support (with alpha too), with two new presets

### Improvements

- Complete UI Refactor

### Fixes

- Fixed the extension of the output not changed when changing a preset.

## 0.1.4

Private testing release

## 0.1.3

### New Features

- Now comes with [a script for *After Effects*](after-effects-install.md).
- Added more [command line options](cli-options.md).
- Added some options to tweak [h264](h264-5.md) encoding (including the ability to output CBR streams).

### Improvements

- Lots of code improvements to make maintenance and contributions easier.

### Fixes

- Fixed crash with the Vidvox HAP codec.
- Fixed console output on Windows.
- Fixed crash when FFmpeg was incorrectly loaded.
- Automatically reload features from FFmpeg if the settings are corrupted.
- Reduced launch time (again).

## 0.1.2

### Fixes

- EXR sequences are now correctly detected.
- Fixed transcoding sequence not working if the framerate is kept to the default value.

## 0.1.1

### New Features

- Added controls for the *Profile* and *Level* when transcoding to h.264
- Added *Quality* control for ProRes.
- Multiple input files
- Select and map streams from the input file(s) to the output media (i.e. easily add sound to an image sequence, or change the sound of a video)
- Added complete color management options (still need to add LUT support).
- Ability to open files with DuME. They will be added as input medias.
- Added a *Help menu* on the top right corner of the window with useful links
- Included a script to be used in After Effects to send compositions to DuME
- Added the ability to add [command line options](cli-options.md) when launching DuME, to automatically add input medias.

### Presets and formats

- Added high compatibility presets for MP4 (Baseline level 3.0)
- Added 16 bpc PNG
- Enabled alpha by default for PNG and TGA
- Added 24-bit and 32-bit WAV
- Removed MP4 presets resized to 1080p and 720p. All presets now keep the input size by default, use the "Resize" option if you need to change the resolution.

### General Improvements

- Complete code refactor. The application is now easier to maintain, and the groundwork is better for further development and new features. The application should be more stable.
- The consoles (log, ffmpeg console, after effects console) have been improved.
- The color management has been completely changed and improved.
- The input information displayed when dropping a file onto DuME is a lot more detailed.
- The UI now uses nice "blocks" for options
- Some options come with useful presets (like standard resolutions for the *Resize* option, color profile settings for the *Color management* block, etc).
- A summary of the output settings is available
- Support of EXR (especially colors) have been improved
- Color conversion when transcoding to YUV files is better
- Improved compatibility of ProRes files.

## 0.0.9

### General Improvements

- The User Interface has been improved, using drop shadows to group parameters. This is a first step to future improvements of the UI.
- Check video size before transcoding using h264: Adjust to even numbers.
- The default gamma for EXR files and After Effects projects is now automatically set to BT.470M (gamma 2.2).

### Fixes

#### Linux

- User Interface: Margins and fonts are now correctly displayed on Linux.

## 0.0.8-Alpha

Initial Release.
