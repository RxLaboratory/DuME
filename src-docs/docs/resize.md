# Resizing video

*DuME* helps you very quickly resize your videos, to help exporting your deliveries even if you have a lot of different formats to output.

![](img/captures/blocks/resize.png)

The *Options* menu contains some presets for the most common formats.

When resizing with *DuME*, you can use three different modes:

- ***Letterbox***: the aspect ratio is kept by decreasing the size when needed to match the output size, and black bars are added around the video if needed to match the desired output resolution.
- ***Crop***: the aspect ratio is kept by increasing the size when needed to match the output size, and parts of the video outside of the output rectangle are cropped.
- ***Stretch***: the aspect ratio is *not* kept, and the video is stretched to the output size. Note that in this case, the pixel aspect ratio is set to square (1:1).

Especially when upscaling, you may want to set a specific scaling algorithm, to adjust the performance/quality ratio. The result depends on the type of image in the input, you may want to try different algotrithms.