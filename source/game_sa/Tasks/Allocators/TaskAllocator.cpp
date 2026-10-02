/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocator
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskAllocator.h"

// 0x69D980
void* CTaskAllocator::operator new(size_t size) {
    return GetTaskAllocatorPool()->New();
}

// 0x69D990
void CTaskAllocator::operator delete(void* obj) {
    GetTaskAllocatorPool()->Delete(static_cast<CTaskAllocator*>(obj));
}

bool CTaskAllocator::IsFinished(CPedGroupIntelligence* intel) {
    return rng::all_of(
        intel->GetPedTaskPairs(),
        [](const CPedTaskPair& tp) { return !tp.Task; }
    );
}

void CTaskAllocator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocator, 0x870E30, 6);
    RH_ScopedCategory("Tasks/Allocators/TaskAllocator");

    RH_ScopedVMTInstall(ProcessGroup, 0x69BB50);
    RH_ScopedVMTInstall(IsFinished, 0x69C3C0);
    RH_ScopedVMTInstall(Abort, 0x5F68E0);
}
