/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskGangHasslePed
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleRunAnim.h"
#include "TaskComplexPlayHandSignalAnim.h"

#include "TaskGangHasslePed.h"

void CTaskGangHasslePed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGangHasslePed, 0x86FA00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65FED0);
    RH_ScopedInstall(Destructor, 0x65FF60);

    RH_ScopedVMTInstall(CreateNextSubTask, 0x6642C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664380);
    RH_ScopedVMTInstall(ControlSubTask, 0x65FFE0);
}

// 0x65FED0
CTaskGangHasslePed::CTaskGangHasslePed(CPed* ped, int32 a3, int32 a4, int32 a5) {
    m_nTime = 0;
    m_nSomeRandomShit = 0;
    m_bFirstSubTaskInitialised = 0;
    m_bRefreshTime = 0;
    dword10 = a3;
    m_RndMin = a4;
    m_Ped = ped;
    m_RndMax = a5;
    m_bAnimBlockRefAdded = false;
    CEntity::SafeRegisterRef(m_Ped);
}

// 0x65FF60
CTaskGangHasslePed::~CTaskGangHasslePed() {
    CEntity::SafeCleanUpRef(m_Ped);

    if (m_bAnimBlockRefAdded) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bAnimBlockRefAdded = false;
    }
}

// Como el original: rand / 32767, duplicacion y suma se redondean en cada operacion x87.
static float GangHassleSeekRadius() {
    const float fraction = float(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL;
    const float twice = fraction + fraction;
    return twice + 3.f;
}

// 0x6642C0
CTask* CTaskGangHasslePed::CreateNextSubTask(CPed*) {
    const float radius = GangHassleSeekRadius(); // El EXE consume rand incluso cuando termina.
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_KILL_PED_ON_FOOT) {
        return nullptr;
    }
    return new CTaskComplexSeekEntityStandard{m_Ped, 999999, 1000, radius, 2.f, 2.f, false, true};
}

// 0x664380
CTask* CTaskGangHasslePed::CreateFirstSubTask(CPed*) {
    if (!m_Ped) {
        return nullptr;
    }
    const float radius = GangHassleSeekRadius();
    m_nTime = CTimer::GetTimeInMS();
    m_nSomeRandomShit = CGeneral::GetRandomIntInRange(m_RndMin, m_RndMax);
    m_bFirstSubTaskInitialised = 1;
    return new CTaskComplexSeekEntityStandard{m_Ped, 999999, 1000, radius, 2.f, 2.f, false, true};
}

// 0x65FFE0
CTask* CTaskGangHasslePed::ControlSubTask(CPed* ped) {
    if (!m_Ped) {
        return nullptr;
    }
    if (m_bFirstSubTaskInitialised) {
        if (m_bRefreshTime) {
            m_nTime = CTimer::GetTimeInMS();
            m_bRefreshTime = false;
        }
        // Como el original: comparacion sin signo de la suma modular del tiempo.
        if (CTimer::GetTimeInMS() >= uint32(m_nTime) + uint32(m_nSomeRandomShit)) {
            if (dword10 == 2) {
                if (m_pSubTask->GetTaskType() != TASK_COMPLEX_KILL_PED_ON_FOOT) {
                    return new CTaskComplexKillPedOnFoot{m_Ped, -1, 0, 0, 0, true};
                }
            } else if (dword10 != 1) {
                return nullptr;
            }
        }
    }
    if (!m_bAnimBlockRefAdded) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto block = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[block].IsLoaded) {
                CAnimManager::AddAnimBlockRef(block);
                m_bAnimBlockRefAdded = true;
            } else {
                CStreaming::RequestModel(IFPToModelId(block), STREAMING_KEEP_IN_MEMORY);
            }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bAnimBlockRefAdded = false;
    }
    if (m_bAnimBlockRefAdded && !ped->IsPlayingHandSignal() && !ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
        const auto choice = CGeneral::GetRandomIntInRange(0, 200);
        CTask* animation{};
        if (choice > 50 && choice < 56) {
            animation = new CTaskSimpleRunAnim{ANIM_GROUP_GANGS, AnimationId(279 + CGeneral::GetRandomIntInRange(0, 8)), 4.f, false};
        } else if (choice == 100) {
            animation = new CTaskComplexPlayHandSignalAnim{ANIM_ID_UNDEFINED, 4.f};
        }
        if (animation) {
            ped->GetTaskManager().SetTaskSecondary(animation, TASK_SECONDARY_PARTIAL_ANIM);
        }
    }
    if (dword10 == 0) {
        ped->Say(CTX_GLOBAL_EYEING_PED, 0, 1.f, false, false, false);
    } else if (dword10 == 1 || dword10 == 2) {
        CTaskComplexGangLeader::DoGangAbuseSpeech(ped, m_Ped);
    }
    return m_pSubTask;
}
