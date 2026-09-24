/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGetUpAndStandStill
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGetUpAndStandStill.h"

#include "TaskSimpleStandStill.h"
#include "TaskSimpleGetUp.h"

void CTaskComplexGetUpAndStandStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGetUpAndStandStill, 0x870404, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x678130);
    RH_ScopedInstall(CreateSubTask, 0x678170);
    // VTABLE
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6782A0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x678240);
    RH_ScopedVMTInstall(ControlSubTask, 0x6782B0);
}

// 0x678130
CTaskComplexGetUpAndStandStill* CTaskComplexGetUpAndStandStill::Constructor() {
    this->CTaskComplexGetUpAndStandStill::CTaskComplexGetUpAndStandStill();
    return this;
}

// 0x678170
CTask* CTaskComplexGetUpAndStandStill::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_GET_UP:
        return new CTaskSimpleGetUp();
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(0, false, false, 8.0F);
    case TASK_FINISHED:
    default:
        return nullptr;
    }
}

// 0x6782A0
CTask* CTaskComplexGetUpAndStandStill::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_GET_UP);
}

// 0x678240
CTask* CTaskComplexGetUpAndStandStill::CreateNextSubTask(CPed* ped) {
    auto subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_SIMPLE_STAND_STILL)
        return CreateSubTask(TASK_FINISHED);

    if (subTaskType == TASK_SIMPLE_GET_UP) {
        auto pSubTask = reinterpret_cast<CTaskSimpleGetUp*>(m_pSubTask);

        if (pSubTask->m_bHasPedGotUp)
            return CreateSubTask(TASK_SIMPLE_STAND_STILL);
        else
            return CreateSubTask(TASK_FINISHED);
    }

    return nullptr;
}

// 0x6782B0
CTask* CTaskComplexGetUpAndStandStill::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}