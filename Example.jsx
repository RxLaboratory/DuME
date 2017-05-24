#include DuFFMpegLib.jsxinc

//create a new FFMpeg instance
var ffmpeg = new FFMpeg('-y -v quiet');
//input file. Note that you can use any path format you want, URI, fsName, or path, using either / or \
var input = new FFMpegInputModule('/Users/duduf/Documents/DEV SRC/Duik/Tests/IO/Audition/Bell.wav');
//output to an audio wav file using PCM 32 bit float little-endian codec
var output = new FFMpegOutputModule('pcm_f32le','/Users/duduf/Documents/test.wav');
//create the render queue item
var item = new FFMpegQueueItem(input,[output]);
//add the item to the render queue
ffmpeg.queue.push(item);

var feedback = ffmpeg.launch(); //and launch the process!
alert(feedback);
