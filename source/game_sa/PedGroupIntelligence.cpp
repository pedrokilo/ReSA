/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupIntelligence
 * Reconstrucción: COMPLETA — 26/26 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 26/26 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedGroupIntelligence.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"
#include <reversiblebugfixes/Bugs.hpp>

void CPedGroupIntelligence::InjectHooks() {
    RH_ScopedClass(CPedGroupIntelligence);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5F7250);

    RH_ScopedInstall(FlushTasks, 0x5F79C0); // Como el original: __thiscall con 2 argumentos (ret 8); como cdecl descuadraba la pila
    RH_ScopedOverloadedInstall(ReportFinishedTask, "Wrapper", 0x5F86F0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*));
    RH_ScopedOverloadedInstall(ReportFinishedTask, "Impl", 0x5F76C0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*, PedTaskPairs& taskPairs));
    RH_ScopedInstall(SetTask, 0x5F7540);
    RH_ScopedInstall(Flush, 0x5F7350);
    RH_ScopedOverloadedInstall(AddEvent, "", 0x5F7470, bool(CPedGroupIntelligence::*)(CEvent*));
    RH_ScopedInstall(SetDefaultTaskAllocatorType, 0x5FBB70);
    RH_ScopedInstall(SetDefaultTaskAllocator, 0x5FB280);
    RH_ScopedInstall(ComputeDefaultTasks, 0x5F88D0);
    RH_ScopedInstall(ProcessIgnorePlayerGroup, 0x5F87A0);
    RH_ScopedInstall(ReportAllBarScriptTasksFinished, 0x5F8780);
    RH_ScopedInstall(GetTaskDefault, 0x5F86C0);
    RH_ScopedInstall(GetTaskScriptCommand, 0x5F8690);
    RH_ScopedInstall(GetTaskSecondarySlot, 0x5F8650);
    RH_ScopedInstall(GetTaskSecondary, 0x5F8620);
    RH_ScopedInstall(GetTaskMain, 0x5F85A0);
    RH_ScopedInstall(SetScriptCommandTask, 0x5F8560);
    RH_ScopedInstall(IsCurrentEventValid, 0x5F77A0);
    RH_ScopedInstall(IsGroupResponding, 0x5F7760);
    //RH_ScopedInstall(SetEventResponseTask, 0x5F8510); // Register allocation is weird, better not to hook it at all
    RH_ScopedInstall(SetEventResponseTaskAllocator, 0x5F7440);
    RH_ScopedInstall(SetPrimaryTaskAllocator, 0x5F7410);
    RH_ScopedInstall(SetGroupDecisionMakerType, 0x5F7340);
    RH_ScopedInstall(ComputeEventResponseTasks, 0x5FC440);
    RH_ScopedInstall(Process, 0x5FC4A0);
    RH_ScopedInstall(ReportAllTasksFinished, 0x5F7730);
}

// 0x5F7250
CPedGroupIntelligence::CPedGroupIntelligence(CPedGroup* owner) : // TODO: Use `CPedGroup&`
    m_pPedGroup{owner},
    m_DefaultTaskAllocator{&CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::STAND_STILL)}
{
    /* Everything else is initialized in the header */
}

// 0x5F7250
// Como el original: no escribe m_pPedGroup, m_CurrentEvent ni m_HighestPriorityEvent (los pone el llamador)
// ni el relleno de los pares; solo los campos de los pares y los del final.
CPedGroupIntelligence* CPedGroupIntelligence::Constructor() {
    for (auto* const tps : { &m_PedTaskPairs, &m_SecondaryPedTaskPairs, &m_ScriptCommandPedTaskPairs, &m_DefaultPedTaskPairs }) {
        for (auto& tp : *tps) {
            tp.Ped      = nullptr;
            tp.Task     = nullptr;
            tp.Slot     = TASK_SECONDARY_INVALID;
            tp.MatchID  = 0;
            tp.UsedTask = false;
        }
    }
    m_DecisionMakerType          = eDecisionMakerType::UNKNOWN;
    m_TaskSeqId                  = -1;
    m_DefaultTaskAllocator       = &CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::STAND_STILL);
    m_PrimaryTaskAllocator       = nullptr;
    m_EventResponseTaskAllocator = nullptr;
    return this;
}

// Seemingly always inlined as `Flush()`
CPedGroupIntelligence::~CPedGroupIntelligence() {
    Flush();
}

// 0x5F7350
// NOTE(Pirulax): For some reason this is called `~CPedGroupIntelligence` in *there*, but it's actually `Flush`!
void CPedGroupIntelligence::Flush() {
    // Como el original: un solo bucle por miembro (principal, secundario, por defecto y de script)
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        m_PedTaskPairs[i].Flush();
        m_SecondaryPedTaskPairs[i].Flush();
        m_DefaultPedTaskPairs[i].Flush();
        m_ScriptCommandPedTaskPairs[i].Flush();
    }

    // Como el original: borra primero el evento actual y después el de mayor prioridad (si es otro)
    if (m_HighestPriorityEvent == m_CurrentEvent) {
        delete m_HighestPriorityEvent;
        m_CurrentEvent = nullptr;
    } else {
        delete m_CurrentEvent;
        m_CurrentEvent = nullptr;
        delete m_HighestPriorityEvent;
    }
    m_HighestPriorityEvent = nullptr;

    delete m_PrimaryTaskAllocator;
    m_PrimaryTaskAllocator = nullptr;
    delete m_EventResponseTaskAllocator;
    m_EventResponseTaskAllocator = nullptr;

    m_DecisionMakerType = eDecisionMakerType::UNKNOWN;
    m_TaskSeqId         = TASK_INVALID;
}

// 0x5F7470
bool CPedGroupIntelligence::AddEvent(CEvent* event) {
    // Como el original: una sola consulta del tipo por la vtable (sin los casts con aserciones)
    if (event->GetEventType() != EVENT_GROUP_EVENT) {
        return false;
    }
    const auto eGrpEvent = static_cast<CEventGroupEvent*>(event);
    if (!eGrpEvent->AffectsPedGroup(m_pPedGroup) || !eGrpEvent->GetEvent().AffectsPedGroup(m_pPedGroup)) {
        return false;
    }
    if (const auto src = eGrpEvent->GetEvent().GetSourceEntity()) {
        if (src->GetIsTypePed() && m_pPedGroup->GetMembership().IsMember(src->AsPed())) {
            return false;
        }
    }
    if (eGrpEvent->GetEvent().HasEditableResponse()) {
        const auto eEditableResponse = static_cast<CEventEditableResponse*>(&eGrpEvent->GetEvent());
        eEditableResponse->ComputeResponseTaskType(m_pPedGroup);
        if (!eEditableResponse->WillRespond()) {
            return false;
        }
    }
    if (!m_HighestPriorityEvent) {
        m_HighestPriorityEvent = static_cast<CEventGroupEvent*>(eGrpEvent->Clone());
        return true;
    }
    // Como el original: si no tiene más prioridad que el pendiente devuelve false
    if (!eGrpEvent->BaseEventTakesPriorityOverBaseEvent(*m_HighestPriorityEvent)) {
        return false;
    }
    // Como el original: borra el pendiente antes de clonar el nuevo
    delete m_HighestPriorityEvent;
    m_HighestPriorityEvent = static_cast<CEventGroupEvent*>(eGrpEvent->Clone());
    return true;
}

// notsa
CPedTaskPair* CPedGroupIntelligence::GetPedsTaskPair(CPed* ped, PedTaskPairs& taskPairs) const {
    for (auto& tp : taskPairs) {
        if (tp.Ped == ped) {
            return &tp;
        }
    }
    return nullptr;
}

// 0x5F85A0
CTask* CPedGroupIntelligence::GetTaskMain(CPed* ped) {
    CTask* t{};
    (t = GetTaskScriptCommand(ped)) || (t = GetTask(ped, m_PedTaskPairs)) || (t = GetTaskDefault(ped));
    return t;
}

// 0x5F7660
CTask* CPedGroupIntelligence::GetTask(CPed* ped, PedTaskPairs& taskPairs) const {
    if (const auto tp = GetPedsTaskPair(ped, taskPairs)) {
        return tp->Task;
    }
    return nullptr;
}

CTask* CPedGroupIntelligence::GetTaskDefault(CPed* ped) {
    return GetTask(ped, m_DefaultPedTaskPairs);
}

// 0x5F8690
CTask* CPedGroupIntelligence::GetTaskScriptCommand(CPed* ped) {
    return GetTask(ped, m_ScriptCommandPedTaskPairs);
}

// 0x5F8620
CTask* CPedGroupIntelligence::GetTaskSecondary(CPed* ped) {
    return GetTask(ped, m_SecondaryPedTaskPairs);
}

// 0x5F8650
eSecondaryTask CPedGroupIntelligence::GetTaskSecondarySlot(CPed* ped) {
    if (const auto tp = GetPedsTaskPair(ped, m_SecondaryPedTaskPairs)) {
        return tp->Slot;
    }
    return TASK_SECONDARY_INVALID; // Como el original: -1 si el peatón no tiene par
}

// 0x5FC4A0
void CPedGroupIntelligence::Process() {
    ProcessIgnorePlayerGroup();

    auto& membership = m_pPedGroup->GetMembership();
    if (m_CurrentEvent && !m_PrimaryTaskAllocator) {
        if (!IsGroupResponding() || !IsCurrentEventValid()) {
            delete m_CurrentEvent;
            m_CurrentEvent = nullptr;
            delete m_EventResponseTaskAllocator;
            m_EventResponseTaskAllocator = nullptr;
        }
    }

    // Como el original: recorre los 8 huecos de miembros por índice, también los nulos
    bool hasMemberListChanged{};
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        const auto mem = membership.GetMember(i);
        if (m_PedTaskPairs[i].Ped == mem) {
            continue;
        }
        m_PedTaskPairs[i].Ped          = mem;
        m_SecondaryPedTaskPairs[i].Ped = mem;
        m_DefaultPedTaskPairs[i].Ped   = mem;
        hasMemberListChanged           = true;
    }
    if (hasMemberListChanged) {
        if (m_PrimaryTaskAllocator) { // 0x5FC616
            m_PrimaryTaskAllocator->AllocateTasks(this);
        } else { // 0x5FC620
            ComputeDefaultTasks(nullptr);
            ComputeScriptCommandTasks();
            if (m_CurrentEvent) { // 0x5FC659
                if (m_EventResponseTaskAllocator) { // 0x5FC65E
                    FlushTasks(m_PedTaskPairs, nullptr);
                    m_EventResponseTaskAllocator->AllocateTasks(this);
                } else {
                    m_EventResponseTaskAllocator = ComputeEventResponseTasks();
                }
            }
        }
    }

    CEvent* eventToAdd{};
    if (m_HighestPriorityEvent) { // 0x5FC6A2
        bool setAsCurrent = !m_CurrentEvent;
        if (!setAsCurrent) {
            // Como el original: con el mismo tipo mira si se puede interrumpir el de mayor prioridad (no el actual)
            const auto highType = m_HighestPriorityEvent->GetEvent().GetEventType();
            const auto curType  = m_CurrentEvent->GetEvent().GetEventType();
            setAsCurrent = (highType != curType || m_HighestPriorityEvent->GetEvent().CanBeInterruptedBySameEvent())
                && m_HighestPriorityEvent->BaseEventTakesPriorityOverBaseEvent(*m_CurrentEvent);
        }
        if (setAsCurrent) { // 0x5FC72F
            delete m_CurrentEvent;
            m_CurrentEvent         = m_HighestPriorityEvent;
            m_HighestPriorityEvent = nullptr;
            // Como el original: borra el asignador antes de calcular el nuevo (sin comprobar si es el principal)
            delete m_EventResponseTaskAllocator;
            m_EventResponseTaskAllocator = nullptr;
            m_EventResponseTaskAllocator = ComputeEventResponseTasks();
        } else { // 0x5FC6F0
            // Como el original: aquí pregunta primero el tipo del evento actual
            const auto curType  = m_CurrentEvent->GetEvent().GetEventType();
            const auto highType = m_HighestPriorityEvent->GetEvent().GetEventType();
            if (highType == curType && m_HighestPriorityEvent->GetEvent().CanBeInterruptedBySameEvent()) {
                eventToAdd = m_HighestPriorityEvent->Clone();
            }
        }
    }

    if (m_PrimaryTaskAllocator && !m_PrimaryTaskAllocator->ProcessGroup(this)) { // 0x5FC770
        delete m_PrimaryTaskAllocator;
        m_PrimaryTaskAllocator = nullptr;
    }

    if (m_HighestPriorityEvent && m_CurrentEvent != m_HighestPriorityEvent) { // 0x5FC792
        delete m_HighestPriorityEvent;
        m_HighestPriorityEvent = nullptr;
    }

    if (!m_PrimaryTaskAllocator && m_CurrentEvent) { // 0x5FC7A7
        if (m_EventResponseTaskAllocator) {
            m_EventResponseTaskAllocator->ProcessGroup(this); // Como el original: el resultado se descarta
        }
    }

    if (eventToAdd) { // 0x5FC7C6
        AddEvent(eventToAdd);
        delete eventToAdd;
    }
}

// 0x5F7410
void CPedGroupIntelligence::SetPrimaryTaskAllocator(CTaskAllocator* ta) {
    // Como el original: borra el anterior sin mirar si es también el de respuesta a eventos
    delete m_PrimaryTaskAllocator;
    m_PrimaryTaskAllocator = ta;
}

// 0x5F8510
void CPedGroupIntelligence::SetEventResponseTask(CPed* ped, bool hasMainTask, const CTask& mainTask, bool hasSecondaryTask, const CTask& secondaryTask, eSecondaryTask slot) {
    if (hasMainTask) {
        SetTask(ped, mainTask, m_PedTaskPairs);
    }
    if (hasSecondaryTask) {
        SetTask(ped, secondaryTask, m_SecondaryPedTaskPairs, slot, false);
    }
}

// 0x5F76C0
bool CPedGroupIntelligence::ReportFinishedTask(const CPed* ped, const CTask* task, PedTaskPairs& taskPairs) {
    for (auto& tp : taskPairs) {
        if (tp.Ped != ped || !tp.Task) {
            continue;
        }
        // Como el original: en cada par pregunta el tipo de su tarea y después el de `task`
        const auto tpType = tp.Task->GetTaskType();
        if (tpType != task->GetTaskType()) {
            continue;
        }
        delete tp.Task;
        tp.Task = nullptr;
        return true;
    }
    return false;
}

// 0x5F86F0
bool CPedGroupIntelligence::ReportFinishedTask(const CPed* ped, const CTask* task) {
    if (ReportFinishedTask(ped, task, m_ScriptCommandPedTaskPairs)) {
        return true;
    }
    if (ReportFinishedTask(ped, task, m_PedTaskPairs)) {
        return true;
    }
    ReportFinishedTask(ped, task, m_SecondaryPedTaskPairs);
    ReportFinishedTask(ped, task, m_DefaultPedTaskPairs);
    return false; // Yes, it's `false` - Intended or not, no clue, but the return value of this function isn't used in any of the xrefs.
}

// 0x5F7540
void CPedGroupIntelligence::SetTask(CPed* ped, const CTask& task, PedTaskPairs& taskPairs, eSecondaryTask slot, bool force) const {
    // Commented out as script tasks are `new`'d, and deleted after this finishes.
    //assert(!GetTaskPool()->IsObjectValid(&task)); // Shouldn't be `new`'d [Keep in mind that there might be false positives]

    const auto tp = GetPedsTaskPair(ped, taskPairs);
    if (!tp) {
        return;
    }
    if (tp->Task) {
        if (!force) {
            // Como el original: primero el tipo de la tarea del par y después el de `task`
            const auto tpType = tp->Task->GetTaskType();
            if (tpType == task.GetTaskType()) {
                return;
            }
        }
        // Como el original: clona, guarda tarea y hueco y después borra la anterior
        const auto old = tp->Task;
        tp->Task = task.Clone();
        tp->Slot = slot;
        delete old;
    } else {
        tp->Task = task.Clone();
        tp->Slot = slot;
    }
}

// 0x5F8560
void CPedGroupIntelligence::SetScriptCommandTask(CPed* ped, const CTask& task) {
    SetTask(ped, task, m_ScriptCommandPedTaskPairs, TASK_SECONDARY_INVALID, true); // Como el original: forzada (push 1)
}

// @unk
void CPedGroupIntelligence::SetDefaultTask(CPed* ped, const CTask& task) {
    SetTask(ped, task, m_DefaultPedTaskPairs);
}

// 0x5F79C0
void CPedGroupIntelligence::FlushTasks(PedTaskPairs& taskPairs, CPed* ped) {
    for (auto& tp : taskPairs) {
        if (ped && tp.Ped != ped) {
            continue;
        }
        delete tp.Task; // Como el original: borra y después anula (0x5F79E6)
        tp.Task = nullptr;
    }
}

// 0x5FB280
void CPedGroupIntelligence::SetDefaultTaskAllocator(const CPedGroupDefaultTaskAllocator& ta) {
    m_DefaultTaskAllocator = &ta;
    for (auto& tp : m_DefaultPedTaskPairs) {
        delete tp.Task;
        tp.Task = nullptr;
    }
    m_DefaultTaskAllocator->AllocateDefaultTasks(m_pPedGroup, nullptr);
}

// 0x5FBB70
void CPedGroupIntelligence::SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType taType) {
    SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(taType));
}

// 0x5F88D0
void CPedGroupIntelligence::ComputeDefaultTasks(CPed* ped) {
    FlushTasks(m_DefaultPedTaskPairs, ped);
    m_DefaultTaskAllocator->AllocateDefaultTasks(m_pPedGroup, ped);
}

// 0x5F87A0
void CPedGroupIntelligence::ProcessIgnorePlayerGroup() {
    if (!FindPlayerWanted()->m_bEverybodyBackOff) {
        return;
    }
    // Como el original: pregunta el tipo del evento una vez por cada caso
    if (m_CurrentEvent && m_CurrentEvent->GetEvent().GetEventType() == EVENT_LEADER_ENTRY_EXIT) {
        return;
    }
    if (m_CurrentEvent && m_CurrentEvent->GetEvent().GetEventType() == EVENT_LEADER_ENTERED_CAR_AS_DRIVER) {
        return;
    }
    const auto leader = m_pPedGroup->GetMembership().GetLeader(); // Como el original: leído antes de FindPlayerPed
    if (FindPlayerPed() != leader) {
        if (!m_CurrentEvent) {
            return;
        }
        const auto src = m_CurrentEvent->GetEvent().GetSourceEntity();
        if (!src || !src->GetIsTypePed()) {
            return;
        }
        const auto grp = src->AsPed()->GetGroup();
        if (!grp) {
            return;
        }
        const auto grpLeader = grp->GetMembership().GetLeader();
        if (grpLeader != FindPlayerPed()) {
            return;
        }
    }
    // Como el original: sin evento actual no hace nada (ni borra el asignador ni vacía las tareas)
    if (!m_CurrentEvent) {
        return;
    }
    delete m_CurrentEvent;
    m_CurrentEvent = nullptr;
    delete m_EventResponseTaskAllocator;
    m_EventResponseTaskAllocator = nullptr;
    FlushTasks(m_PedTaskPairs, nullptr);
    FlushTasks(m_SecondaryPedTaskPairs, nullptr);
}

// 0x5F8780
void CPedGroupIntelligence::ReportAllBarScriptTasksFinished() {
    ReportAllTasksFinished(m_PedTaskPairs);
    ReportAllTasksFinished(m_SecondaryPedTaskPairs);
}

// 0x5F7730
void CPedGroupIntelligence::ReportAllTasksFinished(PedTaskPairs& taskPairs) {
    for (auto& tp : taskPairs) {
        delete tp.Task;
        tp.Task = nullptr;
    }
}

// 0x5F77A0
bool CPedGroupIntelligence::IsCurrentEventValid() {
    if (!m_CurrentEvent || m_CurrentEvent->GetEvent().GetEventType() != EVENT_PLAYER_COMMAND_TO_GROUP) {
        return true;
    }
    // Como el original: pide la entidad origen tres veces (comprobar, mirar el tipo y buscar en el grupo)
    auto& event = m_CurrentEvent->GetEvent();
    if (!event.GetSourceEntity() || !event.GetSourceEntity()->GetIsTypePed()) {
        return true;
    }
    return !m_pPedGroup->GetMembership().IsMember(event.GetSourceEntity()->AsPed());
}

// 0x5F7760
bool CPedGroupIntelligence::IsGroupResponding() {
    const auto CheckTaskPairs = [](PedTaskPairs& tps) {
        for (auto& tp : tps) {
            if (tp.Ped && tp.Task) {
                return true;
            }
        }
        return false;
    };
    return CheckTaskPairs(m_PedTaskPairs)
        || CheckTaskPairs(m_SecondaryPedTaskPairs);
}

// 0x5F7440
void CPedGroupIntelligence::SetEventResponseTaskAllocator(CTaskAllocator* ta) {
    delete m_EventResponseTaskAllocator;
    m_EventResponseTaskAllocator = ta;
}

// 0x5FC440
CTaskAllocator* CPedGroupIntelligence::ComputeEventResponseTasks() {
    FlushTasks(m_PedTaskPairs, nullptr);
    FlushTasks(m_SecondaryPedTaskPairs, nullptr);
    return CGroupEventHandler::ComputeEventResponseTasks(*m_CurrentEvent, m_pPedGroup);
}

void CPedGroupIntelligence::ComputeScriptCommandTasks() {
    plugin::CallMethod<0x5F7800>(this);
}
