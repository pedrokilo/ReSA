/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGoToPointFine
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGoToPointFine.h"
#include "X87Math.h"
#include "TaskSimpleDuck.h"

namespace CTaskSimpleGoToBody {
void __fastcall GoToDestructorBody(CTaskSimpleGoTo* self, void* edx); // TaskSimpleGoTo.cpp: 0x667A00 / 0x667D50
}

namespace {
// 0x65EEF0 (lote 158A, banco Fn158A): cuerpo no virtual de ~CTaskSimpleGoToPointFine del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86F964h ; jmp 667A00`).
// Deja la vtable de CTaskSimpleGoToPointFine del EXE (0x86F964) y salta al cuerpo de ~CTaskSimpleGoTo (0x667A00, con hook), que deja la vtable de CTask tras pasar por CTask::Destructor.
__declspec(noinline) void __fastcall GoToPointFineDestructorBody(CTaskSimpleGoToPointFine* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F964;
    CTaskSimpleGoToBody::GoToDestructorBody(self, nullptr);
}

// 0x6634E0 (lote 179A, sin validar; el listado decia CTaskComplexGangFollower): destructor con borrado de CTaskSimpleGoToPointFine del EXE (ranura 0 de su vtable 0x86F964; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x65EEF0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Destruye con el cuerpo de la clase (0x65EEF0), libera con CTask::operator delete
// (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
__declspec(noinline) CTaskSimpleGoToPointFine* __fastcall GoToPointFineScalarDeletingDestructor(CTaskSimpleGoToPointFine* self, void* /*edx*/, uint8 flags) {
    GoToPointFineDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleGoToPointFine::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGoToPointFine, 0x86F964, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(GoToPointFineDestructorBody, "DestructorBody", 0x65EEF0); // lote 158A (banco Fn158A)
    RH_ScopedNamedGlobalInstall(GoToPointFineScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6634E0); // lote 179A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x65EEE0);                                         // lote 158A: ranura 4 de la vtable 0x86F964, retorno de constante
    RH_ScopedInstall(Constructor, 0x65EEB0);
    RH_ScopedVMTInstall(Clone, 0x662040);
    RH_ScopedVMTInstall(MakeAbortable, 0x663500);
    RH_ScopedVMTInstall(ProcessPed, 0x663540);
    RH_ScopedInstall(SetBlendedMoveAnim, 0x65EF80);
    RH_ScopedInstall(Finish, 0x65EF00);
    RH_ScopedNamedInstall(SetTargetPos_Reversed, "SetTargetPos", 0x65F330);
    RH_ScopedInstall(BaseRatio, 0x65EF30);
    RH_ScopedInstall(SetMoveRatio, 0x65F3A0);
}

// 0x65EEE0 (lote 158A, banco Fn158A; thiscall, `ret`, 6 bytes: `mov eax,39Eh`): TASK_SIMPLE_GO_TO_POINT_FINE (926); no lee el objeto (ranura 4 de la vtable 0x86F964).
eTaskType CTaskSimpleGoToPointFine::GetTaskType() const {
    return Type;
}

// 0x65F3A0
void CTaskSimpleGoToPointFine::SetMoveRatio(float value) {
    m_fMoveRatio = value;
}

// 0x65EEB0
CTaskSimpleGoToPointFine::CTaskSimpleGoToPointFine(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity) : CTaskSimpleGoTo(PEDMOVE_WALK, targetPoint, fRadius) {
    SetMoveRatio(moveRatio);
}

// 0x65EEB0
CTaskSimpleGoToPointFine* CTaskSimpleGoToPointFine::Constructor(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity) {
    this->CTaskSimpleGoToPointFine::CTaskSimpleGoToPointFine(moveRatio, targetPoint, fRadius, entity);
    return this;
}

// 0x662040
CTask* CTaskSimpleGoToPointFine::Clone() const {
    return new CTaskSimpleGoToPointFine(m_fMoveRatio, m_vecTargetPoint, m_fRadius, nullptr);
}

// 0x663500
bool CTaskSimpleGoToPointFine::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    QuitIK(ped);
    Finish(ped);
    return true;
}

// 0x65EF00
void CTaskSimpleGoToPointFine::Finish(CPed* ped) {
    ped->SetMoveState(PEDMOVE_STILL);
    ped->SetMoveAnim();
    ped->SetMoveState(static_cast<eMoveState>(m_moveState));
    ped->SetMoveAnim();
}

// 0x663540
bool CTaskSimpleGoToPointFine::ProcessPed(CPed* ped) {
    CVector2D vecDistance = m_vecTargetPoint - ped->GetPosition();
    if (m_fRadius * m_fRadius >= vecDistance.SquaredMagnitude() || HasCircledTarget(ped)) {
        QuitIK(ped);
        Finish(ped);
        return true;
    }
    SetBlendedMoveAnim(ped);
    const float fAngleInRadians = CGeneral::GetRadianAngleBetweenPoints(vecDistance.x, vecDistance.y, 0.0f, 0.0f);
    ped->m_fAimingRotation = CGeneral::LimitRadianAngle(fAngleInRadians);
    SetUpIK(ped);
    return false;
}

// todo: IDA code, BaseRatio can be used
// 0x65EF80
void CTaskSimpleGoToPointFine::SetBlendedMoveAnim(CPed* ped) {
    auto idleAnimAssoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_IDLE);
    auto walkAnimAssoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_WALK);
    auto runAnimAssoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_RUN);
    auto sprintAnimAssoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SPRINT);
    auto pIdleTiredAnimAssoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_IDLE_TIRED);

    if (ped->bIsDucking && ped->GetIntelligence()->GetTaskDuck(false)) {
        float fMoveSpeedY = m_fMoveRatio * 0.5f;
        fMoveSpeedY = std::min(fMoveSpeedY, 1.0f);
        CTaskSimpleDuck* duckTask = ped->GetIntelligence()->GetTaskDuck(false);
        duckTask->ControlDuckMove({ 0.0f, -fMoveSpeedY });
        return;
    }

    if (ped->bResetWalkAnims) {
        if (walkAnimAssoc)
            walkAnimAssoc->SetCurrentTime(0.0f);
        if (runAnimAssoc)
            runAnimAssoc->SetCurrentTime(0.0f);
        if (sprintAnimAssoc)
            sprintAnimAssoc->SetCurrentTime(0.0f);
        ped->bResetWalkAnims = false;
    }

    if (m_fMoveRatio == 0.0f) {
        if (!idleAnimAssoc) {
            CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 4.0f);
        }

        delete walkAnimAssoc;
        delete runAnimAssoc;
        delete sprintAnimAssoc;
        ped->m_nMoveState = PEDMOVE_STILL;
        m_moveState = PEDMOVE_STILL;
        return;
    }

    if (idleAnimAssoc) {
        if (walkAnimAssoc)
            walkAnimAssoc->SetCurrentTime(0.0f);
        if (runAnimAssoc)
            runAnimAssoc->SetCurrentTime(0.0f);
        delete idleAnimAssoc;
    }

    if (pIdleTiredAnimAssoc) {
        pIdleTiredAnimAssoc->m_BlendDelta = -4.0f;
        delete pIdleTiredAnimAssoc;
    }

    // Como el original: las comparaciones son "<= 1", "<= 2" y "<= 3" (con NaN se sale sin tocar el estado)
    eMoveState moveState;
    if (m_fMoveRatio <= 1.0f) {
        if (!walkAnimAssoc) {
            walkAnimAssoc = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_WALK);
            walkAnimAssoc->m_BlendAmount = 0.0f;
            walkAnimAssoc->m_Speed = 1.0f;
        }
        walkAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        walkAnimAssoc->m_BlendAmount = 1.0f;
        walkAnimAssoc->m_BlendDelta = 0.0f;
        delete runAnimAssoc;
        delete sprintAnimAssoc;
        moveState = PEDMOVE_WALK;
    } else if (m_fMoveRatio <= 2.0f) {
        if (!walkAnimAssoc) {
            walkAnimAssoc = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_WALK);
            walkAnimAssoc->m_BlendAmount = 0.0f;
            walkAnimAssoc->m_Speed = 1.0f;
        }
        walkAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        walkAnimAssoc->m_BlendDelta = 0.0f;
        walkAnimAssoc->m_BlendAmount = 2.0f - m_fMoveRatio;
        if (!runAnimAssoc) {
            runAnimAssoc = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_RUN);
            runAnimAssoc->m_BlendAmount = 0.0f;
            runAnimAssoc->m_Speed = 1.0f;
        }
        runAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        runAnimAssoc->m_BlendDelta = 0.0f;
        runAnimAssoc->m_BlendAmount = m_fMoveRatio - 1.0f;
        delete sprintAnimAssoc;
        moveState = PEDMOVE_RUN;
    } else if (m_fMoveRatio <= 3.0f) {
        delete walkAnimAssoc;
        if (!runAnimAssoc) {
            runAnimAssoc = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_RUN);
            runAnimAssoc->m_BlendAmount = 0.0f;
            runAnimAssoc->m_Speed = 1.0f;
        }
        runAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        runAnimAssoc->m_BlendDelta = 0.0f;
        runAnimAssoc->m_BlendAmount = 3.0f - m_fMoveRatio;
        if (!sprintAnimAssoc) {
            sprintAnimAssoc = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_SPRINT);
            sprintAnimAssoc->m_BlendAmount = 0.0f;
            sprintAnimAssoc->m_Speed = 1.0f;
        }
        sprintAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        sprintAnimAssoc->m_BlendDelta = 0.0f;
        sprintAnimAssoc->m_BlendAmount = m_fMoveRatio - 2.0f;
        moveState = PEDMOVE_SPRINT;
    } else {
        return;
    }
    ped->SetMoveState(moveState);
    ped->m_nSwimmingMoveState = moveState;
    m_moveState = moveState;
}

// 0x65F330
void CTaskSimpleGoToPointFine::SetTargetPos(CVector posn) {
    if (m_vecTargetPoint != posn || m_fRadius != 0.5f) {
        m_fRadius = 0.5f;
        m_vecTargetPoint = posn;
        m_GoToFlags = (m_GoToFlags & 0xF0) | 0x20;
    }
}

// Native TrackEntity::ControlSubTask keeps the child pointer in ECX across this
// call and immediately writes its move ratio. Preserve that executable ABI: ECX = this on return.
#pragma runtime_checks("", off)
uint32 __fastcall CTaskSimpleGoToPointFine::SetTargetPos_Reversed(CTaskSimpleGoToPointFine* self, uint32 /*edx*/, CVector posn) {
    self->SetTargetPos(posn);
    return reinterpret_cast<ResidueEcx*>(self)->Get(0);
}
#pragma runtime_checks("", restore)

// 0x65EF30
float CTaskSimpleGoToPointFine::BaseRatio(eMoveState moveState) {
    switch (moveState) {
    case PEDMOVE_WALK:
        return 1.0f;
    case PEDMOVE_RUN:
        return 2.0f;
    case PEDMOVE_SPRINT:
        return 3.0f;
    default:
        return 0.0f;
    }
}
