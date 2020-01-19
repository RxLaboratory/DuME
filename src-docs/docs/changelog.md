# Changelog

This is the list of what has changed since the first public version of *DuFFmpeg*.

## 0.1.1

### New Features

- Added controls for the *Profile* and *Level* when transcoding to h.264
- Added *Quality* control for ProRes.
- Multiple input files
- Select and map streams from the input file(s) to the output media (i.e. easily add sound to an image sequence, or change the sound of a video)
- Added complete color management options (still need to add LUT support).
- Ability to open files with DuME. They will be added as input medias.
- Added a *Help menu* on the top right corner of the window with useful links

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
