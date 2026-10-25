/* Compile-only FreeType stub (AURA CI build) - freetype/ftoutline.h */
#ifndef FREETYPE_FTOPTLINE_H
#define FREETYPE_FTOPTLINE_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*FT_Outline_MoveTo_Func)  (const FT_Vector* to, void* user);
typedef int (*FT_Outline_LineTo_Func)  (const FT_Vector* to, void* user);
typedef int (*FT_Outline_ConicTo_Func) (const FT_Vector* control, const FT_Vector* to, void* user);
typedef int (*FT_Outline_CubicTo_Func) (const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user);

typedef struct FT_Outline_Funcs_
{
    FT_Outline_MoveTo_Func  move_to;
    FT_Outline_LineTo_Func  line_to;
    FT_Outline_ConicTo_Func conic_to;
    FT_Outline_CubicTo_Func cubic_to;
    FT_Fixed                shift;
    FT_Fixed                delta;
} FT_Outline_Funcs;

FT_Error FT_Outline_Decompose(FT_Outline* outline, const FT_Outline_Funcs* func_interface, void* user);
FT_Error FT_Outline_EmboldenXY(FT_Outline* outline, FT_Pos xstrength, FT_Pos ystrength);
FT_Error FT_Outline_Transform(FT_Outline* outline, const FT_Matrix* matrix);
FT_Error FT_Outline_Translate(FT_Outline* outline, const FT_Vector* delta);
FT_Error FT_Outline_Get_BBox(FT_Outline* outline, void* abbox);

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_FTOPTLINE_H */
