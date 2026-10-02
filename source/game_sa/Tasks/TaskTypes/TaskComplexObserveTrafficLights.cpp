/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexObserveTrafficLights
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexObserveTrafficLights.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"
#include "TrafficLights.h"

void CTaskComplexObserveTrafficLights::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexObserveTrafficLights, 0x86DF98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631790);
    RH_ScopedInstall(Destructor, 0x6317C0);

    RH_ScopedVMTInstall(Clone, 0x636410);
    RH_ScopedVMTInstall(GetTaskType, 0x6317B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6317D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6317E0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6318D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6318E0);
}

// 0x6318E0
CTask* CTaskComplexObserveTrafficLights::ControlSubTask(CPed* ped) {
    if (CTrafficLights::LightForPeds() || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
        return m_pSubTask;
    } else {
        return nullptr;
    }
}

// 0x6317E0
CTask* CTaskComplexObserveTrafficLights::CreateNextSubTask(CPed* ped) {
    if (!m_pSubTask || m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0f, false);
    } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_ANIM) {
        return new CTaskSimpleStandStill(CGeneral::GetRandomNumberInRange(3000, 5000));
    }
    return nullptr;
}
