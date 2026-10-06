/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSignalAtPed
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
#include "TaskComplexSignalAtPed.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskSimpleRunAnim.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexGangLeader.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexSignalAtPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSignalAtPed, 0x86fa8c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660A30);
    RH_ScopedInstall(Destructor, 0x660AB0);

    RH_ScopedVMTInstall(Clone, 0x662140);
    RH_ScopedVMTInstall(GetTaskType, 0x660AA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x660B30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x660CC0);
    RH_ScopedVMTInstall(ControlSubTask, 0x660D80);
}

// 0x660A30
CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(CPed* pedToSignalAt, int32 unused1, bool playAnimAtEnd) :
    m_pedToSignalAt{pedToSignalAt},
    m_playAnimAtEnd{playAnimAtEnd}
{
    CEntity::SafeRegisterRef(m_pedToSignalAt);
}

CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(const CTaskComplexSignalAtPed& o) :
    CTaskComplexSignalAtPed{o.m_pedToSignalAt, o.m_initialPause, o.m_playAnimAtEnd}
{
}

// 0x660AB0
CTaskComplexSignalAtPed::~CTaskComplexSignalAtPed() {
    if (m_areAnimsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_areAnimsReferenced = false;
    }
    CEntity::SafeCleanUpRef(m_pedToSignalAt);
}

// 0x660B30
CTask* CTaskComplexSignalAtPed::CreateNextSubTask(CPed* ped) {
    // Como el original: cadena de comparaciones que vuelve a llamar a GetTaskType del subtask en cada una
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_TURN_TO_FACE_ENTITY) {
        // Como el original: pausa -1 = aleatoria: rand() & 0xFFFF * 2^-15 * 2000 (x87 a 24 bits) -> _ftol2
        auto pause = m_initialPause;
        if (pause == -1) {
            pause = static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * 3.0517578125e-05f * 2000.0f);
        }
        return new CTaskSimpleStandStill(pause, false, false, 8.0f);
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return new CTaskSimpleDoHandSignal; // Como el original: sin () para no poner a cero lo que el EXE no escribe
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_DO_HAND_SIGNAL && m_areAnimsReferenced && m_playAnimAtEnd) {
        // Como el original: grupo 0x34, animacion 0x117 - ftol(rand() * 2^-15 * -8)
        return new CTaskSimpleRunAnim(
            static_cast<AssocGroupId>(0x34),
            static_cast<AnimationId>(0x117 - static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * 3.0517578125e-05f * -8.0f)),
            4.0f,
            false
        );
    }
    return nullptr;
}

// 0x660CC0
CTask* CTaskComplexSignalAtPed::CreateFirstSubTask(CPed* ped) {
    if (!m_pedToSignalAt || !CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        return nullptr;
    }
    ped->StopPlayingHandSignal();
    g_ikChainMan.LookAt("TaskSignalAtPed", ped, m_pedToSignalAt, 5000, BONE_HEAD, nullptr, true, std::bit_cast<float>(0x3E19999Au), 500, 3, false);
    return new CTaskComplexTurnToFaceEntityOrCoord(m_pedToSignalAt, 0.5f, 0.2f);
}

// 0x660D80
CTask* CTaskComplexSignalAtPed::ControlSubTask(CPed* ped) {
    if (!m_areAnimsReferenced) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto blockIdx = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[blockIdx].IsLoaded != 1) {
                CStreaming::RequestModel(IFPToModelId(blockIdx), STREAMING_KEEP_IN_MEMORY);
            } else {
                CAnimManager::AddAnimBlockRef(blockIdx);
                m_areAnimsReferenced = true;
            }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_areAnimsReferenced = false;
    }

    if (m_pedToSignalAt) {
        // Como el original: GetRadianAngleBetweenPoints(dx, dy, 0, 0) con dx/dy redondeados a float, luego LimitRadianAngle
        const auto& pedPos    = ped->GetPosition();
        const auto& targetPos = m_pedToSignalAt->GetPosition();
        const float dx        = targetPos.x - pedPos.x;
        const float dy        = targetPos.y - pedPos.y;
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dx, dy, 0.0f, 0.0f));
    }
    return m_pSubTask;
}
