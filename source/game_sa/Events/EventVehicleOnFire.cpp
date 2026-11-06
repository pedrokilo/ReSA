/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleOnFire
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleOnFire.h"

namespace {
// 0x4B1170 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleOnFire que usan los destructores del EXE. Deja la vtable de CEventVehicleOnFire del EXE (0x85B440), libera la referencia de m_vehicle (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleOnFireDestructorBody(CEventVehicleOnFire* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B440;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventVehicleOnFire::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleOnFire, 0x85B440, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(VehicleOnFireDestructorBody, "Destructor", 0x4B1170);

    RH_ScopedInstall(Constructor, 0x4B10C0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B4FD0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7740);
}

// 0x4B10C0
CEventVehicleOnFire::CEventVehicleOnFire(CVehicle* vehicle)
{
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventVehicleOnFire::~CEventVehicleOnFire()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

CEventVehicleOnFire* CEventVehicleOnFire::Constructor(CVehicle* vehicle)
{
    this->CEventVehicleOnFire::CEventVehicleOnFire(vehicle);
    return this;
}

// 0x4B4FD0
bool CEventVehicleOnFire::AffectsPed(CPed* ped)
{
    if (m_vehicle) {
        if (!ped->m_pVehicle || !ped->bInVehicle || ped->m_pVehicle == m_vehicle) {
            if (!ped->IsPlayer() && ped->IsAlive()) {
                CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);
                if (pedGroup && !pedGroup->GetMembership().IsLeader(ped)) {
                    if (ped->bInVehicle)
                        return false;
                }
                if (ped->bInVehicle)
                    return true;
                // Como el original: suma (x² + z²) + y² y un NaN cuenta como "cerca".
                CVector distance = ped->GetPosition() - m_vehicle->GetPosition();
                const float distanceSq = distance.x * distance.x + distance.z * distance.z + distance.y * distance.y;
                return !(distanceSq > 100.0f);
            }
        }
    }
    return false;
}

// 0x4B7740
CEventEditableResponse* CEventVehicleOnFire::CloneEditable() const noexcept
{
    return new CEventVehicleOnFire(m_vehicle);
}