# DuME Introduction

**_DuME_ is a Free and Open Source Media Encoder**.

It is still in an early development version, but can already be used in a professionnal pipeline.

- It is **fast, versatile, and very easy-to-use**.
- A system of simple **presets** makes it very quick to use. It comes with carefully hand-crafted presets suited for professionnal use.
- Transcoding mostly uses [***FFmpeg***](https://ffmpeg.org/), a very fast and versatile command-line media encoder.
- It supports a **huge variety of both input and output formats**.
- **Color management** is carefully handled, and it has a partial [*OpenColorIO*](http://opencolorio.org) support through LUTs and LUT tools. This *OCIO* support will be completed and improved over time.
- It will handle **openEXR files**, their AOVs, and compressions, using [*OpenImageIO*](https://github.com/OpenImageIO/oiio). This is still in development.
- It renders and transcodes ***Adobe After Effects*** compositions.
- It will render and transcode [***Blender***](http://blender.org) scenes. This is still in development.
- It will have a **render/transcode queue** to plan and queue all your rendering/transcoding jobs. This is still in development.
- It is packed with **a lot of useful features** (**speed** and **framerate** interpolation, applying **LUTs**, **mixing** streams, **resizing**/**cropping**...), all crafted for best quality and performance, with a lot of options but carefully chosen default values.
- It comes with a **Command-Line interface** too, to make it easier to include in your existing pipeline.
- It will include a **server API** through a background daemon, to make it easier to use with an existing pipeline and/or as a renderfarm. This is still in development.

[TOC]

## Table of contents

- Installation
    - [Windows](windows.md)
    - [Mac OS X](mac.md)
    - [Linux](linux.md)
- [The Green Guide to transcoding](green.md)
- Transcoding
    - [Manage Presets](presets.md)
    - [MP4 / h.264 / h.265](h264-5.md)
    - [Image Sequences](sequences.md)
    - [Adding / Changing streams](mapping.md)
    - [Convert color spaces](colors.md)
    - [Applying a LUT](lut.md)
    - [Speed and framerate](speed.md)
    - [Resizing](resize.md)
    - [Filters](filter.md)
- After Effects rendering
    - [Rendering](after-effects-rendering.md)
- Tools
    - [OCIO LUT baker](lut-baker.md)
    - [LUT converter](lut-converter.md)
- Control and automation
    - [Introduction](api.md)
    - [Command line options](cli-options.md)
    - [Server API](server-api.md)

## Description

![](img/captures/DuME_Screenshot.png)

The goal of DuME is to be as **easy to use** as possible even for beginners, without any compromise on **functionnality for advanced users**.

To achieve this goal, the development of the application follows these guidelines:

- The User Interface has to be both intuitive and nice-looking, as well as fast.
- Transcoding to standard formats (mp4, PNG...) is straightforward, thanks to carefully crafted presets.
- The most common parameters (resolution, frames per second, codec, quality...) can easily be set through the user interface
- Advanced users can add any other parameter available in FFmpeg thanks to the "custom parameters", or even input their own commands through the included command line.
- Common parameters and Custom parameters can be saved as easy-to-use User Presets.

### Available Features

- Transcode any audio or video file, [from any format supported by *FFmpeg*](https://ffmpeg.org/general.html#Supported-File-Formats_002c-Codecs-or-Features) to any other format supported by *FFmpeg*.
- Easy presets for: mp4 (h.264/h.265), wav, PNG, EXR, Quicktime HAP, Apple Prores... - *evolving list*.
- Transcode to and from [Image Sequences](sequences.md) (PNG, EXR...).
- [Mix Media files](mapping.md) (e.g. add sound to an image sequence).
- Easily render *After Effects* projects and transcode to any format supported by *FFmpeg*.
- Color managed transcoding.
- Create/convert/apply LUTs.
- Render/Transcode queue, *work in progress, possible with command/batch files thanks to [the command line interface](cli-options.md)*.

### Planned Features

- Simple media player.  
    - Preview of the input.
    - Preview of the output.
- Render *Blender* scenes.
- Append several inputs.
- Add subtitles tracks.
- Burn subtitles in picture.
- Burn Metadata in picture (Timecode...).
- Server API.
- Better openEXR support.
- Full OCIO support.

## About DuME

If you like this free software, please [support the developer on Patreon](http://patreon.com/duduf) to help the continuation of the development of free tools.

## License

### Software

***DuME* Copyright (c) 2019-2021 Nicolas Dufresne and Contributors**  

*DuME* is licensed under the [GNU General Public License v3](gnu-gpl.md).

Read the [credits](credits.md) for information about the developpers and included third-party tools and code.

This program is free software; you can redistribute them and/or modify them under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the *GNU General Public License* along with *DuME*.  If not, see http://www.gnu.org/licenses/.

![GNU](img/logos/gnu.png) ![GPL](img/logos/gplv3.png) 

### Documentation

**Copyright (C)  2019-2020 Nicolas Dufresne and Contributors.**  
Permission is granted to copy, distribute and/or modify this document under the terms of the GNU Free Documentation License, Version 1.3 or any later version published by the Free Software Foundation;  
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the section entitled "[Documentation License](doc-license.md)".

![GNU](img/logos/gnu.png) ![GFDL](img/logos/gfdl-logo.png) 

![META](authors:Nicolas "Duduf" Dufresne;license:GNU-FDL;copyright:2019-2021;updated:2021/03/07)