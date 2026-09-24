/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDied
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleDied.h"

void CEventVehicleDied::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDied, 0x85B400, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B0D10);
    RH_ScopedVMTInstall(Clone, 0x4B76D0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0E00);
}

CEventVehicleDied::CEventVehicleDied (CVehicle* vehicle)
{
    m_vehicle = vehicle;
    m_IsOccupantLeavingCar = false;
    m_vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_vehicle)); // todo: possible nullptr
}

CEventVehicleDied::~CEventVehicleDied ()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B0D10
CEventVehicleDied* CEventVehicleDied::Constructor(CVehicle* vehicle)
{
    this->CEventVehicleDied::CEventVehicleDied(vehicle);
    return this;
}

// 0x4B0E00
bool CEventVehicleDied::AffectsPed(CPed* ped)
{
    if (ped->IsAlive() && !m_IsOccupantLeavingCar) {
        m_IsOccupantLeavingCar = ped->GetTaskManager().Has<TASK_COMPLEX_LEAVE_CAR>();
        if ((m_IsOccupantLeavingCar || ped->bInVehicle) && m_vehicle)
            return ped->m_pVehicle == m_vehicle;
    }
    return false;
}
