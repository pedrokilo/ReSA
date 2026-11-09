/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDamage
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleDamage.h"

#include "PedType.h"

namespace {
// 0x4B1990 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleDamage que usan los destructores del EXE. Deja la vtable de CEventVehicleDamage del EXE (0x85B588), libera las referencias de m_vehicle (+0x14) y despues de m_attacker (+0x18) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleDamageDestructorBody(CEventVehicleDamage* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B588;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    CEntity::SafeCleanUpRef(self->m_attacker);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventVehicleDamage::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDamage, 0x85B588, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(VehicleDamageDestructorBody, "Destructor", 0x4B1990);

    RH_ScopedInstall(Constructor, 0x4B18D0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1A00);
    RH_ScopedVMTInstall(IsCriminalEvent, 0x4B1A90);
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B50B0);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4B1A70);
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1960);          // lote 139A (banco Fn139A): 0
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4B1970);   // lote 139A: 55.0f en ST0 (`fld dword [0x85ADBC]`)
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1980);     // lote 139A: 49
}

// 0x4B18D0
CEventVehicleDamage::CEventVehicleDamage(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    m_attacker = attacker;
    m_vehicle = vehicle;
    m_weaponType = weaponType;
    CEntity::SafeRegisterRef(m_vehicle);
    CEntity::SafeRegisterRef(m_attacker);
}

CEventVehicleDamage::~CEventVehicleDamage()
{
    CEntity::SafeCleanUpRef(m_vehicle);
    CEntity::SafeCleanUpRef(m_attacker);
}

CEventVehicleDamage* CEventVehicleDamage::Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    this->CEventVehicleDamage::CEventVehicleDamage(vehicle, attacker, weaponType);
    return this;
}

// 0x4B1A00
bool CEventVehicleDamage::AffectsPed(CPed* ped)
{
    if (ped->IsAlive() && m_attacker) {
        if (!ped->IsInVehicleThatHasADriver())
            return m_vehicle && ped->m_pVehicle == m_vehicle;
    }
    return false;
}

// 0x4B1A90
bool CEventVehicleDamage::IsCriminalEvent()
{
    if (m_attacker) {
        switch (m_attacker->GetType()) {
        case ENTITY_TYPE_PED:
            return m_attacker->AsPed()->IsPlayer();
        case ENTITY_TYPE_VEHICLE:
            return m_attacker->AsVehicle()->m_pDriver == FindPlayerPed();
        }
    }
    return false;
}

// 0x4B50B0
void CEventVehicleDamage::ReportCriminalEvent(CPed* ped)
{
    if (IsCriminalEvent() && m_attacker) {
        bool bPoliceDontReallyCare = CPedType::PoliceDontCareAboutCrimesAgainstPedType(ped->m_nPedType);
        if (ped->m_nPedType == PED_TYPE_COP) 
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_VEHICLE_DAMAGE, m_attacker->GetPosition(), (uint32)ped, bPoliceDontReallyCare);
    }
}

// 0x4B1A70
CEntity* CEventVehicleDamage::GetSourceEntity() const
{
    if (m_attacker && m_attacker->GetIsTypeVehicle()) {
        CVehicle* vehicle = m_attacker->AsVehicle();
        if (vehicle->m_pDriver)
            return vehicle->m_pDriver;
    }
    return m_attacker;
}
