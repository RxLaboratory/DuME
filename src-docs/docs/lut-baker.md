# Bake OCIO LUTs

![](img/captures/lut-baker.png)

With *DuME*, you can generate LUTs using any *OpenColorIO* configuration, for example to be able to use *OCIO* color management in applications which do not support *OCIO*.

These LUTs can also be applied to your medias using *DuME* so that *DuME* can be used in your *OCIO* color-managed pipeline.

Read the parameters you need from your `ocio.config` file. In this screenshot you can see how to create an *ICC Profile* from the *Blender* *OCIO* config, converting a *Linear* image to *Filmic sRGB* with the *Medium Contrast* look.

*DuME* can bake LUTs to these formats:

- __Discreet / After Effects / Autodesk (*.3dl)__\*
- __Cinespace (*.csp)__\*
- Discreet legacy 1D-LUT / Houdini (*.lut)
- Iridas (*.itx)
- Truelight (*.cub)
- ICC profile (*.icc)

_\*These formats can then be applied to your medias using_ DuME.

!!! warning
    *DuME* needs the *OpenColorIO* tools to work with LUTs.  
    On *Mac OS*, they're not automatically installed with *DuME* (yet). [Read this](ocio-mac.md) for more information.

![META](authors:Nicolas "Duduf" Dufresne;license:GNU-FDL;copyright:2019-2021;updated:2021/03/07)