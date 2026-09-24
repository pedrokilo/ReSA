/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleHitAndRun
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

#include "EventVehicleHitAndRun.h"

void CEventVehicleHitAndRun::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleHitAndRun, 0x85AFA8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AE990);
    RH_ScopedVMTInstall(Clone, 0x4B7100);
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B27D0);
}

CEventVehicleHitAndRun::CEventVehicleHitAndRun(CPed* victim, CVehicle* vehicle)
{
    m_victim = victim;
    m_vehicle = vehicle;
    m_vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_vehicle)); // *
    m_victim->RegisterReference(reinterpret_cast<CEntity**>(&m_victim));   // ** todo: possible nullptr
}

CEventVehicleHitAndRun::~CEventVehicleHitAndRun()
{
    CEntity::SafeCleanUpRef(m_vehicle);
    CEntity::SafeCleanUpRef(m_victim);
}

// 0x4AE990
CEventVehicleHitAndRun* CEventVehicleHitAndRun::Constructor(CPed* victim, CVehicle* vehicle)
{
    this->CEventVehicleHitAndRun::CEventVehicleHitAndRun(victim, vehicle);
    return this;
}

// 0x4B27D0
void CEventVehicleHitAndRun::ReportCriminalEvent(CPed* ped)
{
    if (IsCriminalEvent()) {
        if (m_victim->m_nPedType == PED_TYPE_COP)
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_KILL_COP_PED_WITH_CAR, m_vehicle->GetPosition(), (uint32)m_vehicle->m_pDriver, false);
        else
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_KILL_PED_WITH_CAR, m_vehicle->GetPosition(), (uint32)m_vehicle->m_pDriver, false);
    }
}
