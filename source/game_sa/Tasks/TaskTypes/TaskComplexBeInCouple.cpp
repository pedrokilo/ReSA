/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexBeInCouple
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexBeInCouple.h"
#include "TaskComplexWanderStandard.h"
#include "TaskComplexWalkAlongsidePed.h"
#include "TaskComplexWaitForPed.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexBeInCouple::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeInCouple, 0x870724, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6836F0);
    RH_ScopedInstall(Destructor, 0x683780);

    RH_ScopedInstall(AbortArmIK, 0x6837F0);

    RH_ScopedVMTInstall(Clone, 0x6839C0);
    RH_ScopedVMTInstall(GetTaskType, 0x683770);
    RH_ScopedVMTInstall(MakeAbortable, 0x6847C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6837E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x684840);
    RH_ScopedVMTInstall(ControlSubTask, 0x684930);
}

// 0x6836F0
CTaskComplexBeInCouple::CTaskComplexBeInCouple(
    CPed* ped,
    bool  isLeader,
    bool  holdHands,
    bool  lookAtEachOther,
    float giveUpDist
) :
    CTaskComplex(),
    m_partner{ped},
    m_isLeader{isLeader},
    m_holdHands{holdHands},
    m_lookAtEachOther{lookAtEachOther},
    m_giveUpDist{giveUpDist}
{
    CEntity::SafeRegisterRef(m_partner);
}

// 0x683780
CTaskComplexBeInCouple::~CTaskComplexBeInCouple() {
    CEntity::SafeCleanUpRef(m_partner);
}

// 0x6837E0
CTask* CTaskComplexBeInCouple::CreateFirstSubTask(CPed* ped) {
    return CreateNextSubTask(ped);
}

// 0x6837F0
void CTaskComplexBeInCouple::AbortArmIK(CPed* ped) {
    const auto DoAbortArmIK = [ped](eIKArm arm) {
        if (IKChainManager_c::IsArmPointing(arm, ped)) {
            IKChainManager_c::AbortPointArm(arm, ped, 250);
        }
    };
    DoAbortArmIK(eIKArm::IK_ARM_RIGHT);
    DoAbortArmIK(eIKArm::IK_ARM_LEFT);
}

// 0x6847C0
bool CTaskComplexBeInCouple::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_partner && event && event->HasEditableResponse()) {
        auto editable = static_cast<CEventEditableResponse*>(const_cast<CEvent*>(event)); // Okay let's go!

        const bool bAddToEventGroup = editable->m_bAddToEventGroup;
        editable->m_bAddToEventGroup = false;
        // Como el original: GetEventType() se llama hasta tres veces (9, 31 y 15), una por comparacion
        if (event->GetEventType() == EVENT_DAMAGE
            || event->GetEventType() == EVENT_GUN_AIMED_AT
            || event->GetEventType() == EVENT_SHOT_FIRED) {
            m_partner->GetEventGroup().Add(editable, false);
        }
        editable->m_bAddToEventGroup = bAddToEventGroup;
    }
    AbortArmIK(ped);
    return true;
}

// 0x683840
int32 CTaskComplexBeInCouple::GetSideOfPartner(CPed* ped) const {
    // Como el original: el vector "derecha" de la matriz de la pareja (sin guarda de nulo) y los productos escalares
    // sumados como (y + z) + x; el segundo se le suma al primero negado
    const CVector right = m_partner->m_matrix->GetRight();
    const CVector& partnerPos = m_partner->GetPosition();
    const float partnerDot = -((right.y * partnerPos.y + right.z * partnerPos.z) + right.x * partnerPos.x);
    const CVector& pedPos = ped->GetPosition();
    const float pedDot = (right.y * pedPos.y + right.z * pedPos.z) + right.x * pedPos.x;
    return (pedDot + partnerDot) < 0.0f ? 2 : 1;
}

// 0x684840
CTask* CTaskComplexBeInCouple::CreateNextSubTask(CPed* ped) {
    if (!m_partner) {
        AbortArmIK(ped);
        return nullptr;
    }
    if (m_isLeader) {
        AbortArmIK(ped);
        return new CTaskComplexWanderStandard(PEDMOVE_WALK, static_cast<uint8>(CGeneral::GetRandomIntInRange(0, 8)), true);
    }
    AbortArmIK(ped);
    return new CTaskComplexWalkAlongsidePed(m_partner, m_giveUpDist);
}

// 0x684930
CTask* CTaskComplexBeInCouple::ControlSubTask(CPed* ped) {
    if (!m_partner || !m_partner->IsAlive()) {
        AbortArmIK(ped);
        return nullptr;
    }

    // Como el original: la pareja debe tener su propia tarea con el papel contrario (lider/seguidor) y apuntarnos a nosotros
    auto* const partnerTask = static_cast<CTaskComplexBeInCouple*>(m_partner->GetIntelligence()->FindTaskByType(TASK_COMPLEX_BE_IN_COUPLE));
    if (!partnerTask || (m_isLeader ? partnerTask->m_isLeader : !partnerTask->m_isLeader) || partnerTask->m_partner != ped) {
        AbortArmIK(ped);
        return nullptr;
    }

    auto* const ownActive     = ped->GetIntelligence()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_BE_IN_COUPLE);
    auto* const partnerActive = m_partner->GetIntelligence()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_BE_IN_COUPLE);
    const int32 side          = GetSideOfPartner(ped);

    if (!m_isLeader) {
        // El seguidor coloca el desplazamiento de su tarea de caminar al lado (m_Offset, +0x14) a 1.05 de un lado u otro
        if (auto* const walk = ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_WALK_ALONGSIDE_PED)) {
            auto* const offset = reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(walk) + 0x14);
            *offset = CVector{ side == 2 ? std::bit_cast<float>(0xBF866666u) : std::bit_cast<float>(0x3F866666u), 0.0f, 0.0f };
        }
    }

    const CVector& pedPos     = ped->GetPosition();
    const CVector& partnerPos = m_partner->GetPosition();
    const float    dy         = partnerPos.y - pedPos.y;
    const float    dx         = partnerPos.x - pedPos.x;
    const float    distSq     = dy * dy + dx * dx;
    const float    giveUpSq   = m_giveUpDist * m_giveUpDist;
    // Como el original: fcompp + test ah,41h; solo se rinde con distSq estrictamente mayor (NaN no cuenta)
    if (distSq > giveUpSq || !ownActive || !partnerActive) {
        AbortArmIK(ped);
        return nullptr;
    }

    if (distSq > 4.0f && m_isLeader) {
        AbortArmIK(ped);
        if (!ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_WAIT_FOR_PED)) {
            // El constructor de CTaskComplexWaitForPed (0x683340) aun no esta reconstruido: se llama al del EXE
            auto* const mem = CTask::operator new(sizeof(CTaskComplexWaitForPed));
            return plugin::CallMethodAndReturn<CTask*, 0x683340, CTask*, CPed*, float, uint32, bool>(static_cast<CTask*>(mem), m_partner, 0.75f, 20000u, false);
        }
    }

    if (m_holdHands) {
        const CVector offset{ dx * 0.5f, dy * 0.5f, 0.0f };
        CVector       target = pedPos + offset;
        if (distSq < 2.25f) {
            if (side != static_cast<int32>(static_cast<uint8>(m_prevSide))) { // movzx
                AbortArmIK(ped);
            }
            if (side == 2) {
                g_ikChainMan.PointArm("CTaskComplexBeInCouple", IK_ARM_RIGHT, ped, nullptr, BONE_UNKNOWN, &target, 0.5f, 250, 30.0f);
                m_prevSide = static_cast<int8>(side);
            } else if (side == 1) {
                g_ikChainMan.PointArm("CTaskComplexBeInCouple", IK_ARM_LEFT, ped, nullptr, BONE_UNKNOWN, &target, 0.5f, 250, 30.0f);
                m_prevSide = static_cast<int8>(side);
            }
        } else {
            AbortArmIK(ped);
        }
    }

    if (m_lookAtEachOther) {
        if (!g_ikChainMan.IsLooking(ped)) {
            if (CGeneral::GetRandomIntInRange(0, 100) > 80) {
                const auto time = CGeneral::GetRandomIntInRange(2000, 4000);
                g_ikChainMan.LookAt("TaskBeInCouple", ped, m_partner, time, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
            }
        }
        if (!g_ikChainMan.IsLooking(m_partner)) {
            if (CGeneral::GetRandomIntInRange(0, 100) > 80) {
                const auto time = CGeneral::GetRandomIntInRange(2000, 4000);
                g_ikChainMan.LookAt("TaskBeInCouple", m_partner, ped, time, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
            }
        }
    }

    return m_pSubTask;
}
