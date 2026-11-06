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
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleHitAndRun.h"

namespace {
// 0x4AEA50 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleHitAndRun que usan los destructores del EXE. Deja la vtable de CEventVehicleHitAndRun del EXE (0x85AFA8), libera las referencias de m_vehicle (+0x10) y despues de m_victim (+0xC) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleHitAndRunDestructorBody(CEventVehicleHitAndRun* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AFA8;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    CEntity::SafeCleanUpRef(self->m_victim);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventVehicleHitAndRun::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleHitAndRun, 0x85AFA8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(VehicleHitAndRunDestructorBody, "Destructor", 0x4AEA50);

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
