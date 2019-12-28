# Manage and convert colors

From several tests we've made to and from *YUV (420, 422, 444)* / linear *RGB* / *sRGB* using *mp4*, *PNG* and *EXR* files, these are the options to be used for color conversion between these formats, using FFmpeg > 4.1.

All these options will be automatically set in DuME, but the user has to be able to override them in the UI, probably in an *advanced* section for the input. If they are changed in the input section, by default DuME will also write them in the ouput file, unless the user unchecks the option.

## Converting to sRGB (*JPG* or *PNG* in our tests)

From our tests, it seems there's no need to add the color options, even if the input file is YUV.

This command seems to work fo any input format, except *EXR* which needs the correct `-apply_trc` option or gamma adjustments. Read the section entitled *Converting from EXR* below.

    ffmpeg -i 'Media.mp4' 'Media.jpg'

## Converting to YUV (*mp4* in our tests)

This command seems to work fo any input format, except *EXR* which needs the correct `-apply_trc` option or gamma adjustments. Read the section entitled *Converting from EXR* below.

Simple command:

    ffmpeg -colorspace bt709 -i 'Media.png' -pix_fmt yuv420p 'Media.mp4'

Complete command:

    ffmpeg -color_range tv -colorspace bt709 -color_primaries bt709 -color_trc bt709  -i '/home/duduf/DEV_SRC/DuME/Test Medias/PNG/MediaTest_%5d.png' -pix_fmt yuv420p -color_range tv -colorspace bt709 -color_primaries bt709 -color_trc bt709 '/home/duduf/DEV_SRC/DuME/Test Medias/MediaTest-ffmpeg.mp4'

Here, all color parameters are set to make sure everything is correct, but the result is exactly the same as the simple version, in terms of color. Note that the color parameters are set also for the output, in order to have them in the metadata and to make FFProbe/FFmpeg report them correctly, but it is not needed to store the actual colors.

## Converting from linear RGB *EXR*

To convert from *EXR*, in theory, the right `-apply_trc` option has to be set. Strangely, `-apply_trc bt709` does not result in the right gamma to output *.mp4* files, the colors with this option being a bit too dark. `iec61966_2_1 ` (sRGB) is closer but still a bit too dark The result with the colors the closest to the input file is by using `smpte428_1` (which is used in the 4K DCI specs), but there is still a slight difference in the colors (and not just the transfer curve).

Without the option at all, the transfer curve (gamma) is not right (darker), even if the colors are the right ones.

As it seems the `-apply_trc` option does not work correctly (and exists only for *EXR*), we've tried applying our own LUT to adjust the gamma. From our tests, applying a gamma value of 1/2.4 (~0.417) results in the right gamma and colors in the *mp4* output file. To apply this LUT, use this command: `-vf "lutrgb=r=gammaval(0.416666667):g=gammaval(0.416666667):b=gammaval(0.416666667)"`.

To YUV (*mp4*) (Simple):

    ffmpeg -colorspace bt709 -i 'Media.exr' -vf "lutrgb=r=gammaval(0.416666667):g=gammaval(0.416666667):b=gammaval(0.416666667)" 'Media.mp4'

To YUV (*mp4*) (Complete):

    ffmpeg -color_range tv -colorspace bt709 -color_primaries bt709 -color_trc bt709 -i 'Media.exr' -vf "lutrgb=r=gammaval(0.416666667):g=gammaval(0.416666667):b=gammaval(0.416666667)" -pix_fmt yuv420p -color_range tv -colorspace bt709 -color_primaries bt709 -color_trc bt709 'Media.mp4'

To RGB (*png*):

    ffmpeg -i 'Media.exr' -vf "lutrgb=r=gammaval(0.416666667):g=gammaval(0.416666667):b=gammaval(0.416666667)" 'Media.png'

## Tests

Here are the tests we've run to make sure the files are rendered with the same colors both for the input and the output.

- [x] Linux, FFmpeg 4.1.
  - [x] VLC.
  - [x] Totem.
- [ ] Windows, FFmpeg x.x.
  - [ ] Not tested with VLC.
  - [ ] Not tested with Windows Media Player.
  - [ ] Not tested with After Effects.
- [ ] Mac OS, FFmpeg x.x.
 - [ ] Not tested with VLC.
 - [ ] Not tested with QuickTime Player.

## FFmpeg options list

These options have to be passed before the input `-i inputFile`.

Note: if they are passed to the output file, it seems they do nothing to the actual colors but are stored anyway in the metadata of the file (they are displayed by ffprobe). It may be better for compatibility to add them to the output file too.

### color_primaries   <int>        ED.V..... color primaries (from 1 to INT_MAX) (default unknown)
- bt709                        ED.V..... BT.709
- unknown                      ED.V..... Unspecified
- bt470m                       ED.V..... BT.470 M
- bt470bg                      ED.V..... BT.470 BG
- smpte170m                    ED.V..... SMPTE 170 M
- smpte240m                    ED.V..... SMPTE 240 M
- film                         ED.V..... Film
- bt2020                       ED.V..... BT.2020
- smpte428                     ED.V..... SMPTE 428-1
- smpte428_1                   ED.V..... SMPTE 428-1
- smpte431                     ED.V..... SMPTE 431-2
- smpte432                     ED.V..... SMPTE 422-1
- jedec-p22                    ED.V..... JEDEC P22
- unspecified                  ED.V..... Unspecified
### color_trc         <int>        ED.V..... color transfer characteristics (from 1 to INT_MAX) (default unknown)
- bt709                        ED.V..... BT.709
- unknown                      ED.V..... Unspecified
- gamma22                      ED.V..... BT.470 M
- gamma28                      ED.V..... BT.470 BG
- smpte170m                    ED.V..... SMPTE 170 M
- smpte240m                    ED.V..... SMPTE 240 M
- linear                       ED.V..... Linear
- log100                       ED.V..... Log
- log316                       ED.V..... Log square root
- iec61966-2-4                 ED.V..... IEC 61966-2-4
- bt1361e                      ED.V..... BT.1361
- iec61966-2-1                 ED.V..... IEC 61966-2-1
- bt2020-10                    ED.V..... BT.2020 - 10 bit
- bt2020-12                    ED.V..... BT.2020 - 12 bit
- smpte2084                    ED.V..... SMPTE 2084
- smpte428                     ED.V..... SMPTE 428-1
- arib-std-b67                 ED.V..... ARIB STD-B67
- unspecified                  ED.V..... Unspecified
- log                          ED.V..... Log
- log_sqrt                     ED.V..... Log square root
- iec61966_2_4                 ED.V..... IEC 61966-2-4
- bt1361                       ED.V..... BT.1361
- iec61966_2_1                 ED.V..... IEC 61966-2-1
- bt2020_10bit                 ED.V..... BT.2020 - 10 bit
- bt2020_12bit                 ED.V..... BT.2020 - 12 bit
- smpte428_1                   ED.V..... SMPTE 428-1
### colorspace        <int>        ED.V..... color space (from 0 to INT_MAX) (default unknown)
- rgb                          ED.V..... RGB
- bt709                        ED.V..... BT.709
- unknown                      ED.V..... Unspecified
- fcc                          ED.V..... FCC
- bt470bg                      ED.V..... BT.470 BG
- smpte170m                    ED.V..... SMPTE 170 M
- smpte240m                    ED.V..... SMPTE 240 M
- ycgco                        ED.V..... YCGCO
- bt2020nc                     ED.V..... BT.2020 NCL
- bt2020c                      ED.V..... BT.2020 CL
- smpte2085                    ED.V..... SMPTE 2085
- unspecified                  ED.V..... Unspecified
- ycocg                        ED.V..... YCGCO
- bt2020_ncl                   ED.V..... BT.2020 NCL
- bt2020_cl                    ED.V..... BT.2020 CL
### color_range       <int>        ED.V..... color range (from 0 to INT_MAX) (default unknown)
- unknown                      ED.V..... Unspecified
- tv                           ED.V..... MPEG (219*2^(n-8))
- pc                           ED.V..... JPEG (2^n-1)
- unspecified                  ED.V..... Unspecified
- mpeg                         ED.V..... MPEG (219*2^(n-8))
- jpeg                         ED.V..... JPEG (2^n-1)
### apply_trc (EXR only, does not seem to work correctly)
- bt709                        .D.V.... BT.709
- gamma                        .D.V.... gamma
- gamma22                      .D.V.... BT.470 M
- gamma28                      .D.V.... BT.470 BG
- smpte170m                    .D.V.... SMPTE 170 M
- smpte240m                    .D.V.... SMPTE 240 M
- linear                       .D.V.... Linear
- log                          .D.V.... Log
- log_sqrt                     .D.V.... Log square root
- iec61966_2_4                 .D.V.... IEC 61966-2-4
- bt1361                       .D.V.... BT.1361
- iec61966_2_1                 .D.V.... IEC 61966-2-1 (sRGB)
- bt2020_10bit                 .D.V.... BT.2020 - 10 bit
- bt2020_12bit                 .D.V.... BT.2020 - 12 bit
- smpte2084                    .D.V.... SMPTE ST 2084
- smpte428_1                   .D.V.... SMPTE ST 428-1
