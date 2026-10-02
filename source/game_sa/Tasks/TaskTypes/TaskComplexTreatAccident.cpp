/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexTreatAccident
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

#include "TaskComplexTreatAccident.h"

#include "TaskSimpleGiveCPR.h"
#include "TaskSimpleNone.h"
#include "TaskSimpleAchieveHeading.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexTreatAccident::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexTreatAccident, 0x86F4EC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x658AB0);
    RH_ScopedInstall(CreateSubTask, 0x659E90);
    RH_ScopedInstall(ComputeHeading, 0x658AF0);
    RH_ScopedVMTInstall(Clone, 0x659A90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65A8F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A830);
    RH_ScopedVMTInstall(ControlSubTask, 0x658B90);
}

// 0x658AB0
CTaskComplexTreatAccident* CTaskComplexTreatAccident::Constructor(CAccident* pAcc)
{
    this->CTaskComplexTreatAccident::CTaskComplexTreatAccident(pAcc);
    return this;
}

// 0x658AB0
CTaskComplexTreatAccident::CTaskComplexTreatAccident(CAccident* accident)
{
    m_pAccident = accident;
}

// 0x65A830
CTask* CTaskComplexTreatAccident::CreateNextSubTask(CPed* ped)
{
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (subTaskId == TASK_SIMPLE_GIVE_CPR || subTaskId == TASK_NONE)
        return CreateSubTask(TASK_FINISHED, ped);

    if (subTaskId == TASK_SIMPLE_ACHIEVE_HEADING)
    {
        CPed* targetPed = m_pAccident->m_pPed;

        if (targetPed && !targetPed->bFadeOut)
        {
            targetPed->m_nDeathTimeMS = CTimer::GetTimeInMS();
            auto newSubTask = CreateSubTask(TASK_SIMPLE_GIVE_CPR, ped);
            ped->m_fCurrentRotation = ComputeHeading(ped);
            ped->m_fAimingRotation = ped->m_fCurrentRotation;
            return newSubTask;
        }
        else
            return CreateSubTask(TASK_FINISHED, ped);
    }

    return nullptr;
}

// 0x65A8F0
CTask* CTaskComplexTreatAccident::CreateFirstSubTask(CPed* ped)
{
    CPed* targetPed = m_pAccident->m_pPed;

    if (targetPed && !targetPed->bFadeOut)
    {
        targetPed->m_nDeathTimeMS = CTimer::GetTimeInMS();
        ped->Say(CTX_GLOBAL_VICTIM, 0, 1.0F, false, false, false);
        g_ikChainMan.LookAt("TaskTreatAccident", ped, targetPed, 5000, BONE_HEAD, nullptr, true, 0.25F, 500, 3, false);
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
    }
    else
        return CreateSubTask(TASK_NONE, ped);
}

// 0x658B90
CTask* CTaskComplexTreatAccident::ControlSubTask(CPed* ped)
{
    return m_pSubTask;
}

// 0x659A90
CTask* CTaskComplexTreatAccident::Clone() const {
    return new CTaskComplexTreatAccident(m_pAccident);
}

// 0x659E90
CTask* CTaskComplexTreatAccident::CreateSubTask(eTaskType taskType, CPed* ped)
{
    switch (taskType)
    {
    case TASK_SIMPLE_GIVE_CPR:
        return new CTaskSimpleGiveCPR(m_pAccident);
    case TASK_NONE:
        return new CTaskSimpleNone();
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(ComputeHeading(ped), 0.5F, 0.2F);
    default:
        return nullptr;
    }
}

// 0x658AF0
float CTaskComplexTreatAccident::ComputeHeading(CPed* ped) {
    // Head first, then pelvis (the bone queries may update the skeleton); only X/Y of the midpoint are used.
    const auto head   = m_pAccident->m_pPed->GetBonePosition(BONE_HEAD);
    const auto pelvis = m_pAccident->m_pPed->GetBonePosition(BONE_PELVIS);
    const auto& pos   = ped->GetPosition();
    return CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints((head.x + pelvis.x) * 0.5f, (head.y + pelvis.y) * 0.5f, pos.x, pos.y));
}
