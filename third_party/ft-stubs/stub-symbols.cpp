// Link-time no-op definitions for external C entry points that JUCE's GUI
// modules reference directly (instead of via dlopen) when the real
// development libraries are not available.
//
// This file is only compiled for builds that do NOT link the real
// libfreetype / libX11 (see CMakeLists.txt). The returned errors make the
// font subsystem fail gracefully (empty font list); audio functionality is
// unaffected. On machines with the real libraries installed this file is
// never compiled.

extern "C" {

// ---- FreeType (referenced by JUCE's bundled harfbuzz sources) ----
int  FT_Init_FreeType(void* alibrary)            { if (alibrary) *(void**)alibrary = nullptr; return 1; }
void FT_Done_FreeType(void*) {}
int  FT_New_Face(void*, const char*, long, void*) { return 1; }
int  FT_New_Memory_Face(void*, const void*, long, long, void*) { return 1; }
int  FT_Reference_Face(void*)                     { return 1; }
void FT_Done_Face(void*) {}
int  FT_Set_Char_Size(void*, long, long, unsigned int, unsigned int) { return 0; }
int  FT_Set_Transform(void*, const void*, const void*) { return 0; }
int  FT_Get_Transform(void*, void*, void*)        { return 0; }
int  FT_Load_Glyph(void*, unsigned int, unsigned int) { return 0; }
int  FT_Get_Kerning(void*, unsigned int, unsigned int, int, void*) { return 0; }
unsigned int FT_Get_Char_Index(void*, unsigned long) { return 0; }
unsigned int FT_Get_Name_Index(void*, void*)     { return 0; }
int  FT_Get_Glyph_Name(void*, unsigned int, char*, unsigned int) { return 1; }
unsigned int FT_Face_GetCharVariantIndex(void*, unsigned long, unsigned long) { return 0; }
int  FT_Select_Charmap(void*, long)              { return 1; }
void FT_Set_Charmap(void*, void*) {}
int  FT_Get_Advance(void*, unsigned int, unsigned int, long*) { return 0; }
int  FT_Get_MM_Var(void*, void*)                 { return 1; }
void FT_Done_MM_Var(void*, void*) {}
int  FT_Get_Var_Blend_Coordinates(void*, unsigned int, long*) { return 0; }
int  FT_Set_Var_Design_Coordinates(void*, unsigned int, const long*) { return 0; }
int  FT_New_Library(void*, void*)                { return 1; }
void FT_Add_Default_Modules(void*) {}
void FT_Set_Default_Properties(void*) {}
int  FT_Reference_Library(void*)                 { return 1; }
void FT_Done_Library(void*) {}
int  FT_Outline_Decompose(void*, const void*, void*) { return 0; }
int  FT_Outline_EmboldenXY(void*, long, long)   { return 0; }
int  FT_Load_Sfnt_Table(void*, unsigned long, long, void*, unsigned long*) { return 1; }
int  FT_Sfnt_Table_Info(void*, long, unsigned long*, unsigned long*) { return 1; }
int  FT_Palette_Select(void*, unsigned int, void**) { return 1; }
int  FT_Get_Color_Glyph_Paint(void*, unsigned int, unsigned int, void**) { return 0; }
int  FT_Get_Color_Glyph_ClipBox(void*, unsigned int, void*) { return 0; }
int  FT_Get_Color_Glyph_Layer(void*, unsigned int, unsigned int*, unsigned int*, void*) { return 0; }
int  FT_Get_Paint(void*, void*, void*)          { return 0; }
int  FT_Get_Paint_Layers(void*, void*, void**)  { return 0; }
int  FT_Get_Colorline_Stops(void*, void*, void*) { return 0; }

// ---- X11 (JUCE's dlsym fallback references these directly) ----
int  XDefaultScreen(void*)                       { return 0; }
long XDefaultScreenOfDisplay(void*)              { return 0; }

} // extern "C"
