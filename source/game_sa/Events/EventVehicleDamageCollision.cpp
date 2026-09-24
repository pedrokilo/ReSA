/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDamageCollision
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

#include "EventVehicleDamageCollision.h"

void CEventVehicleDamageCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDamageCollision, 0x8710D0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x6A0620);
    RH_ScopedVMTInstall(CloneEditable, 0x6A0670);
}

// 0x6A0620
CEventVehicleDamageCollision::CEventVehicleDamageCollision(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType) :
    CEventVehicleDamage(vehicle, attacker, weaponType)
{
    // empty
}

CEventVehicleDamageCollision* CEventVehicleDamageCollision::Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    this->CEventVehicleDamageCollision::CEventVehicleDamageCollision(vehicle, attacker, weaponType);
    return this;
}

// 0x6A0670
CEventEditableResponse* CEventVehicleDamageCollision::CloneEditable() const noexcept
{
    return new CEventVehicleDamageCollision(m_vehicle, m_attacker, m_weaponType);
}