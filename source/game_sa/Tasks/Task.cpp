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
 * Validación registrada: 4/4 entradas.
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
#ifdef FIX_BUGS // El original no comprueba nullptr
    if (!object) {
        return;
    }
#endif
    // Como el original: marca el slot libre y baja m_LastFreeSlot (comparación con signo), sin rellenar con 0xDD
    GetTaskPool()->Delete(static_cast<CTask*>(object));
}

// 0x421180
void CTask::StopTimer(const CEvent* event) {
    // NOP
}

// 0x61A360
bool CTask::IsGoToTask(CTask* task) {
    // Como el original: el tipo se pide una vez por comparación (dos llamadas virtuales)
    return task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT
        || task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE;
}

// 0x61A4B0
bool CTask::IsTaskPtr(CTask* task) {
    // Como el original: recorre los slots ocupados de mayor a menor y compara con la dirección exacta del slot
    // (un puntero dentro de un slot pero no a su inicio no cuenta)
    auto* const pool = GetTaskPool();
    for (auto i = (int32)pool->GetSize(); i-- > 0;) {
        if (!pool->IsFreeSlotAtIndex(i) && pool->GetAt(i) == task) {
            return true;
        }
    }
    return false;
}

void CTask::InjectHooks() {
    RH_ScopedCategory("Tasks");
    RH_ScopedVirtualClass(CTask, 0x86D48C, 7);

    RH_ScopedInstall(IsTaskPtr, 0x61A4B0);
    RH_ScopedInstall(operator new, 0x61A5A0);
    RH_ScopedInstall(operator delete, 0x61A5B0);
    RH_ScopedVMTInstall(StopTimer, 0x421180);
}
