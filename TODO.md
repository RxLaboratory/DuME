## ociobakelut

check default path on win & mac

add a console / Debug info ?

test blender luts against the original ones

## au prochain build

- Linux include ociobakelut in static version ?
- linux rename duffmpeg-static to dume-ffmpeg

- add opencolorio-tools dependency >= 1.0

- mac include ociobakelut

- windows include ociobakelut
    - DONE using vcpkg

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
