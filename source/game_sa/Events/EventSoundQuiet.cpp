/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSoundQuiet
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventSoundQuiet.h"

void CEventSoundQuiet::InjectHooks()
{
    RH_ScopedVirtualClass(CEventSoundQuiet, 0x86C2B0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5E05B0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B5240);
    RH_ScopedVMTInstall(CloneEditable, 0x5E0670);
}

// 0x5E05B0
CEventSoundQuiet::CEventSoundQuiet(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position)
{
    m_fLocalSoundLevel = fLocalSoundLevel;
    m_startTimeInMs = startTime;
    m_entity = entity;
    m_position = position;
    CEntity::SafeRegisterRef(m_entity);
    if (m_startTimeInMs == (uint32)(-1)) {
        m_startTimeInMs = CTimer::GetTimeInMS();
        m_position = m_entity->GetPosition();
    }
}

CEventSoundQuiet::~CEventSoundQuiet()
{
    CEntity::SafeCleanUpRef(m_entity);
}

CEventSoundQuiet* CEventSoundQuiet::Constructor(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position)
{
    this->CEventSoundQuiet::CEventSoundQuiet(entity, fLocalSoundLevel, startTime, position);
    return this;
}

// 0x4B5240
bool CEventSoundQuiet::AffectsPed(CPed* ped)
{
    if (ped->IsPlayer() || !ped->IsAlive()|| !GetSourceEntity())
        return false;

    float soundLevel = GetSoundLevel(nullptr, ped->GetPosition());
    if (soundLevel < 30.0f)
        return false;

    auto currentEvent = static_cast<CEventSoundQuiet*>(ped->GetEventHandlerHistory().GetCurrentEvent());
    if (currentEvent &&
        currentEvent->GetEventType() == EVENT_SOUND_QUIET && currentEvent->GetSourceEntity() == GetSourceEntity()) {
        CVector distance = m_position - currentEvent->m_position;
        if (m_startTimeInMs - currentEvent->m_startTimeInMs >= 2000 && distance.SquaredMagnitude() >= 1.0f)
            return true;
        if (soundLevel - currentEvent->GetSoundLevel(nullptr, ped->GetPosition()) < 3.0f)
            return false;
    }
    return true;
}

// 0x5E0670
CEventEditableResponse* CEventSoundQuiet::CloneEditable() const noexcept
{
    return new CEventSoundQuiet(m_entity, m_fLocalSoundLevel, m_startTimeInMs, m_position);
}
