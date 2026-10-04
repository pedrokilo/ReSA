/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskManager
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 18/18 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "TaskManager.h"

#include "TaskComplexFacial.h"

void CTaskManager::InjectHooks() {
    RH_ScopedClass(CTaskManager);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x6816A0);
    RH_ScopedInstall(Destructor, 0x6816D0);
    RH_ScopedInstall(GetActiveTask, 0x681720);
    RH_ScopedInstall(FindActiveTaskByType, 0x681740);
    RH_ScopedInstall(FindTaskByType, 0x6817D0);
    RH_ScopedInstall(GetTaskSecondary, 0x681810);
    RH_ScopedInstall(HasTaskSecondary, 0x681820);
    RH_ScopedInstall(Flush, 0x681850);
    RH_ScopedInstall(FlushImmediately, 0x6818A0);
    RH_ScopedInstall(SetNextSubTask, 0x681920);
    //RH_ScopedOverloadedInstall(GetLastTaskOf, "of-task", 0x681970, CTask*(*)(CTask*));
    //RH_ScopedOverloadedInstall(GetLastTaskOf, "at-index", 0x681A00, CTask*(CTaskManager::*)(ePrimaryTasks));
    RH_ScopedInstall(StopTimers, 0x6819A0);
    RH_ScopedInstall(GetSimplestActiveTask, 0x6819D0);
    RH_ScopedInstall(AddSubTasks, 0x681A30);
    RH_ScopedInstall(ParentsControlChildren, 0x681A80);
    RH_ScopedInstall(SetTask, 0x681AF0);
    RH_ScopedInstall(SetTaskSecondary, 0x681B60);
    RH_ScopedInstall(ClearTaskEventResponse, 0x681BD0);
    RH_ScopedInstall(ManageTasks, 0x681C10);
}

static void DeleteTaskAndNull(CTask*& task) {
    delete task;
    task = nullptr;
}

/// Find the first task (or subtask) that is of the required type
inline CTask* FindFirstTaskOfType(CTask* task, eTaskType type) {
    // Como el original: tras encontrarla aún llama a GetSubTask de la tarea encontrada antes de salir del bucle
    CTask* found = nullptr;
    for (; task && !found; task = task->GetSubTask()) {
        if (task->GetTaskType() == type) {
            found = task;
        }
    }
    return found;
}

// 0x6816A0
CTaskManager::CTaskManager(CPed* ped) :
    m_pPed{ped}
{
}

// 0x6816D0
CTaskManager::~CTaskManager() {
    Flush();
}

// 0x681720
CTask* CTaskManager::GetActiveTask() {
    for (const auto task : m_aPrimaryTasks) {
        if (task) {
            return task;
        }
    }
    return nullptr;
}

size_t CTaskManager::GetActiveTaskIndex() const {
    for (auto&& [i, task] : rngv::enumerate(m_aPrimaryTasks)) {
        if (task) {
            return (size_t)i;
        }
    }
    return (size_t)-1;
}

// 0x681740
CTask* CTaskManager::FindActiveTaskByType(eTaskType taskType) {
    // First try current active task and it's sub-tasks
    if (const auto task = FindFirstTaskOfType(GetActiveTask(), taskType)) {
        return task;
    }

    // Now try secondaries and their sub-tasks
    for (const auto sec : m_aSecondaryTasks) {
        if (const auto task = FindFirstTaskOfType(sec, taskType)) { // NOTE: Original code doesn't break first match, but that's by a bug, not intentional.
            return task;
        }
    }

    // Not found
    return nullptr;
}

// 0x6817D0
CTask* CTaskManager::FindTaskByType(ePrimaryTasks taskIndex, eTaskType taskId) const {
    return FindFirstTaskOfType(GetTaskPrimary(taskIndex), taskId);
}

// 0x681810
CTask* CTaskManager::GetTaskSecondary(eSecondaryTask taskIndex) {
    if (notsa::IsFixBugs() && taskIndex == eSecondaryTask::TASK_SECONDARY_INVALID) {
        return nullptr;
    }
    return m_aSecondaryTasks[taskIndex];
}

CTaskComplexFacial* CTaskManager::GetTaskSecondaryFacial() {
    return notsa::cast<CTaskComplexFacial>(GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
}

// NOTSA?
bool CTaskManager::HasTaskPrimary(const CTask* task) {
    for (auto& primaryTask : m_aPrimaryTasks) {
        if (primaryTask && primaryTask == task) {
            return true;
        }
    }
    return false;
}

// 0x681820
bool CTaskManager::HasTaskSecondary(const CTask* task) {
    // Como el original: compara sin comprobar nulos (con `task` nulo devuelve true si algún hueco secundario está vacío)
    for (auto& secondaryTask : m_aSecondaryTasks) {
        if (secondaryTask == task) {
            return true;
        }
    }
    return false;
}

// 0x681850
void CTaskManager::Flush() {
    ApplyToRootTasks(DeleteTaskAndNull);
}

// 0x6818A0
void CTaskManager::FlushImmediately() {
    ApplyToRootTasks([this](CTask*& task) {
        if (task && task->MakeAbortable(m_pPed, ABORT_PRIORITY_IMMEDIATE, nullptr)) {
            DeleteTaskAndNull(task); // Como el original: borra y después pone el hueco a nulo
        }
    });
}

// 0x681920
void CTaskManager::SetNextSubTask(CTaskComplex* ofTask) {
    if (!ofTask) {
        return;
    }

    assert(ofTask->IsComplex());

    for (auto task = ofTask; task; task = task->GetParent()) {
        if (const auto next = task->CreateNextSubTask(m_pPed)) {
            task->SetSubTask(next);
            AddSubTasks(next);
            return;
        }

        // If no subtask is created, the `task` is considered done.
        // So we go to it's parent and create it's next subtask.
        task->SetSubTask(nullptr); // This also deletes the previous sub-task
    }
}

// 0x681970
CTask* CTaskManager::GetLastTaskOf(CTask* task) {
    auto last{task};
    for (; task; task = task->GetSubTask())
        last = task;
    return last;
}

// 0x6819A0
void CTaskManager::StopTimers(const CEvent* event) {
    for (auto& primaryTask : m_aPrimaryTasks) {
        if (primaryTask) {
            primaryTask->StopTimer(event);
        }
    }
}

// 0x681A30
void CTaskManager::AddSubTasks(CTask* toTask) {
    if (!toTask) {
        return;
    }

    for (auto task = toTask; !task->IsSimple();) {
        const auto ctask = task->AsComplex();
        assert(CTask::IsTaskPtr(task)); // Sometimes tasks seem to get deleted which causes pure virtual calls (because of destructcors setting the VMT to their's)
        if (const auto sub = ctask->CreateFirstSubTask(m_pPed)) {
            ctask->SetSubTask(sub);
            task = sub; // Go on creating the subtask of the created task
        } else { // No sub task created, so we can stop here
            if (const auto parent = ctask->GetParent()) {
                SetNextSubTask(parent);
            }
            break;
        }
    }
}

// 0x681A80
void CTaskManager::ParentsControlChildren(CTask* parent) {
    for (auto task = parent; task && !task->IsSimple();) {
        const auto ctask      = task->AsComplex();
        const auto oldSubTask = ctask->GetSubTask();

        const auto newSubTask = ctask->ControlSubTask(m_pPed);

        if (newSubTask != oldSubTask) {
            oldSubTask->MakeAbortable(m_pPed);
            ctask->SetSubTask(newSubTask);
            AddSubTasks(newSubTask);
            return;
        }

        task = task->GetSubTask();
    }
}

void CTaskManager::AbortFirstPrimaryTaskIn(std::initializer_list<ePrimaryTasks> slots, CPed* ped, eAbortPriority priority, const CEvent* event) {
    for (const auto slot : slots) {
        if (const auto task = GetTaskPrimary(slot)) {
            task->MakeAbortable(ped, priority, event);
            break;
        }
    }
}

// 0x681BD0
void CTaskManager::ClearTaskEventResponse() {
    // Looking at the code, I think `nontemp` is only present if `temp` is, as it's not deleted otherwise.
    for (const auto idx : { TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
        if (auto& task = m_aPrimaryTasks[idx]) {
            DeleteTaskAndNull(task);
            break;
        }
    }
}

// 0x681C10
void CTaskManager::ManageTasks() {
    // Árbol de la tarea primaria activa (la primera no nula)
    if (const auto activeTaskIdx = GetActiveTaskIndex(); activeTaskIdx != (size_t)-1) {
        auto*& primary = m_aPrimaryTasks[activeTaskIdx];

        // Un árbol tiene que acabar en una tarea simple; si no, se borra y no se procesa nada más (ni las secundarias)
        if (!GetLastTaskOf(primary)->IsSimple()) {
            DeleteTaskAndNull(primary);
            return;
        }

        // Como el original: el contador se incrementa antes de compararlo con 10, así que hace hasta 11 pasadas
        for (int32 pass = 1;; pass++) {
            const bool lastPass = pass > 10;

            ParentsControlChildren(primary);

            if (const auto last = GetLastTaskOf(primary); !last->IsSimple()) {
                // Ha fallado la creación de subtareas: se intenta la siguiente subtarea del padre
                SetNextSubTask(last->GetParent());
                if (!GetLastTaskOf(primary)->IsSimple()) {
                    DeleteTaskAndNull(primary); // La tarea primaria ha terminado
                    break;
                }
            }

            // Como el original: vuelve a buscar la última tarea del árbol
            const auto last = GetLastTaskOf(primary);
            if (!last->AsSimple()->ProcessPed(m_pPed)) {
                break; // La tarea simple sigue activa
            }

            // La tarea simple ha terminado: se crea la siguiente subtarea
            SetNextSubTask(last->GetParent());
            if (!primary->GetSubTask()) {
                DeleteTaskAndNull(primary); // El árbol ha terminado
                break;
            }

            if (lastPass) {
                break;
            }
        }
    }

    // Árboles de las tareas secundarias (sin límite de pasadas)
    for (auto& secondaryTask : m_aSecondaryTasks) {
        const auto root = secondaryTask;
        if (!root) {
            continue;
        }
        for (;;) {
            ParentsControlChildren(root);

            // Como el original: en las secundarias un árbol que no acaba en tarea simple se da por terminado
            if (!GetLastTaskOf(root)->IsSimple()) {
                delete root;
                secondaryTask = nullptr;
                break;
            }

            const auto last = GetLastTaskOf(root);
            if (!last->AsSimple()->ProcessPed(m_pPed)) {
                break;
            }

            SetNextSubTask(last->GetParent());
            if (!root->GetSubTask()) {
                delete root;
                secondaryTask = nullptr;
                break;
            }
        }
    }
}

// @notsa
CTask* CTaskManager::GetPresistentEventResponseTask() const {
    for (const auto i : { TASK_PRIMARY_PHYSICAL_RESPONSE, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
        if (const auto t = GetTaskPrimary(i)) {
            return t; 
        }
    }
    return nullptr;
}

CTask* CTaskManager::GetTemporaryEventResponseTask() const {
    return  GetTaskPrimary(TASK_PRIMARY_EVENT_RESPONSE_TEMP);
}

void CTaskManager::ChangeTaskInSlot(CTask*& taskInSlot, CTask* changeTo) {
    // Nothing to do in case they're the same
    if (taskInSlot == changeTo) {
        return;
    }

    // Delete current
    delete taskInSlot;

    // Set the task in the slot, and add it's subtasks
    taskInSlot = changeTo;
    AddSubTasks(taskInSlot);

    // Task trees must always end with a simple task, so if that's not the case delete it immidiately.
    if (taskInSlot && !GetLastTaskOf(taskInSlot)->IsSimple()) {
        DeleteTaskAndNull(taskInSlot); // Como el original: borra y después pone el hueco a nulo
    }
}
