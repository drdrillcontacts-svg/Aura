/*
 * AURA Windows cross-build: msvcrt gap fillers.
 *
 * zig's x86_64-windows-gnu target links against zig's own CRT support
 * (zigc_zcu.obj), which references a few msvcrt symbols that are not
 * provided in this cross setup (long-double math variants, __isnan*,
 * the NaN/Inf sentinel constants, and CoCreateGuid from ole32).
 * This file provides them so the plugin DLL links. The double-based
 * fallbacks are only reached on long-double paths that AURA's DSP does
 * not take. The sentinel data objects themselves are emitted as raw
 * bit patterns in aura_crt_gaps_data.s.
 */
#include <math.h>
#include <stddef.h>
#include <stdint.h>

/*----------------------------------------------------------------------*/
/* long double math (msvcrt exports these; implemented via double)      */
/*----------------------------------------------------------------------*/
long double atanl(long double x)
{
    return (long double) atan((double) x);
}

long double copysignl(long double x, long double y)
{
    return (long double) copysign((double) x, (double) y);
}

long double fdiml(long double x, long double y)
{
    double dx = (double) x, dy = (double) y;
    double r = dx > dy ? dx - dy : 0.0;
    return (long double) r;
}

float frexpf(float x, int* exponent)
{
    return (float) frexp((double) x, exponent);
}

long double frexpl(long double x, int* exponent)
{
    return (long double) frexp((double) x, exponent);
}

long lrintl(long double x)
{
    return (long) lrint((double) x);
}

long lroundl(long double x)
{
    return (long) lround((double) x);
}

long double modfl(long double x, long double* integerPart)
{
    double ip;
    double fp = modf((double) x, &ip);
    *integerPart = (long double) ip;
    return (long double) fp;
}

long double nanl(const char* tag)
{
    (void) tag;
    return (long double) (double) NAN;
}

long double rintl(long double x)
{
    return (long double) rint((double) x);
}

/*----------------------------------------------------------------------*/
/* __isnan family (msvcrt)                                              */
/*----------------------------------------------------------------------*/
int __isnan(double x)       { return x != x; }
int __isnanf(float x)       { return x != x; }
int __isnanl(long double x) { return (double) x != (double) x; }

/*----------------------------------------------------------------------*/
/* Sentinel constants defined in aura_crt_gaps_data.s (bit patterns)    */
/*----------------------------------------------------------------------*/
const unsigned int __DENORM = 0x00400000u;
const unsigned int __INF    = 0x7F800000u;
const unsigned int __QNAN   = 0x7FC00000u;
const unsigned int __SNAN   = 0x7F800001u;

extern const float      __DENORMF;
extern const float      __INFF;
extern const float      __QNANF;
extern const float      __SNANF;
extern const long double __DENORML;
extern const long double __INFL;
extern const long double __QNANL;
extern const long double __SNANL;

/*----------------------------------------------------------------------*/
/* CoCreateGuid (ole32) — pseudo-unique GUIDs for the VST3 wrapper      */
/*----------------------------------------------------------------------*/
#include <winerror.h>
#include <combaseapi.h>

static uint64_t auraGuidCounter = 0x2F6E2B1Full;

HRESULT CoCreateGuid(GUID* g)
{
    if (g == NULL)
        return E_INVALIDARG;

    uint64_t entropy = (uint64_t) (uintptr_t) g
                     ^ (uint64_t) (uintptr_t) &g
                     ^ (auraGuidCounter += 0x9E3779B97F4A7C15ull);

    uint8_t* p = (uint8_t*) g;
    for (int i = 0; i < 16; ++i)
        p[i] = (uint8_t) (entropy += 0x61C8864680B583EBull);

    p[6] = (uint8_t) ((p[6] & 0x0F) | 0x40); /* version 4 */
    p[8] = (uint8_t) ((p[8] & 0x3F) | 0x80); /* RFC 4122 variant */
    return S_OK;
}
