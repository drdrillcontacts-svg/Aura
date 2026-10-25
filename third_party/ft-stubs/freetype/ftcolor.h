/* Compile-only FreeType stub (AURA CI build) - freetype/ftcolor.h */
#ifndef FREETYPE_FTCOLOR_H
#define FREETYPE_FTCOLOR_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FT_Color_
{
    FT_Byte red;
    FT_Byte green;
    FT_Byte blue;
    FT_Byte alpha;
} FT_Color;

typedef struct FT_ClipBox_
{
    FT_Vector bottom_left;
    FT_Vector top_right;
    FT_Vector top_left;
    FT_Vector bottom_right;
} FT_ClipBox;

typedef struct FT_Affine_
{
    FT_Fixed xx, xy, yx, yy;
    FT_Fixed dx, dy;
} FT_Affine;

struct FT_Paint_;
typedef struct FT_Paint_* FT_OpaquePaint;

typedef struct FT_COLR_PaintPalette_
{
    FT_ushort palette_index;
    FT_Byte   alpha;
} FT_COLR_PaintPalette;

typedef struct FT_ColorStop_
{
    FT_Byte              alpha;
    FT_COLR_PaintPalette color;
    FT_Fixed             stop_offset;
} FT_ColorStop;

typedef struct FT_ColorStopIterator_
{
    FT_UInt         num_color_stops;
    FT_UInt         num_color_stops_left;
    FT_UInt         current_color_stop;
} FT_ColorStopIterator;

typedef int FT_COLR_PaintExtend;
#define FT_COLR_PAINT_EXTEND_PAD     0
#define FT_COLR_PAINT_EXTEND_REPEAT  1
#define FT_COLR_PAINT_EXTEND_REFLECT 2

typedef struct FT_ColorLine_
{
    FT_ColorStopIterator color_stop_iterator;
    FT_COLR_PaintExtend  extend;
    FT_Pixel_Mode        mode;
    FT_Vector            start_point;
    FT_Vector            end_point;
    FT_Fixed             center;
    FT_UInt              num_stops;
    FT_ColorStop*        color_stops;
} FT_ColorLine;

typedef struct FT_LayerIterator_
{
    FT_UInt     num_layers;
    FT_UInt     layer;
    const void* p;
} FT_LayerIterator;

typedef int FT_COLR_PaintFormat;
#define FT_COLR_PAINTFORMAT_SOLID            0
#define FT_COLR_PAINTFORMAT_GLYPH            1
#define FT_COLR_PAINTFORMAT_COLR_GLYPH       2
#define FT_COLR_PAINTFORMAT_COLR_LAYERS      3
#define FT_COLR_PAINTFORMAT_LINEAR_GRADIENT  4
#define FT_COLR_PAINTFORMAT_RADIAL_GRADIENT  5
#define FT_COLR_PAINTFORMAT_SWEEP_GRADIENT   6
#define FT_COLR_PAINTFORMAT_TRANSFORM        7
#define FT_COLR_PAINTFORMAT_TRANSLATE        8
#define FT_COLR_PAINTFORMAT_SCALE            9
#define FT_COLR_PAINTFORMAT_SKEW             10
#define FT_COLR_PAINTFORMAT_ROTATE           11
#define FT_COLR_PAINTFORMAT_COMPOSITE        12
#define FT_COLR_PAINTFORMAT_UNSUPPORTED      13
#define FT_COLR_PAINT_FORMAT_MAX             14

typedef struct FT_COLR_PaintSolid_
{
    FT_COLR_PaintPalette color;
} FT_COLR_PaintSolid;

typedef struct FT_COLR_PaintGlyph_
{
    FT_UInt        glyphID;
    FT_OpaquePaint paint;
} FT_COLR_PaintGlyph;

typedef struct FT_COLR_PaintColrGlyph_
{
    FT_UInt glyphID;
} FT_COLR_PaintColrGlyph;

typedef struct FT_COLR_PaintLayers_
{
    FT_LayerIterator layer_iterator;
} FT_COLR_PaintLayers;

typedef struct FT_COLR_PaintLinearGradient_
{
    FT_ColorLine colorline;
    FT_Vector    p0;
    FT_Vector    p1;
    FT_Vector    p2;
} FT_COLR_PaintLinearGradient;

typedef struct FT_COLR_PaintRadialGradient_
{
    FT_ColorLine colorline;
    FT_Vector    c0;
    FT_Vector    c1;
    FT_Fixed     r0;
    FT_Fixed     r1;
} FT_COLR_PaintRadialGradient;

typedef struct FT_COLR_PaintSweepGradient_
{
    FT_ColorLine colorline;
    FT_Vector    center;
    FT_Fixed     start_angle;
    FT_Fixed     end_angle;
} FT_COLR_PaintSweepGradient;

typedef struct FT_COLR_PaintTransform_
{
    FT_Affine      affine;
    FT_OpaquePaint paint;
} FT_COLR_PaintTransform;

typedef struct FT_COLR_PaintTranslate_
{
    FT_Fixed       dx, dy;
    FT_OpaquePaint paint;
} FT_COLR_PaintTranslate;

typedef struct FT_COLR_PaintScale_
{
    FT_Fixed       center_x, center_y;
    FT_Fixed       scale_x, scale_y;
    FT_OpaquePaint paint;
} FT_COLR_PaintScale;

typedef struct FT_COLR_PaintSkew_
{
    FT_Fixed       center_x, center_y;
    FT_Fixed       x_skew_angle, y_skew_angle;
    FT_OpaquePaint paint;
} FT_COLR_PaintSkew;

typedef struct FT_COLR_PaintRotate_
{
    FT_Fixed       center_x, center_y;
    FT_Fixed       angle;
    FT_OpaquePaint paint;
} FT_COLR_PaintRotate;

typedef int FT_Composite_Mode;
#define FT_COLR_COMPOSITE_CLEAR           0
#define FT_COLR_COMPOSITE_SRC             1
#define FT_COLR_COMPOSITE_DEST            2
#define FT_COLR_COMPOSITE_SRC_OVER        3
#define FT_COLR_COMPOSITE_DEST_OVER       4
#define FT_COLR_COMPOSITE_SRC_IN          5
#define FT_COLR_COMPOSITE_DEST_IN         6
#define FT_COLR_COMPOSITE_SRC_OUT         7
#define FT_COLR_COMPOSITE_DEST_OUT        8
#define FT_COLR_COMPOSITE_SRC_ATOP        9
#define FT_COLR_COMPOSITE_DEST_ATOP       10
#define FT_COLR_COMPOSITE_XOR             11
#define FT_COLR_COMPOSITE_PLUS            12
#define FT_COLR_COMPOSITE_SCREEN          13
#define FT_COLR_COMPOSITE_OVERLAY         14
#define FT_COLR_COMPOSITE_DARKEN          15
#define FT_COLR_COMPOSITE_LIGHTEN         16
#define FT_COLR_COMPOSITE_COLOR_DODGE     17
#define FT_COLR_COMPOSITE_COLOR_BURN      18
#define FT_COLR_COMPOSITE_HARD_LIGHT      19
#define FT_COLR_COMPOSITE_SOFT_LIGHT      20
#define FT_COLR_COMPOSITE_DIFFERENCE      21
#define FT_COLR_COMPOSITE_EXCLUSION       22
#define FT_COLR_COMPOSITE_MULTIPLY        23
#define FT_COLR_COMPOSITE_HSL_HUE         24
#define FT_COLR_COMPOSITE_HSL_SATURATION  25
#define FT_COLR_COMPOSITE_HSL_COLOR       26
#define FT_COLR_COMPOSITE_HSL_LUMINOSITY  27
#define FT_COLR_COMPOSITE_MAX             28

typedef struct FT_COLR_PaintComposite_
{
    FT_OpaquePaint   backdrop_paint;
    FT_OpaquePaint   source_paint;
    FT_Composite_Mode composite_mode;
} FT_COLR_PaintComposite;

typedef struct FT_Paint_
{
    FT_COLR_PaintFormat format;
    union
    {
        FT_COLR_PaintSolid           solid;
        FT_COLR_PaintGlyph           glyph;
        FT_COLR_PaintColrGlyph       colr_glyph;
        FT_COLR_PaintLayers          colr_layers;
        FT_COLR_PaintLinearGradient  linear_gradient;
        FT_COLR_PaintRadialGradient  radial_gradient;
        FT_COLR_PaintSweepGradient   sweep_gradient;
        FT_COLR_PaintTransform       transform;
        FT_COLR_PaintTranslate       translate;
        FT_COLR_PaintScale           scale;
        FT_COLR_PaintSkew            skew;
        FT_COLR_PaintRotate          rotate;
        FT_COLR_PaintComposite       composite;
    } u;
} FT_Paint;

typedef FT_Paint FT_COLR_Paint;

#define FT_COLOR_NO_ROOT_TRANSFORM 0

FT_Error FT_Palette_Select(FT_Face face, FT_UInt palette_index, FT_Color** palette);
FT_Error FT_Palette_Set_Default(FT_Face face);
FT_Bool  FT_Get_Color_Glyph_Paint(FT_Face face, FT_UInt glyph, FT_UInt flags, FT_OpaquePaint* paint);
FT_Bool  FT_Get_Color_Glyph_ClipBox(FT_Face face, FT_UInt glyph, FT_ClipBox* clip_box);
FT_Bool  FT_Get_Color_Glyph_Layer(FT_Face face, FT_UInt glyph, FT_UInt* glyph_index, FT_UInt* palette_index, FT_LayerIterator* iterator);
FT_Bool  FT_Get_Paint(FT_Face face, FT_OpaquePaint opaque_paint, FT_Paint* paint);
FT_Bool  FT_Get_Paint_Layers(FT_Face face, FT_LayerIterator* iterator, FT_OpaquePaint* paint);
FT_Bool  FT_Get_Colorline_Stops(FT_Face face, FT_ColorStop* stop, FT_ColorStopIterator* iterator);

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_FTCOLOR_H */
