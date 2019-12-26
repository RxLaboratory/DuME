# Changelog

This is the list of what has changed since the first public version of *DuFFmpeg*.

## 0.1.0-Alpha

### General Improvements

- Complete code refactor. The application is now easier to maintain, and the groundwork is better for further development and new features. The application should be more stable.
- The consoles (log, ffmpeg console) have been improved.
- The color transform modes (gamma) have been renamed to be easier to understand (e.g. sRGB instead of IECXXXX).
- The default color transform when transcoding EXR files is now sRGB by default. This should be adjusted according to the output format, which will be automated in the future.
- The input information displayed when dropping a file onto DuME is a lot more detailed.

## 0.0.9-Alpha

### General Improvements

- The User Interface has been improved, using drop shadows to group parameters. This is a first step to future improvements of the UI.
- Check video size before transcoding using h264: Adjust to even numbers.
- The default gamma for EXR files and After Effects projects is now automatically set to BT.470M (gamma 2.2).

### Platform Specific Fixes

- User Interface: Margins and fonts are now correctly displayed on Linux.

## 0.0.8-Alpha

Initial Release.
