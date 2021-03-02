if ffmpeg/ocio not found, reset path in settings, and search again

## ociobakelut

check default path on win & mac

add a console / Debug info ?

## au prochain build

- fix progress issues

- check reset settings pour voir si les chemins (win, linuw ok) sont bien trouvés

- mac include ociobakelut

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
