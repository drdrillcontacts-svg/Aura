/*
 * AURA Windows cross-build: __mingw_uuidof gap fillers.
 *
 * zig's mingw headers map __uuidof(x) to __mingw_uuidof<__typeof(x)>()
 * (clang-based toolchain => USE___UUIDOF == 0). JUCE's DirectX interop
 * calls __uuidof on ComSmartPtr variables (e.g. GetDevice(__uuidof(device))),
 * which instantiates __mingw_uuidof for the smart-pointer type itself;
 * the primary template has no generic definition, so those two
 * instantiations are provided here.
 *
 * The specializations must be plain (non-inline, non-constexpr) functions:
 * a constexpr/inline definition that is unreferenced within this
 * translation unit would not be emitted, leaving the referencing
 * translation units with unresolved external symbols.
 * The GUID values match the platform SDK (dxgi.h).
 */
#include <windows.h>

#if defined(__cplusplus) && (USE___UUIDOF == 0)

namespace juce
{
    template <class ComClass> class ComSmartPtr;
}

struct IDXGIDevice;
struct IDXGISurface;

template <> const GUID & __mingw_uuidof<juce::ComSmartPtr<IDXGIDevice>>()
{
    // IID_IDXGIDevice = 54ec77fa-1377-44e6-8c32-88fd5f44c84c
    static const GUID inst = { 0x54ec77fa, 0x1377, 0x44e6,
                               { 0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c } };
    return inst;
}

template <> const GUID & __mingw_uuidof<juce::ComSmartPtr<IDXGISurface>>()
{
    // IID_IDXGISurface = cafcb56c-6ac3-4889-bf47-9e23bbd260ec
    static const GUID inst = { 0xcafcb56c, 0x6ac3, 0x4889,
                               { 0xbf, 0x47, 0x9e, 0x23, 0xbb, 0xd2, 0x60, 0xec } };
    return inst;
}

#endif
