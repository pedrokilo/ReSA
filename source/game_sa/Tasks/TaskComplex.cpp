/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplex
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplex.h"
#include "Pools/PoolRawDelete.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x61A620 (lote 169A, banco Fn169A): destructor con borrado de CTaskComplex del EXE (ranura 0 de su vtable 0x86D4CC; thiscall, un argumento de pila, `ret 4`, 59 bytes:
// `push esi; mov esi,ecx; call 0x61A3D0; test byte [esp+8],1; mov eax,esi; je ..;` y, con el bit 0 del BYTE de banderas, la liberacion de la ranura del pool de tareas EN LINEA). Como el original: destruye con el
// cuerpo de CTaskComplex (0x61A3D0, con hook), libera la ranura de `this` en el pool de tareas (0xB744A8; indice = desplazamiento aritmetico de 7 bits, estados[indice] |= 0x80, cursor = min(cursor, indice) con
// signo) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTask* __fastcall CTaskComplexScalarDeletingDestructor(CTask* self, void* /*edx*/, uint8 flags) {
    CTaskComplexBody::Destructor(self, nullptr);
    if (flags & 1) {
        PoolFreeSlotRawShift(GetTaskPool(), self, 7);
    }
    return self;
}
} // namespace

// Funciones de CTaskComplex del EXE (vtable 0x86D4CC, 9 ranuras: [0] 0x61A620, [2] 0x421190, [3] 0x4211A0, [6] 0x4211B0, [7] 0x61A430). Las comparten, por la misma direccion, las vtables de las tareas complejas
// que no las redefinen (p. ej. CTaskComplexWaitForBus 0x86E18C, CTaskComplexGoPickUpEntity).
void CTaskComplex::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplex, 0x86D4CC, 9);
    RH_ScopedCategory("Tasks");

    RH_ScopedNamedGlobalInstall(CTaskComplexScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61A620); // lote 169A (banco Fn169A)
    RH_ScopedVMTInstall(GetSubTask, 0x421190);                                                               // lote 169A: ranura 2
    RH_ScopedVMTInstall(IsSimple, 0x4211A0);                                                                 // lote 169A: ranura 3
    RH_ScopedVMTInstall(MakeAbortable, 0x4211B0);                                                            // lote 169A: ranura 6
}

// 0x421190 (lote 169A, banco Fn169A; thiscall, `ret`, 4 bytes: `mov eax,[ecx+8]`): la subtarea guardada (+8); no hace nada mas.
CTask* CTaskComplex::GetSubTask() const {
    return m_pSubTask;
}

// 0x4211A0 (lote 169A, banco Fn169A; thiscall, `ret`, 3 bytes: `xor al,al`): una tarea compleja nunca es simple; no lee el objeto.
bool CTaskComplex::IsSimple() const {
    return false;
}

// 0x4211B0 (lote 169A, banco Fn169A; thiscall, tres argumentos de pila, 8 bytes: `mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]`): salto a la ranura 6 (MakeAbortable) de la vtable de la subtarea guardada (+8)
// con los mismos argumentos; el EXE no comprueba que la subtarea exista (con subtarea nula se cae). Aqui es una llamada virtual normal con el resultado devuelto.
bool CTaskComplex::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}
