/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventCarUpsideDown
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventCarUpsideDown.h"


void CEventCarUpsideDown::InjectHooks() {
    RH_ScopedVirtualClass(CEventCarUpsideDown, 0x85B660, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1CC0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1DB0);
}

// 0x4B1CC0
CEventCarUpsideDown* CEventCarUpsideDown::Constructor(CVehicle* vehicle) { this->CEventCarUpsideDown::CEventCarUpsideDown(vehicle); return this; }

// 0x4B1CC0
CEventCarUpsideDown::CEventCarUpsideDown(CVehicle* vehicle) : CEvent() {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventCarUpsideDown::~CEventCarUpsideDown() {
#if FIX_BUGS
    CEntity::SafeCleanUpRef(m_vehicle);
#else
    CEntity::SafeRegisterRef(m_vehicle);
#endif
}

// 0x4B1DB0
bool CEventCarUpsideDown::AffectsPed(CPed* ped) {
    if (ped->IsPlayer()) return false;
    if (ped->bGetOutUpsideDownCar == 0u) return false;
    if (ped->bInVehicle == 0u) return false;
    if (ped->IsCreatedByMission() && (!ped->m_pVehicle || ped->m_pVehicle->IsBoat())) return false;
    if (ped->m_pVehicle->IsBike() || ped->m_pVehicle->IsQuad()) return false;
    return ped->IsAlive();
}