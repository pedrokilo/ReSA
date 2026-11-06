/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventStuckInAir
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
#include "EventStuckInAir.h"


namespace {
// 0x4B1520 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventStuckInAir que usan los destructores del EXE. Deja la vtable de CEventStuckInAir del EXE (0x85B508), libera la referencia de m_ped (+0xC) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall StuckInAirDestructorBody(CEventStuckInAir* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B508;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventStuckInAir::InjectHooks()
{
    RH_ScopedVirtualClass(CEventStuckInAir, 0x85B508, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(StuckInAirDestructorBody, "Destructor", 0x4B1520);

    RH_ScopedInstall(Constructor, 0x4B1490);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1600);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1580);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B15B0);
}

// 0x4B1490
CEventStuckInAir::CEventStuckInAir(CPed* ped)
{
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventStuckInAir::~CEventStuckInAir()
{
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B1600
int32 CEventStuckInAir::GetEventPriority() const {
    // Como el original: 75 solo si hay mas de un evento en cola (comparacion con signo, jg)
    return m_ped && m_ped->GetIntelligence()->GetEventHandler().GetCurrentEventType() != EVENT_STUCK_IN_AIR && (int32)m_ped->GetIntelligence()->GetEventGroup().GetNumEventsInQueue() > 1
        ? 75
        : 63;
}

// 0x4B1490
CEventStuckInAir* CEventStuckInAir::Constructor(CPed* ped)
{
    this->CEventStuckInAir::CEventStuckInAir(ped);
    return this;
}

// 0x4B1580
bool CEventStuckInAir::AffectsPed(CPed* ped)
{
    if (ped->bIsStanding)
        return false;

    return ped->GetStuckChecker().m_state != PED_STUCK_STATE_NONE;
}

// 0x4B15B0
bool CEventStuckInAir::TakesPriorityOver(const CEvent& refEvent)
{
    if (refEvent.GetEventPriority() < 74 && refEvent.GetEventType() != EVENT_STUCK_IN_AIR)
        return true;

    return CEvent::TakesPriorityOver(refEvent);
}