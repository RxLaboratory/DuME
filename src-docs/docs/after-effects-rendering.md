# *After Effects* Rendering

Drop or open an *After Effects* project on the input side of *DuME*.

![](img/captures/ae_input_options.png)

!!! warning
    The framerate is not (yet) detected by *DuME*, don't forget to set it manually.

There are two ways to choose which composition will be rendered: either set the exact composition name, or add the composition to the render queue in *After Effects*, and set the index of the render queue item in *DuME*.

!!! Tip
    A quick way to do this is to make sure the render queue in *After Effects* is empty, then add the composition you want to render. *DuME* automatically renders the first (and only in this case) item in the queue.

You can also use *DuME* to render the whole queue, but in this case *DuME* cannot automatically transcode the result to other formats.

You have the option to use several threads at once when rendering *After Effects* projects, but be careful as this may not work correctly if the number of threads is too high, especially with projects needing a lot of memory.