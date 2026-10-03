/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEvasiveDiveAndGetUp
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
#include "TaskComplexEvasiveDiveAndGetUp.h"
#include "TaskSimpleEvasiveDive.h"
#include "TaskSimplePause.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleAchieveHeading.h"

void CTaskComplexEvasiveDiveAndGetUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEvasiveDiveAndGetUp, 0x86f268, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6536B0);
    RH_ScopedInstall(Destructor, 0x653740);

    RH_ScopedInstall(CreateSubTask, 0x653820);
    RH_ScopedVMTInstall(Clone, 0x655940);
    RH_ScopedVMTInstall(GetTaskType, 0x653730);
    RH_ScopedVMTInstall(MakeAbortable, 0x6537A0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656000);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6560A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x653810);
}

// 0x6536B0
CTaskComplexEvasiveDiveAndGetUp::CTaskComplexEvasiveDiveAndGetUp(CVehicle* evadeVeh, int32 unconsciousTime, const CVector& diveDir, bool bAchieveHeadingFirst) :
    m_UnconsciousTime{unconsciousTime},
    m_bAchieveHeadingFirst{bAchieveHeadingFirst},
    m_DiveDir{diveDir},
    m_EvadeVeh{evadeVeh}
{
    // Como el original: registra la referencia al vehículo a esquivar
    CEntity::SafeRegisterRef(m_EvadeVeh);
}

// 0x653740
CTaskComplexEvasiveDiveAndGetUp::~CTaskComplexEvasiveDiveAndGetUp() {
    // Como el original: limpia la referencia al vehículo
    CEntity::SafeCleanUpRef(m_EvadeVeh);
}

// 0x655940
CTaskComplexEvasiveDiveAndGetUp::CTaskComplexEvasiveDiveAndGetUp(const CTaskComplexEvasiveDiveAndGetUp& o) :
    CTaskComplexEvasiveDiveAndGetUp{ o.m_EvadeVeh, o.m_UnconsciousTime, o.m_DiveDir, o.m_bAchieveHeadingFirst }
{
}

// 0x653820
CTask* CTaskComplexEvasiveDiveAndGetUp::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_EVASIVE_DIVE:
        return new CTaskSimpleEvasiveDive{ m_EvadeVeh };
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ m_UnconsciousTime };
    case TASK_SIMPLE_GET_UP:
        return new CTaskSimpleGetUp{};
    case TASK_SIMPLE_ACHIEVE_HEADING:
        // Como el original: GetRadianAngleBetweenPoints(x, y, 0, 0) y tolerancia leída de 0x86FC80
        return new CTaskSimpleAchieveHeading{ CGeneral::GetRadianAngleBetweenPoints(m_DiveDir.x, m_DiveDir.y, 0.f, 0.f), 2.f, 0.2f };
    default: // Como el original: TASK_FINISHED y tipos desconocidos devuelven nullptr
        return nullptr;
    }
}

// 0x6537A0
bool CTaskComplexEvasiveDiveAndGetUp::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    const auto subTaskAborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (subTaskAborted && priority == ABORT_PRIORITY_IMMEDIATE) {
        if (const auto anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_EV_DIVE)) {
            anim->SetBlendDelta(-1000.f);
        }
    }
    return subTaskAborted;
}

// 0x656000
CTask* CTaskComplexEvasiveDiveAndGetUp::CreateNextSubTask(CPed* ped) {
    switch (const auto subtt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_EVASIVE_DIVE);
    case TASK_SIMPLE_EVASIVE_DIVE:
        return CreateSubTask(TASK_SIMPLE_PAUSE);
    case TASK_SIMPLE_PAUSE: {
        // Como el original: LimitRadianAngle(actual) - HALF_PI (double exacto en 0x86F418); escribe 0x558 y después 0x55C
        const float rot = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation) - std::bit_cast<float>(0x3FC90FDBu);
        ped->m_fCurrentRotation = rot;
        ped->m_fAimingRotation = rot;
        return CreateSubTask(TASK_SIMPLE_GET_UP);
    }
    case TASK_SIMPLE_GET_UP:
        return CreateSubTask(TASK_FINISHED);
    default: // Como el original: subtareas desconocidas devuelven nullptr
        return nullptr;
    }
}

// 0x6560A0
CTask* CTaskComplexEvasiveDiveAndGetUp::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(m_bAchieveHeadingFirst ? TASK_SIMPLE_ACHIEVE_HEADING : TASK_SIMPLE_EVASIVE_DIVE);
}
