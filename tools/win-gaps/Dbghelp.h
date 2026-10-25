#pragma once
// Minimal Dbghelp.h for the AURA Windows cross build (zig's bundled Win32
// headers do not include it). Layouts follow the Windows SDK; only the
// symbols JUCE's juce_SystemStats.cpp uses are declared.
#ifndef _DBGHELP_H_
#define _DBGHELP_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SYM_NAME 2048
#define MAX_PATH_DBG 260

typedef struct _SYMBOL_INFO
{
    DWORD     SizeOfStruct;
    DWORD     TypeIndex;
    ULONG64   Reserved[2];
    ULONG64   Address;
    ULONG     Size;
    ULONG64   ModBase;
    DWORD     Flags;
    ULONG64   Value;
    ULONG64   AddressString;
    ULONG     Index;
    ULONG     BlockIndex;
    ULONG     MaxNameLen;
    char      Name[1];
} SYMBOL_INFO, *PSYMBOL_INFO;

typedef struct _IMAGEHLP_MODULE64
{
    DWORD     SizeOfStruct;
    DWORD32   Type;
    DWORD     BaseOfImage;
    DWORD     SizeOfImage;
    DWORD     Characteristics;
    DWORD     TimeDateStamp;
    DWORD     LoaderFlags;
    DWORD     RegionSize;
    DWORD     Reserved1;
    DWORD     Reserver2;
    WORD      Machine;
    WORD      Characteristics2;
    WCHAR     FileName[MAX_PATH_DBG];
    WORD      CheckSum;
    WORD      NumSections;
    WORD     NumberOfHeaders;
    WORD      SizeOfOptionalHeader;
    WORD      Reserver3[10];
    ULONG     Magic;
    WCHAR     ModuleName[MAX_SYM_NAME];
    WCHAR     ImageName[MAX_PATH_DBG];
    ULONG     LoadedImageHint;
} IMAGEHLP_MODULE64, *PIMAGEHLP_MODULE64;

BOOL WINAPI SymInitialize (HANDLE hProcess, PSTR UserSearchPath, BOOL fLoadSymbolics);
BOOL WINAPI SymCleanup (HANDLE hProcess);
BOOL WINAPI SymFromAddr (HANDLE hProcess, DWORD64 dwAddress, PDWORD64 pdwDisplacement, PSYMBOL_INFO pSymbolInfo);
BOOL WINAPI SymGetModuleInfo64 (HANDLE hProcess, DWORD64 dwAddr, PIMAGEHLP_MODULE64 ModuleInfo);
// CaptureStackBackTrace is provided by zig's winbase.h (macro to RtlCaptureStackBackTrace).

#ifdef __cplusplus
}
#endif

#endif // _DBGHELP_H_
