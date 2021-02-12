# The Green Guide to transccoding

When it comes to digital goods, we tend to forget too easily that they have an ecological impact, as well as any *physical* good. Because we don't really see that the bigger our digital files are, the bigger the storing devices have to be, that these devices need power to run, that the internet consumes a lot of electricity to connect us, and that all of this is made of materials which have a very big negative impact during the extractions. And these are just examples, one could write a near infinite list of what the digital world consumes and burns.

Here is a guide to help all of use keep in mind we can try to reduce our ecological impact, especially when working with media files and *DuME*. In future versions, *DuME* will help you improve your transcoding process and motion pictures production workflow to reduce your ecological impact.

[TOC]

## What we need to take care of

Here is a short list of what we can work on to reduce our impact on the environment with our media files.

- File size.

One media file may be small. We could even consider that all our media files are not so big after all, compared to all the data stored in the world. But let's not forget video is the biggest part of the traffic on the internet, and even a very small decrease in the average video file size could save a lot both in storage and bandwidth.

- Decoding performance

A media file is meant to be played. And most of the time, the goal is for it to be played by as many people as possible. If the format is easier to decode, in terms of performance and bandwidth, that means less energy consumption to play it, and could save a lot of energy worldwide.

- Encoding performance

This is much less important compared to the two previous points, as it happens just a few times in the lifetime of a media file, but it consumes much more power than decoding, so whenever possible, it would be good to reduce encoding times and consumption. One of the easiest way is to be able to re-render only part of the video when making changes.

- What the file will be used for

The needs for editing a media file, streaming it or screening it are different. When choosing the best format and adjusting decoding performance, we have to take the target and use into account.

!!! note
    Of course, the following recommendations are just guidelines. You may need to improve quality depending on your specific needs durint the production; the goal of this document is to show you which parameter you could tweak to try to reduce your impact on the environment.

## Intermediary formats

Intermediary Media files, which will be used during the production process between each step and software, need to keep a good quality and as much data as possible, so it will not be easy to reduce the file size a lot. That being said, we have to ask ourselves if we *really* need that lossless encoding or if a small theoretical loss could be acceptable, considering that even at some significant compression levels the human eye may still be incapable of seeing any difference. Of course the real question is if you'll still have enough data to edit the image afterwards, so that depends on your pipeline and what you need to do in the next step, but keep in mind you may not really need that full quality 32bpc lossless huge file.

### Internet and streaming

- **Intra-Frame**  
    Decoding has to be very efficient as when working on the file you have to be able to jump at any point in time. It is better to use either an file sequence, or a video using an intra-frame encoding.
- **Lossy**  
    Do you really need a lossless format, or can you afford a theoretical loss of data, considering you won't see the difference?
- **YUV 422**  
    Reducing the size of chroma channels can save a lot of space without impacting the decoding performance. If you don't need chroma keying, why not use a 422 format as soon as possible in the production, considering the video will be in 420 in the end?
- **8 bits**  
    Do you really need more than 8 bits per channel in intermediary format? Of course, your working space may need more accuracy, but won't the file used for transfering data have enough precision with 8 bits?
- **HD / 2K**  
    Will there be so many people watching this video on a huge screen, and close enough to it to see the difference if the video is in 4K and contains **4 times** more data? Is it really worth it?

### Television or HDR screens

- **Intra-Frame**  
    Decoding has to be very efficient as when working on the file you have to be able to jump at any point in time. It is better to use either an file sequence, or a video using an intra-frame encoding.
- **Lossy**  
    Do you really need a lossless format, or can you afford a theoretical loss of data, considering you won't see the difference?
- **YUV 422**  
    Reducing the size of chroma channels can save a lot of space without impacting the decoding performance. If you don't need chroma keying, why not use a 422 format as soon as possible in the production, considering the video will be in 420 in the end?
- **10 bits**  
    Television standards are made for bigger and often better screens, sometimes capable of HDR colors. You may need to keep 10- or 12-bit channels, but keeping this parameter as low as possible is better.
- **HD / 2K**  
    Will there be so many people watching this video on a huge screen, and close enough to it to see the difference if the video is in 4K and contains **4 times** more data? Is it really worth it?

### Cinema

- **Intra-Frame**  
    Decoding has to be very efficient as when working on the file you have to be able to jump at any point in time. It is better to use either an file sequence, or a video using an intra-frame encoding.
- **Lossy**  
    Do you really need a lossless format, or can you afford a theoretical loss of data, considering you won't see the difference?
- **RGB or YUV 444**  
    Your media is meant to be screened on huge screens, it is not recommended to reduce the size of chroma channels.
- **16 bits**  
    The final export has to be delivered with at least 16 bits of data per channel, and the final DCP file uses 12 bits of data. Very often people use 32-bit intermediary files, but do you real need this insane amount of colors?
- **HD / 2K** or 4K?  
    Don't you agree that most of the people in cinemas won't see any difference between 2K and 4K? Do you even notice the difference? Does your theater even screen 4K films, have they changed their expensive projectors when 4K was made available? Is this slight difference really worth it compared to the environmental impact of files 4 times bigger? Is entertainment really that important?

### Recommended formats

- ***h.265 Intra-frame mp4, 8-bit YUV 422*** will achieve all of the above points, and the resulting file size can be quite small, while being very easy to decode. It can use a lossless compression if you need, and you could also use all chroma data with a *444* encoding if you need. You can also increase to 10- or 12-bit channels. The only caveat is that it is not a frame sequence, so you won't be able to easily re-render only part of the video. Use only for short videos. If your software can't encode or decode *h.265*, try with intra-frame *h.264 (AVC-Intra)*.

- ***OpenEXR sequence, 16-bit RGB, Luma/Chroma, DWAA Compression*** can achieve very small files. The *Luma/Chroma* option is equivalent to a *422* reduction and also saves space without perceptible loss. If you need chroma key in the next step you could just omit the option. *DWAA* compression is an efficient lossless or lossy compression, and even when lossy the loss can still be imperceptible.

### Formats to avoid

- ***Prores*** and ***DnxHD/HR***, although very common, are not that great. They are lossy intra-frame formats, but resulting file sizes are huge. You can have better quality and the same decoding performance with intra-frame *h.265* or *openEXR*.

- ***PNG*** is nice, but *openEXR* is better. With the right compression settings, *openEXR* files can be smaller or at least the same size, and at least *Adobe After Effects* is much more performant for encoding and decoding *openEXR*. *OpenEXR* can be both lossy or lossless.

- ***Not Intra-frame formats*** such as standard *h.264* for example, as this introduces a non-negligeable computing overhead while scrolling in the video file when working.

## Last step: edit/color grading

When reaching the last step, which should be edit and maybe color grading, you won't need as much data as previous steps, and can use formats closer to the final delivery, but you still need a good decoding performance when scrolling in your timeline.

### Internet and streaming

- **Intra-Frame**  
    Decoding has to be very efficient as when working on the file you have to be able to jump at any point in time. It is better to use either an file sequence, or a video using an intra-frame encoding.
- **Lossy**  
    You won't be making big changes on the video anymore, and color grading can afford a slight loss of data without any perceptible change for the human eye.
- **YUV 420**  
    Reducing the size of chroma channels can save a lot of space without impacting the decoding performance. The final delivery will surely be a *420* file, you can already work with this when editing the video.
- **8 bits**  
    You won't be making big changes on the video anymore, you don't need more data than what will be delivered in the end. Only if you have a lot of color grading to do it may be useful to keep 10 bits.
- **HD / 2K**  
    Will there be so many people watching this video on a huge screen, and close enough to it to see the difference if the video is in 4K and contains **4 times** more data? Is it really worth it?

### Television or HDR screens

- **Intra-Frame**  
    Decoding has to be very efficient as when working on the file you have to be able to jump at any point in time. It is better to use either an file sequence, or a video using an intra-frame encoding.
- **Lossy**  
    You won't be making big changes on the video anymore, and color grading can afford a slight loss of data without any perceptible change for the human eye.
- **YUV 420**  
    Reducing the size of chroma channels can save a lot of space without impacting the decoding performance. The final delivery may be a *420* file, you can already work with this when editing the video.
- **10 bits**  
    Television standards are made for bigger and often better screens, sometimes capable of HDR colors. You may need to keep 10- or 12-bit channels, but keeping this parameter as low as possible is better.
- **HD / 2K**  
    Will there be so many people watching this video on a huge screen, and close enough to it to see the difference if the video is in 4K and contains **4 times** more data? Is it really worth it?

### Cinema

- **Intra-Frame**  
    Decoding has to be very efficient as when working on the file you have to be able to jump at any point in time. It is better to use either an file sequence, or a video using an intra-frame encoding.
- **Lossy**  
    Do you really need a lossless format, or can you afford a theoretical loss of data, considering you won't see the difference?
- **RGB or YUV 444**  
    Your media is meant to be screened on huge screens, it is not recommended to reduce the size of chroma channels.
- **12 bits**  
    The final DCP file uses 12 bits of data. Very often people use 16- or 32-bit intermediary files, but do you real need this insane amount of colors?
- **HD / 2K** or 4K?  
    Don't you agree that most of the people in cinemas won't see any difference between 2K and 4K? Do you even notice the difference? Does your theater even screen 4K films, have they changed their expensive projectors when 4K was made available? Is this slight difference really worth it compared to the environmental impact of files 4 times bigger? Is entertainment really that important?

### Recommended formats

- ***h.265 Intra-frame mp4, 8-bit YUV 420*** will achieve all of the above points, and the resulting file size can be quite small, while being very easy to decode. Its lossy compression is very efficient, and you could also use more chroma data with a *422* or *444* encoding if you need. You can also increase to 10- or 12-bit channels. The only caveat is that it is not a frame sequence, so you won't be able to easily re-render only part of the video. Use only for short videos. If your software can't encode or decode *h.265*, try with intra-frame *h.264 (AVC-Intra)*.

- ***OpenEXR sequence, 16-bit RGB, Luma/Chroma, DWAA Compression*** can achieve very small files. The *Luma/Chroma* option is equivalent to a *422* reduction and also saves space without perceptible loss. *DWAA* compression is an efficient lossless or lossy compression, and even when lossy the loss can still be imperceptible.

### Formats to avoid

- ***Prores*** and ***DnxHD/HR***, although very common, are not that great. They are lossy intra-frame formats, but resulting file sizes are huge. You can have better quality and the same decoding performance with intra-frame *h.265* or *openEXR*.

- ***PNG*** is nice, but *openEXR* is better. With the right compression settings, *openEXR* files can be smaller or at least the same size, and at least *Adobe After Effects* is much more performant for encoding and decoding *openEXR*. *OpenEXR* can be both lossy or lossless.

- ***Not Intra-frame formats*** such as standard *h.264* for example, as this introduces a non-negligeable computing overhead while scrolling in the video file when working.

## Final delivery

The final media file can be as small as possible and it's not a problem to use imperceptible lossy compression. You don't need it anymore to be performant when scrolling in time, as most of the time it will just be played at its intended speed, so **you don't need the format to be Intra-frame anymore** (i.e. *Prores* and *DnxHD/HR* are not meant to be used as a final delivery). 

### Internet and streaming

- **Lossy**  
    You won't be making big changes on the video anymore, and color grading can afford a slight loss of data without any perceptible change for the human eye.
- **YUV 420**  
    It is very standard to use *420* chroma reduction in final files, and most of time nobody sees any difference.
- **8 bits**  
    That's just what screens connected to the internet can show, no less, no more.
- **HD / 2K**  
    Will there be so many people watching this video on a huge screen, and close enough to it to see the difference if the video is in 4K and contains **4 times** more data? Is it really worth it?

### Television or HDR screens

- **Lossy**  
    You won't be making big changes on the video anymore, and color grading can afford a slight loss of data without any perceptible change for the human eye.
- **YUV 420**  
    It is very standard to use *420* chroma reduction in final files, and most of time nobody sees any difference. Some TV Chanels may ask for *422* files.
- **10 bits**  
    Television standards are made for bigger and often better screens, sometimes capable of HDR colors. You may need to keep 10-bit channels, but keeping only 8 bits would be better.
- **HD / 2K**  
    Will there be so many people watching this video on a huge screen, and close enough to it to see the difference if the video is in 4K and contains **4 times** more data? Is it really worth it?

### Cinema

- **Lossless**  
    The DCI standard and final DCP files are lossless. Theoratically, we could still use lossy (but with an invisible loss) formats.
- **RGB or YUV 444**  
    Your media is meant to be screened on huge screens, it is not recommended to reduce the size of chroma channels.
- **12 bits**  
    The final DCP file uses 12 bits of data. Very often people use 16- or 32-bit intermediary files, but do you real need this insane amount of colors?
- **HD / 2K** or 4K?  
    Don't you agree that most of the people in cinemas won't see any difference between 2K and 4K? Do you even notice the difference? Does your theater even screen 4K films, have they changed their expensive projectors when 4K was made available? Is this slight difference really worth it compared to the environmental impact of files 4 times bigger? Is entertainment really that important?

### Recommended formats

- ***h.265 standard mp4, 8-bit YUV 420*** is small and very efficient. As it may not be supported everywhere yet, you could also use the less efficient h.264 instead. OGG Video (.ogv) is great too. All of them are capable of 10- or 12-bit HDR too. Note that some codecs have presets or settings to improve decoding performance and to adjust compatibility with older hardware; these settings usually reduce power consumption when playing the file.

### Formats to avoid

- ***Prores*** and ***DnxHD/HR***. Do you really want to send files that big over the internet?

- ***Image sequences***. Decoding performance is much better with standard video files.
