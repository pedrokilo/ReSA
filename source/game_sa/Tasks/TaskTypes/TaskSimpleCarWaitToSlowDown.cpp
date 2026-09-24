/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarWaitToSlowDown
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleCarWaitToSlowDown.h"

void CTaskSimpleCarWaitToSlowDown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarWaitToSlowDown, 0x86ed98, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646990);
    RH_ScopedInstall(Destructor, 0x646A00);
    RH_ScopedVMTInstall(Clone, 0x649CB0);
    RH_ScopedVMTInstall(GetTaskType, 0x6469F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x646A60);
    RH_ScopedVMTInstall(ProcessPed, 0x646AD0);
    // crash RH_ScopedVMTInstall(SetPedPosition, 0x646AB0);
}

// 0x646990
CTaskSimpleCarWaitToSlowDown::CTaskSimpleCarWaitToSlowDown(CVehicle* vehicle, SlowDownType type) :
    CTaskSimple(),
    m_TargetVehicle{ vehicle },
    m_SlowType{ type }
{
    CEntity::SafeRegisterRef(m_TargetVehicle);
}

// 0x646A00
CTaskSimpleCarWaitToSlowDown::~CTaskSimpleCarWaitToSlowDown() {
    CEntity::SafeCleanUpRef(m_TargetVehicle);
}

// 0x646A60
bool CTaskSimpleCarWaitToSlowDown::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
        return true;
    case ABORT_PRIORITY_URGENT: {
        if (event && event->GetEventType() == EVENT_DAMAGE) {
            const auto dmg = static_cast<const CEventDamage*>(event);
            if (dmg->m_damageResponse.m_bHealthZero && dmg->m_bAddToEventGroup) {
                return true;
            }
        }
        break;
    }
    }
    m_SlowType = SlowDownType::DONT_WAIT;
    return false;
}

// 0x646AD0
bool CTaskSimpleCarWaitToSlowDown::ProcessPed(CPed* ped) {
    if (!m_TargetVehicle || m_SlowType == SlowDownType::DONT_WAIT) {
        return true;
    }

    if (const auto driver = m_TargetVehicle->m_pDriver) {
        if (!driver->IsPlayer() && ped == driver) {
            // Start stopping the car completely
            auto& ap = m_TargetVehicle->m_autoPilot;
            ap.SetCruiseSpeed(0);
            ap.SetCarMission(MISSION_NONE);
        }
    }

    switch (m_SlowType) {
    case SlowDownType::SLOW_ENOUGH_TO_STEP:
        return m_TargetVehicle->CanPedStepOutCar(false);
    case SlowDownType::SLOW_ENOUGH_TO_STEP_OR_JUMP:
        return m_TargetVehicle->CanPedStepOutCar(false) || m_TargetVehicle->CanPedJumpOutCar(ped);
    default:
        return false;
    }
    return false;
}

// 0x646AB0
bool CTaskSimpleCarWaitToSlowDown::SetPedPosition(CPed* ped) {
    if (m_TargetVehicle) {
        ped->SetPedPositionInCar();
    }
    return true;
}
