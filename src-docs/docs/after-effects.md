# *After Effects* Rendering

If *Adobe After Effects* is installed on the system, *DuME* is able to render and encode *After Effects* Compositions.

Before these compositions can be rendered, some presets need to be created inside *After Effects*. This operation has to be done only once for each version of *After Effects* you want to use.  
A small script is provided with *DuME* to make this first installation easy.

## Installation

### Using the provided script

1- Launch the version of After Effects you want to use with *DuME*.
2- Run the script via `File/Scripts/Run Script File...`. The script is located in the *After Effects* folder where you have installed *DuME*.

### Manually

If for some reason this script does not work or you cannot use it, follow these steps in the rendere queue:

1- Select the "Lossless" output module. Set its format to `OpenEXR Sequence`, and the channels to RGB+Alpha. Leave all other options to their default values. Save this new module as "DuEXR".
2- Select the "Lossless" output module. Set its format to `WAV`, and the audio options to `96,000Hz / 32 bit / Stereo`. Save this new module as "DuWAV".
3- Select the "Best" render settings. and check the "Skip existing files" box. Save these new settings as "DuMultiMachine".
4- Select the "Best" render settings. Save these settings as "DuBest".

## Use

Drop or open an *After Effects* project on the input side of *DuME*.

![](img/captures/ae_input_options.png)

The framerate is not (yet) detected by *DuME*, don't forget to set it manually.

To choose which composition will be rendered, you have to possibilities: Either set the exact composition name, or add the composition to the render queue in *After Effects*, and set the number of its item in the render queue in *DuME* (the render settings do not matter at all).  
The quickest way to do this is to make sure the rendere queue in *After Effects* is empty, then add the composition you want to render. *DuME* automatically renders the first (and only in this case) item in the queue.

You can also use *DuME* to render the whole queue, but in this case *DuME* cannot automatically transcode the result into other formats.

You have the option to use several threads at once when rendering *After Effects* projects, but be careful as this may not work correctly if the number of threads is too high, especially with projects needing a lot of memory.