/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ExeCrt
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include "CrtString.h"

// Reconstruccion en C++ de las funciones de cadenas del CRT del EXE (lote CRT1A). Ver CrtString.h.
// Sin intrinsecos de cadena del compilador (los bucles son explicitos) para que el resultado no dependa de otra copia del CRT.

namespace ExeCrt {
// 0x826330
unsigned int __cdecl Strlen(const char* str) {
    const char* p = str;
    while (*p) {
        ++p;
    }
    return static_cast<unsigned int>(p - str);
}

// 0x8263C0
int __cdecl Strcmp(const char* a, const char* b) {
    const auto* pa = reinterpret_cast<const unsigned char*>(a);
    const auto* pb = reinterpret_cast<const unsigned char*>(b);
    for (;; ++pa, ++pb) {
        if (*pa != *pb) {
            return *pa < *pb ? -1 : 1;
        }
        if (*pa == 0) {
            return 0;
        }
    }
}

// 0x8214D0
int __cdecl Strncmp(const char* a, const char* b, unsigned int count) {
    const auto* pa = reinterpret_cast<const unsigned char*>(a);
    const auto* pb = reinterpret_cast<const unsigned char*>(b);
    for (; count; --count, ++pa, ++pb) {
        if (*pa != *pb) {
            return *pa < *pb ? -1 : 1;
        }
        if (*pa == 0) {
            return 0;
        }
    }
    return 0;
}

// 0x826590
char* __cdecl Strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++) != 0) {
    }
    return dest;
}

// 0x8265A0
char* __cdecl Strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) {
        ++d;
    }
    while ((*d++ = *src++) != 0) {
    }
    return dest;
}

// 0x821F40
char* __cdecl Strncpy(char* dest, const char* src, unsigned int count) {
    char* d = dest;
    while (count && (*d = *src) != 0) {
        ++d;
        ++src;
        --count;
    }
    while (count) { // relleno con ceros hasta count (tambien el primer cero si se copio hasta el final de la cadena)
        *d++ = 0;
        --count;
    }
    return dest;
}

// 0x826450
char* __cdecl Strncat(char* dest, const char* src, unsigned int count) {
    if (count == 0) {
        return dest;
    }
    char* d = dest;
    while (*d) {
        ++d;
    }
    while (count-- && (*d++ = *src++) != 0) {
    }
    if (count == static_cast<unsigned int>(-1)) { // se agoto count sin ver el cero de src
        *d = 0;
    }
    return dest;
}

// 0x822650
char* __cdecl Strstr(const char* haystack, const char* needle) {
    if (*needle == 0) {
        return const_cast<char*>(haystack);
    }
    for (const char* h = haystack; *h; ++h) {
        const char* a = h;
        const char* b = needle;
        while (*a && *b && *a == *b) {
            ++a;
            ++b;
        }
        if (*b == 0) {
            return const_cast<char*>(h);
        }
    }
    return nullptr;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtStringInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Strlen, 0x826330);
    RH_ScopedGlobalInstall(ExeCrt::Strcmp, 0x8263C0);
    RH_ScopedGlobalInstall(ExeCrt::Strncmp, 0x8214D0);
    RH_ScopedGlobalInstall(ExeCrt::Strcpy, 0x826590);
    RH_ScopedGlobalInstall(ExeCrt::Strcat, 0x8265A0);
    RH_ScopedGlobalInstall(ExeCrt::Strncpy, 0x821F40);
    RH_ScopedGlobalInstall(ExeCrt::Strncat, 0x826450);
    RH_ScopedGlobalInstall(ExeCrt::Strstr, 0x822650);
}
#endif
