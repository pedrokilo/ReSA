/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeanOnVehicle
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
#include "EventLeanOnVehicle.h"


void CEventLeanOnVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventLeanOnVehicle, 0x86F878, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x65DAF0);
    RH_ScopedVMTInstall(IsValid, 0x4B16C0);
}

// 0x65DAF0
CEventLeanOnVehicle* CEventLeanOnVehicle::Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) { this->CEventLeanOnVehicle::CEventLeanOnVehicle(vehicle, leanAnimDurationInMs); return this; }

// 0x65DAF0
CEventLeanOnVehicle::CEventLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs) : CEvent() {
    m_vehicle = vehicle;
    m_leanAnimDurationInMs = leanAnimDurationInMs;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventLeanOnVehicle::~CEventLeanOnVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B16C0
bool CEventLeanOnVehicle::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();

    // Como el original: CEvent::IsValid en linea (la llamada virtual sin calificar se llamaba a si misma sin fin)
    if (CEvent::IsValid(ped))
        return true;

    return false;
}