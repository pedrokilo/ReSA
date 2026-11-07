/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGroupEvent
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventGroupEvent.h"

void CEventGroupEvent::InjectHooks() {
    RH_ScopedVirtualClass(CEventGroupEvent, 0x85AE48, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ADFD0);
    RH_ScopedInstall(Destructor, 0x4AE070);
    RH_ScopedInstall(BaseEventTakesPriorityOverBaseEvent, 0x4AE100);
}

// 0x4ADFD0
CEventGroupEvent::CEventGroupEvent(CPed* ped, CEvent* event) : CEvent() {
    m_ped = ped;
    m_event = event;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AE070
CEventGroupEvent::~CEventGroupEvent() {
    CEntity::SafeCleanUpRef(m_ped);
    delete m_event;
}

// NOTSA, inlined
bool CEventGroupEvent::IsPriorityEvent() const {
    switch (m_event->GetEventType()) {
    case EVENT_LEADER_ENTERED_CAR_AS_DRIVER:
    case EVENT_LEADER_EXITED_CAR_AS_DRIVER:
    case EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER:
    case EVENT_PLAYER_COMMAND_TO_GROUP:
    case EVENT_PLAYER_COMMAND_TO_GROUP_GATHER:
    case EVENT_DRAGGED_OUT_CAR:
    case EVENT_LEADER_ENTRY_EXIT:
        return true;
    }
    return false;
}

// 0x4AE100
bool CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent(const CEventGroupEvent& other) {
    // Como el original: calcula las dos comprobaciones de prioridad (primero la propia, despues la del otro) antes de decidir;
    // solo si ninguna lo es llama a TakesPriorityOver (ranura 11) del evento propio con el evento del otro.
    const bool thisIsPriority  = IsPriorityEvent();
    const bool otherIsPriority = other.IsPriorityEvent();
    if (thisIsPriority) {
        return true;
    }
    if (otherIsPriority) {
        return false;
    }
    return GetEvent().TakesPriorityOver(other.GetEvent());
}
