#pragma once
// ---------------------------------------------------------------------------
// AURA Windows cross-build shim.
//
// JUCE 8 officially supports MSVC only; this header fills small gaps in the
// Win32/UIA headers bundled with the zig cross toolchain so the (stripped)
// JUCE clone compiles unmodified where possible.
//
// UNICODE must be defined before any Win32 header is parsed (JUCE normally
// does this itself right before including them; this force-include shim runs
// first, so it sets it here).
// ---------------------------------------------------------------------------
#ifndef UNICODE
#define UNICODE 1
#endif
#ifndef _UNICODE
#define _UNICODE 1
#endif

#include <stddef.h>

// Wide-char secure printf: declared by the full UCRT, missing from zig's
// bundled headers (implementation is provided by zig's mingw runtime).
#ifdef __cplusplus
extern "C" int swprintf_s(wchar_t* _DstBuf, size_t _DstSizeInWords, const wchar_t* _Format, ...);
#else
int swprintf_s(wchar_t* _DstBuf, size_t _DstSizeInWords, const wchar_t* _Format, ...);
#endif

#include <uiautomation.h>
#include <nserror.h>   // NS_E_* HRESULTs (used by JUCE's Windows Media Audio format)

// UIA CaretPosition enum: present in the full Windows SDK (uiautomationcoreapi.h),
// missing from zig's bundled UIA headers.
#ifndef __aura_caret_position_defined
#define __aura_caret_position_defined
typedef enum CaretPosition
{
    CaretPosition_Unknown = 0,
    CaretPosition_BeginningOfLine = 1,
    CaretPosition_EndOfLine = 2
} CaretPosition;
#endif
