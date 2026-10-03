/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGroup
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Event.h"
#include "EventGroup.h"

void CEventGroup::InjectHooks() {
    RH_ScopedVirtualClass(CEventGroup, 0x85AAB0, 1);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AB340);

    RH_ScopedOverloadedInstall(Add, "", 0x4AB420, CEvent*(CEventGroup::*)(CEvent*, bool));
    RH_ScopedInstall(HasScriptCommandOfTaskType, 0x4AB840);
    RH_ScopedInstall(HasEventOfType, 0x4AB5E0);
    RH_ScopedInstall(HasEvent, 0x4AB6A0);
    RH_ScopedInstall(GetHighestPriorityEvent, 0x4AB7C0);
    RH_ScopedInstall(TickEvents, 0x4AB6D0);
    RH_ScopedInstall(Remove, 0x4AB5A0);
    RH_ScopedInstall(RemoveInvalidEvents, 0x4AB760);
    RH_ScopedInstall(Reorganise, 0x4AB700);
    RH_ScopedInstall(Flush, 0x4AB370);
    RH_ScopedInstall(GetEventOfType, 0x4AB650);
}

// 0x4AB340
CEventGroup::CEventGroup(CPed* ped) :
    m_pPed{ped}
{
}

CEventGroup::~CEventGroup() {
    Flush();
}

// 0x4AB420
CEvent* CEventGroup::Add(CEvent* event, bool bValid) {
    if (m_pPed) {
        // Como el original: los flags se inicializan antes y HasEditableResponse se consulta dos veces
        bool bAddToEventGroup        = true;
        bool bInformGroup            = false;
        bool bInformRespectedFriends = false;
        bool bTriggerLookAt          = false;
        if (event->HasEditableResponse()) {
            const auto eventResp = static_cast<CEventEditableResponse*>(event);

            bInformGroup            = eventResp->ComputeResponseTaskOfType(m_pPed, TASK_SIMPLE_INFORM_GROUP);
            bInformRespectedFriends = eventResp->ComputeResponseTaskOfType(m_pPed, TASK_SIMPLE_INFORM_RESPECTED_FRIENDS);
            bTriggerLookAt          = eventResp->ComputeResponseTaskOfType(m_pPed, TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD);

            eventResp->ComputeResponseTaskType(m_pPed, false);

            // Como el original: GetEventType solo se consulta si no va a responder
            if (!eventResp->WillRespond()) {
                if (event->GetEventType() != EVENT_DAMAGE || !eventResp->m_bAddToEventGroup) {
                    bAddToEventGroup = false;
                }
            }
        }
        if (!event->AffectsPed(m_pPed)) {
            return nullptr;
        }
        if (event->HasEditableResponse()) {
            const auto eventResp = static_cast<CEventEditableResponse*>(event);
            if (bInformGroup) {
                eventResp->InformGroup(m_pPed);
            }
            if (bInformRespectedFriends && (event->GetEventType() != EVENT_DAMAGE || !static_cast<CEventDamage*>(event)->m_bStealthMode)) {
                eventResp->InformRespectedFriends(m_pPed);
            }
            if (bTriggerLookAt) {
                eventResp->TriggerLookAt(m_pPed);
            }
            eventResp->InformVehicleOccupants(m_pPed);
        }
        // Como el original: la prioridad se pide antes que el tipo
        const auto intel    = m_pPed->GetIntelligence();
        const auto priority = event->GetEventPriority();
        const auto type     = event->GetEventType();
        intel->RecordEventForScript(type, priority);
        if (!bAddToEventGroup) {
            return nullptr;
        }
    }

    if ((int32)m_count >= TOTAL_EVENTS_PER_EVENTGROUP) {
        return nullptr;
    }

    const auto clonedEvent = event->Clone();
    clonedEvent->m_bValid  = bValid;
    if (m_pPed) {
        clonedEvent->ReportCriminalEvent(m_pPed);
    }
    m_events.data()[m_count++] = clonedEvent;
    
    return clonedEvent;
}

// 0x4AB840
bool CEventGroup::HasScriptCommandOfTaskType(eTaskType taskId) {
    for (auto& event : GetEvents()) {
        // Como el original: salta los huecos nulos
        if (event && event->GetEventType() == EVENT_SCRIPT_COMMAND) {
            const auto eScriptCmd = static_cast<CEventScriptCommand*>(event);
            if (eScriptCmd->m_task && eScriptCmd->m_task->GetTaskType() == taskId) {
                return true;
            }
        }
    }
    return false;
}

// 0x4AB5E0
bool CEventGroup::HasEventOfType(CEvent* eventOfType) const {
    // Como el original: un evento nulo cuenta como encontrado
    if (!eventOfType) {
        return true;
    }
    // Como el original: el tipo del evento buscado se vuelve a pedir en cada iteración, después del de la lista
    for (auto& event : GetEvents()) {
        const auto type = event->GetEventType();
        if (eventOfType->GetEventType() == type) {
            return true;
        }
    }
    return false;
}

// 0x4AB7C0
CEvent* CEventGroup::GetHighestPriorityEvent() {
    CEvent* theEvent = nullptr;
    int32 highestPriority = -1;
    for (auto& event : GetEvents()) {
        int32 eventPriority = event->GetEventPriority();
        bool bIsPriorityGreater = false;
        if (event->GetEventType() == EVENT_SCRIPT_COMMAND) {
            if (eventPriority > highestPriority)
                bIsPriorityGreater = true;
        } else if (eventPriority >= highestPriority) {
            bIsPriorityGreater = true;
        }
        if (bIsPriorityGreater && event->AffectsPed(m_pPed)) {
            highestPriority = eventPriority;
            theEvent = event;
        }
    }
    return theEvent;
}

// 0x4AB6D0
void CEventGroup::TickEvents() {
    rng::for_each(GetEvents(), &CEvent::Tick);
}

// 0x4AB6A0
bool CEventGroup::HasEvent(CEvent* needle) {
    return notsa::contains(GetEvents(), needle);
}

// 0x4AB5A0
void CEventGroup::Remove(CEvent* event) {
    const auto it = rng::find(GetEvents(), event);
    if (it != GetEvents().end()) {
        delete std::exchange(*it, nullptr);
    }
}

// 0x4AB760
void CEventGroup::RemoveInvalidEvents(bool bEverythingButScriptEvents) {
    for (auto& event : GetEvents()) {
        if (!event) {
            continue;
        }
        if (event->IsValid(m_pPed)) {
            if (!bEverythingButScriptEvents || event->GetEventType() == EVENT_SCRIPT_COMMAND) {
                continue;
            }
        }
        delete std::exchange(event, nullptr);
    }
}

// 0x4AB700
void CEventGroup::Reorganise() {
    // Como el original: los eventos no nulos se mueven a un temporal y su hueco queda a cero
    CEvent* old[TOTAL_EVENTS_PER_EVENTGROUP];
    int32   cnt = 0;
    for (int32 i = 0; i < (int32)m_count; i++) {
        if (const auto e = m_events.data()[i]) {
            old[cnt++]           = e;
            m_events.data()[i]   = nullptr;
        }
    }
    m_count = (size_t)cnt;
    for (int32 i = 0; i < (int32)m_count; i++) {
        m_events.data()[i] = old[i];
    }
}

// 0x4AB370
void CEventGroup::Flush(bool bKeepJoinGroupScriptCommands) {
    const auto eScriptCmdBeInGroup = [=, this]() -> CEvent* {
        if (bKeepJoinGroupScriptCommands) {
            for (auto& event : GetEvents()) {
                if (const auto eScriptCmd = notsa::dyn_cast<CEventScriptCommand>(event)) {
                    if (eScriptCmd->m_task && eScriptCmd->m_task->GetTaskType() == TASK_COMPLEX_BE_IN_GROUP) {
                        return std::exchange(event, nullptr); // null the slot out so it doesn't get deleted
                    }
                }
            }
        }
        return nullptr;
    }();

    for (auto& event : GetEvents()) {
        delete std::exchange(event, nullptr);
    }

    m_count = 0;
    if (eScriptCmdBeInGroup) {
        m_events[m_count++] = eScriptCmdBeInGroup;
    }
}

// 0x4AB650
CEvent* CEventGroup::GetEventOfType(eEventType type) const noexcept {
    for (auto& event : GetEvents()) {
        if (event->GetEventType() == type) {
            return event;
        }
    }
    return nullptr;
}
