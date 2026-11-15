#include "StdInc.h"

#include "ListItem_c.h"

namespace {
// 0x4A8DB0 (lote 218A, sin validar; el listado decia FxInfoManager_c): constructor de la base ListItem_c<T> del EXE, codigo compartido por todas las instancias de la plantilla (thiscall, sin argumentos de pila, `ret`, 16 bytes):
// `mov eax,ecx; mov dword ptr [eax],0; mov dword ptr [eax+4],0; ret`. Solo deja a cero los enlaces anterior (+0) y siguiente (+4), en ese orden, y devuelve `this`; no toca nada mas.
struct ListItemLinksExe {
    void* prev;
    void* next;
};

__declspec(noinline) ListItemLinksExe* __fastcall ListItemConstructorExe(ListItemLinksExe* self, void* /*edx*/) {
    volatile ListItemLinksExe* const links = self;
    links->prev = nullptr;
    links->next = nullptr;
    return self;
}
} // namespace

void ListItemInjectHooksLote218A() {
    RH_ScopedNamespaceName("ListItem_c");
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(ListItemConstructorExe, "Constructor", 0x4A8DB0);
}
