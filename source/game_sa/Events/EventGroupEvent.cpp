/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGroupEvent
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventGroupEvent.h"

namespace {
// 0x4B5E50 (lote 204A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable 0x85AE48 de CEventGroupEvent; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado del EXE. Como el original: llama al cuerpo del
// destructor (0x4AE070, Destructor con hook: libera la referencia al peaton y borra el evento interno con su destructor virtual con borrado) y, con el bit 0 del BYTE de banderas, libera la ranura en el pool de eventos (0xB744AC) EN LINEA,
// igual que CEvent::operator delete (0x4B5630, con hook); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CEventGroupEvent* __fastcall GroupEventScalarDeletingDestructor(CEventGroupEvent* self, void* /*edx*/, uint8 flags) {
    self->CEventGroupEvent::~CEventGroupEvent();
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventGroupEvent::InjectHooks() {
    RH_ScopedVirtualClass(CEventGroupEvent, 0x85AE48, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ADFD0);
    RH_ScopedInstall(Destructor, 0x4AE070);
    RH_ScopedVMTInstall(Clone, 0x4B6EE0); // lote 202A (sin validar): ranura 4
    RH_ScopedNamedGlobalInstall(GroupEventScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5E50); // lote 204A (sin validar)
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

// 0x4B6EE0 (lote 202A, sin validar; el listado decia CEventVehicleCollision; ranura 4 (Clone) de la vtable 0x85AE48; thiscall, sin argumentos, `ret`, 90 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo SIN clonar el evento interno; si no, llama a Clone (ranura 4, llamada virtual) del evento interno (+0x10) y luego al constructor 0x4ADFD0
// (Constructor, con hook) con (peaton +0xC, el clon) y devuelve el evento nuevo. Orden observable: reserva, Clone del evento interno, constructor.
CEvent* CEventGroupEvent::Clone() const noexcept {
    auto* const object = static_cast<CEventGroupEvent*>(CEvent::operator new(sizeof(CEventGroupEvent)));
    if (!object) {
        return nullptr;
    }
    CEvent* const clonedEvent = m_event->Clone();
    return object->Constructor(m_ped, clonedEvent);
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
