#include <cxxabi.h>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <exception>

typedef void (*throw_fn)(void*, std::type_info*, void (*)(void*));

extern "C" void __cxa_throw(void* thrown, std::type_info* tinfo, void (*dest)(void*))
{
    static throw_fn real_throw = (throw_fn)dlsym(RTLD_NEXT, "__cxa_throw");
    const char* nm = tinfo->name();
    int s = 0;
    char* d = abi::__cxa_demangle(tinfo->name(), nullptr, nullptr, &s);
    if (d) { nm = d; }
    fprintf(stderr, "[throwhook] exception thrown: %s\n", nm);
    fflush(stderr);
    // best-effort: if it derives from std::exception, print what() directly
    // (safe: same layout; vtable call only valid if it really is one)
    if (d && (s != 0)) { /* demangled ok */ }
    if (thrown)
    {
        // walk the type: if tinfo is a std::exception derivative, call what()
        // via its virtual function (2nd slot) - guarded by checking name prefix.
        const char* base = tinfo->name();
        if (d) free(d);
        // names of std exception types start with 'NSt...' and contain 'exception'
        // simplest robust check: compare against known std::exception typeinfo? skip.
    }
    real_throw(thrown, tinfo, dest);
}
