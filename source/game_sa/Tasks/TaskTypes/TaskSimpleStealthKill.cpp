/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleStealthKill
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "TaskSimpleStealthKill.h"

namespace {
// 0x622680 (lotes 126A y 164A, bancos Fn126A y Fn164A): cuerpo del destructor de CTaskSimpleStealthKill del EXE (thiscall, sin argumentos de pila, `ret`, 109 bytes con marco SEH que solo desenrolla;
// lo llama el destructor con borrado 0x6296B0). Deja la vtable de CTaskSimpleStealthKill del EXE (0x86DA74); libera con CEntity::CleanUpOldReference (0x571A00, con hook) la referencia no nula del
// objetivo (+0x0C); si hay animacion (+0x18) le pone el callback de borrado por defecto (CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0, con hook, con el DefaultAnimCB del EXE 0x4D1390 y dato
// nulo); y llama a CTask::Destructor (0x61A3A0, con hook). El destructor generado por el compilador dejaria la vtable de la DLL; el marco SEH se omite. Funcion propia sin expansion en linea para
// que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x622680` del EXE. Se engancha como S.
__declspec(noinline) void __fastcall StealthKillDestructorBody(CTaskSimpleStealthKill* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DA74;
    CEntity::SafeCleanUpRef(self->m_target);
    if (self->m_anim) {
        self->m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}
} // namespace

void CTaskSimpleStealthKill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleStealthKill, 0x86da74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6225F0);
    RH_ScopedNamedGlobalInstall(StealthKillDestructorBody, "DestructorBody", 0x622680); // lote 164A (banco Fn164A)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x6296B0);

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

// Destructor de la DLL (el del EXE es StealthKillDestructorBody, 0x622680; sin el la referencia al objetivo quedaba registrada al borrar la tarea)
CTaskSimpleStealthKill::~CTaskSimpleStealthKill() {
    CEntity::SafeCleanUpRef(m_target);
    if (m_anim) {
        m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// atan2(y, x) con fpatan como el EXE (precision completa del FPU, redondeo al guardar)
static float StealthKillAtan2(float y, float x) {
    return TrigAtan2Cpp(y, x); // X87Math.cpp
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

uintptr_t Lote126AStealthKillDestructorBody() { return reinterpret_cast<uintptr_t>(&StealthKillDestructorBody); } // para el banco Fn126A

// 0x6296B0 (lote 126A, banco Fn126A) : destructor con borrado de CTaskSimpleStealthKill del EXE (ranura 0 de su vtable 0x86DA74; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x622680; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x622680), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleStealthKill* CTaskSimpleStealthKill::ScalarDeletingDestructor(uint8 flags) {
    StealthKillDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
