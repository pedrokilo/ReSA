/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskGoToVehicleAndLean
 * Reconstrucción: INCOMPLETA — 2/7 funciones reconstruidas.
 * Pendientes de reconstruir: 5.
 * Funciones bloqueadas: 5.
 * Evidencia aprobada vinculada: 0/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x006621B0 | S
 *   ControlSubTask | 0x00664E60 | S
 *   CreateFirstSubTask | 0x00664D40 | S
 *   CreateNextSubTask | 0x00664590 | S
 *   MakeAbortable | 0x00664500 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x006621B0 | S
 *   ControlSubTask | 0x00664E60 | S
 *   CreateFirstSubTask | 0x00664D40 | S
 *   CreateNextSubTask | 0x00664590 | S
 *   MakeAbortable | 0x00664500 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskGoToVehicleAndLean.h"

void CTaskGoToVehicleAndLean::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGoToVehicleAndLean, 0x86FAC8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660E60);
    RH_ScopedInstall(Destructor, 0x660EE0);
    RH_ScopedInstall(Clone, 0x6621B0, { .reversed = false });
    RH_ScopedInstall(MakeAbortable, 0x664500, { .reversed = false });
    RH_ScopedInstall(CreateNextSubTask, 0x664590, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x664D40, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x664E60, { .reversed = false });
}

// 0x660E60
CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs) : CTaskComplex() {
    m_Vehicle = vehicle;
    m_LeanAnimDurationInMs = leanAnimDurationInMs;
    m_LeanOnVehicle = false;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x660EE0
CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean() {
    if (m_LeanOnVehicle) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x664500
bool CTaskGoToVehicleAndLean::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x664500, CTaskGoToVehicleAndLean*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

// 0x664590
CTask* CTaskGoToVehicleAndLean::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x664590, CTaskGoToVehicleAndLean*, CPed*>(this, ped);
}

// 0x664D40
CTask* CTaskGoToVehicleAndLean::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x664D40, CTaskGoToVehicleAndLean*, CPed*>(this, ped);
}

// 0x664E60
CTask* CTaskGoToVehicleAndLean::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x664E60, CTaskGoToVehicleAndLean*, CPed*>(this, ped);
}

// 0x660F60
void CTaskGoToVehicleAndLean::DoTidyUp(CPed* ped) {
    if (m_Vehicle)
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;

    if (m_pSubTask)
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
}
