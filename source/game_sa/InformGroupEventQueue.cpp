/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CInformGroupEventQueue
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "InformGroupEventQueue.h"

void CInformGroupEventQueue::InjectHooks() {
    RH_ScopedClass(CInformGroupEventQueue);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x4B2AD0);
    RH_ScopedInstall(Add, 0x4B7CD0);
    RH_ScopedInstall(Flush, 0x4AC410);
    RH_ScopedInstall(Process, 0x4B2AE0);
}

// 0x4B2AD0
void CInformGroupEventQueue::Init() {
    CInformGroupEventQueue::Flush();
}

// 0x4B7CD0
bool CInformGroupEventQueue::Add(CPed* ped, CPedGroup* pedGroup, CEventEditableResponse* event) {
    CInformGroupEvent* freeField{};
    for (auto& e : ms_informGroupEvents) {
        if (e.m_Ped == ped) {
            if (event->GetEventType() == e.m_Event->GetEventType()) {
                return false; // Already added
            }
        } else if (!freeField && !e.m_Event) {
            freeField = &e;
        }
    }

    if (!freeField) {
        return false; // Queue is full.
    }

    const auto extraTime = [ped, pedGroup, event] {
        if (const auto et = event->GetEventType(); et != EVENT_ACQUAINTANCE_PED_HATE && et != EVENT_ACQUAINTANCE_PED_DISLIKE) {
            return 0;
        }

        event->ComputeResponseTaskType(pedGroup);
        if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(ped) ||
            ped->IsCreatedByMission() || !static_cast<CEventAcquaintancePed*>(event)->m_AcquaintancePed->bInVehicle ||
            event->m_TaskId == TASK_NONE) {
            return 0;
        }

        return 5'000;
    }();

    freeField->m_Ped      = ped;
    CEntity::SafeRegisterRef(freeField->m_Ped);
    freeField->m_PedGroup = pedGroup;
    freeField->m_Event    = event;
    freeField->m_Time     = CTimer::GetTimeInMS() + extraTime;
    return true;
}

// 0x4AC410
void CInformGroupEventQueue::Flush() {
    for (auto& e : ms_informGroupEvents) {
        CEntity::SafeCleanUpRef(e.m_Ped);
        e.m_Ped = nullptr;
        delete std::exchange(e.m_Event, nullptr);
        e.m_Time = -1;
    }
}

// 0x4B2AE0
void CInformGroupEventQueue::Process() {
    rng::for_each(ms_informGroupEvents, &CInformGroupEvent::Process);
}
