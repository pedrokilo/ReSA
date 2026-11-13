/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDie
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleDie.h"

// Espacio de nombres con enlace externo: CTaskSimpleDrown (0x62FF10) salta a este cuerpo como el `jmp 0x62FB40` del EXE (lote 177A, sin validar).
namespace CTaskSimpleDieBody {
// 0x62FB40 (lote 159A, banco Fn159A): cuerpo del destructor de CTaskSimpleDie del EXE (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla; lo llaman 0x62FC70, el cuerpo
// de CTaskSimpleDieInCar, por salto y el destructor con borrado de la clase): deja la vtable de la clase del EXE (0x86DDBC; la de la DLL no vale para los llamadores del EXE); si hay animacion (+0x24) le pone la
// funcion de fin por defecto (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0, con hook, con CDefaultAnimCallback::DefaultAnimCB 0x4D1390 y datos nulos); y sigue con el cuerpo de ~CTask (0x61A3A0, con
// hook). El destructor generado por el compilador dejaria la vtable de la DLL. El marco SEH se omite. Funcion propia sin expansion en linea.
__declspec(noinline) void __fastcall SimpleDieDestructorBodyExe(CTaskSimpleDie* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DDBC;
    if (self->m_animAssociation) {
        self->m_animAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}
} // namespace

namespace {
// 0x637500 (lote 183A, sin validar; el listado decia CTaskSimpleHitFromFront): destructor con borrado de CTaskSimpleDie del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62FB40;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo es el de ~CTaskSimpleDie (0x62FB40, con hook: vtable 0x86DDBC, callback de fin por defecto y CTask::Destructor). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CTaskSimpleDie* __fastcall SimpleDieScalarDeletingDestructor(CTaskSimpleDie* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleDieBody::SimpleDieDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

// 0x62FA50 (lote 159A, banco Fn159A; thiscall, `ret`, 6 bytes: `mov eax,0D4h`): TASK_SIMPLE_DIE (212); no lee el objeto (ranura 4 de la vtable 0x86DDBC).
eTaskType CTaskSimpleDie::GetTaskType() const {
    return Type;
}

// 0x62FA00
CTaskSimpleDie::CTaskSimpleDie(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed) : CTaskSimple() {
    m_animId          = animId;
    m_animGroupId     = animGroupId;
    m_animSpeed       = animSpeed;
    m_blendDelta      = blendDelta;
    m_animHierarchy   = nullptr;
    m_animAssociation = nullptr;
    m_animFlags       = ANIMATION_DEFAULT;
    m_nFlags         &= ~3; // Como el original: and byte ptr [esi+20h],0FCh (solo se limpian los dos bits)
}

// 0x62FA60
CTaskSimpleDie::CTaskSimpleDie(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed) : CTaskSimple() {
    m_animId          = ANIM_ID_KO_SHOT_FRONT_0;
    m_animGroupId     = ANIM_GROUP_DEFAULT;
    m_animSpeed       = animSpeed;
    m_blendDelta      = blendDelta;
    m_animHierarchy   = CAnimManager::GetAnimation(animName, CAnimManager::GetAnimationBlock(animBlock));
    m_animAssociation = nullptr;
    m_animFlags       = animFlags;
    m_nFlags         &= ~3; // Como el original: and byte ptr [esi+20h],0FCh
}

// 0x62FAF0
CTaskSimpleDie::CTaskSimpleDie(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed) : CTaskSimple() {
    m_animId          = ANIM_ID_KO_SHOT_FRONT_0;
    m_animGroupId     = ANIM_GROUP_DEFAULT;
    m_animSpeed       = animSpeed;
    m_blendDelta      = blendDelta;
    m_animFlags       = animFlags;
    m_animHierarchy   = animHierarchy;
    m_animAssociation = nullptr;
    m_nFlags         &= ~3; // Como el original: and byte ptr [esi+20h],0FCh
}

// 0x62FB40
CTaskSimpleDie::~CTaskSimpleDie() {
    if (m_animAssociation) {
        m_animAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x637520
void CTaskSimpleDie::StartAnim(CPed* ped) {
    if (m_animHierarchy)
        m_animAssociation = CAnimManager::BlendAnimation(ped->GetRpClump(), m_animHierarchy, m_animFlags, m_blendDelta);
    else
        m_animAssociation = CAnimManager::BlendAnimation(ped->GetRpClump(), m_animGroupId, m_animId, m_blendDelta);

    m_animAssociation->SetFinishCallback(FinishAnimDieCB, this);
    m_animAssociation->m_Flags &= ~ANIMATION_IS_FINISH_AUTO_REMOVE; // Como el original: and byte ptr [eax+2Eh],0F7h (no toca el byte alto)

    if (m_animSpeed > 0.0f)
        m_animAssociation->m_Speed = m_animSpeed;

    ped->ClearAll();
    ped->m_fHealth = 0.0f;                     // todo: SetHealth or something ?
    ped->StopNonPartialAnims();
    ped->m_nDeathTimeMS = CTimer::GetTimeInMS(); // todo: m_nDeathTimeMS int32 to uint32?
}

// 0x635DA0

// 0x0
CTask* CTaskSimpleDie::Clone() const {
    if (m_animHierarchy) {
        return new CTaskSimpleDie(m_animHierarchy, m_animFlags, m_blendDelta, m_animSpeed);
    } else {
        return new CTaskSimpleDie(m_animGroupId, m_animId, m_blendDelta, m_animSpeed);
    }
}

// 0x62FBA0
bool CTaskSimpleDie::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_animAssociation)
            m_animAssociation->m_BlendDelta = -1000.0f;

        ped->SetPedState(PEDSTATE_IDLE);
        ped->SetMoveState(PEDMOVE_STILL);
        ped->SetMoveAnim();
        ped->bIsDyingStuck = false; // Como el original: and dword ptr [esi+478h],0FFFFFFDFh (bit 5)
        return true;
    } else if (event && event->GetEventType() == EVENT_DEATH) {
        ped->bIsDyingStuck = false; // Como el original: bit 5 de +0x478
        return true;
    }

    return false;
}

// 0x6397E0
bool CTaskSimpleDie::ProcessPed(CPed* ped) {
    ped->m_pedIK.bSlopePitch = true;

    if (m_dieAnimFinished || m_animId == ANIM_ID_NO_ANIMATION_SET) {
        ped->bIsDyingStuck = true; // Como el original: or dword ptr [esi+478h],20h (bit 5)
        if (!m_alreadyDead) {
            if (
                ped->physicalFlags.bSubmergedInWater
                || !ped->GetUsesCollision()
                || ped->physicalFlags.bDontApplySpeed
                || ped->bIsStanding
                && ( !ped->m_standingOnEntity
                   || ped->IsPlayer()
                   || ped->m_standingOnEntity->AsPhysical()->physicalFlags.bDisableCollisionForce
               )
            ) {
                CEventDeath event(m_animId == ANIM_ID_DROWN);
                ped->GetEventGroup().Add(&event, false);
                m_alreadyDead = true;
                ped->bIsDyingStuck = false; // Como el original: and eax,0FFFFFFDFh (bit 5)
            }
        }
    } else if (!m_animAssociation) {
        StartAnim(ped);
        ped->SetPedState(PEDSTATE_DIE);
    }

    return false;
}

// 0x62FC10
void CTaskSimpleDie::FinishAnimDieCB(CAnimBlendAssociation* association, void* data) {
    auto task = static_cast<CTaskSimpleDie*>(data);
    task->m_dieAnimFinished = true;
    task->m_animAssociation = nullptr;
}

void CTaskSimpleDie__InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDie, 0x86DDBC, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedOverloadedInstall(Constructor, "1", 0x62FA00, CTaskSimpleDie*(CTaskSimpleDie::*)(AssocGroupId, AnimationId, float, float));
    RH_ScopedOverloadedInstall(Constructor, "2", 0x62FA60, CTaskSimpleDie*(CTaskSimpleDie::*)(const char*, const char*, eAnimationFlags, float, float));
    RH_ScopedOverloadedInstall(Constructor, "3", 0x62FAF0, CTaskSimpleDie*(CTaskSimpleDie::*)(CAnimBlendHierarchy *, eAnimationFlags, float, float));
    RH_ScopedNamedGlobalInstall(CTaskSimpleDieBody::SimpleDieDestructorBodyExe, "DestructorBody", 0x62FB40); // lote 159A (banco Fn159A)
    RH_ScopedNamedGlobalInstall(SimpleDieScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637500); // lote 183A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x62FA50); // lote 159A (banco Fn159A): retorno de constante
    RH_ScopedInstall(FinishAnimDieCB, 0x62FC10);
    RH_ScopedInstall(StartAnim, 0x637520);
    // clang moment: RH_ScopedVirtualOverloadedInstall(Clone, "", 0x635DA0,  CTask *(CTaskSimpleDie::*)());
    RH_ScopedVMTInstall(MakeAbortable, 0x62FBA0);
    RH_ScopedVMTInstall(ProcessPed, 0x6397E0);
}

// 0x62FA00
CTaskSimpleDie* CTaskSimpleDie::Constructor(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed) {
    this->CTaskSimpleDie::CTaskSimpleDie(animGroupId, animId, blendDelta, animSpeed);
    return this;
}
// 0x62FA60
CTaskSimpleDie* CTaskSimpleDie::Constructor(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed) {
    this->CTaskSimpleDie::CTaskSimpleDie(animName, animBlock, animFlags, blendDelta, animSpeed);
    return this;
}

// 0x62FAF0
CTaskSimpleDie* CTaskSimpleDie::Constructor(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed) {
    this->CTaskSimpleDie::CTaskSimpleDie(animHierarchy, animFlags, blendDelta, animSpeed);
    return this;
}

// 0x62FBA0// 0x6397E0
