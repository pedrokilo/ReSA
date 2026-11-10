/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFall
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

#include "TaskSimpleFall.h"

namespace {
// Callback por defecto de animaciones del EXE (CDefaultAnimCallback::DefaultAnimCB, 0x4D1390: `ret`). Como el original, el destructor pasa la direccion del EXE a SetFinishCallback.
using AnimFinishCallback = void (*)(CAnimBlendAssociation*, void*);
const AnimFinishCallback kExeDefaultAnimCB = reinterpret_cast<AnimFinishCallback>(0x4D1390);

// 0x678310 (lote 154A, banco Fn154A): cuerpo del destructor de CTaskSimpleFall del EXE (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla): deja la vtable de la clase (0x870430); si hay
// animacion (m_pAnim, +0x14) le pone SetFinishCallback(0x4D1390, nullptr) (0x4CEBE0, con hook); y llama al cuerpo de ~CTask (0x61A3A0, con hook: vtable base 0x86D48C). El marco SEH se omite.
void __fastcall CTaskSimpleFallDestructorBodyExe(CTaskSimpleFall* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870430;
    if (self->m_pAnim) {
        self->m_pAnim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}
} // namespace

// 0x678300 (lote 154A, banco Fn154A; thiscall, `ret`, 6 bytes: `mov eax,0xCF`): TASK_SIMPLE_FALL; no lee el objeto.
eTaskType CTaskSimpleFall::GetTaskType() const {
    return Type;
}

void CTaskSimpleFall::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskSimpleFall, 0x870430, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleFallDestructorBodyExe, "DestructorBody", 0x678310); // lote 154A (banco Fn154A)
    RH_ScopedVMTInstall(GetTaskType, 0x678300); // lote 154A (banco Fn154A): retorno de constante
    RH_ScopedInstall(Constructor, 0x6782C0);
    RH_ScopedInstall(StartAnim, 0x67CA40);
    RH_ScopedInstall(ProcessFall, 0x6784C0);
    RH_ScopedInstall(FinishFallAnimCB, 0x6786B0);
    RH_ScopedVMTInstall(ProcessPed, 0x67FAF0);
    RH_ScopedVMTInstall(MakeAbortable, 0x678370);
}

CTaskSimpleFall* CTaskSimpleFall::Constructor(AnimationId nAnimId, AssocGroupId nAnimGroup, int32 nDownTime)
{
    this->CTaskSimpleFall::CTaskSimpleFall(nAnimId, nAnimGroup, nDownTime);
    return this;
}

// 0x6782C0
CTaskSimpleFall::CTaskSimpleFall(AnimationId nAnimId, AssocGroupId nAnimGroup, int32 nDownTime)
{
    m_nAnimId = nAnimId;
    m_nAnimGroup = nAnimGroup;
    m_nTotalDownTime = nDownTime;
    m_nCurrentDownTime = nDownTime;

    m_bIsFinished = false;
    m_pAnim = nullptr;
}

// 0x678310
CTaskSimpleFall::~CTaskSimpleFall()
{
    if (m_pAnim)
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
}

// 0x67FAF0
bool CTaskSimpleFall::ProcessPed(CPed* ped)
{
    ped->m_pedIK.bSlopePitch = true;
    ped->bFallenDown = true;

    if (m_bIsFinished && (ped->bIsStanding || ped->bIsDrowning))
    {
        // Como el original: ms_fTimeStep * 0.02f * 1000.0f truncado con _ftol2 (GetTimeStepInMS divide entre 50)
        const auto nTimeStep = (uint32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.0f);
        if (m_nCurrentDownTime <= nTimeStep)
        {
            m_nCurrentDownTime = 0;
            return true;
        }

        m_nCurrentDownTime -= nTimeStep;

        if (m_nCurrentDownTime > m_nMaxPlayerDownTime
            && ped->IsPlayer()
            && !ped->bIsBeingArrested
            && ped->m_nPedState != PEDSTATE_ARRESTED
            && ped->AsPlayer()->GetPadFromPlayer()
            && !ped->AsPlayer()->GetPadFromPlayer()->DisablePlayerControls
            )
        {
            m_nCurrentDownTime = m_nMaxPlayerDownTime - nTimeStep;
            return false;
        }
    }
    else
    {
        if (!m_pAnim && !m_bIsFinished)
            StartAnim(ped);

        ProcessFall(ped);
    }

    return false;
}

// 0x678370
bool CTaskSimpleFall::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    auto pFallAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FALL_FRONT);
    if (!pFallAnim)
        pFallAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FALL_BACK);

    if (priority == ABORT_PRIORITY_IMMEDIATE)
    {
        auto pThisAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_nAnimId);

        if (pThisAnim)
            pThisAnim->m_BlendDelta = -1000.0F;

        if (pFallAnim)
            pFallAnim->m_BlendDelta = -1000.0F;

        m_bIsFinished = true;
        m_nCurrentDownTime = 0;
        return true;
    }

    if (priority == ABORT_PRIORITY_URGENT)
    {
        if (event)
        {
            auto eventType = event->GetEventType();
            if (eventType == EVENT_DAMAGE && ped->m_fHealth < 1.0F
                || eventType == EVENT_IN_WATER
                || event->GetEventPriority() == 71
                || eventType == EVENT_STUCK_IN_AIR
                )
            {
                float fBlendDelta = -2.0F;
                if (eventType == EVENT_IN_WATER || eventType == EVENT_STUCK_IN_AIR)
                    fBlendDelta = -8.0F;

                if (pFallAnim)
                    pFallAnim->m_BlendDelta = fBlendDelta;

                if (m_pAnim)
                {
                    m_pAnim->m_BlendDelta = fBlendDelta;
                    m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                }

                m_pAnim = nullptr;
                m_bIsFinished = true;
                m_nCurrentDownTime = 0;

                return true;
            }
        }
    }

    return false;
}

// 0x67CA40
bool CTaskSimpleFall::StartAnim(CPed* ped)
{
    if (ped->m_pAttachedTo)
        return false;

    if (m_nAnimId == ANIM_ID_NO_ANIMATION_SET)
    {
        m_pAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_BIKE_FALL_OFF);
        if (!m_pAnim)
            m_pAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_BIKE_FALLR);

        if (m_pAnim)
            m_pAnim->SetFinishCallback(CTaskSimpleFall::FinishFallAnimCB, this);
    }
    else
    {
        m_pAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_nAnimId);
        if (m_pAnim)
        {
            m_pAnim->Start(0.0F);
            m_pAnim->SetBlend(0.0F, 8.0F);
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            m_pAnim->m_Flags &= ~ANIMATION_IS_FINISH_AUTO_REMOVE;
            m_pAnim->SetFinishCallback(CTaskSimpleFall::FinishFallAnimCB, this);
        }
        else
        {
            m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_nAnimGroup, m_nAnimId, 8.0F);
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            m_pAnim->m_Flags &= ~ANIMATION_IS_FINISH_AUTO_REMOVE;
            m_pAnim->SetFinishCallback(CTaskSimpleFall::FinishFallAnimCB, this);
            if (m_nAnimId == ANIM_ID_BIKE_FALLR)
                m_pAnim->SetCurrentTime(0.4F);
        }
    }

    return true;
}

// 0x6784C0
void CTaskSimpleFall::ProcessFall(CPed* ped)
{
    if (ped->IsPlayer()
        && (ped->bKnockedUpIntoAir || ped->bKnockedOffBike)
        && !ped->bIsStanding
        )
    {
        CAnimBlendAssociation* anim;
        auto pFirstAnim = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_PARTIAL);

        if (pFirstAnim && (pFirstAnim->m_AnimId == ANIM_ID_FALL_BACK || pFirstAnim->m_AnimId == ANIM_ID_FALL_FRONT))
            anim = pFirstAnim;
        else
            anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FALL_BACK);

        if (!anim)
            anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FALL_FRONT);

        if (anim)
        {
            if (anim->m_BlendAmount > 0.3F
                && anim->m_BlendDelta >= 0.0F
                && anim->m_CurrentTime > 0.667F
                && anim->m_CurrentTime - anim->m_TimeStep <= 0.667F
                )
            {
                anim->Start(0.0F);
            }
        }
        else
        {
            if (pFirstAnim && pFirstAnim->m_CurrentTime > pFirstAnim->m_BlendHier->m_fTotalTime * 0.8F)
                CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, pFirstAnim->m_Flags & ANIMATION_IS_FRONT ? ANIM_ID_FALL_FRONT : ANIM_ID_FALL_BACK, 8.0F);
        }
    }
    else if ((ped->bKnockedUpIntoAir || ped->bKnockedOffBike)
        && ped->bIsStanding
        && !ped->bWasStanding)
    {
        auto anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FALL_BACK);
        if (!anim)
            anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_FALL_FRONT);

        if (anim)
        {
            ped->bKnockedUpIntoAir = false;
            anim->m_Speed = 3.0F;
        }
        else
        {
            auto firstAnim = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_PARTIAL);
            if (firstAnim && !(firstAnim->m_Flags & ANIMATION_IS_PLAYING))
                ped->bKnockedUpIntoAir = false;
        }
    }
}

// 0x6786B0
void CTaskSimpleFall::FinishFallAnimCB(CAnimBlendAssociation* anim, void* data)
{
    CTaskSimpleFall* task = reinterpret_cast<CTaskSimpleFall*>(data);
    task->m_pAnim = nullptr;
    task->m_bIsFinished = true;
}

