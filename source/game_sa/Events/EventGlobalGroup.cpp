/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGlobalGroup
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
#include "EventGlobalGroup.h"

namespace {
// 0x4AB9B0 (lote 143A, banco Fn143A): cuerpo del destructor de CEventGroup que usa el destructor con borrado de CEventGlobalGroup del EXE (thiscall, sin argumentos, `ret`: `push 0; mov [ecx],0x85AAB0; call 0x4AB370`
// = Flush(false); sin hook propio). Funcion propia sin expansion en linea para que el banco pueda sustituirla; el destructor con borrado la llama como el `call 0x4AB9B0` del EXE.
__declspec(noinline) void __fastcall EventGlobalGroupDestructorBody(CEventGlobalGroup* self, void* /*edx*/) { self->CEventGlobalGroup::~CEventGlobalGroup(); }

// 0x4ABAA0 (lote 143A, banco Fn143A): destructor con borrado de CEventGlobalGroup del EXE (ranura 0 de su vtable 0x85AAB4; thiscall, un argumento de pila, `ret 4`, 27 bytes:
// `push esi; mov esi,ecx; call 0x4AB9B0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con `operator delete` (0x8214BD, CRT, sin hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventGlobalGroup* __fastcall EventGlobalGroupScalarDeletingDestructor(CEventGlobalGroup* self, void* /*edx*/, uint8 flags) {
    EventGlobalGroupDestructorBody(self, nullptr);
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote143AEventGroupDestructorBody() { return reinterpret_cast<uintptr_t>(&EventGlobalGroupDestructorBody); } // para el banco Fn143A

void CEventGlobalGroup::InjectHooks() {
    RH_ScopedVirtualClass(CEventGlobalGroup, 0x85AAB4, 1);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(EventGlobalGroupScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4ABAA0); // lote 143A (banco Fn143A)
    RH_ScopedInstall(GetSoundLevel, 0x4AB900);
    RH_ScopedInstall(AddEventsToPed, 0x4AB9C0);
    RH_ScopedInstall(AddEventsToGroup, 0x4AB8A0);
}

// 0x4AB900
float CEventGlobalGroup::GetSoundLevel(CEntity* entity, CVector& position) {
    // Como el original: el número de eventos se relee (con signo) en cada vuelta.
    float soundLevel = 0.0f;
    for (int32 i = 0; i < (int32)m_count; i++) {
        CEvent* event = m_events.data()[i];
        if (event->GetSourceEntity() == entity || !entity) {
            float eventSoundLevel = event->GetSoundLevel(entity, position);
            if (eventSoundLevel > 0.0f) {
                soundLevel += CEvent::CalcSoundLevelIncrement(soundLevel, eventSoundLevel);
            }
        }
    }
    return soundLevel;
}

// 0x4AB9C0
void CEventGlobalGroup::AddEventsToPed(CPed* ped) {
    // Como el original: el número de eventos se relee (con signo) en cada vuelta.
    for (int32 i = 0; i < (int32)m_count; i++) {
        CEvent* clonedEvent = m_events.data()[i]->Clone();
        ped->GetEventGroup().Add(clonedEvent, false);
        delete clonedEvent;
    }
}

// 0x4AB8A0
void CEventGlobalGroup::AddEventsToGroup(CPedGroup* pedGroup) {
    // Como el original: el número de eventos se relee (con signo) en cada vuelta.
    for (int32 i = 0; i < (int32)m_count; i++) {
        CEvent* clonedEvent = m_events.data()[i]->Clone();
        pedGroup->GetIntelligence().AddEvent(clonedEvent);
        delete clonedEvent;
    }
}
