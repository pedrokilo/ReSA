/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCarSlowBeDraggedOutAndStandUp
 * Reconstrucción: INCOMPLETA — 3/9 funciones reconstruidas.
 * Pendientes de reconstruir: 6.
 * Funciones bloqueadas: 6.
 * Evidencia aprobada vinculada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x0064A190 | S
 *   ControlSubTask | 0x00648B80 | S
 *   CreateFirstSubTask | 0x00648A10 | S
 *   CreateNextSubTask | 0x006488F0 | S
 *   CreateSubTask | 0x00648710 | S
 *   MakeAbortable | 0x006486F0 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x0064A190 | S
 *   ControlSubTask | 0x00648B80 | S
 *   CreateFirstSubTask | 0x00648A10 | S
 *   CreateNextSubTask | 0x006488F0 | S
 *   CreateSubTask | 0x00648710 | S
 *   MakeAbortable | 0x006486F0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"

void CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOutAndStandUp, 0x86EF80, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648620);
    RH_ScopedInstall(Destructor, 0x648690);
    RH_ScopedInstall(CreateSubTask, 0x648710, { .reversed = false });
    RH_ScopedInstall(Clone, 0x64A190, { .reversed = false });
    RH_ScopedInstall(GetTaskType, 0x648680);
    RH_ScopedInstall(MakeAbortable, 0x6486F0, { .reversed = false });
    RH_ScopedInstall(CreateNextSubTask, 0x6488F0, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x648A10, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x648B80, { .reversed = false });
}

// 0x648620
CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 a3) : CTaskComplex() {
    m_Vehicle = vehicle;
    dword10 = a3;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648690
CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x648710
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648710, CTaskComplexCarSlowBeDraggedOutAndStandUp*, eTaskType, CPed*>(this, taskType, ped);
}

// 0x6486F0
bool CTaskComplexCarSlowBeDraggedOutAndStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x6486F0, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*, int32, CEvent const*>(this, ped, priority, event);
}

// 0x6488F0
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6488F0, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*>(this, ped);
}

// 0x648A10
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648A10, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*>(this, ped);
}

// 0x648B80
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648B80, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*>(this, ped);
}
