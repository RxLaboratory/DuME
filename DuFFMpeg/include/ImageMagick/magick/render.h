/*
  Copyright (C) 2003 - 2018 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Drawing methods.
*/
#ifndef _MAGICK_RENDER_H
#define _MAGICK_RENDER_H

#include "magick/type.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedAlign,
  LeftAlign,
  CenterAlign,
  RightAlign
} AlignType;

typedef enum
{
  UserSpace,
  UserSpaceOnUse,
  ObjectBoundingBox
} ClipPathUnits;

typedef enum
{
  NoDecoration,
  UnderlineDecoration,
  OverlineDecoration,
  LineThroughDecoration
} DecorationType;

typedef enum
{
  UndefinedRule,
#undef EvenOddRule
  EvenOddRule,
  NonZeroRule
} FillRule;

typedef enum
{
  UndefinedGradient,
  LinearGradient,
  RadialGradient
} GradientType;

typedef enum
{
  UndefinedCap,
  ButtCap,
  RoundCap,
  SquareCap
} LineCap;

typedef enum
{
  UndefinedJoin,
  MiterJoin,
  RoundJoin,
  BevelJoin
} LineJoin;

typedef enum
{
  PointMethod = 0,
  ReplaceMethod,
  FloodfillMethod,
  FillToBorderMethod,
  ResetMethod
} PaintMethod;

typedef enum
{
  UndefinedPrimitive = 0,
  PointPrimitive,
  LinePrimitive,
  RectanglePrimitive,
  RoundRectanglePrimitive,
  ArcPrimitive,
  EllipsePrimitive,
  CirclePrimitive,
  PolylinePrimitive,
  PolygonPrimitive,
  BezierPrimitive,
  ColorPrimitive,
  MattePrimitive,
  TextPrimitive,
  ImagePrimitive,
  PathPrimitive
} PrimitiveType;

typedef enum
{
  UndefinedReference,
  GradientReference
} ReferenceType;

typedef enum
{
  UndefinedSpread,
  PadSpread,
  ReflectSpead,
  RepeatSpread
} SpreadMethod;

/*
  Typedef declarations.
*/
typedef struct _GradientInfo
{
  GradientType
    type;

  PixelPacket
    color;

  SegmentInfo
    stop;

  unsigned long
    length;

  SpreadMethod
    spread;

  unsigned long
    signature;

  struct _GradientInfo
    *previous,
    *next;
} GradientInfo;

typedef struct _ElementReference
{
  char
    *id;

  ReferenceType
    type;

  GradientInfo
    gradient;

  unsigned long
    signature;

  struct _ElementReference
    *previous,
    *next;
} ElementReference;

struct _DrawInfoExtra;  /* forward decl.; see member "extra" below */

typedef struct _DrawInfo
{
  char
    *primitive,
    *geometry;

  AffineMatrix
    affine;

  GravityType
    gravity;

  PixelPacket
    fill,
    stroke;

  double
    stroke_width;

  GradientInfo
    gradient;

  Image
    *fill_pattern,
    *tile,
    *stroke_pattern;

  unsigned int
    stroke_antialias,
    text_antialias;

  FillRule
    fill_rule;

  LineCap
    linecap;

  LineJoin
    linejoin;

  unsigned long
    miterlimit;

  double
    dash_offset;

  DecorationType
    decorate;

  CompositeOperator
    compose;

  char
    *text,
    *font,
    *family;

  StyleType
    style;

  StretchType
    stretch;

  unsigned long
    weight;

  char
    *encoding;

  double
    pointsize;

  char
    *density;

  AlignType
    align;

  PixelPacket
    undercolor,
    border_color;

  char
    *server_name;

  double
    *dash_pattern; /* Terminated by value 0.0 (i.e. < MagickEpsilon)*/

#if 0
  char
    *clip_path;
#endif
  /*
    Allow for expansion of DrawInfo without increasing its size.  The
    internals are defined only in render.c.  Clients outside of render.c
    can access the internals via the provided access functions (see below).

    This location in DrawInfo used to be occupied by char *clip_path. The
    clip_path member now lives in _DrawInfoExtra.
  */
  struct _DrawInfoExtra
    *extra;

  SegmentInfo
    bounds;

  ClipPathUnits
    clip_units;

  Quantum
    opacity;

  unsigned int
    render,
    /*
      Bit fields in flags:

        Bit 0: should this DrawInfo should be drawn as:
          0:  normal (original behavior)
          1:  SVG compliant
        Bit 1: is the DrawInfo a clipping path:
          0:  is not a clipping path
          1:  is a clipping path

      Access to these bits should be through functions (defined in render.c):
        IsDrawInfoClippingPath()
        IsDrawInfoSVGCompliant()
        IsDrawInfoSVGCompliantClippingPath()
        SetDrawInfoClippingPath()
        SetDrawInfoSVGCompliant()

      At the present time the SVG compliance bit only affects how clipping paths are drawn.
    */
    flags;  /* previously "unused1" */

  ElementReference
    element_reference;

  unsigned long
    signature;
} DrawInfo;

#if defined(MAGICK_IMPLEMENTATION)

typedef struct _ElementInfo
{
  double
    cx,
    cy,
    major,
    minor,
    angle;
} ElementInfo;

#endif /* if defined(MAGICK_IMPLEMENTATION) */

typedef struct _PointInfo
{
  double
    x,
    y;
} PointInfo;

#if defined(MAGICK_IMPLEMENTATION)

typedef struct _PrimitiveInfo
{
  PointInfo
    point;

  unsigned long
    coordinates;

  PrimitiveType
    primitive;

  PaintMethod
    method;

  char
    *text;

  /*
    "flags" indicates:

       bit 0:  shape/subpath is closed (e.g., rectangle, path with 'z' or 'Z')

    Macro arg "pi" is a PrimitiveInfo *.
    Macro arg "zero_or_one" should be 0 (turn off) or 1 (turn on).
  */
  unsigned long
    flags;
#define PRIMINF_CLEAR_FLAGS(pi) ((pi)->flags=0)
#define PRIMINF_GET_IS_CLOSED_SUBPATH(pi) ((MagickBool)((pi)->flags&1U))
#define PRIMINF_SET_IS_CLOSED_SUBPATH(pi,zero_or_one) ((pi)->flags=((pi)->flags&(~1U))|(unsigned long)zero_or_one)
} PrimitiveInfo;

#endif /* if defined(MAGICK_IMPLEMENTATION) */

typedef struct _TypeMetric
{
  PointInfo
    pixels_per_em;

  double
    ascent,
    descent,
    width,
    height,
    max_advance;

  SegmentInfo
    bounds;

  double
    underline_position,
    underline_thickness;
} TypeMetric;

/*
  Method declarations.
*/
extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport MagickPassFail
  AnnotateImage(Image *,const DrawInfo *),
  DrawAffineImage(Image *,const Image *,const AffineMatrix *),
  DrawClipPath(Image *,const DrawInfo *,const char *),
  DrawImage(Image *,const DrawInfo *),
  DrawPatternPath(Image *,const DrawInfo *,const char *,Image **),
  GetTypeMetrics(Image *,const DrawInfo *,TypeMetric *);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

/* provide public access to the clip_path member of DrawInfo */
extern MagickExport char
  **DrawInfoGetClipPath(const DrawInfo *) MAGICK_FUNC_PURE;

/* provide public access to the composite_path member of DrawInfo */
extern MagickExport char
  **DrawInfoGetCompositePath(const DrawInfo *) MAGICK_FUNC_PURE;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MAGICK_RENDER_H */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * fill-column: 78
 * End:
 */
