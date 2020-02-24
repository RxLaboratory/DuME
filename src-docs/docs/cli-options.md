# Command Line options

[TOC]

DuME can be launched with several options, which is an easy way to add medias in DuME from other applications and scripts.

`DuME [inputOptions] inputFile1 [[inputOptions] inputFile2 ... [inputOptions] inputFileN] [outputOptions]`

!!! Note
    On Windows, don't forget to add the extension `.exe` to the DuME command if it's not in your *PATH* variable.

## Examples

Add a PNG sequence and set its framerate:  
`DuME -framerate 29.97 "image sequence_0001.png"`

Add a PNG sequence and set its framerate along with a WAV sound file, select a preset, and autostart the transcoding process:  
`DuME -framerate 24 "image sequence_0001.png" "an awesome sound.wav" -preset "MP4 - Standard" -autostart`

## Getting help

| Option | Type | Description |
| --- | --- | --- |
| `-h` | | print options but does not launch DuME. |
| `-help` | | print options but does not launch DuME. |
| `-help:presets` | | prints the list of available presets. |
| `-h:presets` | | prints the list of available presets. |

## Input options

| Option | Type | Description |
| --- | --- | --- |
| `-colorProfile profile` | string | Sets the input color profile. One of:<br>`srgb` sRGB<br>`bt709` HD Video BT.709<br>`bt2020_10` UHD (4K/8K) Video BT.2020-10bits<br>`bt2020_12` UHD (4K/8K) HDR Video BT.2020-12bits |

### Image Sequence and After Effects options

Read the [After Effects Rendering](after-effects-rendering.md) section for more details about these options.

| Option | Type | Description |
| --- | --- | --- |
| `-framerate fps` | float | Sets the input framerate. |
| `-fps fps` | float | Sets the input framerate. |

#### After Effects options

| Option | Type | Description |
| --- | --- | --- |
| `-comp composition` | string | The name of the composition to render. |
| `-rqItem item` | integer | The index of the After EFfects render queue item to render. |
| `-useQueue` | | Just launches the After Effects render queue. |

## Output options

| Option | Type | Description |
| --- | --- | --- |
| `-output file` | string | The name of the output file. |
| `-o file` | string | The name of the output file. |
| `-preset name` | string | The name of the preset to use. Use `-help:presets` to get the list of available presets |
| `-p name` | string | The name of the preset to use. Use `-h:presets` to get the list of available presets |
| `-autostart` | | Autostart the transcoding process. |
| `-autoquit` | | If `autostart` is set, automatically closes DuME once the transcoding process is finished. |
