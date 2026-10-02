/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderEnteredCarAsDriver
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
#include "EventLeaderEnteredCarAsDriver.h"


void CEventLeaderEnteredCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderEnteredCarAsDriver, 0x85A198, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x48E1C0);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B0EF0);
}

// 0x48E1C0
CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(CVehicle* vehicle)
{
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventLeaderEnteredCarAsDriver::~CEventLeaderEnteredCarAsDriver()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x48E1C0
CEventLeaderEnteredCarAsDriver* CEventLeaderEnteredCarAsDriver::Constructor(CVehicle* vehicle)
{
    this->CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(vehicle);
    return this;
}

// 0x4B0EF0
bool CEventLeaderEnteredCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup)
{
    if (m_vehicle && pedGroup->m_bMembersEnterLeadersVehicle) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (!member)
                continue;

            if (!member->bInVehicle
                || member->m_pVehicle != m_vehicle
                || member->GetTaskManager().Find<TASK_COMPLEX_LEAVE_CAR>())
            {
                return true;
            }
        }
    }
    return false;
}