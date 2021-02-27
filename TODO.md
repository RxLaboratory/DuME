## modifs WIP

- check gamma values in After Effects

- in inputblockcolor, list only input coloritems

- in outputblockcolor, list only output coloritems if convert
- in outputblockcolor, list only metadata coloritems if embed

- option to disable color management (which deactivates corresponding blocks and luts) for faster transcoding
- activate or not color management in ui by setting a working color profile
    - if deactivated, disable lut input/output spaces
    - Filter color options depending on working space

## Tests

- disable matrices in color management and check the difference

## Refactor

- Remove the "silent" param from setters in MediaInfo, VideoInfo and AudioInfo; use QSignalBlocker instead

- Move sequence related methods from MediaInfo to VideoInfo
    - Update getDescription on both

- Move ffmpegsequencename to ffmpegrenderer

## Improvements

- Improve "getDescription" in MediaInfo, VideoInfo and AudioInfo, depending if it's an input or output media.

- option to change working space (and adjust available output profiles)

## LUT Implementation

à tester

## Windows / Mac (Ae)

- [ ] Detect ae errors

## Filters to add

- denoise (bm3d)
- interlace (add to the existing deinterlace block)
- premultiply (add to the existing unmultiply block)
