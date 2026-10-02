/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTask
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Task.h"

// 0x61A5A0
void* CTask::operator new(size_t size) {
    return GetTaskPool()->New();
}

// 0x61A5B0
void CTask::operator delete(void* object) {
    GetTaskPool()->Delete(static_cast<CTask*>(object));
}

// 0x421180
void CTask::StopTimer(const CEvent* event) {
    // NOP
}

// 0x61A360
bool CTask::IsGoToTask(CTask* task) {
    switch (task->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT:
    case TASK_SIMPLE_GO_TO_POINT_FINE:
        return true;
    default:
        return false;
    }
}

// 0x61A4B0
bool CTask::IsTaskPtr(CTask* task) {
    return task && GetTaskPool()->IsObjectValid(task);
}

void CTask::InjectHooks() {
    RH_ScopedCategory("Tasks");
    RH_ScopedVirtualClass(CTask, 0x86D48C, 6);

    RH_ScopedInstall(IsTaskPtr, 0x61A4B0);
    RH_ScopedInstall(operator new, 0x61A5A0);
    RH_ScopedInstall(operator delete, 0x61A5B0);
    RH_ScopedVMTInstall(StopTimer, 0x421180);
}
