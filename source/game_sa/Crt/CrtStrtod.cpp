#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <cstdint>
#include <cstring>
#include <limits>
#include "CrtStrtod.h"
#include "CrtFloatIn.h"
#include "CrtCtype.h"
#include "CrtLowIo.h"
#include "CrtString.h"

// Ver CrtStrtod.h. Reconstruido desde el desensamblado del EXE, sin __asm: toda la aritmetica de 96 bits la hacen las funciones de CrtFloatIn (enteras, sin x87).

namespace ExeCrt {
// 0x82BF62 (el EXE guarda ademas una galleta de pila alrededor del cuerpo: sin efecto observable)
Flt* __cdecl Fltin2(Flt* out, const char* str, int /*len*/, int /*scale*/, int /*decpt*/) {
    Ld12        ld{};
    const char* end = str;
    uint32_t    dbl[2] = {0, 0};
    int         flags  = 0;

    const int ret = Strgtold12(&ld, &end, str, 0, 0, 0, 0);
    if (ret & 4) { // sin digitos
        flags = 0x200;
    } else {
        const int conv = Ld12ToDouble(&ld, dbl);
        if ((ret & 2) || conv == 1) {
            flags = 0x80;
        }
        if ((ret & 1) || conv == 2) {
            flags |= 0x100;
        }
    }
    out->flags  = flags;
    out->nbytes = static_cast<int>(end - str);
    std::memcpy(&out->dval, dbl, sizeof(double));
    return out;
}

// 0x826071
double __cdecl Strtod(const char* str, char** end) {
    const char* p = str;
    while (Isspace(static_cast<unsigned char>(*p))) {
        ++p;
    }
    Flt flt{};
    Fltin2(&flt, p, static_cast<int>(Strlen(p)), 0, 0);
    if (end) {
        *end = const_cast<char*>(p) + flt.nbytes;
    }
    if (flt.flags & 0x240) { // sin numero
        if (end) {
            *end = const_cast<char*>(str);
        }
        return 0.0;
    }
    if (flt.flags & 0x81) { // desbordamiento: +-inf y ERANGE
        const double inf = std::numeric_limits<double>::infinity();
        *ErrnoPtr() = 0x22;
        return *p == '-' ? -inf : inf;
    }
    if (flt.flags & 0x100) { // desbordamiento por abajo: 0.0 y ERANGE
        *ErrnoPtr() = 0x22;
        return 0.0;
    }
    return flt.dval;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtStrtodInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Fltin2, 0x82BF62);
    RH_ScopedGlobalInstall(ExeCrt::Strtod, 0x826071);
}
#endif
