/* Compile-only FreeType stub (AURA CI build) - freetype/ftmm.h */
#ifndef FREETYPE_FTMM_H
#define FREETYPE_FTMM_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FT_MM_Axis_
{
    FT_Fixed  scale;
    FT_Fixed  min_value;
    FT_Fixed  max_value;
    FT_Fixed  default_value;
    FT_String name;
} FT_MM_Axis;

typedef struct FT_MM_NamedStyle_
{
    FT_Fixed coordinates[16];
} FT_MM_NamedStyle;

typedef struct FT_MM_Var_
{
    unsigned int      num_axis;
    unsigned int      num_namedstyles;
    FT_MM_Axis*       axis;
    FT_MM_NamedStyle* namedstyle;
} FT_MM_Var;

FT_Error FT_Get_MM_Var(FT_Face face, FT_MM_Var** avar);
void     FT_Done_MM_Var(FT_Library library, FT_MM_Var* mm_var);
FT_Error FT_Get_Var_Blend_Coordinates(FT_Face face, unsigned int numcoords, FT_Fixed* coords);
FT_Error FT_Set_Var_Design_Coordinates(FT_Face face, unsigned int numcoords, const FT_Fixed* coords);

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_FTMM_H */
