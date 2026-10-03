/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexBeStill
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexBeStill.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexBeStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeStill, 0x86C7BC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Destructor, 0x5F6700);

    RH_ScopedVMTInstall(Clone, 0x5F6680);
    RH_ScopedVMTInstall(GetTaskType, 0x5F66D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62F8D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62F910);
    RH_ScopedVMTInstall(ControlSubTask, 0x62F9F0);
}

// 0x62F8D0
CTask* CTaskComplexBeStill::CreateNextSubTask(CPed* ped) {
    auto taskType = m_pSubTask->GetTaskType();
    if (taskType == TASK_SIMPLE_STAND_STILL || taskType == TASK_SIMPLE_CAR_DRIVE)
        return CreateFirstSubTask(ped);
    else
        return nullptr;
}

// 0x62F910
CTask* CTaskComplexBeStill::CreateFirstSubTask(CPed* ped) {
    if (ped->bInVehicle) {
        return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
    }
    return new CTaskSimpleStandStill(0, true, false, 8.0f);
}

// 0x62F9F0
CTask* CTaskComplexBeStill::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
