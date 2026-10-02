/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleToSteal
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventVehicleToSteal.h"

void CEventVehicleToSteal::InjectHooks() {
    RH_ScopedVirtualClass(CEventVehicleToSteal, 0x85B210, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AF670);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF760);
}

// 0x4AF670
CEventVehicleToSteal::CEventVehicleToSteal(CVehicle* vehicle) {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventVehicleToSteal::~CEventVehicleToSteal() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4AF670
CEventVehicleToSteal* CEventVehicleToSteal::Constructor(CVehicle* vehicle) {
    this->CEventVehicleToSteal::CEventVehicleToSteal(vehicle);
    return this;
}

// 0x4AF760
bool CEventVehicleToSteal::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && m_vehicle) {
        auto tEnterCarAsDriver = notsa::cast_if_present<CTaskComplexEnterCarAsDriver>(FindPlayerPed()->GetTaskManager().FindTaskByType(TASK_PRIMARY_PRIMARY, TASK_COMPLEX_ENTER_CAR_AS_DRIVER));
        if (!tEnterCarAsDriver || !tEnterCarAsDriver->GetTargetCar()) {
            if (m_vehicle == FindPlayerPed()->m_pVehicle && (CTheScripts::IsPlayerOnAMission() || CPad::GetPad(0)->bPlayerSafe)) {
                return false;
            }
            return ped->m_nPedType == PED_TYPE_CRIMINAL && ped->m_pVehicle != m_vehicle
                || ped->m_nPedType == PED_TYPE_COP && m_vehicle == ped->m_pVehicle;
        }
    }
    return false;
}
