# Apply a LUT

*DuME* can apply a *1D* or *3D* LUT to the image.

The supported LUT formats depend on the associated version of FFmpeg. By default you should be able to use:

- *3dl*: Discreet / After Effects / Autodesk
- *cube*: Iridas / Resolve
- *dat*: DaVinci
- *m3d*: Pandora
- *csp*: cineSpace

!!! tip
    If the LUT you need to apply is not available in one of these formats, you may use the [LUT converter](lut-converter.md) in the tools to convert it to one of these supported formats.

Just select the *Apply LUT* item in the video options, then select the LUT you need.

![](/img/captures/blocks/lut.png)

Some LUTs are provided with DuME, and you can load your own.

By default, the LUT is applied after all other filters, except color management conversions, which means it is applied on *input* color space. You can choose to apply it after color conversions, on the *output* color space.

!!! hint
    Using a LUT to transform colors of an image depends both on the input format and colors and the output color space. It is NOT the best way to [convert color spaces](colors.md)...

When using custom LUTs, you can set the input and output [colorspace](colors.md) so that DuME can automatically handle needed [conversions of the media colorspaces](colors.md) to correctly apply the LUT on the right color data.

Don't forget to set the type to 1D if the LUT only applies to luma/gamma and not the gammut (saturation), to avoid un-necessary [color conversions](colors.md).

!!! note
    When using built-in LUTs, there's no need to set these parameters as they're automatically handled by *DuLME*. They are also deactivated if [*color management*](colors.md) is deactivated.