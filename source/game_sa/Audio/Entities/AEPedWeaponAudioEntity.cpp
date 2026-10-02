/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEPedWeaponAudioEntity
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

#include "AEPedWeaponAudioEntity.h"

void CAEPedWeaponAudioEntity::InjectHooks() {
    // No redefine ningún virtual: su vtable del EXE (0x86C2AC, escrita por el
    // constructor 0x5DE990) solo hereda UpdateParameters de CAEWeaponAudioEntity
    // (0x504B70), que ya se instala desde CAEWeaponAudioEntity::InjectHooks.
    RH_ScopedClass(CAEPedWeaponAudioEntity);
    RH_ScopedCategory("Audio/Entities");

    // El constructor 0x5DE990 no se instala aquí: ya está registrado (bloqueado y
    // deshabilitado) en CAEWeaponAudioEntity::InjectHooks; duplicarlo chocaría.
    RH_ScopedInstall(Initialise, 0x4E6AA0);
    RH_ScopedInstall(Terminate, 0x4E6AC0);
    RH_ScopedInstall(AddAudioEvent, 0x4E69F0);
    RH_ScopedInstall(Service, 0x4E6AE0);
}

// 0x5DE990
CAEPedWeaponAudioEntity::CAEPedWeaponAudioEntity() : CAEWeaponAudioEntity() {
    m_Ped = nullptr;
    m_bInitialised = false;
}

// 0x4E6AA0
void CAEPedWeaponAudioEntity::Initialise(CPed* ped) {
    m_Ped = ped;
    CAEWeaponAudioEntity::Initialise();
    m_bInitialised = true;
}

// 0x4E6AC0
void CAEPedWeaponAudioEntity::Terminate() {
    m_bInitialised = false;
    m_Ped = nullptr;
    CAEWeaponAudioEntity::Terminate();
}

// 0x4E69F0
void CAEPedWeaponAudioEntity::AddAudioEvent(eAudioEvents event) {
    if (!m_bInitialised || !m_Ped)
        return;

    // Como el original: no descarta WEAPON_UNARMED; solo comprueba el puntero
    // al arma activa (siempre válido) y despacha por el evento.
    const auto& weapon = m_Ped->GetActiveWeapon();

    switch (event) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_MINIGUN_AMMO:
    case AE_WEAPON_FIRE_MINIGUN_NO_AMMO:
        WeaponFire(weapon.m_Type, m_Ped, event);
        break;
    case AE_WEAPON_RELOAD_A:
    case AE_WEAPON_RELOAD_B:
        WeaponReload(weapon.m_Type, m_Ped, event);
        break;
    case AE_WEAPON_CHAINSAW_IDLE:
    case AE_WEAPON_CHAINSAW_ACTIVE:
    case AE_WEAPON_CHAINSAW_CUTTING:
        ReportChainsawEvent(m_Ped, event);
        break;
    case AE_WEAPON_STEALTH_KILL:
        ReportStealthKill(weapon.m_Type, m_Ped, event);
        break;
    default:
        return;
    }
}

// 0x4E6AE0
void CAEPedWeaponAudioEntity::Service() {
    if (  !m_Ped
        || m_Ped->bInVehicle // Como el original: bit 8 de 0x46C
        || m_Ped->GetIntelligence()->GetTaskSwim()
        || m_Ped->GetActiveWeapon().m_Type != WEAPON_FLAMETHROWER
    ) {
        StopFlameThrowerIdleGasLoop();
    } else {
        PlayFlameThrowerIdleGasLoop(m_Ped);
    }
}
