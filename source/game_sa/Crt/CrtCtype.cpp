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
#include "CrtState.h"

// Reconstruccion de las funciones de clasificacion de caracteres del CRT del EXE (lote CRT4A). Ver CrtCtype.h.

namespace ExeCrt {
namespace {
unsigned char* LocInfoOfThread() {
    unsigned char* li = static_cast<unsigned char*>(Getptd()->locInfo);
    if (li != *reinterpret_cast<unsigned char**>(0x8E2F14)) {
        li = static_cast<unsigned char*>(reinterpret_cast<void*(__cdecl*)()>(0x825456)()); // __updatetlocinfo
    }
    return li;
}
inline int Field(const unsigned char* li, int offset) { int v; std::memcpy(&v, li + offset, 4); return v; }

int ClassifyByMask(int c, int mask) {
    unsigned char* li = LocInfoOfThread();
    if (Field(li, 0x28) > 1) {
        return reinterpret_cast<int(__cdecl*)(void*, int, int)>(0x82AAE2)(li, c, mask); // _isctype
    }
    const unsigned char* table = *reinterpret_cast<unsigned char**>(li + 0x48);
    return table[c * 2] & mask;
}
} // namespace

// 0x825D50
int __cdecl Tolower(int c) {
    unsigned char* li = LocInfoOfThread();
    if (Field(li, 0x14) == 0 || (Field(li, 0x24) != 0 && static_cast<unsigned int>(c) <= 0x7F)) {
        return (c >= 0x41 && c <= 0x5A) ? c + 0x20 : c;
    }
    return reinterpret_cast<int(__cdecl*)(void*, int)>(0x825C88)(li, c); // _tolower_lk
}
int __cdecl Isdigit(int c)  { return ClassifyByMask(c, 4); }    // 0x822789
int __cdecl Isspace(int c)  { return ClassifyByMask(c, 8); }    // 0x822802
int __cdecl Isxdigit(int c) { return ClassifyByMask(c, 0x80); } // 0x8227C3
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtCtypeInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Tolower, 0x825D50);
    RH_ScopedGlobalInstall(ExeCrt::Isdigit, 0x822789);
    RH_ScopedGlobalInstall(ExeCrt::Isspace, 0x822802);
    RH_ScopedGlobalInstall(ExeCrt::Isxdigit, 0x8227C3);
}
#endif
