/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDead
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleDead.h"
#include "AnimBlendAssociation.h"
#include "AnimManager.h"
#include "AccidentManager.h"
#include "CarEnterExit.h"
#include "Localisation.h"
#include "Shadows.h"
#include "WaterLevel.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "Events/EventDeadPed.h"
#include "Events/EventGroup.h"

void CTaskSimpleDead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDead, 0x86DEA4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x630590);
    RH_ScopedInstall(Destructor, 0x6305F0);
    RH_ScopedVMTInstall(Clone, 0x636100);
    RH_ScopedVMTInstall(GetTaskType, 0x6305D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6305E0);
    RH_ScopedVMTInstall(ProcessPed, 0x630600);
}

// NOTSA: *deathTime* originally int32
// 0x630590
CTaskSimpleDead::CTaskSimpleDead(uint32 deathTime, bool hasDrowned) :
    m_nDeathTimeMS{deathTime},
    m_bHasDrowned{hasDrowned}
{
}

// Helper NOTSA; Clone (0x636100) usa el constructor con (tiempo, ahogado)
CTaskSimpleDead::CTaskSimpleDead(const CTaskSimpleDead& o) :
    CTaskSimpleDead{o.m_nDeathTimeMS, o.m_bHasDrowned}
{
}

// 0x636100
CTask* CTaskSimpleDead::Clone() const {
    // Como el original: pasa m_bHasDrowned (bit 1 del byte), no m_bFirstTime
    return new CTaskSimpleDead(m_nDeathTimeMS, m_bHasDrowned);
}

namespace {
// Como el original: asin del CRT del EXE (0x821E70): fpatan(x, sqrt((1 + x)(1 - x))) con la palabra de control
// (anterior & 0x300) | 0x7F; el resultado queda en ST0
float __declspec(naked) __cdecl DeadAsinX87(float x) {
    __asm {
        sub    esp, 4
        fnstcw word ptr [esp]
        mov    ax, word ptr [esp]
        and    ax, 300h
        or     ax, 7Fh
        mov    word ptr [esp + 2], ax
        fldcw  word ptr [esp + 2]
        fld    dword ptr [esp + 8]
        fld1
        fadd   st, st(1)
        fld1
        fsub   st, st(2)
        fmulp  st(1), st
        fsqrt
        fpatan
        fldcw  word ptr [esp]
        add    esp, 4
        ret
    }
}
} // namespace

// 0x630600
bool CTaskSimpleDead::ProcessPed(CPed* ped) {
    bool wasStanding = false; // [esp+13h]
    bool isPlayerTarget = false; // bl

    if (m_bFirstTime) {
        if (ped->bInVehicle) {
            const auto door = CCarEnterExit::ComputeTargetDoorToExit(ped->m_pVehicle, ped);
            if (door == TARGET_DOOR_DRIVER || door == TARGET_DOOR_REAR_LEFT) {
                CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CAR_DEAD_LHS, 4.0f);
            } else {
                CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CAR_DEAD_RHS, 4.0f);
            }
        }

        ped->SetPedState(PEDSTATE_DEAD);
        m_bFirstTime = false;

        if (ped->bIsStanding || ped->bWasStanding) {
            wasStanding = true;
        }

        // Como el original: FindPlayerPed(0) sin comprobar nulo; FindPlayerPed(1) se llama dos veces
        if (FindPlayerPed(0)->m_pTargetedObject == ped) {
            isPlayerTarget = true;
        } else if (FindPlayerPed(1) && FindPlayerPed(1)->m_pTargetedObject == ped) {
            isPlayerTarget = true;
        }

        if (!m_bHasDrowned && !ped->m_standingOnEntity && !isPlayerTarget) {
            ped->m_bUsesCollision = false;
        }

        ped->m_fHealth = 0.0f;
        const auto  slot       = (int8)ped->m_nActiveWeaponSlot;
        const auto  weaponType = ped->m_aWeapons.data()[slot].m_Type;
        ped->RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nModelId1);
        ped->m_nActiveWeaponSlot = 0;

        if (!ped->IsPlayer()) {
            ped->RemoveWeaponAnims(0, -1000.0f);
            ped->CreateDeadPedWeaponPickups();
            ped->CreateDeadPedMoney();
        }

        {
            CEventDeadPed event{ped, m_bHasDrowned, m_nDeathTimeMS};
            GetEventGlobalGroup()->Add(&event, false);
            CAccidentManager::GetInstance()->ReportAccident(ped);
        }
    }

    bool updateSlope = false;
    if (!m_bFirstTime && m_bHasDrowned && ped->bIsStanding) {
        ped->m_bUsesCollision = false;
        m_bHasDrowned = false;
        auto* assoc = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_FLOOR_HIT_F, 8.0f);
        assoc->m_Flags &= ~ANIMATION_IS_FINISH_AUTO_REMOVE;
        updateSlope = true;
    } else if (   ped->m_bUsesCollision
               && !m_bHasDrowned
               && ped->bIsStanding
               && !ped->m_standingOnEntity
               && FindPlayerPed(0)->m_pTargetedObject != ped
               && !(FindPlayerPed(1) && FindPlayerPed(1)->m_pTargetedObject == ped)
               && !isPlayerTarget) {
        ped->m_bUsesCollision = false;
        updateSlope = true;
    } else if (wasStanding) {
        updateSlope = true;
    }

    if (updateSlope) {
        // Como el original: productos punto en el orden z, y, x; clamp [-1, 1] (NaN pasa) y asin del CRT
        const auto& matrix  = *ped->m_matrix;
        const auto& forward = matrix.GetForward();
        const auto& right   = matrix.GetRight();
        const float slopeForward = ped->field_578.z * forward.z + ped->field_578.y * forward.y + ped->field_578.x * forward.x;
        const float slopeRight   = ped->field_578.z * right.z + ped->field_578.y * right.y + ped->field_578.x * right.x;
        ped->m_pedIK.m_fSlopeRoll  = DeadAsinX87(std::clamp(slopeRight, -1.0f, 1.0f));
        ped->m_pedIK.m_fSlopePitch = DeadAsinX87(std::clamp(slopeForward, -1.0f, 1.0f));
    }

    ped->DeadPedMakesTyresBloody();

    if (CLocalisation::Blood() && !m_bHasDrowned) {
        const uint32 elapsed = CTimer::GetTimeInMS() - m_nDeathTimeMS;

        // Como el original: radio 0 (0x858B50) antes de 2 s, 0.75000006 (0x85B900) tras 7 s, y entre medias
        // (elapsed - 2000) * 0x86DECC
        constexpr float kGrow = std::bit_cast<float>(0x391D4952u);  // 0x86DECC
        constexpr float kGrowNeg = std::bit_cast<float>(0xB91D4952u); // 0x86DEC8
        float radius;
        if (elapsed < 2000u) {
            radius = 0.0f;
        } else if (elapsed > 7000u) {
            radius = std::bit_cast<float>(0x3F400001u);
        } else {
            radius = (float)(int32)(elapsed - 2000u) * kGrow;
        }
        const float radiusSq = radius * radius;

        // Como el original: 16 entidades del escáner de peds; ebp queda en 200 tras el bucle
        CEntity** const entities = ped->GetIntelligence()->GetPedEntities();
        for (size_t i = 0; i < 16; i++) {
            CEntity* const entity = entities[i];
            if (!entity) {
                continue;
            }
            const auto& pedPos = ped->GetPosition();
            const auto& entPos = entity->GetPosition();
            const float dx = entPos.x - pedPos.x;
            const float dy = entPos.y - pedPos.y;
            const float dz = entPos.z - pedPos.z;
            const float t0 = dz * dz;
            const float t1 = dy * dy;
            const float t2 = t0 + t1;
            const float t3 = dx * dx;
            const float distSq = t2 + t3;
            if (radiusSq > distSq) {
                auto* other = static_cast<CPed*>(entity);
                other->m_nDeathTimeMS = 200;
                other->bDoBloodyFootprints = true;
            }
        }

        if (elapsed > 2000u && !m_bBloodPuddleCreated) {
            const CVector pos = ped->GetPosition();
            uint32 elapsed2 = elapsed - 2000u;
            if (elapsed2 <= CTimer::GetTimeInMS() - CTimer::GetPreviousTimeInMS()) {
                float waterLevel; // sin inicializar como el original si no hay agua
                if (CWaterLevel::GetWaterLevelNoWaves(ped->GetPosition(), &waterLevel, nullptr, nullptr)) {
                    if (ped->GetPosition().z <= waterLevel) {
                        m_bBloodPuddleCreated = true;
                    }
                }
            }

            if (!m_bBloodPuddleCreated && CLocalisation::Blood()) {
                if (elapsed2 < 5000u) {
                    const float f = (float)(int32)elapsed2;
                    const float sideY  = kGrowNeg * f;
                    const float frontX = f * kGrow;
                    CShadows::StoreStaticShadow(
                        (uint32)(uintptr_t)this + 0x11, SHADOW_DEFAULT, gpBloodPoolTex, pos,
                        frontX, 0.0f, 0.0f, sideY,
                        255, 200, 0, 0, 4.0f, 1.0f, 40.0f, false, 0.0f);
                } else {
                    CShadows::AddPermanentShadow(
                        SHADOW_DEFAULT, gpBloodPoolTex, const_cast<CVector*>(&pos),
                        std::bit_cast<float>(0x3F400001u), 0.0f, 0.0f, std::bit_cast<float>(0xBF400001u),
                        255, 200, 0, 0, 4.0f, 40000, 1.0f);
                    m_bBloodPuddleCreated = true;
                }
            }
        }
    }

    if (m_bHasDrowned) {
        ped->bIsStanding = false;
        ped->bWasStanding = false;
    } else {
        ped->m_pedIK.bSlopePitch = true;
        ped->m_vecMoveSpeed = CVector{0.0f, 0.0f, 0.0f};
    }
    return false;
}
