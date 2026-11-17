/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: TList_c
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "List_c.h"

// Lote 105A (banco Fn105A): las tres funciones de TList_c<T> que el EXE conserva como codigo compartido por todas las instancias (el diseno de la lista es el mismo para cualquier T):
// AddItem (0x4A8DF0), RemoveAll (0x4A8EB0) y GetNumItems (0x4A8EC0). Los cuerpos son los del EXE (sin las comprobaciones de la plantilla, que en Debug abortarian con listas del banco) y
// cada acceso va en el orden del EXE (volatile: si el elemento solapa la lista o el antiguo primero es la propia lista, las escrituras anteriores se ven). Son thiscall en el EXE: __fastcall con
// un EDX ficticio, igual que thiscall (ECX = lista y los argumentos de pila los limpia el callee).
namespace {
struct ListC105ANode {
    ListC105ANode* prev;
    ListC105ANode* next;
};
struct ListC105AList {
    ListC105ANode* head;
    ListC105ANode* tail;
    uint32       count;
};

// 0x4A8DF0 (`ret 4`; EAX = recuento anterior): el elemento pasa a ser el primero.
uint32 __fastcall ListAddItemExe(ListC105AList* list, void* /*edx*/, ListC105ANode* item)
{
    volatile ListC105AList* const l = list;
    volatile ListC105ANode* const n = item;
    ListC105ANode* const oldHead = l->head;
    l->head = item;
    n->prev = nullptr;
    n->next = oldHead;
    if (oldHead) {
        static_cast<volatile ListC105ANode*>(oldHead)->prev = item;
    } else {
        l->tail = item;
    }
    const uint32 count = l->count;
    l->count = count + 1;
    return count;
}

// 0x4A8EB0 (EAX = 0)
uint32 __fastcall ListRemoveAllExe(ListC105AList* list, void* /*edx*/)
{
    volatile ListC105AList* const l = list;
    l->head = nullptr;
    l->tail = nullptr;
    l->count = 0;
    return 0;
}

// 0x4A8EC0
uint32 __fastcall ListGetNumItemsExe(const ListC105AList* list, void* /*edx*/)
{
    return static_cast<const volatile ListC105AList*>(list)->count;
}

// 0x4A8DD0 (lote 219A, sin validar; el listado decia FxInfoManager_c; thiscall, sin argumentos de pila, `ret`, 13 bytes): constructor de TList_c<T> del EXE, codigo compartido por todas las instancias:
// `mov eax,ecx; xor ecx,ecx; mov [eax],ecx; mov [eax+4],ecx; mov [eax+8],ecx; ret`. Deja a cero cabeza (+0), cola (+4) y contador (+8), en ese orden, y devuelve this. ECX sale a cero (nadie lo lee).
ListC105AList* __fastcall ListConstructorExe(ListC105AList* list, void* /*edx*/)
{
    volatile ListC105AList* const l = list;
    l->head  = nullptr;
    l->tail  = nullptr;
    l->count = 0;
    return list;
}

// 0x4A8DE0 (lote 219A, sin validar; thiscall, `ret`, 1 byte): destructor de TList_c<T> del EXE. No hace nada (la lista no libera sus elementos).
void __fastcall ListDestructorExe(ListC105AList* /*list*/, void* /*edx*/)
{
}

// 0x4A8E30 (lote 219A, sin validar; thiscall con el elemento en la pila, `ret 4`, 59 bytes; EAX = recuento anterior): RemoveItem. Lee siguiente (+4) y anterior (+0) del elemento, en ese orden. Si hay siguiente, su anterior pasa
// a ser el anterior del elemento; si no, la cola de la lista (+4) pasa a ser el anterior. Si hay anterior, su siguiente pasa a ser el siguiente; si no, la cabeza (+0) pasa a ser el siguiente, RELEIDO del elemento.
// Despues recuento - 1. No comprueba que el elemento pertenezca a la lista ni que el recuento sea mayor que cero (las comprobaciones de la plantilla se omiten, como en 105A).
uint32 __fastcall ListRemoveItemExe(ListC105AList* list, void* /*edx*/, ListC105ANode* item)
{
    volatile ListC105AList* const l = list;
    volatile ListC105ANode* const n = item;
    ListC105ANode* const next = n->next;
    ListC105ANode* const prev = n->prev;
    if (next) {
        static_cast<volatile ListC105ANode*>(next)->prev = prev;
    } else {
        l->tail = prev;
    }
    if (prev) {
        static_cast<volatile ListC105ANode*>(prev)->next = next;
    } else {
        l->head = n->next;
    }
    const uint32 count = l->count;
    l->count = count - 1;
    return count;
}

// 0x4A8E70 (lote 219A, sin validar; thiscall sin argumentos de pila, `ret`, 54 bytes): RemoveHead. Devuelve la cabeza anterior (EAX) o nulo si la lista esta vacia (sin tocar nada). Con un solo elemento (cabeza == cola)
// calcula recuento - 1, deja cola y cabeza a cero y guarda el recuento. Si no, la cabeza pasa a ser el siguiente de la cabeza (+4), el anterior de esa nueva cabeza (+0) se pone a cero si existe y el recuento baja en uno.
ListC105ANode* __fastcall ListRemoveHeadExe(ListC105AList* list, void* /*edx*/)
{
    volatile ListC105AList* const l = list;
    ListC105ANode* const oldHead = l->head;
    if (!oldHead) {
        return nullptr;
    }
    if (oldHead == l->tail) {
        const uint32 count = l->count - 1;
        l->tail  = nullptr;
        l->head  = nullptr;
        l->count = count;
        return oldHead;
    }
    ListC105ANode* const next = static_cast<volatile ListC105ANode*>(oldHead)->next;
    l->head = next;
    if (next) {
        static_cast<volatile ListC105ANode*>(next)->prev = nullptr;
    }
    l->count = l->count - 1;
    return oldHead;
}

// 0x4A9010 (lote 219A, sin validar; thiscall con (desdeCabeza: BYTE, desplazamiento: int32) en la pila, `ret 8`, 67 bytes): GetItemOffset. Parte de la cabeza (+0 de la lista) o de la cola (+4) segun el BYTE; con
// desplazamiento <= 0 (comparacion con signo) devuelve ese extremo tal cual (puede ser nulo). Si no, hasta `desplazamiento` pasos (contador con signo) por siguiente (+4) desde la cabeza o por anterior (+0) desde la cola;
// si el nodo actual es nulo antes de un paso devuelve nulo. Devuelve el nodo final (nulo si se pasa del extremo).
ListC105ANode* __fastcall ListGetItemOffsetExe(const ListC105AList* list, void* /*edx*/, uint8 fromHead, int32 offset)
{
    const volatile ListC105AList* const l = list;
    if (fromHead) {
        ListC105ANode* node = l->head;
        if (offset <= 0) {
            return node;
        }
        int32 step = 0;
        do {
            if (!node) {
                return nullptr;
            }
            node = static_cast<volatile ListC105ANode*>(node)->next;
            ++step;
        } while (step < offset);
        return node;
    }
    ListC105ANode* node = l->tail;
    if (offset <= 0) {
        return node;
    }
    int32 step = 0;
    do {
        if (!node) {
            return nullptr;
        }
        node = static_cast<volatile ListC105ANode*>(node)->prev;
        ++step;
    } while (step < offset);
    return node;
}
} // namespace

void TListInjectHooksLote105A()
{
    RH_ScopedNamespaceName("TList_c");
    RH_ScopedCategory("Core");

    RH_ScopedGlobalInstall(ListAddItemExe, 0x4A8DF0);
    RH_ScopedGlobalInstall(ListRemoveAllExe, 0x4A8EB0);
    RH_ScopedGlobalInstall(ListGetNumItemsExe, 0x4A8EC0);
    // Lote 219A (sin validar)
    RH_ScopedGlobalInstall(ListConstructorExe, 0x4A8DD0);
    RH_ScopedGlobalInstall(ListDestructorExe, 0x4A8DE0);
    RH_ScopedGlobalInstall(ListRemoveItemExe, 0x4A8E30);
    RH_ScopedGlobalInstall(ListRemoveHeadExe, 0x4A8E70);
    RH_ScopedGlobalInstall(ListGetItemOffsetExe, 0x4A9010);
}
