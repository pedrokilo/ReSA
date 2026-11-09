#include "StdInc.h"

namespace details {
void* CPtrNodeDoubleLink__operator_new(size_t sz) {
    assert(sz == sizeof(CPtrNodeDoubleLink<void*>));
    return GetPtrNodeDoubleLinkPool()->New();
}

void CPtrNodeDoubleLink__operator_delete(void* data, size_t sz) {
    assert(sz == sizeof(CPtrNodeDoubleLink<void*>));
    GetPtrNodeDoubleLinkPool()->Delete(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(data));
}
};

namespace details {
// 0x552470 (lote 131A, banco Fn131A; el listado decia CPtrNodeDoubleLinkPool): Flush de una lista doble de punteros (thiscall, ECX = &cabeza, sin argumentos, `ret`, 112 bytes); codigo de plantilla
// del EXE compartido por todas las listas dobles (CSector::Dummies, CCover::Init, CGame::Shutdown, ...). Como el original: mientras haya cabeza, lee el siguiente (+4) ANTES de desenlazar; si el nodo
// es la cabeza la mueve al siguiente; si el nodo tiene anterior (+8) le pone como siguiente el (+4) del nodo; si tiene siguiente le pone como anterior el (+8) del nodo; y lo libera en linea en el pool doble
// (0xB74488, sin asserts): indice = (nodo - almacenamiento) / 12 (con signo, truncado hacia cero), estados[indice] |= 0x80 y, si indice < +0xC (con signo), +0xC = indice.
void __fastcall CPtrListDoubleLink__FlushExe(void* list, void* /*edx*/) {
    auto* const head = reinterpret_cast<uint8**>(list);
    uint8*      next = *head;
    if (!next) {
        return;
    }
    do {
        uint8* const cur = next;
        next             = *reinterpret_cast<uint8**>(cur + 4);
        if (*head == cur) {
            *head = next;
        }
        uint8* const prev = *reinterpret_cast<uint8**>(cur + 8);
        if (prev) {
            *reinterpret_cast<uint8**>(prev + 4) = *reinterpret_cast<uint8**>(cur + 4);
        }
        uint8* const after = *reinterpret_cast<uint8**>(cur + 4);
        if (after) {
            *reinterpret_cast<uint8**>(after + 8) = *reinterpret_cast<uint8**>(cur + 8);
        }
        auto* const pool = reinterpret_cast<uint8*>(GetPtrNodeDoubleLinkPool());
        const int32 idx  = int32(reinterpret_cast<uintptr_t>(cur) - reinterpret_cast<uintptr_t>(*reinterpret_cast<uint8**>(pool + 0x0))) / 12;
        *(*reinterpret_cast<uint8**>(pool + 0x4) + idx) |= 0x80;
        if (idx < *reinterpret_cast<int32*>(pool + 0xC)) {
            *reinterpret_cast<int32*>(pool + 0xC) = idx;
        }
    } while (next);
}
};
