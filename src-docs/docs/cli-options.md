# Command Line options

[TOC]

DuME can be launched with several options, which is an easy way to add medias in DuME from other applications and scripts.

`DuME [general options] [input options] inputFile1 [[input options] inputFile2 ... [input options] inputFileN] [output options]`

!!! Note
    On Windows, don't forget to add the extension `.exe` to the DuME command if it's not in your *PATH* variable.

## Examples

Add a PNG sequence and set its framerate:  
`DuME --framerate 29.97 "image sequence_0001.png"`

Add a PNG sequence and set its framerate along with a WAV sound file, select a preset, and autostart the transcoding process:  
`DuME --framerate 24 "image sequence_0001.png" "an awesome sound.wav" --preset "MP4 - Standard" --autostart`

## Getting help

| Option | Type | Description |
| --- | --- | --- |
| `-h` / `--help`| | print options but does not launch DuME. |
| `--help:presets` / `-h:p` | | prints the list of available presets. |

## General options

| Option | Type | Description |
| --- | --- | --- |
| `--no-banner` | | Does not output the first descriptive paragraphs (name, version, license and copyright notice). |
| `--hide-console` | | On *Windows*, hides the console as soon as DuME is launched. This is implicit if there are no other arguments. Useful when launching DuME from other applications. |
| `--minimize` / `-m` | | Minimizes the window as soon as it is ready. |

## Input options

| Option | Type | Description |
| --- | --- | --- |
| `--color-profile profile` | string | Sets the input color profile. One of:<br>`srgb` sRGB<br>`bt709` HD Video BT.709<br>`bt2020_10` UHD (4K/8K) Video BT.2020-10bits<br>`bt2020_12` UHD (4K/8K) HDR Video BT.2020-12bits |

### Image Sequence and After Effects options

Read the [After Effects Rendering](after-effects-rendering.md) section for more details about these options.

| Option | Type | Description |
| --- | --- | --- |
| `--framerate fps` / `-f fps` | float | Sets the input framerate. |

#### After Effects options

| Option | Type | Description |
| --- | --- | --- |
| `--comp composition` / `-c composition` | string | The name of the composition to render. |
| `--rq-item item` | integer | The index of the After EFfects render queue item to render. |
| `--use-queue` | | Just launches the After Effects render queue. |
| `--aerender aerenderpath` | string | Explicitly set the After Effects renderer to use. |

## Output options

| Option | Type | Description |
| --- | --- | --- |
| `--output file` / `-o file` | string | The name of the output file. |
| `--preset name` / `-p name` | string | The name of the preset to use. Use `--help:presets` to get the list of available presets |
| `--autostart` | | Autostart the transcoding process. |
| `--autoquit` | | If `autostart` is set, automatically closes DuME once the transcoding process is finished. |
