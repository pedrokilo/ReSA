/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleRunAnim
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleRunAnim.h"

void CTaskSimpleRunAnim::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskSimpleRunAnim, 0x86D528, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x61A8B0);
    RH_ScopedInstall(Constructor2, 0x61A900);
    RH_ScopedInstall(StartAnim, 0x61A950);
    //VTABLE
    RH_ScopedVMTInstall(Clone, 0x61B6D0);
    RH_ScopedVMTInstall(ProcessPed, 0x61BAC0);
}

CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, bool bHoldLastFrame)
{
    this->CTaskSimpleRunAnim::CTaskSimpleRunAnim(animGroup, animId, fBlendDelta, bHoldLastFrame);
    return this;
}

CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor2(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, int32 nTaskType, const char* taskName, bool bHoldLastFrame)
{
    this->CTaskSimpleRunAnim::CTaskSimpleRunAnim(animGroup, animId, fBlendDelta, nTaskType, taskName, bHoldLastFrame);
    return this;
}

// 0x61A900
CTaskSimpleRunAnim::CTaskSimpleRunAnim(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, int32 nTaskType, const char* taskName _IGNORED_, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame)
{
    m_nAnimGroup = animGroup;
    m_nAnimId = animId;
    m_fBlendDelta = fBlendDelta;
    m_nTaskType = nTaskType;
}

// 0x61A8B0
CTaskSimpleRunAnim::CTaskSimpleRunAnim(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame)
{
    m_nAnimGroup = animGroup;
    m_nAnimId = animId;
    m_fBlendDelta = fBlendDelta;
    m_nTaskType = TASK_SIMPLE_ANIM;
}

// 0x61B6D0
CTask* CTaskSimpleRunAnim::Clone() const {
    return new CTaskSimpleRunAnim(m_nAnimGroup, m_nAnimId, m_fBlendDelta, m_bHoldLastFrame);
}

// 0x61BAC0
bool CTaskSimpleRunAnim::ProcessPed(CPed* ped)
{
    if (m_bIsFinished)
        return true;

    if (!m_pAnim && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_nAnimId))
        StartAnim(ped);

    return m_bIsFinished;
}

// 0x61A950
void CTaskSimpleRunAnim::StartAnim(CPed* ped)
{
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_nAnimGroup, m_nAnimId, m_fBlendDelta);
    m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
}

