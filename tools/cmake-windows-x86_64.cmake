# AURA Windows x86_64 cross-compile toolchain (zig-backed, no mingw-w64 needed).
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(_shim_dir "${CMAKE_CURRENT_LIST_DIR}/mingw-shim")
set(CMAKE_C_COMPILER "${_shim_dir}/x86_64-w64-mingw32-gcc")
set(CMAKE_CXX_COMPILER "${_shim_dir}/x86_64-w64-mingw32-g++")
set(CMAKE_RC_COMPILER "${_shim_dir}/x86_64-w64-mingw32-windres")
set(CMAKE_AR "${_shim_dir}/x86_64-w64-mingw32-ar" CACHE FILEPATH "")
# zig ar already writes the MS symbol table; GNU ranlib corrupts
# MS-format COFF archives (lld then cannot resolve their symbols).
set(CMAKE_RANLIB "${_shim_dir}/x86_64-w64-mingw32-ranlib" CACHE FILEPATH "")
# Assembly sources (.s) are assembled by the same zig cc driver.
set(CMAKE_ASM_COMPILER "${_shim_dir}/x86_64-w64-mingw32-cc" CACHE FILEPATH "")
set(CMAKE_FIND_ROOT_PATH /)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_EXECUTABLE_SUFFIX .exe)

# PE linking via zig's bundled lld
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

# JUCE 8 has an #error guard for __MINGW32__ (MinGW dropped as a supported
# compiler). This cross build uses standard Win32 APIs only, so the guard is
# bypassed and the 64-bit width is declared explicitly (JUCE normally derives
# it from _MSC_VER).
add_compile_options(-U__MINGW32__ -DJUCE_64BIT=1)
add_compile_options(-include "${CMAKE_CURRENT_LIST_DIR}/aura_win_shim.h")
# Gap headers (Dbghelp.h etc.) that zig's bundled Win32 headers lack.
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/win-gaps")
add_compile_options(-Wno-date-time)
set(AURA_WINDOWS_CROSS_BUILD ON)
