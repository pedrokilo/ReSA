/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CInformFriendsEvent
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CInformFriendsEventQueue
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "InformFriendsEventQueue.h"

// Lote 169A (banco Fn169A): cuatro funciones del EXE sobre un elemento de la cola (CInformFriendsEvent, 12 bytes: peaton +0, evento +4, instante +8) que ningun llamador del EXE usa por esas direcciones (excepto
// 0x4B2950, el destructor de elemento que el EXE pasa al iterador de vectores del arreglo estatico; 0x4AC0F0 es una copia byte a byte). Son funciones propias de la DLL (la estructura es un agregado sin metodos).
namespace CInformFriendsEventBody {
// 0x4B2950 y 0x4AC0F0 (thiscall, sin argumentos de pila, `ret`, 50 bytes cada una, identicas): si hay peaton libera su referencia (CEntity::CleanUpOldReference 0x571A00, con hook; ECX = peaton, argumento = &m_Ped)
// y pone el peaton a nulo; si hay evento lo borra con su destructor virtual con borrado (`push 1; call [vtable]`) y pone el evento a nulo; el instante queda en -1. Sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall Destructor(CInformFriendsEvent* self, void* /*edx*/) {
    if (self->m_Ped) {
        CEntity::SafeCleanUpRef(self->m_Ped);
        self->m_Ped = nullptr;
    }
    if (self->m_Event) {
        delete self->m_Event;
        self->m_Event = nullptr;
    }
    self->m_Time = -1;
}

// 0x4AC0F0: la copia byte a byte, con su propia funcion de la DLL (dos entradas del EXE no pueden compartir el mismo cuerpo de hook: HookRoute resolveria una por la otra).
__declspec(noinline) void __fastcall DestructorCopy(CInformFriendsEvent* self, void* /*edx*/) {
    Destructor(self, nullptr);
}

// 0x4AC130 (thiscall, tres dwords de pila, `ret 0Ch`, 45 bytes): guarda el peaton y, si no es nulo, registra la referencia (CEntity::RegisterReference 0x571B70, con hook; ECX = peaton, argumento = &m_Ped);
// guarda el evento y el instante `reloj de juego (0xB7CB84) + retardo` (suma de 32 bits). El EXE deja el evento en EAX (nadie lo lee: la funcion no tiene llamadores).
__declspec(noinline) void __fastcall Constructor(CInformFriendsEvent* self, void* /*edx*/, CPed* ped, CEvent* event, uint32 delayMs) {
    self->m_Ped = ped;
    CEntity::SafeRegisterRef(self->m_Ped);
    self->m_Event = event;
    self->m_Time = static_cast<int32>(CTimer::m_snTimeInMilliseconds + delayMs);
}

// 0x4AC160 (thiscall, sin argumentos de pila, `ret`, 87 bytes): igual que el cuerpo de CInformFriendsEventQueue::Process para un elemento. Con peaton: si el instante (sin signo) no ha pasado (`m_Time >= reloj`)
// no hace nada; si no, anade el evento (aunque sea nulo) al grupo de eventos del peaton (inteligencia +0x47C, grupo +0x68; CEventGroup::Add 0x4AB420, con hook, con `false`), libera la referencia del
// peaton y lo pone a nulo. Despues (tambien sin peaton) borra el evento si lo hay y pone el instante a -1.
__declspec(noinline) void __fastcall Process(CInformFriendsEvent* self, void* /*edx*/) {
    if (self->m_Ped) {
        if (static_cast<uint32>(self->m_Time) >= CTimer::m_snTimeInMilliseconds) {
            return;
        }
        self->m_Ped->GetEventGroup().Add(self->m_Event, false);
        CEntity::SafeCleanUpRef(self->m_Ped);
        self->m_Ped = nullptr;
    }
    if (self->m_Event) {
        delete self->m_Event;
        self->m_Event = nullptr;
    }
    self->m_Time = -1;
}

static void InjectHooks() {
    RH_ScopedClass(CInformFriendsEvent);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(Destructor, "Destructor", 0x4B2950);   // lote 169A (banco Fn169A)
    RH_ScopedNamedGlobalInstall(DestructorCopy, "Destructor-2", 0x4AC0F0); // lote 169A: copia byte a byte de 0x4B2950 (sin llamadores directos)
    RH_ScopedNamedGlobalInstall(Constructor, "Constructor", 0x4AC130); // lote 169A
    RH_ScopedNamedGlobalInstall(Process, "Process", 0x4AC160);         // lote 169A
}
} // namespace CInformFriendsEventBody

void CInformFriendsEventQueue::InjectHooks() {
    RH_ScopedClass(CInformFriendsEventQueue);
    RH_ScopedCategoryGlobal();

    //RH_ScopedInstall(Constructor, 0x0, { .reversed = false }); <-- nop
    //RH_ScopedInstall(Destructor, 0x0, { .reversed = false }); <-- nop
    RH_ScopedInstall(Init, 0x4B2990);
    RH_ScopedInstall(Flush, 0x4AC2A0);
    RH_ScopedInstall(Add, 0x4AC1E0);
    RH_ScopedInstall(Process, 0x4AC2E0);

    CInformFriendsEventBody::InjectHooks();
}

// 0x4B2990
void CInformFriendsEventQueue::Init() {
    CInformFriendsEventQueue::Flush();
}

// 0x4AC2A0
void CInformFriendsEventQueue::Flush() {
    for (auto& e : ms_informFriendsEvents) {
        CEntity::SafeCleanUpRef(e.m_Ped);
        e.m_Ped = nullptr;
        delete std::exchange(e.m_Event, nullptr);
        e.m_Time = -1;
    }
}

// 0x4AC1E0
bool CInformFriendsEventQueue::Add(CPed* ped, CEvent* event) {
    CInformFriendsEvent* freeField{};
    for (auto& e : ms_informFriendsEvents) {
        if (e.m_Ped == ped) {
            if (event->GetEventType() == e.m_Event->GetEventType()) {
                // Already added
                return false;
            }
        } else if (!freeField && !e.m_Event) {
            freeField = &e;
        }
    }

    if (!freeField) {
        return false; // Queue is full.
    }

    freeField->m_Ped   = ped;
    CEntity::SafeRegisterRef(freeField->m_Ped);
    freeField->m_Event = event;
    freeField->m_Time  = CTimer::GetTimeInMS() + CGeneral::GetRandomIntInRange(300, 800); // 0x4AC235: [300, 799]
    return true;
}

// 0x4AC2E0
void CInformFriendsEventQueue::Process() {
    for (auto& e : ms_informFriendsEvents) {
        if (e.m_Ped) {
            // FIXME: This cast isn't safe. Also in CInformGroupEvent::Process.
            if (static_cast<uint32>(e.m_Time) >= CTimer::GetTimeInMS()) {
                // Event is vaild and not yet elapsed, do not remove.
                continue;
            }
            e.m_Ped->GetEventGroup().Add(e.m_Event);

            CEntity::SafeCleanUpRef(e.m_Ped);
            e.m_Ped = nullptr;
        }
        if (e.m_Event) {
            delete std::exchange(e.m_Event, nullptr);
        }
        e.m_Time = -1;
    }
}
