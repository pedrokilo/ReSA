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


void CEventGlobalGroup::InjectHooks() {
    RH_ScopedVirtualClass(CEventGlobalGroup, 0x85AAB4, 1);
    RH_ScopedCategory("Events");

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
