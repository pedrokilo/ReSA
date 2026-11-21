/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ExeCrt
 * Reconstrucción: COMPLETA — 63/63 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/63 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <cstring>
#include "CrtCtype.h"
#include "CrtLocale.h"
#include "CrtState.h"

// Reconstruccion de las funciones de clasificacion de caracteres y de cadenas dependientes del locale del CRT del EXE (lotes CRT4A y CRT4A-locale).
// Ver CrtCtype.h.

namespace ExeCrt {
namespace {
// Clasificacion por mascara: con mb_cur_max > 1 pasa por _isctype (IsctypeMt); si no, lee la tabla de tipos; la anchura de la lectura
// (byte o palabra de 16 bits) es la de cada funcion del EXE
template <bool WordRead>
int ClassifyByMask(int c, int mask) {
    LocInfo* const li = CurrentLocInfo();
    if (li->mbCurMax > 1) {
        return IsctypeMt(li, c, mask);
    }
    if constexpr (WordRead) {
        unsigned short w;
        std::memcpy(&w, li->pctype + c * 2, 2);
        return w & mask;
    } else {
        return li->pctype[c * 2] & mask;
    }
}
} // namespace

// 0x825D50
int __cdecl Tolower(int c) {
    LocInfo* const li = CurrentLocInfo();
    if (li->handle[2] == 0 || (li->clike != 0 && static_cast<unsigned int>(c) <= 0x7F)) {
        return (c >= 0x41 && c <= 0x5A) ? c + 0x20 : c;
    }
    return TolowerLk(li, c);
}
// 0x823A0E
int __cdecl Toupper(int c) {
    return ToupperLk(CurrentLocInfo(), c);
}
int __cdecl Isdigit(int c)  { return ClassifyByMask<false>(c, 4); }    // 0x822789
int __cdecl Isspace(int c)  { return ClassifyByMask<false>(c, 8); }    // 0x822802
int __cdecl Isxdigit(int c) { return ClassifyByMask<false>(c, 0x80); } // 0x8227C3
int __cdecl Isalpha(int c)  { return ClassifyByMask<true>(c, 0x103); } // 0x8226D6
int __cdecl Isupper(int c)  { return ClassifyByMask<false>(c, 1); }    // 0x822715
int __cdecl Islower(int c)  { return ClassifyByMask<false>(c, 2); }    // 0x82274F
int __cdecl Isalnum(int c)  { return ClassifyByMask<true>(c, 0x107); } // 0x822876

// 0x822506 (_atoi 0x82258E es un salto a esta): el locale solo decide que es un espacio
int __cdecl Atol(const char* str) {
    LocInfo* const li = CurrentLocInfo();
    const unsigned char* p = reinterpret_cast<const unsigned char*>(str);
    for (;;) {
        const unsigned int ch = *p;
        const int space = li->mbCurMax > 1 ? IsctypeMt(li, static_cast<int>(ch), 8) : (li->pctype[ch * 2] & 8);
        if (!space) {
            break;
        }
        ++p;
    }
    unsigned int c = *p++;
    const unsigned int sign = c;
    if (c == '-' || c == '+') {
        c = *p++;
    }
    unsigned int total = 0;
    while (c >= 0x30 && c <= 0x39) {
        total = total * 10u + (c - 0x30u);
        c = *p++;
    }
    return static_cast<int>(sign == '-' ? 0u - total : total);
}

// 0x82ABC0: comparacion sin distinguir mayusculas, solo A-Z (devuelve -1, 0 o 1)
int __cdecl AsciiStricmp(const char* a, const char* b) {
    const unsigned char* pa = reinterpret_cast<const unsigned char*>(a);
    const unsigned char* pb = reinterpret_cast<const unsigned char*>(b);
    const auto lower = [](unsigned char ch) -> unsigned char { return static_cast<unsigned char>(ch - 0x41u) < 0x1Au ? static_cast<unsigned char>(ch + 0x20) : ch; };
    unsigned char c2;
    do {
        c2 = *pb++;
        const unsigned char c1 = *pa++;
        if (c1 != c2) {
            const unsigned char l1 = lower(c1);
            const unsigned char l2 = lower(c2);
            if (l1 != l2) {
                return l1 < l2 ? -1 : 1;
            }
        }
    } while (c2 != 0);
    return 0;
}

// 0x836990: igual, con limite de n caracteres (se detiene tambien en el primer cero de cualquiera de las dos)
int __cdecl AsciiStrnicmp(const char* a, const char* b, unsigned int n) {
    if (n == 0) {
        return 0;
    }
    const unsigned char* pa = reinterpret_cast<const unsigned char*>(a);
    const unsigned char* pb = reinterpret_cast<const unsigned char*>(b);
    unsigned char c1, c2;
    for (;;) {
        c1 = *pa;
        c2 = *pb;
        if (c1 == 0 || c2 == 0) {
            break;
        }
        ++pa;
        ++pb;
        if (c1 >= 0x41 && c1 <= 0x5A) {
            c1 = static_cast<unsigned char>(c1 + 0x20);
        }
        if (c2 >= 0x41 && c2 <= 0x5A) {
            c2 = static_cast<unsigned char>(c2 + 0x20);
        }
        if (c1 != c2) {
            return c1 < c2 ? -1 : 1;
        }
        if (--n == 0) {
            break;
        }
    }
    if (c1 == c2) {
        return 0;
    }
    return c1 < c2 ? -1 : 1;
}

// 0x8229B6
int __cdecl Stricmp(const char* a, const char* b) {
    LocInfo* const li = CurrentLocInfo();
    if (li->handle[2] == 0) {
        return AsciiStricmp(a, b);
    }
    const unsigned char* pa = reinterpret_cast<const unsigned char*>(a);
    const unsigned char* pb = reinterpret_cast<const unsigned char*>(b);
    int f, c;
    do {
        f = TolowerLk(li, *pa);
        c = TolowerLk(li, *pb);
        ++pa;
        ++pb;
    } while (f != 0 && f == c);
    return f - c;
}

// 0x835AC7
int __cdecl Strnicmp(const char* a, const char* b, unsigned int n) {
    LocInfo* const li = CurrentLocInfo();
    if (n == 0) {
        return 0;
    }
    if (li->handle[2] == 0) {
        return AsciiStrnicmp(a, b, n);
    }
    const unsigned char* pa = reinterpret_cast<const unsigned char*>(a);
    const unsigned char* pb = reinterpret_cast<const unsigned char*>(b);
    int f, c;
    do {
        f = TolowerLk(li, *pa);
        c = TolowerLk(li, *pb);
        ++pa;
        ++pb;
        --n;
    } while (n != 0 && f != 0 && f == c);
    return f - c;
}

namespace {
// Nucleo de _strlwr (0x836C58) y _strupr (0x836FBA): locale "C" = A-Z / a-z con signo (los bytes >= 0x80 no se tocan); otro locale =
// __crtLCMapStringA sobre toda la cadena y copia del resultado
char* CaseString(char* str, bool upper) {
    LocInfo* const li = CurrentLocInfo();
    if (li->handle[2] == 0) {
        for (char* p = str; *p; ++p) {
            const signed char ch = static_cast<signed char>(*p);
            if (upper) {
                if (ch >= 0x61 && ch <= 0x7A) {
                    *p = static_cast<char>(ch - 0x20);
                }
            } else if (ch >= 0x41 && ch <= 0x5A) {
                *p = static_cast<char>(ch + 0x20);
            }
        }
        return str;
    }
    const unsigned int flag = upper ? 0x200u : 0x100u;
    const int n = CrtLCMapStringA(li->handle[2], flag, str, -1, nullptr, 0, static_cast<int>(li->codePage), 1);
    if (n == 0) {
        return str;
    }
    char stackBuf[0x400];
    char* buf = stackBuf;
    char* heap = nullptr;
    if (n > static_cast<int>(sizeof stackBuf)) {
        heap = static_cast<char*>(reinterpret_cast<void*(__cdecl*)(unsigned int)>(0x824257)(static_cast<unsigned int>(n))); // malloc del EXE
        if (!heap) {
            return str;
        }
        buf = heap;
    }
    if (CrtLCMapStringA(li->handle[2], flag, str, -1, buf, n, static_cast<int>(li->codePage), 1)) {
        const char* s = buf;
        for (char* d = str; (*d++ = *s++) != 0;) {}
    }
    if (heap) {
        reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(heap);
    }
    return str;
}
} // namespace

char* __cdecl Strlwr(char* str) { return CaseString(str, false); } // 0x836C58
char* __cdecl Strupr(char* str) { return CaseString(str, true); }  // 0x836FBA
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtCtypeInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Tolower, 0x825D50);
    RH_ScopedGlobalInstall(ExeCrt::Toupper, 0x823A0E);
    RH_ScopedGlobalInstall(ExeCrt::Isdigit, 0x822789);
    RH_ScopedGlobalInstall(ExeCrt::Isspace, 0x822802);
    RH_ScopedGlobalInstall(ExeCrt::Isxdigit, 0x8227C3);
    RH_ScopedGlobalInstall(ExeCrt::Isalpha, 0x8226D6);
    RH_ScopedGlobalInstall(ExeCrt::Isupper, 0x822715);
    RH_ScopedGlobalInstall(ExeCrt::Islower, 0x82274F);
    RH_ScopedGlobalInstall(ExeCrt::Isalnum, 0x822876);
    RH_ScopedGlobalInstall(ExeCrt::Atol, 0x822506);
    RH_ScopedGlobalInstall(ExeCrt::AsciiStricmp, 0x82ABC0);
    RH_ScopedGlobalInstall(ExeCrt::AsciiStrnicmp, 0x836990);
    RH_ScopedGlobalInstall(ExeCrt::Stricmp, 0x8229B6);
    RH_ScopedGlobalInstall(ExeCrt::Strnicmp, 0x835AC7);
    RH_ScopedGlobalInstall(ExeCrt::Strlwr, 0x836C58);
    RH_ScopedGlobalInstall(ExeCrt::Strupr, 0x836FBA);
}
#endif
