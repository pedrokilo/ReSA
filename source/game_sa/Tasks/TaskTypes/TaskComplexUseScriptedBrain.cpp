/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseScriptedBrain
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
#include "TaskComplexUseScriptedBrain.h"
#include "ScriptedBrainTaskStore.h"
#include "TaskSimplePause.h"

void CTaskComplexUseScriptedBrain::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseScriptedBrain, 0x86e568, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6358B0);
    RH_ScopedInstall(Destructor, 0x635900);

    RH_ScopedVMTInstall(Clone, 0x6371F0);
    RH_ScopedVMTInstall(GetTaskType, 0x6358F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x635970);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6359C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x635A90);
    RH_ScopedVMTInstall(ControlSubTask, 0x635B60);
}

// 0x6358B0
CTaskComplexUseScriptedBrain::CTaskComplexUseScriptedBrain(char const* name) {
    // Como el original: strcpy sin límite (strcpy_s rellena el resto del búfer en debug)
    strcpy(m_name, name);
}

// Usado por `Clone` @ 0x6371F0: el original pasa el nombre de la tarea origen
CTaskComplexUseScriptedBrain::CTaskComplexUseScriptedBrain(const CTaskComplexUseScriptedBrain& o) :
    CTaskComplexUseScriptedBrain{ o.m_name }
{
}

// 0x635900
CTaskComplexUseScriptedBrain::~CTaskComplexUseScriptedBrain() {
    ClearCurrentTask();
}

// 0x635970
bool CTaskComplexUseScriptedBrain::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: comprueba que la tarea del comando no sea nula antes de pedir su tipo
    if (event && event->GetEventType() == EVENT_SCRIPT_COMMAND) {
        const auto* const task = static_cast<const CEventScriptCommand*>(event)->m_task;
        if (task && task->GetTaskType() == TASK_COMPLEX_USE_EFFECT) {
            m_aborting = true;
            return false;
        }
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x6359C0
CTask* CTaskComplexUseScriptedBrain::CreateNextSubTask(CPed* ped) {
    ClearCurrentTask();

    // Como el original: si la subtarea es TASK_SIMPLE_FINISH_BRAIN, la tarea termina
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_FINISH_BRAIN) {
        return nullptr;
    }

    return CreateFirstSubTask(ped);
}

// 0x635A90
CTask* CTaskComplexUseScriptedBrain::CreateFirstSubTask(CPed* ped) {
    if (m_currTask = CScriptedBrainTaskStore::GetTask(ped)) {
        return m_currTask->Clone();
    }
    // Como el original: sin tarea en el cerebro se espera con una pausa de 2000 ms
    return new CTaskSimplePause{ 2000 };
}

// 0x635B60
CTask* CTaskComplexUseScriptedBrain::ControlSubTask(CPed* ped) {
    const auto brainsTask = CScriptedBrainTaskStore::GetTask(ped);

    // Task is same, or can't abort it..
    // Como el original: prioridad URGENT (push 1)
    if (brainsTask == m_currTask || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }

    // Como el original: se guarda la nueva tarea actual antes de clonarla
    m_currTask = brainsTask;

    // Return task cloned (if any)
    return brainsTask
        ? brainsTask->Clone()
        : nullptr;
}

void CTaskComplexUseScriptedBrain::ClearCurrentTask() {
    if (m_currTask) {
        CScriptedBrainTaskStore::Clear(m_currTask);
        m_currTask = nullptr;
    }
}
