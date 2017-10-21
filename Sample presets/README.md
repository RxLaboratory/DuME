# Output Preset Samples

DuFFmpeg uses standard [JSON](http://json.org) to store output presets in simple text files.  
To differentiate those files from other JSON files, you should use a "_*.dffp_" extension.

In this folder, you'll find several samples to use as examples if ever you need to build your own presets without using the methods provided by DuFFmpeg.

## Specifications

All the information is stored in a `duffmpeg` object, this and mandatory and used by DuFFmpeg to validate the file, as long as the `version`.

- __version__ \<string>
- __muxer__ \<object> _The Muxer/Container_
  * __name__ \<string> _The muxer's name_
  * __prettyName__ \<string> _The muxer's pretty name_
  * __isSequence__ \<bool> _Is this an image sequence_
  * __extensions__ \<array> _The common extensions used by this muxer. Does not include the leading "."_
    1. __extension__ \<string>
    1. __extension__ \<string>
    1. __extension__ \<string>
- __hasVideo__ \<bool> _Does this preset outputs video_
- __hasAudio__ \<bool> _Does this preset outputs audio_
- __loop__ \<int> _`-1` for no loop, `0` for infinite loops, maximum is `65535`_
- __video__ \<object>
  * __codecName__ \<string> _`"default"` to let app select default codec_
  * __codecPrettyName__ \<string>
  * __width__ \<int> _`0` for auto_
  * __height__ \<int> _`0` for auto_
  * __framerate__ \<float> _in Hz, `0` for auto_
  * __bitrate__ \<int> _in bps, `0` for auto_
  * __quality__ \<int> _from `0` (worst) to `100` (best), `-1` to ignore_
  * __profile__ \<int> _`-1` to ignore, depends on the codec used, see FFmpeg codec documentation_
- __audio__ \<object>
  * __codecName__ \<string> _`"default"` to let app select default codec_
  * __codecPrettyName__ \<string>
  * __sampling__ \<int> _in Hz, `0` for auto_
  * __bitrate__ \<int> _in bps, `0` for auto_
- __options__ \<array> _FFmpeg command line options__
  1. __option__ \<object>
     * __name__ \<string> _FFmpeg option name_
     * __value__ \<string> _Optionnal FFmpeg option value, default = `""`_
  1. __option__ \<object>
     * __name__ \<string>
     * __value__ \<string>
  1. __option__ \<object>
     * __name__ \<string>
     * __value__ \<string>
