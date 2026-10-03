/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleStealthKill
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleStealthKill.h"

void CTaskSimpleStealthKill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleStealthKill, 0x86da74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6225F0);

    RH_ScopedInstall(ManageAnim, 0x6296D0);
    RH_ScopedInstall(FinishAnimStealthKillCB, 0x622790);

    RH_ScopedVMTInstall(ProcessPed, 0x62E540);
    RH_ScopedVMTInstall(Clone, 0x623830);
    RH_ScopedVMTInstall(GetTaskType, 0x622670);
    RH_ScopedVMTInstall(MakeAbortable, 0x6226F0);
}

// 0x6225F0
CTaskSimpleStealthKill::CTaskSimpleStealthKill(bool keepTargetAlive, CPed* target, AssocGroupId groupId) :
    m_bKeepTargetAlive{ keepTargetAlive },
    m_target{ target },
    m_animGrpId{ groupId }
{
    CEntity::SafeRegisterRef(m_target);
}

// NOTSA (For 0x623830)
// Como el original: el clon copia los campos del original (la DLL leia los del objeto sin construir)
CTaskSimpleStealthKill::CTaskSimpleStealthKill(const CTaskSimpleStealthKill& o) :
    CTaskSimpleStealthKill{ o.m_bKeepTargetAlive, o.m_target, o.m_animGrpId }
{
}

// 0x622680 (el EXE lo tiene; sin el la referencia al objetivo quedaba registrada al borrar la tarea)
CTaskSimpleStealthKill::~CTaskSimpleStealthKill() {
    CEntity::SafeCleanUpRef(m_target);
    if (m_anim) {
        m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// atan2(y, x) con fpatan como el EXE (precision completa del FPU, redondeo al guardar)
static float StealthKillAtan2(float y, float x) {
    float result;
    __asm {
        fld y
        fld x
        fpatan
        fstp result
    }
    return result;
}

// 0x62E540
bool CTaskSimpleStealthKill::ProcessPed(CPed* ped) {
    if (m_bIsAborting || !m_target) {
        return true;
    }

    if (!m_anim) {
        if (!m_bKeepTargetAlive) {
            ped->SetPedState(PEDSTATE_DIE_BY_STEALTH);
        }

        ManageAnim(ped);

        if (!m_anim && m_bIsFinished) {
            return false;
        }
    }

    ped->m_pedIK.bSlopePitch = true;

    const auto pedToTarget = m_target->GetPosition() - ped->GetPosition();

    if (!m_bKeepTargetAlive) {
        if (!m_anim || m_anim->m_AnimId == ANIM_ID_KILL_KNIFE_PED_DAMAGE) {
            // Como el original: rumbo del vector objetivo->peaton, atan2(dx, -dy) (no -Heading())
            ped->m_fAimingRotation = StealthKillAtan2(pedToTarget.x, -pedToTarget.y);
        }
        return false;
    }

    // Como el original: modulo con el orden (z*z + x*x) + y*y
    const auto dist  = std::sqrt((pedToTarget.z * pedToTarget.z + pedToTarget.x * pedToTarget.x) + pedToTarget.y * pedToTarget.y);
    const auto shift = dist - 1.0f;
    if (std::abs(shift) > 0.02f) {
        const auto maxShift = CTimer::GetTimeStep() * 0.05f;
        ped->m_vecAnimMovingShiftLocal.y = shift > maxShift ? maxShift : shift; // Como el original: fcom / test ah,41h
        ped->m_vecAnimMovingShiftLocal.x = 0.0f;
    }

    ped->m_fAimingRotation = StealthKillAtan2(-pedToTarget.x, pedToTarget.y); // Como el original: fpatan

    return false;
}

// 0x6226F0
bool CTaskSimpleStealthKill::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            // Como el original: se corta la mezcla y se cambia el callback de borrado (no el de fin)
            m_anim->m_BlendDelta = -1000.0f;
            m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_anim = nullptr;
        }
        m_bIsAborting = true;
        return true;
    }
    case ABORT_PRIORITY_URGENT: {
        if (m_bKeepTargetAlive) {
            return false;
        }
        if (!m_anim || m_anim->m_AnimId != ANIM_ID_KILL_KNIFE_PED_DIE) {
            return false;
        }
        if (!event || event->GetEventType() != EVENT_DAMAGE || static_cast<const CEventDamage*>(event)->m_pSourceEntity != m_target) {
            return false;
        }
        // Como el original: se aborta la animacion
        m_bIsAborting = true;
        m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_anim = nullptr;
        return true;
    }
    }
    return false;
}

// 0x6296D0
void CTaskSimpleStealthKill::ManageAnim(CPed* ped) {
    CAnimBlock* pAnimBlock = CAnimManager::GetAssocGroups()[m_animGrpId].m_AnimBlock;
    if (!pAnimBlock || !pAnimBlock->IsLoaded) {
        // Como el original: ms_fTimeStep * 0.02 * 1000 truncado (no / 50)
        m_spentWaitingMs += (uint32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.0f);
        if (m_spentWaitingMs > 10000) {
            m_bIsAborting = true;
        }
        return;
    }

    if (m_bKeepTargetAlive) {
        m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_animGrpId, ANIM_ID_KILL_KNIFE_PLAYER, 8.0f);
    } else if (!m_bIsFinished) {
        m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_animGrpId, ANIM_ID_KILL_KNIFE_PED_DAMAGE, 8.0f);
    } else {
        // Como el original: la animacion se mezcla antes de calcular el dano
        m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_animGrpId, ANIM_ID_KILL_KNIFE_PED_DIE, 8.0f);

        // Como el original: el causante es el objetivo y el hueco activo se lee con signo (movsx)
        const auto weaponType = m_target->m_aWeapons.data()[(int8)m_target->m_nActiveWeaponSlot].m_Type;
        CPedDamageResponseCalculator damageCalculator{
            m_target,
            CPedDamageResponseCalculator::ms_damageFactor,
            weaponType,
            PED_PIECE_TORSO,
            false
        };
        CEventDamage eventDamage{
            m_target,
            CTimer::GetTimeInMS(),
            m_target->m_aWeapons.data()[(int8)m_target->m_nActiveWeaponSlot].m_Type,
            PED_PIECE_TORSO,
            0,
            false,
            !!ped->bInVehicle
        };
        if (eventDamage.AffectsPed(ped)) {
            damageCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);

            eventDamage.m_nAnimGroup = m_animGrpId;
            eventDamage.m_nAnimID = ANIM_ID_KILL_KNIFE_PED_DIE;
            eventDamage.m_fAnimBlend = 8.0f;
            eventDamage.m_fAnimSpeed = 1.0f;
            eventDamage.m_bAnimAdded = true; // Como el original: or dl,4 sobre el byte +0x25 (no la direccion)
            ped->GetEventGroup().Add(&eventDamage, false);
        }
    }
    m_anim->SetFinishCallback(FinishAnimStealthKillCB, this);
    m_bIsFinished = true;
}

// 0x622790
void CTaskSimpleStealthKill::FinishAnimStealthKillCB(CAnimBlendAssociation* pAnimAssoc, void* data) {
    const auto self = static_cast<CTaskSimpleStealthKill*>(data);
    // Como el original: m_bIsAborting solo se activa con estas animaciones; si no, se deja como estaba
    if (pAnimAssoc->m_AnimId == ANIM_ID_KILL_KNIFE_PLAYER || pAnimAssoc->m_AnimId == ANIM_ID_KILL_KNIFE_PED_DIE) {
        self->m_bIsAborting = true;
    }
    self->m_anim = nullptr;
}
