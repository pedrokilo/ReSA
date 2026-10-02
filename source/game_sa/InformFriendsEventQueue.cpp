/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
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

void CInformFriendsEventQueue::InjectHooks() {
    RH_ScopedClass(CInformFriendsEventQueue);
    RH_ScopedCategoryGlobal();

    //RH_ScopedInstall(Constructor, 0x0, { .reversed = false }); <-- nop
    //RH_ScopedInstall(Destructor, 0x0, { .reversed = false }); <-- nop
    RH_ScopedInstall(Init, 0x4B2990);
    RH_ScopedInstall(Flush, 0x4AC2A0);
    RH_ScopedInstall(Add, 0x4AC1E0);
    RH_ScopedInstall(Process, 0x4AC2E0);
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
