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
} // namespace

void TListInjectHooksLote105A()
{
    RH_ScopedNamespaceName("TList_c");
    RH_ScopedCategory("Core");

    RH_ScopedGlobalInstall(ListAddItemExe, 0x4A8DF0);
    RH_ScopedGlobalInstall(ListRemoveAllExe, 0x4A8EB0);
    RH_ScopedGlobalInstall(ListGetNumItemsExe, 0x4A8EC0);
}
