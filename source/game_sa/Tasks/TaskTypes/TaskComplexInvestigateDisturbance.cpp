/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexInvestigateDisturbance
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

#include "TaskComplexInvestigateDisturbance.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleRunAnim.h"
#include "SeekEntity/TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h"
#include "TaskComplexGoToPointAndStandStill.h"

#include <Ragdoll/IKChainManager.h>

void CTaskComplexInvestigateDisturbance::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexInvestigateDisturbance, 0x870010, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66C720);
    RH_ScopedInstall(Destructor, 0x66C7A0);

    RH_ScopedVMTInstall(Clone, 0x66D3D0);
    RH_ScopedVMTInstall(GetTaskType, 0x66C790);
    RH_ScopedVMTInstall(MakeAbortable, 0x66C800);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6749F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66C820);
    RH_ScopedVMTInstall(ControlSubTask, 0x66CA30);
}

// 0x66C720
CTaskComplexInvestigateDisturbance::CTaskComplexInvestigateDisturbance(CVector const& targetPos, CEntity* srcEntity) :
    m_entity{srcEntity},
    m_pos{targetPos}
{
    CEntity::SafeRegisterRef(m_entity);
}

// NOTSA (For 0x66D3D0)
CTaskComplexInvestigateDisturbance::CTaskComplexInvestigateDisturbance(const CTaskComplexInvestigateDisturbance& o) :
    CTaskComplexInvestigateDisturbance{ o.m_pos, o.m_entity }
{
}

// 0x66C7A0
CTaskComplexInvestigateDisturbance::~CTaskComplexInvestigateDisturbance() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x6749F0
CTask* CTaskComplexInvestigateDisturbance::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType())
    {
    case TASK_SIMPLE_ANIM: { // 0x674a23
        ped->Say(CTX_GLOBAL_STEALTH_NOTHING_THERE);
        return new CTaskSimpleScratchHead{};
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
    case TASK_COMPLEX_SEEK_ENTITY: // 0x674a8a
        return new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS);
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: { // 0x674A9A
        if (m_entity) {
            const auto task = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>{
                m_entity,
                50'000,
                1000,
                0.5f,
                2.f,
                2.f,
                true,
                true,
                CEntitySeekPosCalculatorFixedPos{ m_pos }
            };
            // Como el original (0x66CA40): el calculador incrustado (+0x40) lleva la vtable del EXE (0x86FD18)
            *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8*>(task) + 0x40) = 0x86FD18;
            return task;
        } else {
            // Como el original: radio 0.5 (0x86FC84) y radio de estado 2.0 (0x86FC88)
            return new CTaskComplexGoToPointAndStandStill{
                PEDMOVE_WALK,
                m_pos,
                0.5f,
                2.f
            };
        }
    }
    default:
        return nullptr;
    }
}

// 0x66C820
CTask* CTaskComplexInvestigateDisturbance::CreateFirstSubTask(CPed* ped) {
    // Como el original: rand() * (1/32767) < 0.2 elige la frase genérica; el tipo del evento se consulta en cada comparación
    const auto SayAlert = [ped](eGlobalSpeechContext other) {
        const float r = static_cast<float>(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u);
        ped->Say(r < 0.2f ? CTX_GLOBAL_STEALTH_ALERT_GENERIC : other);
    };
    if (const auto event = ped->GetIntelligence()->GetEventHandler().GetHistory().GetCurrentEvent()) {
        if (event->GetEventType() == EVENT_SOUND_QUIET) {
            SayAlert(CTX_GLOBAL_STEALTH_ALERT_SOUND);
        } else if (event->GetEventType() == EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT) {
            SayAlert(CTX_GLOBAL_STEALTH_ALERT_SIGHT);
        }
    }

    // Como el original: la dirección al punto se normaliza (0x59C910), el producto se suma como (z·z' + y·y') + x·x',
    // el umbral es 0.707 (0x863C1C) y un NaN también agacha (fcomp + test ah,5)
    CVector dir = m_pos - ped->GetPosition();
    dir.Normalise();
    const auto& fwd = ped->GetForward();
    const float dot = (dir.z * fwd.z + dir.y * fwd.y) + dir.x * fwd.x;
    if (!(dot < 0.707f)) {
        ped->GetIntelligence()->SetTaskDuckSecondary(5000);
    } else {
        g_ikChainMan.LookAt("TaskInvDisturb", ped, nullptr, 5000, BONE_UNKNOWN, &m_pos, false, 0.25f, 500, 3, false);
    }

    return m_entity
        ? new CTaskComplexTurnToFaceEntityOrCoord{ m_entity, 3.f }
        : new CTaskComplexTurnToFaceEntityOrCoord{ m_pos, 3.f };
}
