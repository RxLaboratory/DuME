/*
  Copyright (C) 2003 - 2018 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  GraphicsMagick Utility Methods.
*/
#ifndef _MAGICK_UTILITY_H
#define _MAGICK_UTILITY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  RootPath,
  HeadPath,
  TailPath,
  BasePath,
  ExtensionPath,
  MagickPath,
  SubImagePath,
  FullPath
} PathType;

/*
  Typedef declarations.
*/
typedef struct _TokenInfo
{
  int
    state;

  unsigned int
    flag;

  long
    offset;

  char
    quote;
} TokenInfo;

/*
  Utilities methods.
*/

#undef ARG_NOT_USED
#define ARG_NOT_USED(arg) (void) arg

/*
  A callback function which behaves similar to strlcpy() except which
  optionally translates text while it is copied and always returns
  the number of characters which were actually copied rather than
  the number of characters which were available to copy.
*/
typedef size_t
  (*MagickTextTranslate)(char *dst, const char *src, const size_t size);

extern MagickExport char
  *AcquireString(const char *),
  *AllocateString(const char *),
  *Base64Encode(const unsigned char *,const size_t,size_t *),
  *EscapeString(const char *,const char),
  *GetPageGeometry(const char *),
  **ListFiles(const char *,const char *,long *),
  **StringToArgv(const char *,int *),
  **StringToList(const char *),
  *TranslateText(const ImageInfo *,Image *,const char *),
  *TranslateTextEx(const ImageInfo *,Image *,const char *,MagickTextTranslate);

extern MagickExport const char
  *GetClientFilename(void) MAGICK_FUNC_CONST,
  *GetClientName(void) MAGICK_FUNC_CONST,
  *GetClientPath(void) MAGICK_FUNC_CONST,
  *SetClientFilename(const char *),
  *SetClientName(const char *),
  *SetClientPath(const char *);

extern MagickExport double
  StringToDouble(const char *,const double);

extern MagickExport int
  GetGeometry(const char *,long *,long *,unsigned long *,unsigned long *),
  GlobExpression(const char *,const char *) MAGICK_FUNC_PURE,
  LocaleNCompare(const char *,const char *,const size_t) MAGICK_FUNC_PURE,
  LocaleCompare(const char *,const char *) MAGICK_FUNC_PURE,
  GetMagickDimension(const char *str,double *width,double *height,double *xoff,double *yoff),
  GetMagickGeometry(const char *geometry,long *x,long *y,unsigned long *width,
    unsigned long *height),
  MagickRandReentrant(unsigned int *seed) MAGICK_FUNC_DEPRECATED,
  MagickSpawnVP(const unsigned int verbose, const char *file, char *const argv[]),
  SystemCommand(const unsigned int,const char *),
  Tokenizer(TokenInfo *,unsigned,char *,size_t,char *,char *,char *,char *,
    char,char *,int *,char *);

extern MagickExport unsigned int
  MagickRandNewSeed(void) MAGICK_FUNC_DEPRECATED;

extern MagickExport unsigned char
  *Base64Decode(const char *, size_t *);

extern MagickExport MagickPassFail
  CloneString(char **,const char *),
  ConcatenateString(char **,const char *),
  ExpandFilenames(int *,char ***),
  GetExecutionPath(char *),
  GetExecutionPathUsingName(char *),
  MagickCreateDirectoryPath(const char *dir,ExceptionInfo *exception);

extern MagickExport MagickBool
  IsAccessible(const char *),
  IsAccessibleNoLogging(const char *),
  IsAccessibleAndNotEmpty(const char *),
  IsGeometry(const char *),
  IsGlob(const char *) MAGICK_FUNC_PURE,
  IsWriteable(const char *),
  MagickSceneFileName(char *filename,const char* filename_template,
    const char* scene_template,const MagickBool force,unsigned long scene),
  SubstituteString(char **buffer,const char *search,const char *replace);

extern MagickExport unsigned long
  MultilineCensus(const char *) MAGICK_FUNC_PURE;

extern MagickExport void
  AppendImageFormat(const char *,char *),
  DefineClientName(const char *),
  DefineClientPathAndName(const char *),
  ExpandFilename(char *),
  FormatSize(const magick_int64_t size,char *format),
  GetPathComponent(const char *,PathType,char *),
  GetToken(const char *,char **,char *) MAGICK_FUNC_DEPRECATED,
  LocaleLower(char *),
  LocaleUpper(char *),
  Strip(char *),
  SetGeometry(const Image *,RectangleInfo *);

extern MagickExport void
  FormatString(char *string,const char *format,...) MAGICK_ATTRIBUTE((__format__ (__printf__,2,3))),
  FormatStringList(char *string,const char *format,va_list operands) MAGICK_ATTRIBUTE((__format__ (__printf__,2,0))),
  MagickFormatString(char *string,const size_t length,const char *format,...) MAGICK_ATTRIBUTE((__format__ (__printf__,3,4))),
  MagickFormatStringList(char *string,const size_t length,const char *format,va_list operands) MAGICK_ATTRIBUTE((__format__ (__printf__,3,0)));

extern MagickExport magick_int64_t
  MagickSizeStrToInt64(const char *str,const unsigned int kilo);

extern MagickExport size_t
  MagickGetToken(const char *start,char **end,char *token,
                 const size_t buffer_length),
  MagickStripSpacesFromString(char *string),
  MagickStrlCat(char *dst, const char *src, const size_t size) MAGICK_FUNC_NONNULL,
  MagickStrlCpy(char *dst, const char *src, const size_t size) MAGICK_FUNC_NONNULL,
  MagickStrlCpyTrunc(char *dst, const char *src, const size_t size) MAGICK_FUNC_NONNULL;

#if defined(MAGICK_IMPLEMENTATION)

/*
  Force argument into range accepted by <ctype.h> functions.
*/
#define CTYPE_ARG(value) ((int) ((unsigned char) (value)))

#if !defined(HAVE_STRLCAT)
#  define strlcat(dst,src,size) MagickStrlCat(dst,src,size)
#endif

#if !defined(HAVE_STRLCPY)
#  define strlcpy(dst,src,size) MagickStrlCpy(dst,src,size)
#endif

extern double MagickFmin(const double x, const double y) MAGICK_FUNC_CONST;
extern double MagickFmax(const double x, const double y) MAGICK_FUNC_CONST;

extern MagickExport MagickPassFail MagickAtoFChk(const char *str, double *value);
extern MagickExport MagickPassFail MagickAtoIChk(const char *str, int *value);
extern MagickExport MagickPassFail MagickAtoUIChk(const char *str, unsigned int *value);
extern MagickExport MagickPassFail MagickAtoLChk(const char *str, long *value);
extern MagickExport MagickPassFail MagickAtoULChk(const char *str, unsigned long *value);

/*
  Compute a value which is the next kilobyte power of 2 larger than
  the requested value or MaxTextExtent, whichever is larger.

  The objective is to round up the size quickly (and in repeatable
  steps) in order to reduce the number of memory copies due to realloc
  for strings which grow rapidly, while producing a reasonable size
  for smaller strings.
*/
#define MagickRoundUpStringLength(size) \
{ \
  size_t \
    _rounded; \
 \
  for (_rounded=256U; _rounded < (Max(size,256)); _rounded *= 2); \
  size=_rounded; \
}

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * fill-column: 78
 * End:
 */
