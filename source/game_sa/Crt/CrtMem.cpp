#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <cstring>
#include "CrtMem.h"
#include "CrtHeap.h"
#include "CrtString.h"

// Ver CrtMem.h. Cada funcion delega en el CRT de MSVC con el que se enlaza la DLL (sin bucles propios: estas rutinas son el caso "ENLAZAR" del plan de codigo vivo).

namespace ExeCrt {
// 0x8266F0
void* __cdecl Memcpy(void* dest, const void* src, size_t count) {
    return std::memmove(dest, src, count);
}

// 0x822A20
void* __cdecl Memmove(void* dest, const void* src, size_t count) {
    return std::memmove(dest, src, count);
}

// 0x82D4C0
void* __cdecl Memset(void* dest, int value, size_t count) {
    return std::memset(dest, value, count);
}

// 0x8213D0
char* __cdecl Strchr(const char* str, int ch) {
    return const_cast<char*>(std::strchr(str, ch));
}

// 0x82514C: sin cadena (nulo) o sin memoria devuelve nulo
char* __cdecl Strdup(const char* str) {
    if (!str) {
        return nullptr;
    }
    const unsigned int size = Strlen(str) + 1;
    char* const copy = static_cast<char*>(Malloc(size));
    if (!copy) {
        return nullptr;
    }
    Strcpy(copy, str);
    return copy;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtMemInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Memcpy, 0x8266F0);
    RH_ScopedGlobalInstall(ExeCrt::Memmove, 0x822A20);
    RH_ScopedGlobalInstall(ExeCrt::Memset, 0x82D4C0);
    RH_ScopedGlobalInstall(ExeCrt::Strchr, 0x8213D0);
    RH_ScopedGlobalInstall(ExeCrt::Strdup, 0x82514C);
}
#endif
