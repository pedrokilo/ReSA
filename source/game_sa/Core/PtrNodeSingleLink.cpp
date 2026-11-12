#include "StdInc.h"

namespace details {
// 0x552380 (lote 173A, banco Fn173A; cdecl, el EXE ignora el tamano de la pila, `ret`, 11 bytes: `mov ecx,[0xB74484]; jmp 0x552240`; el listado decia CPtrNodeDoubleLinkPool): operator new de los nodos
// de las listas simples de punteros: reserva un nodo en el pool simple (CPools 0xB74484) con New (0x552240; devuelve nulo con el pool lleno).
void* CPtrNodeSingleLink__operator_new(size_t sz) {
    assert(sz == sizeof(CPtrNodeSingleLink<void*>));
    return GetPtrNodeSingleLinkPool()->New();
}

// 0x552390 (lote 173A, banco Fn173A; cdecl, UN argumento (el EXE no recibe el tamano), `ret`, 41 bytes; el listado decia CPtrNodeDoubleLinkPool): operator delete de los nodos de las listas simples:
// libera el nodo en el pool simple (0xB74484) sin comprobar nada: indice = (nodo - almacenamiento) >> 3, estados[indice] |= 0x80 y, si indice < +0xC (con signo), +0xC = indice.
void CPtrNodeSingleLink__operator_delete(void* data) {
    GetPtrNodeSingleLinkPool()->Delete(reinterpret_cast<CPtrNodeSingleLink<void*>*>(data));
}
};

namespace details {
// 0x552400 (lote 131A, banco Fn131A; el listado decia CPtrNodeDoubleLinkPool): Flush de una lista simple de punteros (thiscall, ECX = &cabeza, sin argumentos, `ret`, 99 bytes); es el codigo de
// plantilla que el EXE comparte entre todas las listas simples (CSector::Buildings, CWorld::ShutDown, ...). Como el original: mientras haya cabeza, lee el siguiente (+4) ANTES de desenlazar, si el nodo es
// la cabeza la mueve al siguiente (si no, lo busca en la lista y lo salta; el EXE relee el siguiente del nodo), y libera el nodo en linea en el pool simple (CPools 0xB74484, sin asserts): indice =
// (nodo - almacenamiento) >> 3 (desplazamiento aritmetico), estados[indice] |= 0x80 y, si indice < +0xC (con signo), +0xC = indice.
void __fastcall CPtrListSingleLink__FlushExe(void* list, void* /*edx*/) {
    auto* const head = reinterpret_cast<uint8**>(list);
    uint8*      node = *head;
    if (!node) {
        return;
    }
    do {
        uint8* const cur  = node;
        node              = *reinterpret_cast<uint8**>(cur + 4);
        uint8* first      = *head;
        if (first == cur) {
            *head = node;
        } else {
            while (first) {
                uint8* const after = *reinterpret_cast<uint8**>(first + 4);
                if (after == cur) {
                    *reinterpret_cast<uint8**>(first + 4) = *reinterpret_cast<uint8**>(cur + 4);
                    break;
                }
                first = after;
            }
        }
        auto* const pool    = reinterpret_cast<uint8*>(GetPtrNodeSingleLinkPool());
        const int32 idx     = int32(reinterpret_cast<uintptr_t>(cur) - reinterpret_cast<uintptr_t>(*reinterpret_cast<uint8**>(pool + 0x0))) >> 3;
        *(*reinterpret_cast<uint8**>(pool + 0x4) + idx) |= 0x80;
        if (idx < *reinterpret_cast<int32*>(pool + 0xC)) {
            *reinterpret_cast<int32*>(pool + 0xC) = idx;
        }
    } while (node);
}
};
