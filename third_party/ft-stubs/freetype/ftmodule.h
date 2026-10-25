/* Compile-only FreeType stub (AURA CI build) - freetype/ftmodule.h */
#ifndef FREETYPE_FTMODULE_H
#define FREETYPE_FTMODULE_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

FT_Error FT_New_Library(FT_Memory memory, FT_Library* alibrary);
void     FT_Add_Default_Modules(FT_Library library);
void     FT_Set_Default_Properties(FT_Library library);
FT_Error FT_Reference_Library(FT_Library library);
void     FT_Done_Library(FT_Library library);

#ifdef __cplusplus
}
#endif

#endif /* FREETYPE_FTMODULE_H */
