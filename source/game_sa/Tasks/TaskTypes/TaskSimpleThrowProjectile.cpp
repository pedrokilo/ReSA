/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleThrowProjectile
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
#include "X87Math.h"

#include "TaskSimpleThrowProjectile.h"

void CTaskSimpleThrowProjectile::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleThrowProjectile, 0x86d790, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61F660);
    RH_ScopedInstall(Destructor, 0x61F700);

    RH_ScopedInstall(ControlThrow, 0x61F810);
    RH_ScopedInstall(StartAnim, 0x6259E0);
    RH_ScopedInstall(FinishAnimThrowProjectileCB, 0x61F890);

    RH_ScopedVMTInstall(Clone, 0x623030);
    RH_ScopedVMTInstall(GetTaskType, 0x61F6F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F780);
    RH_ScopedVMTInstall(ProcessPed, 0x62AF50);
}

// 0x61F660
CTaskSimpleThrowProjectile::CTaskSimpleThrowProjectile(CEntity* targetEntity, CVector targetPos) :
    m_TargetPos{ targetPos },
    m_TargetEntity{ targetEntity },
    m_ButtonCounter{ CTimer::GetTimeInMS() }
{
    CEntity::SafeRegisterRef(m_TargetEntity);
}

// 0x623030
CTaskSimpleThrowProjectile::CTaskSimpleThrowProjectile(const CTaskSimpleThrowProjectile& o) :
    CTaskSimpleThrowProjectile{ o.m_TargetEntity, o.m_TargetPos }
{
}

// 0x61F700
CTaskSimpleThrowProjectile::~CTaskSimpleThrowProjectile() {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
        m_Anim = nullptr;
    }
    CEntity::SafeCleanUpRef(m_TargetEntity);
}

// 0x61F810
bool CTaskSimpleThrowProjectile::ControlThrow(bool bButtonReleased, CEntity* targetEntity, const CVector* targetPos) {
    if (m_bIsFinished) {
        return true;
    }

    if (bButtonReleased && !m_bButtonReleased) {
        m_ButtonCounter = CTimer::GetTimeInMS() - m_ButtonCounter;
        m_bButtonReleased = true;
    }

    if (targetEntity && m_TargetEntity != targetEntity) {
        CEntity::ChangeEntityReference(m_TargetEntity, targetEntity);
    }

    if (targetPos) {
        m_TargetPos = *targetPos;
    }

    return false;
}

// 0x61F890
void CTaskSimpleThrowProjectile::FinishAnimThrowProjectileCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleThrowProjectile*>(data);

    self->m_Anim = nullptr;
    if (self->m_bStartThrowFinished) {
        self->m_bIsFinished = true;
    } else {
        self->m_bStartThrowFinished = true;
    }
}

// 0x6259E0
void CTaskSimpleThrowProjectile::StartAnim(CPed* ped) {
    // Como el original: la info del arma activa con la habilidad estándar
    const auto& wi = *CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().GetType(), eWeaponSkill::STD);
    const auto StartAnim = [&, this](AnimationId animId, float blendDelta) {
        m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), wi.m_eAnimGroup, animId, blendDelta);
        m_Anim->SetFinishCallback(FinishAnimThrowProjectileCB, this);
    };

    if (!m_bStartThrowFinished) {
        StartAnim(ANIM_ID_GRENADE_WEAPON_START_THROW, 16.f);
        return;
    }

    // Como el original: distancia al cuadrado sumada como (z*z + y*y) + x*x
    const auto IsFarFromPed = [ped](const CVector& pos) {
        const auto& pedPos = ped->GetPosition();
        const float dx = pos.x - pedPos.x, dy = pos.y - pedPos.y, dz = pos.z - pedPos.z;
        return dz * dz + dy * dy + dx * dx > 100.0f;
    };
    // Como el original: lanzamiento normal si el objetivo está lejos; "U" solo de cerca con el botón soltado o con la carga satchel
    if (m_TargetEntity && IsFarFromPed(m_TargetEntity->GetPosition())) {
        StartAnim(ANIM_ID_GRENADE_WEAPON_THROW, 1000.f);
    } else if (m_TargetPos != CVector{ 0.0f, 0.0f, 0.0f } && IsFarFromPed(m_TargetPos)) {
        StartAnim(ANIM_ID_GRENADE_WEAPON_THROW, 1000.f);
    } else if (m_bButtonReleased || ped->GetActiveWeapon().GetType() == WEAPON_REMOTE_SATCHEL_CHARGE) {
        StartAnim(ANIM_ID_GRENADE_WEAPON_THROWU, 1000.f);
    } else {
        StartAnim(ANIM_ID_GRENADE_WEAPON_THROW, 1000.f);
    }
}

// 0x61F780
bool CTaskSimpleThrowProjectile::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetDefaultFinishCallback();
            m_Anim->m_BlendDelta = -1000.0f;
            m_Anim                = nullptr;
        }
    } else if (const auto eDmg = notsa::dyn_cast_if_present<const CEventDamage>(event)) {
        if (eDmg->m_damageResponse.m_bHealthZero && eDmg->m_bAddToEventGroup || eDmg->m_bFallDown) {
            if (m_Anim) {
                m_Anim->SetDefaultFinishCallback();
                m_Anim->m_BlendDelta = -4.0f;
                m_Anim                = nullptr;
            }
            m_bIsFinished = true;
        }
    }
    return true;
}

// 0x62AF50
bool CTaskSimpleThrowProjectile::ProcessPed(CPed* ped) {
    // Como el original: info del arma activa con la habilidad estándar (no la del peatón)
    const auto wi = CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().GetType(), eWeaponSkill::STD);

    if (m_bIsFinished || !wi->flags.bThrow) {
        return true;
    }

    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }

    switch (m_Anim->m_AnimId) {
    case ANIM_ID_GRENADE_WEAPON_THROW:
    case ANIM_ID_GRENADE_WEAPON_THROWU: {
        const auto animFireTime = m_Anim->m_AnimId == ANIM_ID_GRENADE_WEAPON_THROW
            ? wi->m_fAnimLoop2Fire
            : wi->m_fAnimLoopFire;
        if (animFireTime < m_Anim->m_CurrentTime && (m_Anim->m_CurrentTime - m_Anim->m_TimeStep) <= animFireTime && m_Anim->IsPlaying()) {
            if (ped->IsPlayer()) {
                if (!m_bButtonReleased) {
                    m_ButtonCounter = CTimer::GetTimeInMS() - m_ButtonCounter;
                }
                // Como el original: tope con signo y fild * 0.05f
                if ((int32)m_ButtonCounter > 533) {
                    m_ButtonCounter = 533;
                }
                const int32 counter = (int32)m_ButtonCounter;
                const float attackScale = 0.05f; // 0x858C28
                const float attackCounter = IntTimesFloatCpp(counter, attackScale); // X87Math.cpp
                ped->GetPlayerData()->m_fAttackButtonCounter = attackCounter;
            }

            if (m_TargetEntity) {
                m_TargetPos = m_TargetEntity->GetPosition();
            }

            // Como el original: se copia el desplazamiento y se transforma en el sitio con la matriz del hueso de la mano
            CVector firePos = wi->m_vecFireOffset;
            const auto hier = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
            const auto index = RpHAnimIDGetIndex(hier, ped->m_apBones[PED_NODE_RIGHT_HAND]->BoneTag);
            RwV3dTransformPoints(&firePos, &firePos, 1, &RpHAnimHierarchyGetMatrixArray(hier)[index]);
            const bool hasTarget = m_TargetPos != CVector{ 0.0f, 0.0f, 0.0f };
            ped->GetActiveWeapon().Fire(ped, &firePos, &firePos, nullptr, hasTarget ? &m_TargetPos : nullptr, nullptr);
        }
        break;
    }
    }

    return false;
}
