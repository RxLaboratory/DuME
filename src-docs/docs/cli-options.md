# Command Line options

DuME can be launched with several options, which is an easy way to add medias in DuME from other applications and scripts.

`DuME [options] inputFile1 [[options] inputFile2 ... [options] inputFileN]`

!!! Note
    On Windows, don't forget to add the extension `.exe` to the DuME command if it's not in your *PATH* variable.

## Examples

Add a PNG sequence and set its framerate:  
`DuME -framerate 29.97 "image sequence_0001.png"`

Add a PNG sequence and set its framerate along with a WAV sound file:  
`DuME -framerate 24 "image sequence_0001.png" "an awesome sound.wav"`

## Getting help

| Option | Type | Description |
| --- | --- | --- |
| `-h` | | print options but does not launch DuME. |
| `-help` | | print options but does not launch DuME. |

## Global Input options

| Option | Type | Description |
| --- | --- | --- |
| `-colorProfile profile` | string | Sets the input color profile. One of:<br>`srgb` sRGB<br>`bt709` HD Video BT.709<br>`bt2020_10` UHD (4K/8K) Video BT.2020-10bits<br>`bt2020_12` UHD (4K/8K) HDR Video BT.2020-12bits |

## Image Sequence and After Effects options

Read the [After Effects Rendering](after-effects-rendering.md) section for more details about these options.

| Option | Type | Description |
| --- | --- | --- |
| `-framerate fps` | float | Sets the input framerate. |
| `-fps fps` | float | Sets the input framerate. |

## After Effects options

| Option | Type | Description |
| --- | --- | --- |
| `-comp composition` | string | The name of the composition to render. |
| `-rqItem item` | integer | The index of the After EFfects render queue item to render. |
| `-useQueue` | | Just launches the After Effects render queue. |
