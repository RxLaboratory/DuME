/*
  Copyright (C) 2003 - 2009 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  GraphicsMagick Color Lookup Methods.
*/
#ifndef _MAGICK_COLOR_LOOKUP_H
#define _MAGICK_COLOR_LOOKUP_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

/*
  Specifications that color is compliant with.
*/
typedef enum
{
  UndefinedCompliance = 0x0000,
  NoCompliance = 0x0000,
  SVGCompliance = 0x0001,
  X11Compliance = 0x0002,
  XPMCompliance = 0x0004,
  AllCompliance = 0xffff
} ComplianceType;

extern MagickExport char
  **GetColorList(const char *pattern,unsigned long *number_colors);

extern MagickExport unsigned int
  QueryColorDatabase(const char *name,PixelPacket *color,ExceptionInfo *exception),
  QueryColorname(const Image *image,const PixelPacket *color,
    const ComplianceType compliance,char *name,ExceptionInfo *exception);

#if defined(MAGICK_IMPLEMENTATION)

/*
  Information about a named color (Internal).
*/
typedef struct _ColorInfo
{
  char
    *path,
    *name;

  ComplianceType
    compliance;

  PixelPacket
    color;

  unsigned int
    stealth;

  unsigned long
    signature;

  struct _ColorInfo
    *previous,
    *next;
} ColorInfo;

extern MagickExport const ColorInfo
  *GetColorInfo(const char *name, ExceptionInfo *exception);

extern MagickExport ColorInfo
  **GetColorInfoArray(ExceptionInfo *exception);

extern MagickExport void
  DestroyColorInfo(void);

extern MagickExport unsigned int
  ListColorInfo(FILE *file,ExceptionInfo *exception);

extern MagickPassFail
  InitializeColorInfo(void);

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COLOR_LOOKUP_H */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * fill-column: 78
 * End:
 */
