# Convert LUTs

![](img/captures/lut-converter.png)

If you have to convert LUTs, *DuME* is able to handle a lot of different formats thanks to *OCIO*.
    
*DuME* can convert LUTs [from all formats supported by OCIO](https://opencolorio.readthedocs.io/en/latest/guides/using_ocio/using_ocio.html#supported-lut-formats), and to these formats:

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