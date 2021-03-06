if ffmpeg/ocio not found, reset path in settings, and search again

## ociobakelut

check default path on win & linux

add a console / Debug info ?

## au prochain build

- check reset settings pour voir si les chemins (win, linux, mac) sont bien trouvés

- mac include ociobakelut DONE (homebrew)

- Check UI mac & win (font size)

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
