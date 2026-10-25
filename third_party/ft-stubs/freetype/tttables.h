/* Compile-only FreeType stub (AURA CI build) - freetype/tttables.h */
#ifndef FREETYPE_TTTABLES_H
#define FREETYPE_TTTABLES_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FT_Sfnt_Table_
{
    FT_ULong      tag;
    unsigned long offset;
    unsigned long length;
} FT_Sfnt_Table;

FT_Error FT_Load_Sfnt_Table(FT_Face face, FT_ULong tag, long offset, void* buffer, FT_ULong* length);
FT_Error FT_Sfnt_Table_Info(FT_Face face, long table_index, FT_ULong* tag, FT_ULong* length);

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_TTTABLES_H */
