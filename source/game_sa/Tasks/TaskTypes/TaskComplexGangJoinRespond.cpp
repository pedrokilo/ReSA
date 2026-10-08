/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGangJoinRespond
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 5.
 * Validación registrada: 5/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   MakeAbortable | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexGangJoinRespond::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangJoinRespond, 0x86FB5C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6616F0);
    RH_ScopedInstall(Destructor, 0x661720);

    RH_ScopedVMTInstall(Clone, 0x662290);
    RH_ScopedVMTInstall(MakeAbortable, 0x661790);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6617A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6618D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x661950);
}

// 0x6616F0
CTaskComplexGangJoinRespond::CTaskComplexGangJoinRespond(bool response) :
    m_response{response}
{
}

// 0x661720
CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond() {
    if (m_animsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }
}

// 0x6617A0
CTask* CTaskComplexGangJoinRespond::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x190)) {
        return nullptr;
    }
    if (m_animsReferenced) {
        return new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34), static_cast<AnimationId>(m_response == 1 ? 0x132 : 0x133), 4.0f, false);
    }
    if (++m_attempts > 10) {
        return nullptr;
    }
    return new CTaskSimpleStandStill(250, false, false, 8.0f);
}

// 0x6618D0
CTask* CTaskComplexGangJoinRespond::CreateFirstSubTask(CPed* ped) {
    m_attempts = 0;
    return new CTaskComplexTurnToFaceEntityOrCoord(FindPlayerPed(0), 0.5f, 0.2f);
}

// 0x661950
CTask* CTaskComplexGangJoinRespond::ControlSubTask(CPed* ped) {
    const auto& playerPos = FindPlayerPed(-1)->GetPosition();
    const auto& pedPos = ped->GetPosition();
    CVector direction{playerPos.x - pedPos.x, playerPos.y - pedPos.y, 0.0f};
    direction.Normalise();
    ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(direction.x, direction.y, 0.0f, 0.0f));
    if (!m_animsReferenced) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto index = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[index].IsLoaded == 1) {
                CAnimManager::AddAnimBlockRef(index);
                m_animsReferenced = true;
            } else {
                CStreaming::RequestModel(IFPToModelId(index), STREAMING_KEEP_IN_MEMORY);
            }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }
    return m_pSubTask;
}
