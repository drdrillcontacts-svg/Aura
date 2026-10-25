/* Compile-only FreeType stub (AURA CI build) - freetype/ftadvanc.h */
#ifndef FREETYPE_FTADVANC_H
#define FREETYPE_FTADVANC_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

FT_Error FT_Get_Advance(FT_Face face, unsigned int glyph_index, unsigned int load_flags, FT_Fixed* advance);
FT_Error FT_Get_Advances(FT_Face face, unsigned int n, void* glyphs, int mode, void* advances);

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_FTADVANC_H */
