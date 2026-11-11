/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexOpenDriverDoor
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexOpenDriverDoor.h"

// 0x643CA0 (lote 165A, banco Fn165A): destructor con borrado de CTaskComplexOpenDriverDoor del EXE (ranura 0 de su vtable 0x86EB0C; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6403D0; test byte
// [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el destructor sin borrado de la clase (0x6403D0, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo
// si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CTaskComplexOpenDriverDoor* CTaskComplexOpenDriverDoor::ScalarDeletingDestructorExe(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

void CTaskComplexOpenDriverDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenDriverDoor, 0x86EB0C, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x640390);
    RH_ScopedInstall(Destructor, 0x6403D0);
    RH_ScopedInstall(ScalarDeletingDestructorExe, 0x643CA0); // lote 165A (banco Fn165A)

    RH_ScopedVMTInstall(Clone, 0x643870);
    RH_ScopedVMTInstall(GetTaskType, 0x6403C0);
}
