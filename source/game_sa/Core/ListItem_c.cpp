/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ListItem_c
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ListItem_c.h"

namespace {
// 0x4A8DB0 (lote 218A, sin validar; el listado decia FxInfoManager_c): constructor de la base ListItem_c<T> del EXE, codigo compartido por todas las instancias de la plantilla (thiscall, sin argumentos de pila, `ret`, 16 bytes):
// `mov eax,ecx; mov dword ptr [eax],0; mov dword ptr [eax+4],0; ret`. Solo deja a cero los enlaces anterior (+0) y siguiente (+4), en ese orden, y devuelve `this`; no toca nada mas.
struct ListItemLinksExe {
    void* prev;
    void* next;
};

// 0x4A8DB0
__declspec(noinline) ListItemLinksExe* __fastcall ListItemConstructorExe(ListItemLinksExe* self, void* /*edx*/) {
    volatile ListItemLinksExe* const links = self;
    links->prev = nullptr;
    links->next = nullptr;
    return self;
}

// 0x4A8DC0 (lote 219A, sin validar; el listado decia FxInfoManager_c; thiscall, `ret`, 1 byte): destructor de la base ListItem_c<T> del EXE (0x49EA70 y otros son `jmp 0x4A8DC0`). No hace nada: no toca los enlaces.
void __fastcall ListItemDestructorExe(ListItemLinksExe* /*self*/, void* /*edx*/) {
}
} // namespace

void ListItemInjectHooksLote218A() {
    RH_ScopedNamespaceName("ListItem_c");
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(ListItemConstructorExe, "Constructor", 0x4A8DB0);
    RH_ScopedNamedGlobalInstall(ListItemDestructorExe, "Destructor", 0x4A8DC0); // lote 219A (sin validar)
}
