/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScriptCommand
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventScriptCommand.h"

#include "TaskSimpleRunAnim.h"

void CEventScriptCommand::InjectHooks()
{
    RH_ScopedVirtualClass(CEventScriptCommand, 0x85B378, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B0A00);
    RH_ScopedInstall(GetEventType, 0x4B0A30);
    RH_ScopedInstall(GetEventPriority, 0x4B0B20);
    RH_ScopedInstall(Clone, 0x4B6490);
    RH_ScopedInstall(AffectsPed, 0x4B0AF0);
    RH_ScopedInstall(TakesPriorityOver, 0x4B0BA0);
    RH_ScopedInstall(IsValid, 0x4B0AB0);
    RH_ScopedInstall(CloneScriptTask, 0x4B0AA0);
}

// 0x4B0A00
CEventScriptCommand::CEventScriptCommand(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds)
{
    m_primaryTaskIndex = primaryTaskIndex;
    m_task = task;
    m_affectsDeadPeds = affectsDeadPeds;
}

CEventScriptCommand::~CEventScriptCommand()
{
    delete m_task;
}

CEventScriptCommand* CEventScriptCommand::Constructor(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds)
{
    this->CEventScriptCommand::CEventScriptCommand(primaryTaskIndex, task, affectsDeadPeds);
    return this;
}

// 0x4B0B20
int32 CEventScriptCommand::GetEventPriority() const
{
    if (m_affectsDeadPeds)
        return 75;
    if (!m_task)
        return 53;
    const int32 taskId = m_task->GetTaskType();
    if (taskId == TASK_SIMPLE_NAMED_ANIM) {
        CTaskSimpleRunAnim* pTaskRunAnim = static_cast<CTaskSimpleRunAnim*>(m_task);
        if (pTaskRunAnim->m_nFlags & ANIMATION_IS_LOOPED)
            return 71;
    }
    if (taskId == TASK_SIMPLE_DIE || taskId == TASK_SIMPLE_SWIM || taskId == TASK_COMPLEX_USE_MOBILE_PHONE)
        return 71;
    if (taskId != TASK_COMPLEX_DIE)
        return 53;
    return 71;
}

// 0x4B0AF0
bool CEventScriptCommand::AffectsPed(CPed* ped)
{
    return ped->IsAlive() || m_affectsDeadPeds;
}

// 0x4B0BA0
bool CEventScriptCommand::TakesPriorityOver(const CEvent& refEvent)
{
    eEventType refEventType = refEvent.GetEventType();
    if (m_affectsDeadPeds && (refEventType == EVENT_DEATH || m_affectsDeadPeds && refEventType == EVENT_DAMAGE))
        return true;
    return GetEventPriority() >= refEvent.GetEventPriority();
}

// 0x4B0AB0
bool CEventScriptCommand::IsValid(CPed* ped)
{
    if (ped)
        return ped->IsAlive();
    if (m_bValid || m_nTimeActive <= GetLifeTime())
        return true;
    return false;
}

// 0x4B0AA0
CTask* CEventScriptCommand::CloneScriptTask() const
{
    if (m_task)
        return m_task->Clone();
    return nullptr;
}
