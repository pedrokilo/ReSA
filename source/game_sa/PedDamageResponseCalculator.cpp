/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedDamageResponseCalculator
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 4.
 * Validación registrada: 4/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   AccountForPedArmour | 0x004AD550 | S
 *   AccountForPedDamageStats | 0x004AD430 | S
 *   ComputeDamageResponse | 0x004B5AC0 | S
 *   ComputeWillForceDeath | 0x004AD610 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <reversiblebugfixes/Bugs.hpp>
#include "PedDamageResponseCalculator.h"
#include "PedStats.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"

void CPedDamageResponseCalculator::InjectHooks() {
    RH_ScopedClass(CPedDamageResponseCalculator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AD3F0);
    RH_ScopedInstall(AccountForPedDamageStats, 0x4AD430);
    RH_ScopedInstall(AccountForPedArmour, 0x4AD550);
    RH_ScopedInstall(ComputeWillForceDeath, 0x4AD610);
    RH_ScopedInstall(ComputeWillKillPed, 0x4B3210);
    // 0x4B5C2A es un bloque interno de ComputeDamageResponse, sin ABI de función.
    // IsBleedingWeapon es el auxiliar nativo de ese bloque; no instalar un hook aquí.
    RH_ScopedInstall(ComputeDamageResponse, 0x4B5AC0);
}

// 0x4AD3F0
CPedDamageResponseCalculator::CPedDamageResponseCalculator(const CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak) {
    m_pDamager      = entity;
    m_fDamageFactor = fDamage;
    m_bodyPart      = bodyPart;
    m_weaponType    = weaponType;
    m_bSpeak        = bSpeak;
}

CPedDamageResponseCalculator* CPedDamageResponseCalculator::Constructor(CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak) {
    this->CPedDamageResponseCalculator::CPedDamageResponseCalculator(entity, fDamage, weaponType, bodyPart, bSpeak);
    return this;
}

/*!
 *
 * @param ped
 * @param response
 * @addr 0x4AD430
 */
void CPedDamageResponseCalculator::AccountForPedDamageStats(CPed* ped, CPedDamageResponse& response) {
    const bool defaultDamage = m_fDamageFactor == ms_damageFactor;
    m_fDamageFactor *= ped->IsPlayer() ? std::bit_cast<float>(0x3EA8F5C3u) : ped->m_pStats->m_fDefendWeakness;
    if (m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED && static_cast<const CPed*>(m_pDamager)->IsPlayer()) {
        if (!CPedGroups::AreInSameGroup(ped, static_cast<const CPed*>(m_pDamager))
            || m_weaponType == WEAPON_EXPLOSION || m_weaponType == WEAPON_FLAMETHROWER || defaultDamage) {
            return;
        }
    } else {
        if (ped && ped->IsPlayer() && m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED
            && CPedIntelligence::AreFriends(*ped, *static_cast<const CPed*>(m_pDamager))) {
            const float scaled = m_fDamageFactor * 0.1f;
            m_fDamageFactor = scaled > 12.5f ? 12.5f : scaled;
            return;
        }
        if (!m_pDamager || m_pDamager->GetType() != ENTITY_TYPE_PED || ped->GetCreatedBy() != 2
            || !CPedGroups::AreInSameGroup(ped, static_cast<const CPed*>(m_pDamager))
            || m_weaponType == WEAPON_EXPLOSION || m_weaponType == WEAPON_FLAMETHROWER) {
            return;
        }
    }
    const float scaled = m_fDamageFactor * 0.1f;
            m_fDamageFactor = scaled > 12.5f ? 12.5f : scaled;
}

/*!
 *
 * @param ped
 * @param response
 * @addr 0x4AD550
 */
void CPedDamageResponseCalculator::AccountForPedArmour(CPed* ped, CPedDamageResponse& response) {
    if (ped->m_fArmour == 0.0f || m_weaponType == WEAPON_DROWNING || m_weaponType == WEAPON_FALL) {
        return;
    }
    if (FindPlayerPed() == ped) {
        CWorld::Players[CWorld::PlayerInFocus].m_nLastTimeArmourLost = CTimer::GetTimeInMS();
    }
    // Como el original: unordered consume toda la armadura.
    if (!(m_fDamageFactor <= ped->m_fArmour)) {
        m_fDamageFactor -= ped->m_fArmour;
        response.m_fDamageArmor = ped->m_fArmour;
        ped->m_fArmour = 0.0f;
    } else {
        response.m_fDamageArmor = m_fDamageFactor;
        ped->m_fArmour -= m_fDamageFactor;
        m_fDamageFactor = 0.0f;
    }
}

/*!
 *
 * @param ped
 * @addr Added in Android
 */
void CPedDamageResponseCalculator::AdjustPedDamage(CPed* ped) {
    // TODO: Reverted function from GTA SA 2.10 version ABI : arm64-v8a (x64)
    // offset: 0x444370

    if (!ped) {
        return;
    }

    const auto plyr = FindPlayerPed(-1);
    if (!plyr) {
        return;
    }

    const bool isUnarmedBrassOrParachute = m_weaponType == WEAPON_UNARMED
        || m_weaponType == WEAPON_BRASSKNUCKLE
        || m_weaponType == WEAPON_PARACHUTE;

    if (CCheat::m_aCheatsActive[CHEAT_COUNTRY_TRAFFIC] && m_weaponType <= eWeaponType::WEAPON_PARACHUTE && isUnarmedBrassOrParachute) {
        m_fDamageFactor = ped->m_fHealth;
    }

    if (plyr == ped && CTheScripts::pActiveScripts && !strcmp(CTheScripts::pActiveScripts->m_szName, "intro1")) {
        m_fDamageFactor = m_fDamageFactor * 0.77f;
    }
}

/*!
 *
 * @param ped
 * @param response
 * @return
 * @addr 0x4AD610
 */
bool CPedDamageResponseCalculator::ComputeWillForceDeath(CPed* ped, CPedDamageResponse& response) {
    const auto flag = [ped] { return (*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(ped) + 0x470) & 0x1000) != 0; };
    if (flag()) {
        return false;
    }
    switch (m_weaponType) {
    case WEAPON_KATANA: {
        if (!m_pDamager || m_pDamager->GetType() != ENTITY_TYPE_PED || !static_cast<const CPed*>(m_pDamager)->IsPlayer()) {
            return false;
        }
        auto* intelligence = static_cast<const CPed*>(m_pDamager)->GetIntelligence();
        if (!intelligence->GetTaskFighting() || intelligence->GetTaskFighting()->m_nComboSet != 11) {
            return false;
        }
        if (intelligence->GetTaskFighting()->m_nCurrentMove == 2 && (CGeneral::GetRandomNumber() & 1)) {
            return true;
        }
        return (CGeneral::GetRandomNumber() & 7) == 0;
    }
    case WEAPON_GRENADE: case WEAPON_RLAUNCHER: case WEAPON_RLAUNCHER_HS:
    case WEAPON_REMOTE_SATCHEL_CHARGE: case WEAPON_EXPLOSION:
        return CLocalisation::KillPeds() && !ped->IsPlayer() && !ped->bInVehicle
            && m_fDamageFactor + 1.0f > ped->m_fHealth;
    case WEAPON_PISTOL: case WEAPON_PISTOL_SILENCED: case WEAPON_DESERT_EAGLE:
    case WEAPON_SHOTGUN: case WEAPON_SAWNOFF_SHOTGUN: case WEAPON_SPAS12_SHOTGUN:
    case WEAPON_MICRO_UZI: case WEAPON_MP5: case WEAPON_AK47: case WEAPON_M4:
    case WEAPON_TEC9: case WEAPON_COUNTRYRIFLE: case WEAPON_SNIPERRIFLE:
    case WEAPON_MINIGUN: case WEAPON_UZI_DRIVEBY: {
        int random = 1;
        if (!ped->IsPlayer() && !flag()) {
            random = m_weaponType == WEAPON_COUNTRYRIFLE || m_weaponType == WEAPON_SNIPERRIFLE ? 0 : CGeneral::GetRandomNumber() & 7;
        }
        if (!(ped->bInVehicle && ped->m_pVehicle && !ped->IsPlayer() && !flag())) {
            bool special = false;
            if (!ped->IsPlayer() && m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED && static_cast<const CPed*>(m_pDamager)->IsPlayer()) {
                const auto* data = static_cast<const CPed*>(m_pDamager)->GetPlayerData();
                special = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(data) + 0x70) == 5
                    || (*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(data) + 0x34) & 8) != 0;
            }
            if (!special && random) {
                return false;
            }
        }
        return m_bodyPart == PED_PIECE_HEAD;
    }
    default:
        return false;
    }
}

/*!
 *
 * @param ped
 * @param response
 * @param bSpeak
 * @addr 0x4B3210
 */
void CPedDamageResponseCalculator::ComputeWillKillPed(CPed* ped, CPedDamageResponse& rsp, bool bSpeak) {
    if (ped->IsPlayer()) {
        if (CCheat::IsActive(CHEAT_INFINITE_HEALTH) && m_weaponType < WEAPON_LAST_WEAPON) {
            return;
        }
    }

    rsp.m_bForceDeath = ComputeWillForceDeath(ped, rsp);

    if (CCheat::IsActive(CHEAT_MEGA_PUNCH)) {
        if (notsa::contains({ WEAPON_UNARMED, WEAPON_BRASSKNUCKLE, WEAPON_PARACHUTE }, m_weaponType)) {
            m_fDamageFactor = ped->m_fHealth;
        }
    }

    if (!rsp.m_bForceDeath && m_weaponType == WEAPON_FALL && m_bSpeak) {
        const auto prevHP = ped->m_fHealth;
        const auto factor = m_fDamageFactor;
        const float minimumHP = 5.0f;
        float remainingHealth{};
        float damageHealth{};
        // 0x4B3290..0x4B32C4: retain the x87 remainder after storing health.
        // Damage uses that unrounded remainder, not the float health in memory.
        __asm {
            fld prevHP
            fsub factor
            fld minimumHP
            fcomp
            fnstsw ax
            test ah, 0x41
            jnz keepRemainingForKillPed
            fstp st(0)
            fld minimumHP
        keepRemainingForKillPed:
            fst remainingHealth
            fld prevHP
            fsub st(0), st(1)
            fstp damageHealth
            fstp st(0)
        }
        ped->m_fHealth      = remainingHealth;
        rsp.m_bHealthZero   = false;
        rsp.m_fDamageHealth = damageHealth;
        if (bSpeak) {
            ped->Say(CTX_GLOBAL_PAIN_LOW, 0, 1.0, 0, 0, 0);
        }
        return;
    }

    if (rsp.m_bForceDeath || ped->m_fHealth - m_fDamageFactor < 1.0f) {
        rsp.m_fDamageHealth = ped->m_fHealth;
        rsp.m_bHealthZero   = true;
        ped->m_fHealth      = 0.0f;
        return;
    }

    rsp.m_bHealthZero   = false;
    rsp.m_fDamageHealth = m_fDamageFactor;
    ped->m_fHealth      = ped->m_fHealth - m_fDamageFactor;

    if (bSpeak) {
        ped->Say(
            m_weaponType == WEAPON_DROWNING
                ? CTX_GLOBAL_PAIN_CJ_DROWNING
                : m_fDamageFactor < 5.0f && ped->m_fHealth > 10.0f
                    ? CTX_GLOBAL_PAIN_LOW
                    : CTX_GLOBAL_PAIN_HIGH
        );
    }
}

/*!
 *
 * @param ped
 * @return
 * @addr 0x4B5C2A inlined
 */
bool CPedDamageResponseCalculator::IsBleedingWeapon(CPed* ped) const {
    if (ped->IsPlayer())
        return false;

    if (m_weaponType == WEAPON_KNIFE || m_weaponType == WEAPON_KATANA || m_weaponType == WEAPON_CHAINSAW)
        return true;

    return false;
}

/*!
 *
 * @param ped
 * @param response
 * @param bSpeak
 * @addr 0x4B5AC0
 */
void CPedDamageResponseCalculator::ComputeDamageResponse(CPed* ped, CPedDamageResponse& response, bool bSpeak) {
    if (response.m_bDamageCalculated) {
        return;
    }
    response.m_fDamageHealth = 0.0f;
    response.m_fDamageArmor = 0.0f;
    response.m_bHealthZero = false;
    response.m_bForceDeath = false;
    response.m_bDamageCalculated = true;
    response.m_bCheckIfAffectsPed = true;
    AccountForPedDamageStats(ped, response);
    AccountForPedArmour(ped, response);
    ComputeWillKillPed(ped, response, bSpeak);
    if (m_pDamager && (m_pDamager == FindPlayerPed() || m_pDamager == FindPlayerVehicle())
        && m_pDamager != ped && response.m_fDamageHealth + response.m_fDamageArmor > 3.0f) {
        ++CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused;
    }
    if (ped == FindPlayerPed()) {
        FindPlayerPed()->AnnoyPlayerPed(false);
    }
    if (ped->IsPlayer() && m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED) {
        CEventDamage event{const_cast<CEntity*>(m_pDamager), 0, m_weaponType, m_bodyPart, 0, false, ped->bInVehicle};
        static_cast<CPlayerPed*>(ped)->MakeGroupRespondToPlayerTakingDamage(event);
    }
    if (!(response.m_fDamageHealth + response.m_fDamageArmor > 0.0f)) {
        return;
    }
    if (IsBleedingWeapon(ped)) {
        *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(ped) + 0x72F) = 200;
    }
    if (ped == FindPlayerPed()) {
        CWorld::Players[CWorld::PlayerInFocus].m_nLastTimeEnergyLost = CTimer::GetTimeInMS();
    }
    ped->m_nLastWeaponDamage = static_cast<char>(m_weaponType);
    if (m_pDamager) {
        ped->m_pLastEntityDamage = const_cast<CEntity*>(m_pDamager);
        const_cast<CEntity*>(m_pDamager)->RegisterReference(&ped->m_pLastEntityDamage);
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x768) = CTimer::GetTimeInMS();
        if (m_pDamager->GetType() == ENTITY_TYPE_PED && static_cast<const CPed*>(m_pDamager)->IsPlayer()) {
            FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(ped);
        }
    }
}

