if ffmpeg/ocio not found, reset path in settings, and search again

## au prochain build

- check default ocio path on win & linux

- check reset settings pour voir si les chemins (win, linux) sont bien trouvés

- Check UI win (font size)

- doc: add ociobakelut instruction for macos

- build doc

## Refactor

- Remove the "silent" param from setters in MediaInfo, VideoInfo and AudioInfo; use QSignalBlocker instead

- Remove the "freezeui" flag from blocks, use QSignalBlocker instead

- Move sequence related methods from MediaInfo to VideoInfo
    - Update getDescription on both

- Move ffmpegsequencename to ffmpegrenderer

## Improvements

- Improve "getDescription" in MediaInfo, VideoInfo and AudioInfo, depending if it's an input or output media.

## LUT Implementation

à tester

## Windows / Mac (Ae)

- [ ] Detect ae errors

## Filters to add

- denoise (bm3d)
- interlace (add to the existing deinterlace block)
- premultiply (add to the existing unmultiply block)
