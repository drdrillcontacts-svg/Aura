/* Compile-only FreeType stubs (AURA CI build).
 * Mirrors the include layout of the real freetype2 development headers:
 *   #include <ft2build.h>  then  #include FT_FREETYPE_H  etc.
 * Only the API surface used by JUCE (juce_graphics) and its bundled
 * harfbuzz sources is provided. No runtime functionality.
 */
#ifndef FT2BUILD_H
#define FT2BUILD_H

#define FT_FREETYPE_H          "freetype/freetype.h"
#define FT_ADVANCES_H          "freetype/ftadvanc.h"
#define FT_COLOR_H             "freetype/ftcolor.h"
#define FT_GLYPH_H             "freetype/ftglyph.h"
#define FT_MODULE_H            "freetype/ftmodule.h"
#define FT_MULTIPLE_MASTERS_H  "freetype/ftmm.h"
#define FT_OUTLINE_H           "freetype/ftoutline.h"
#define FT_SYNTHESIS_H         "freetype/ftsyn.h"
#define FT_TRUETYPE_TABLES_H   "freetype/tttables.h"

#endif /* FT2BUILD_H */
