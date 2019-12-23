# Officially supported codecs

DuFFmpeg is based on FFmpeg, and thus supports all the codecs supported by FFmpeg (which means **_a lot_**).  
If default options used by FFmpeg are ok for you, this means it's very easy for you to use it: just drop your input file and select the output format and codec you like.  
If you need to adjust how the encoding is made, you can use _custom options_ in DuFFmpeg - which are the same than FFmpeg command line options - to do anything you'd like, but there are easier user interfaces for the most common codecs and options; these are the codecs _officially supported_ by DuFFmpeg.

This is the list of those codecs.

When it's checked, it's already supported, if not, it's planned ;)

- [x] __H.264__ (Partial: needs multi pass implementation, and some other parameters)
- [ ] H.265
- [x] __Prores__
- [ ] DnxHD
- [x] __PNG__
- [x] __JPG__ (Partial: needs a quality setting, using only ffmpeg default for now)
- [ ] MJpeg
- [x] __TGA__
- [ ] JPEG2000 (and DCP)
- [x] __GIF__ (still needs better default parameters)
- [ ] Quicktime Animation
- [x] __HAP__
- [x] __EXR__ (input only, DWA and Luma/Chroma support in development)
