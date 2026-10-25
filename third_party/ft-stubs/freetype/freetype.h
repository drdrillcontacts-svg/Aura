/* Compile-only FreeType stub (AURA CI build) - freetype/freetype.h */
#ifndef FREETYPE_FREETYPE_H
#define FREETYPE_FREETYPE_H

#define FREETYPE_MAJOR 2
#define FREETYPE_MINOR 13
#define FREETYPE_PATCH 2

#ifdef __cplusplus
extern "C" {
#endif

/* ---- basic types ------------------------------------------------ */
typedef signed long    FT_Long;
typedef unsigned long  FT_ULong;
typedef signed int     FT_Int;
typedef unsigned int   FT_UInt;
typedef unsigned short FT_ushort;
typedef unsigned char  FT_Byte;
typedef long           FT_Fixed;   /* 16.16 */
typedef long           F2Dot14;
typedef long           F16Dot16;
typedef long           F26Dot6;
typedef long           FT_Pos;
typedef int            FT_Error;
typedef int            FT_Bool;
typedef const char*    FT_String;

#define FT_Err_Ok 0

/* ---- memory ------------------------------------------------------ */
struct FT_MemoryRec_;
typedef struct FT_MemoryRec_* FT_Memory;

typedef void* (*FT_AllocFunc)  (FT_Memory memory, long size);
typedef void  (*FT_FreeFunc)   (FT_Memory memory, void* block);
typedef void* (*FT_ReallocFunc)(FT_Memory memory, long cur_size, long new_size, void* block);

struct FT_MemoryRec_
{
    void*         user;
    FT_AllocFunc  alloc_function;
    FT_FreeFunc   free_function;
    FT_ReallocFunc realloc_function;
};

/* ---- geometry ---------------------------------------------------- */
typedef struct FT_Vector_
{
    FT_Pos x;
    FT_Pos y;
} FT_Vector;

typedef struct FT_Matrix_
{
    FT_Fixed xx, xy, yx, yy;
} FT_Matrix;

/* ---- encodings ---------------------------------------------------- */
typedef long FT_Encoding;
#define FT_ENCODING_NONE        0x00000000L
#define FT_ENCODING_MS_SYMBOL   0x73796d62L /* 'sym' */
#define FT_ENCODING_UNICODE     0x756e6963L /* 'unic' */
#define FT_ENCODING_ADOBE_LATIN_1 0x61646c31L
#define FT_ENCODING_OLD_LATIN_2   0x61646c32L
#define FT_ENCODING_SJIS          0x736a6973L
#define FT_ENCODING_PRC           0x70726320L
#define FT_ENCODING_GB2312        0x67622072L
#define FT_ENCODING_BIG5          0x62696735L

/* deprecated lowercase aliases kept for compatibility */
#define ft_encoding_none          FT_ENCODING_NONE
#define ft_encoding_unicode       FT_ENCODING_UNICODE
#define ft_encoding_symbol        FT_ENCODING_MS_SYMBOL
#define ft_encoding_latin_1       FT_ENCODING_ADOBE_LATIN_1
#define ft_encoding_latin_2       FT_ENCODING_OLD_LATIN_2
#define ft_encoding_sjis          FT_ENCODING_SJIS
#define ft_encoding_gb2312        FT_ENCODING_PRC
#define ft_encoding_big5          FT_ENCODING_BIG5

/* ---- face flags ---------------------------------------------------- */
#define FT_STYLE_FLAG_NORMAL     0
#define FT_STYLE_FLAG_ITALIC     1
#define FT_STYLE_FLAG_BOLD       2

#define FT_FACE_FLAG_SCALABLE         (1 << 0)
#define FT_FACE_FLAG_FIXED_WIDTH      (1 << 4)
#define FT_FACE_FLAG_COLOR            (1 << 6)

/* ---- glyph formats / pixel modes ----------------------------------- */
typedef enum FT_Glyph_Format_
{
    FT_GLYPH_FORMAT_NONE = 0,
    FT_GLYPH_FORMAT_OUTLINE,
    FT_GLYPH_FORMAT_COMPOSITE,
    FT_GLYPH_FORMAT_BITMAP,
    FT_GLYPH_FORMAT_MAX
} FT_Glyph_Format;

typedef enum FT_Pixel_Mode_
{
    FT_PIXEL_MODE_MONO  = 0,
    FT_PIXEL_MODE_GRAY  = 1,
    FT_PIXEL_MODE_GRAY2 = 2,
    FT_PIXEL_MODE_GRAY4 = 3,
    FT_PIXEL_MODE_GRAY8 = 4,
    FT_PIXEL_MODE_BGRA  = 7,
    FT_PIXEL_MODE_BGRX  = 8,
    FT_PIXEL_MODE_RGBA  = 9,
    FT_PIXEL_MODE_MAX
} FT_Pixel_Mode;

/* ---- load flags ---------------------------------------------------- */
#define FT_LOAD_DEFAULT             0
#define FT_LOAD_NO_HINTING          (1L << 1)
#define FT_LOAD_FORCE_AUTOHINT      (1L << 2)
#define FT_LOAD_NO_ROUND            (1L << 3)
#define FT_LOAD_MONOCHROME          (1L << 4)
#define FT_LOAD_NO_BITMAP           (1L << 5)
#define FT_LOAD_VERTICAL_LAYOUT     (1L << 6)
#define FT_LOAD_IGNORE_WIDTH        (1L << 7)
#define FT_LOAD_BITMAP_METRICS_ONLY (1L << 8)
#define FT_LOAD_COLOR               (1L << 9)
#define FT_LOAD_NO_AUTOHINT         (1L << 15)

typedef enum FT_Kerning_Mode_
{
    FT_KERNING_DEFAULT = 0,
    FT_KERNING_UNSCALED,
    FT_KERNING_UNFITTED
} FT_Kerning_Mode;

/* ---- charmaps ------------------------------------------------------ */
struct FT_CharMapRec;
typedef struct FT_CharMapRec* FT_CharMap;
struct FT_CharMapRec { FT_Encoding encoding; };

/* ---- generic hook -------------------------------------------------- */
typedef void (*FT_Generic_Finalizer)(void* object);

typedef struct FT_Generic_
{
    void*                data;
    FT_Generic_Finalizer finalizer;
} FT_Generic;

/* ---- sizes --------------------------------------------------------- */
typedef struct FT_Bitmap_Size_
{
    char*    family_name;
    char*    style_name;
    FT_UInt  x_ppem;
    FT_UInt  y_ppem;
    FT_Pos   width;
    FT_Pos   height;
    FT_Int   horz_resolution;
    FT_Int   vert_resolution;
} FT_Bitmap_Size;

typedef struct FT_Size_Metrics_
{
    FT_Fixed x_scale;
    FT_Fixed y_scale;
    FT_UInt  x_ppem;
    FT_UInt  y_ppem;
    FT_Pos   width;
    FT_Pos   height;
    FT_Pos   ascender;
    FT_Pos   descender;
} FT_Size_Metrics;

struct FT_SizeRec;
typedef struct FT_SizeRec* FT_Size;

struct FT_SizeRec
{
    FT_Size_Metrics metrics;
};

/* ---- library ------------------------------------------------------- */
struct FT_LibraryRec;
typedef struct FT_LibraryRec* FT_Library;

/* ---- outline ------------------------------------------------------- */
typedef struct FT_Outline_
{
    FT_Pos       n_points;
    FT_Pos       n_contours;
    FT_Vector*   points;
    FT_Pos*      contours;
    FT_Byte*     tags;
    FT_Int       flags;
    FT_UInt      face_flags;
} FT_Outline;

/* ---- glyph slot ---------------------------------------------------- */
typedef struct FT_Bitmap_
{
    FT_Pixel_Mode pixel_mode;
    unsigned int  width;
    unsigned int  rows;
    int           pitch;
    unsigned int  num_grays;
    unsigned int  pixel;
    unsigned int  palette;
    void*         buffer;
} FT_Bitmap;

typedef struct FT_Glyph_Metrics_
{
    FT_Pos   width, height;
    FT_Pos   xoffset, yoffset;
    FT_Pos   x0, y0, x1, y1;
    FT_Fixed horiAdvance;
    FT_Pos   horiBearingX, horiBearingY;
    FT_Pos   vertBearingX, vertBearingY;
    FT_Pos   vertAdvance;
} FT_Glyph_Metrics;

typedef struct FT_GlyphSlotRec_
{
    FT_Outline         outline;
    FT_Glyph_Metrics   metrics;
    FT_Bitmap          bitmap;
    FT_Library         library;
    FT_Glyph_Format    format;
} FT_GlyphSlotRec;
typedef FT_GlyphSlotRec* FT_GlyphSlot;

/* ---- stream -------------------------------------------------------- */
typedef FT_Error (*FT_Stream_ReadFunc)(void* stream, unsigned char* buffer, unsigned long count);

typedef struct FT_StreamRec_
{
    unsigned char*     base;
    unsigned long      size;
    unsigned long      position;
    FT_Stream_ReadFunc read;
    void*              descriptor;
} FT_Stream;
typedef struct FT_StreamRec_* FT_StreamPtr;

/* ---- face ------------------------------------------------------------ */
struct FT_FaceRec
{
    int             num_faces;
    int             face_index;
    FT_Glyph_Format glyph_format;
    int             face_flags;
    int             style_flags;
    FT_Long         num_glyphs;

    FT_Bitmap_Size* available_sizes;
    int             num_fixed_sizes;

    FT_Generic      generic;
    FT_Size         size;
    FT_GlyphSlot    glyph;

    FT_CharMap      charmap;
    FT_CharMap*     charmaps;
    int             num_charmaps;

    FT_Long         units_per_EM;
    FT_Long         ascender;
    FT_Long         descender;
    FT_Long         height;
    FT_Long         max_advance_width;
    FT_Long         max_advance_height;

    FT_String       family_name;
    FT_String       style_name;

    FT_StreamPtr    stream;
};
typedef struct FT_FaceRec* FT_Face;

/* ---- functions ------------------------------------------------------- */
FT_Error  FT_Init_FreeType(FT_Library* alibrary);
void      FT_Done_FreeType(FT_Library library);

FT_Error  FT_New_Face(FT_Library library, const char* filename, long face_index, FT_Face* aface);
FT_Error  FT_New_Memory_Face(FT_Library library, const FT_Byte* file_base, FT_Long file_size, long face_index, FT_Face* aface);
FT_Error  FT_Reference_Face(FT_Face face);
void      FT_Done_Face(FT_Face face);

FT_Error  FT_Set_Char_Size(FT_Face face, FT_Fixed char_width, FT_Fixed char_height, unsigned int horz_resolution, unsigned int vert_resolution);
FT_Error  FT_Set_Transform(FT_Face face, const FT_Matrix* matrix, const FT_Vector* delta);
FT_Error  FT_Get_Transform(FT_Face face, FT_Matrix* matrix, FT_Vector* adelta);

FT_Error  FT_Load_Glyph(FT_Face face, unsigned int glyph_index, unsigned int load_flags);
FT_Error  FT_Get_Kerning(FT_Face face, unsigned int left, unsigned int right, FT_Kerning_Mode mode, FT_Vector* adelta);

FT_UInt   FT_Get_Char_Index(FT_Face face, FT_ULong char_code);
FT_UInt   FT_Get_Name_Index(FT_Face face, void* glyph_name);
FT_Error  FT_Get_Glyph_Name(FT_Face face, unsigned int glyph_index, char* name_buf, unsigned int buf_len);
FT_UInt   FT_Face_GetCharVariantIndex(FT_Face face, FT_ULong char_code, FT_ULong variation_selector);

FT_Error  FT_Select_Charmap(FT_Face face, FT_Encoding charset);
void      FT_Set_Charmap(FT_Face face, FT_CharMap charmap);

static inline FT_Fixed FT_MulFix(FT_Fixed a, FT_Fixed b) { return (FT_Fixed)(((long long)a * b) >> 16); }
static inline FT_Fixed FT_DivFix(FT_Fixed a, FT_Fixed b) { return (FT_Fixed)(((long long)a << 16 / (b == 0 ? 1 : b))); }
static inline FT_Fixed FT_MulDiv(FT_Fixed a, FT_Fixed b, FT_Fixed c) { return (FT_Fixed)(((long long)a * b / (c == 0 ? 1 : c))); }

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_FREETYPE_H */
