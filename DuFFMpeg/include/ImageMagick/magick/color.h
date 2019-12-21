/*
  Copyright (C) 2003 - 2018 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  GraphicsMagick Color Utility Methods.
*/
#ifndef _MAGICK_COLOR_H
#define _MAGICK_COLOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

/*
  PixelPacket with usage count, used to support color histograms.
*/
typedef struct _HistogramColorPacket
{
  PixelPacket
    pixel;

  unsigned long
    count;
} HistogramColorPacket;

extern MagickExport HistogramColorPacket
  *GetColorHistogram(const Image *image,unsigned long *colors,
     ExceptionInfo *exception);

extern MagickExport unsigned long
  GetNumberColors(const Image *image,FILE *file,ExceptionInfo *exception);

extern MagickExport void
  GetColorTuple(const PixelPacket *color,const unsigned int depth,
    const unsigned int matte,const unsigned int hex,char *tuple);

extern MagickExport MagickBool
  IsPaletteImage(const Image *image,ExceptionInfo *exception);

#if defined(MAGICK_IMPLEMENTATION)

/*
  Macros for testing a pixel to see if it is grayscale, bilevel,
  black, or white
*/
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).red == (color).blue))
#define IsMonochrome(color) \
  (((0 == (color).red) || (MaxRGB == (color).red)) && IsGray(color))

#define IsBlackPixel(color) \
  (((color).red == 0U) && IsGray(color))

#define IsWhitePixel(color) \
  (((color).red == MaxRGB) && IsGray(color))

/*
  Compare two colors
*/
#define ColorMatch(p,q)                                         \
  (((p)->red == (q)->red) &&                                    \
   ((p)->green == (q)->green) &&                                \
   ((p)->blue == (q)->blue))

#define NotColorMatch(p,q)                                      \
  (((p)->red != (q)->red) ||                                    \
   ((p)->green != (q)->green) ||                                \
   ((p)->blue != (q)->blue))

extern MagickExport unsigned int
  FuzzyColorMatch(const PixelPacket *p,const PixelPacket *q,const double fuzz) MAGICK_FUNC_PURE;

/*
  Compare two pixels (including opacity)
*/
#define PixelMatch(p,q,matte)                                   \
  (ColorMatch(p,q) &&                                           \
   (!matte || ((p)->opacity == (q)->opacity)))

#define NotPixelMatch(p,q,matte)                                \
  (NotColorMatch(p,q) ||                                        \
   (matte && ((p)->opacity != (q)->opacity)))


#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COLOR_H */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * fill-column: 78
 * End:
 */
