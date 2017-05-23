# DuFFMpeg
ExtendScript tools to use FFMpeg inside Adobe After Effects

You will need FFMpeg binaries which can be downloaded here: http://ffmpeg.org/download.html
More info about FFMpeg on the official website: http://ffmpeg.org/

Right now, it only works under windows, mac implementation will be done in a few days.

DuFFMpegLib.jsxinc contains custom classes to quickly and easily launch a transcoding process using the FFMpeg binary.
The classes are fully documented inside the .jsxinc file.

Options and codecs are those used by the FFMpeg binary, which can be listed in a command line
`ffmpeg -h` for general help
`ffmpeg -encoders` to get a list of available encoders

Here are some usage examples:

To use the library, simply include it in the beginning of your scripts
`#include DuFFMpegLib.jsxinc`

Very simple transcoding
```javascript
var ffmpeg = new FFMpeg(); //create a new FFMpeg instance
var input = new FFMpegInputModule('C:/RAINBOX/Test video 1.mp4'); //input file. Note that you can use any path format you want, URI, fsName, or path, using either / or \
var output = new FFMpegOutputModule('pcm_f32le','C:/RAINBOX/Test audio 1.wav'); //output to an audio wav file using PCM 32 bit float little-endian codec
var item = new FFMpegQueueItem(input,[output]); //create the render queue item
ffmpeg.queue.push(item);  //add the item to the render queue

ffmpeg.launch(); //and launch the process!
```

Transcoding two different files, and a few more options
```javascript
var ffmpeg = new FFMpeg('-y -stats'); //new FFMpeg instance, encoding will overwrite files, and statistics will be shown
var input = new FFMpegInputModule('C:/RAINBOX/Test video 1.mp4'); //input file
var output = new FFMpegOutputModule('pcm_f32le','C:/RAINBOX/Test audio 1.wav','-ar 48000 -vn'); //output to an audio wav file using PCM 32 bit float little-endian codec, forcing 48000Hz sampling and disabling video
var item = new FFMpegQueueItem(input,[output]); //create the render queue item
ffmpeg.queue.push(item); //add the item to the render queue

//another render queue item
var input = new FFMpegInputModule('C:/RAINBOX/Test video 2.mp4');
var output = new FFMpegOutputModule('pcm_f32le','C:/RAINBOX/Test audio 2.wav','-ar 48000 -vn');
var item = new FFMpegQueueItem(input,[output]);
ffmpeg.queue.push(item);


ffmpeg.launch(); //and launch the process! Both items will be transcoded.
```
