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

// 0x43E1A0 (lote 200A, sin validar; el listado decia CPlaceable; thiscall, sin argumentos, 5 bytes: `jmp 0x552400`; solo lo alcanzan saltos desde las funciones de destruccion estatica del EXE (0x837730, 0x837810, 0x837818,
// 0x83CC20, 0x83CCA0, ...), sin llamadores directos): salto puro al Flush de las listas simples (0x552400, CPtrListSingleLink__FlushExe, con hook); ECX (la cabeza de la lista) pasa intacto y no se toca nada mas.
void __fastcall CPtrListSingleLink__FlushJumpExe(void* list, void* edx) {
    CPtrListSingleLink__FlushExe(list, edx);
}
};

namespace details {
// 0x5335E0 (lote 222A, sin validar; el listado decia CEntity; thiscall, ECX = &cabeza, un argumento de pila (el puntero), `ret 4`, 47 bytes): AddItem de una lista simple de punteros, codigo de plantilla del EXE
// (el mismo que comparten CWorld y CSector). Como el original: reserva un nodo de 8 bytes con operator new de los nodos simples (0x552380, con hook; nulo si el pool esta lleno), guarda el puntero en +0,
// el antiguo primero en +4 (se lee la cabeza UNA vez) y deja el nodo como cabeza; devuelve el nodo en EAX. Con el pool lleno el EXE escribe en la direccion 4 (`xor eax,eax; mov [eax+4],edx`) y revienta: se
// reproduce con una escritura volatil en la direccion 4.
__declspec(noinline) void* __fastcall CPtrListSingleLink__AddItemExe(void* list, void* /*edx*/, void* item) {
    void** const head = reinterpret_cast<void**>(list);
    auto* const  node = static_cast<uint8*>(CPtrNodeSingleLink__operator_new(8));
    if (!node) {
        *reinterpret_cast<void* volatile*>(4) = *head;
        *head                                 = nullptr;
        return nullptr;
    }
    *reinterpret_cast<void**>(node) = item;
    void* const first               = *head;
    *reinterpret_cast<void**>(node + 4) = first;
    *head                               = node;
    return node;
}

// 0x533610 (lote 222A, sin validar; el listado decia CEntity; thiscall, ECX = &cabeza, un argumento de pila (el puntero), `ret 4`, 81 bytes): RemoveItem de una lista simple (plantilla del EXE). Como el original:
// lista vacia -> nada; recorre desde la cabeza guardando el anterior hasta encontrar el nodo cuyo +0 es el puntero (si no esta, termina sin tocar nada); si es la cabeza, la cabeza pasa a ser su +4; si no,
// el +4 del anterior pasa a ser el +4 del nodo; el nodo se libera con operator delete de los nodos simples (0x552390, con hook). No devuelve nada.
__declspec(noinline) void __fastcall CPtrListSingleLink__RemoveItemExe(void* list, void* /*edx*/, void* item) {
    void** const head  = reinterpret_cast<void**>(list);
    uint8* const first = static_cast<uint8*>(*head);
    uint8*       node  = first;
    uint8*       prev  = nullptr;
    if (!node) {
        return;
    }
    while (*reinterpret_cast<void**>(node) != item) {
        prev = node;
        node = *reinterpret_cast<uint8**>(node + 4);
        if (!node) {
            return;
        }
    }
    if (first == node) {
        *head = *reinterpret_cast<void**>(node + 4);
        CPtrNodeSingleLink__operator_delete(node);
        return;
    }
    if (prev) {
        *reinterpret_cast<void**>(prev + 4) = *reinterpret_cast<void**>(node + 4);
    }
    CPtrNodeSingleLink__operator_delete(node);
}
};
