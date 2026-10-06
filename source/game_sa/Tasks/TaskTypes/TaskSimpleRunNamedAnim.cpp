/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleRunNamedAnim
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

#include "TaskSimpleRunNamedAnim.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "EventGroup.h"

void CTaskSimpleRunNamedAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleRunNamedAnim, 0x86D54C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "Default", 0x6674B0, CTaskSimpleRunNamedAnim*(CTaskSimpleRunNamedAnim::*)());
    RH_ScopedOverloadedInstall(Constructor, "Anim", 0x61A990, CTaskSimpleRunNamedAnim*(CTaskSimpleRunNamedAnim::*)(char const*, char const*, int32, float, int32, bool, bool, bool, bool));
    RH_ScopedInstall(Destructor, 0x61BF10);

    RH_ScopedVMTInstall(Clone, 0x61B770);
    RH_ScopedVMTInstall(GetTaskType, 0x61AA90);
    RH_ScopedVMTInstall(ProcessPed, 0x61BF20);
    RH_ScopedInstall(OffsetPedPosition, 0x61AB00);
}

// 0x6674B0
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim() :
    CTaskSimpleAnim{ false }
{
    // Rest done in header
}

// 0x61A990
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim(
    const char* animName,
    const char* animGroupName,
    uint32 animFlags,
    float blendDelta,
    uint32 endTime,
    bool bDontInterrupt,
    bool bRunInSequence,
    bool bOffsetPed,
    bool bHoldLastFrame
) :
    CTaskSimpleAnim(bHoldLastFrame),
    m_pAnimHierarchy{},
    m_Time{ endTime },
    m_vecOffsetAtEnd{},
    m_animFlags{ animFlags }
{
    m_bDontInterrupt = bDontInterrupt;
    m_bRunInSequence = bRunInSequence;
    m_bOffsetAtEnd = bOffsetPed;
    m_bOffsetAvailable = false;
    m_fBlendDelta = blendDelta;
    // Como el original: copia byte a byte hasta el nulo, sin relleno ni comprobación de tamaño
    strcpy(m_animName, animName);
    strcpy(m_animGroupName, animGroupName);
    if (const auto block = CAnimManager::GetAnimationBlock(m_animGroupName)) {
        m_pAnimHierarchy = CAnimManager::GetAnimation(m_animName, block);
    }
}

// 0x61BF20
bool CTaskSimpleRunNamedAnim::ProcessPed(CPed* ped) {
    if (m_bOffsetAvailable) {
        OffsetPedPosition(ped);
    }

    if (!m_bIsFinished) {
        // Como el original: sin jerarquía (o sin secuencias) no hay nada que reproducir y la tarea termina
        if (!m_pAnimHierarchy || !m_pAnimHierarchy->m_pSequences) {
            return true;
        }
        // Como el original: con m_bDontInterrupt se purgan los eventos no válidos (salvo animación fija sin bucle)
        if (m_bDontInterrupt) {
            if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) || (m_animFlags & ANIMATION_IS_LOOPED)) {
                ped->GetEventGroup().RemoveInvalidEvents(true);
                ped->GetEventGroup().Reorganise();
            }
        }
        if (m_Timer.m_bStarted && m_Timer.IsOutOfTime()) {
            if (m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) {
                MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            } else {
                if (m_pAnim) {
                    m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                }
                m_bIsFinished = true;
                m_pAnim       = nullptr;
                return true;
            }
        }
        if (!m_pAnim) {
            // Como el original: si ya hay una asociación de esta jerarquía con datos de callback, la tarea termina
            if (const auto* const assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_pAnimHierarchy)) {
                if (assoc->m_pCallbackData) {
                    return true;
                }
            }
            StartAnim(ped);
        }
        return m_bIsFinished;
    }

    // Como el original: con el desplazamiento final pendiente de aplicar no se hace nada más
    if (m_bOffsetAtEnd) {
        return true;
    }
    const auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_pAnimHierarchy);
    CVector bonePos{};
    ped->GetBonePosition(&bonePos, BONE_SPINE1, false);
    if (m_fBlendDelta > 100.0f && assoc) {
        // Como el original: si el peatón está a más de 1 del hueso se fuerza el fundido de salida de la asociación
        const CVector& pos = ped->GetPosition();
        const float dx = pos.x - bonePos.x;
        const float dy = pos.y - bonePos.y;
        const float dz = pos.z - bonePos.z;
        const float dist = std::sqrt((dz * dz + dy * dy) + dx * dx);
        if (dist > 1.0f) {
            assoc->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            assoc->m_BlendDelta = m_fBlendDelta * -1.0f;
        }
    }
    return true;
}

// 0x61BB10
void CTaskSimpleRunNamedAnim::StartAnim(CPed* ped) {
    // Como el original: el temporizador solo arranca con un tiempo final con signo no negativo
    if (static_cast<int32>(m_Time) >= 0) {
        m_Timer.m_nStartTime = CTimer::m_snTimeInMilliseconds;
        m_Timer.m_nInterval  = static_cast<int32>(m_Time);
        m_Timer.m_bStarted   = true;
    }
    if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE)) {
        m_animFlags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    }
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_pAnimHierarchy, static_cast<int32>(m_animFlags), m_fBlendDelta);
    if (m_bOffsetAtEnd) {
        m_pAnim->SetFinishCallback(FinishRunAnimMovePedCB, this);
    } else if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) && !m_bRunInSequence) {
        m_pAnim->SetDeleteCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    } else {
        m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    }
    m_nAnimId = static_cast<int16>(m_pAnim->GetAnimId());
}

// 0x61AAA0
void CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB(CAnimBlendAssociation* blendAssoc, void* data) {
    auto* const task = static_cast<CTaskSimpleRunNamedAnim*>(data);
    // Como el original: se guarda en la tarea el desplazamiento de la raíz del primer nodo
    blendAssoc->GetNode(0)->GetCurrentTranslation(task->m_vecOffsetAtEnd, 0.0f);
    task->m_bOffsetAvailable = true;
    blendAssoc->m_Flags |= ANIMATION_IGNORE_ROOT_TRANSLATION;
    if (!(blendAssoc->m_Flags & ANIMATION_IS_FINISH_AUTO_REMOVE) && blendAssoc->m_nCallbackType != ANIM_BLEND_CALLBACK_DELETE) {
        blendAssoc->SetDeleteCallback(CTaskSimpleAnim::FinishRunAnimCB, task);
        return;
    }
    task->m_bIsFinished = true;
    task->m_pAnim       = nullptr;
}

// 0x61AB00
void CTaskSimpleRunNamedAnim::OffsetPedPosition(CPed* ped) {
    ped->UpdateRpHAnim();
    ped->m_bDontUpdateHierarchy = true;
    auto& pos = ped->GetPosition();
    // Como el original: Multiply3x3 (0x59C790) con el orden de sumas del EXE
    const CMatrix& mat = *ped->m_matrix;
    const CVector& v = m_vecOffsetAtEnd;
    const CVector off{
        mat.GetUp().x * v.z + mat.GetForward().x * v.y + mat.GetRight().x * v.x,
        mat.GetUp().y * v.z + mat.GetRight().y * v.x + mat.GetForward().y * v.y,
        mat.GetUp().z * v.z + mat.GetRight().z * v.x + mat.GetForward().z * v.y,
    };
    pos.x = off.x + pos.x;
    pos.y = off.y + pos.y;
    pos.z = off.z + pos.z;
    m_bOffsetAvailable = false;
}
