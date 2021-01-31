# Filters

Some of the options available in *DuME* are actually filters applied to the video or audio streams, which modify the data of the streams.

As such, the order in which the filters are applied may be important. It is not yet possible to re-order them, but here is the list of these filters and the order in which they're applied.

This specific order has been chosen to keep the best quality possible in the stream.

## Video Filter Graph

1. Unmultiply alpha
2. Deinterlace
3. Change speed
4. Crop
5. Any user defined filters
6. [LUT](lut.md)

## Audio Filter Graph

There are no audio filter applied yet.

!!! hint
    The *user defined filters* are those added as custom FFmpeg parameters using the `-vf`, `-va`, `-filter:v` or `-filter:a` options.