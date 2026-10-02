/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDamageWeapon
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleDamageWeapon.h"

void CEventVehicleDamageWeapon::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDamageWeapon, 0x86D640, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x61C2E0);
    RH_ScopedVMTInstall(CloneEditable, 0x61C330);
}

// 0x61C2E0
CEventVehicleDamageWeapon::CEventVehicleDamageWeapon(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType) :
    CEventVehicleDamage(vehicle, attacker, weaponType)
{
    // nothing here
}

CEventVehicleDamageWeapon* CEventVehicleDamageWeapon::Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    this->CEventVehicleDamageWeapon::CEventVehicleDamageWeapon(vehicle, attacker, weaponType);
    return this;
}

// 0x61C330
CEventEditableResponse* CEventVehicleDamageWeapon::CloneEditable() const noexcept
{
    return new CEventVehicleDamageWeapon(m_vehicle, m_attacker, m_weaponType);
}