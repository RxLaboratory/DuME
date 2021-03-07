# Filters

Some of the options available in *DuME* are actually filters applied to the video or audio streams, which modify the data of the streams.

As such, the order in which the filters are applied may be important. It is not yet possible to re-order them, but here is the list of these filters and the order in which they're applied.

This specific order has been chosen to keep the best quality possible in the stream.

## Video Filter Graph

1. Unmultiply alpha
2. Deinterlace
3. [Change speed](speed.md)
4. [Motion interpolation](speed.md)
4. Crop
5. Any user defined filters
6. [LUT](lut.md) on *input* color space
7. [Resize](resize.md)
8. [Color management](colors.md)
9. [LUT](lut.md) on *output* color space

## Audio Filter Graph

There are no audio filter applied yet.

!!! hint
    The *user defined filters* are those added as custom FFmpeg parameters using the `-vf`, `-va`, `-filter:v` or `-filter:a` options.

![META](authors:Nicolas "Duduf" Dufresne;license:GNU-FDL;copyright:2019-2021;updated:2021/03/07)