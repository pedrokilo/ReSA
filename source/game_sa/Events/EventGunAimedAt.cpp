/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGunAimedAt
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventGunAimedAt.h"

#include "PedType.h"

namespace {
// Cuerpo no virtual de ~CEventGunAimedAt del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventGunAimedAt del EXE (0x85B330), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall GunAimedAtDestructorBody(CEventGunAimedAt* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B330;
    CEntity::SafeCleanUpRef(self->m_AimedBy);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventGunAimedAt::InjectHooks() {
    RH_ScopedVirtualClass(CEventGunAimedAt, 0x85B330, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(GunAimedAtDestructorBody, "Destructor", 0x4B07B0);

    RH_ScopedInstall(Constructor, 0x4B0700);

    RH_ScopedVMTInstall(AffectsPed, 0x4B4EE0);          // lote 181A (sin validar): ranura 5 (el listado decia CEventKnockOffBike)
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B0810);   // lote 181A (sin validar): ranura 11 (el listado decia CEventKnockOffBike)
}

// 0x4B0700
CEventGunAimedAt::CEventGunAimedAt(CPed* ped) : CEventEditableResponse() {
    m_AimedBy = ped;
    CEntity::SafeRegisterRef(m_AimedBy);
}

// 0x4B07B0
CEventGunAimedAt::~CEventGunAimedAt() {
    CEntity::SafeCleanUpRef(m_AimedBy);
}

// 0x4B0700
CEventGunAimedAt* CEventGunAimedAt::Constructor(CPed* ped) {
    this->CEventGunAimedAt::CEventGunAimedAt(ped);
    return this;
}

// 0x4B4EE0 (lote 181A, sin validar; el listado decia CEventKnockOffBike; la clase sale de la ranura 5 de la vtable 0x85B330): thiscall, un argumento de pila (el peaton), `ret 4`, 232 bytes. Como el original:
// 1) CPed::IsPlayer (0x5DF8F0) -> falso; 2) la tarea activa del gestor de tareas (+0x47C de inteligencia, +4 del gestor: CTaskManager::GetActiveTask 0x681720) se pide DOS veces (la segunda solo si la primera no es nula) y si
//    su tipo (ranura 4) es 601 (TASK_COMPLEX_REACT_TO_GUN_AIMED_AT) -> falso; 3) m_AimedBy nulo -> falso; 4) CPedIntelligence::IsInSeeingRange (0x600C60) con la posicion de quien apunta (matriz +0x30 o, sin matriz, +4) -> si no, falso;
//    5) CPed::IsAlive (0x5E0170); 6) CPedGroups::AreInSameGroup (0x5F7F40, cdecl) -> falso; 7) en vehiculo con conductor (bit 8 de +0x46C, vehiculo +0x58C, IsPassenger 0x6D1BD0 y conductor +0x460 no nulo) -> falso;
//    8) si es policia (+0x598 == 6) CCrime::ReportCrime(22, ped, FindPlayerPed(-1)) (0x532010, cdecl); verdadero.
bool CEventGunAimedAt::AffectsPed(CPed* ped) {
    static_assert(TASK_COMPLEX_REACT_TO_GUN_AIMED_AT == 601);
    if (ped->IsPlayer())
        return false;

    if (ped->GetTaskManager().GetActiveTask()) {
        CTask* const activeTask = ped->GetTaskManager().GetActiveTask(); // segunda llamada del EXE (0x4B4F09)
        if (activeTask->GetTaskType() == TASK_COMPLEX_REACT_TO_GUN_AIMED_AT)
            return false;
    }

    if (!m_AimedBy)
        return false;
    if (!ped->GetIntelligence()->IsInSeeingRange(m_AimedBy->GetPosition()))
        return false;
    if (!ped->IsAlive())
        return false;
    if (CPedGroups::AreInSameGroup(ped, m_AimedBy))
        return false;
    if (ped->IsInVehicleThatHasADriver())
        return false;

    if (ped->m_nPedType == PED_TYPE_COP)
        CCrime::ReportCrime(eCrimeType::CRIME_AIM_GUN, ped, FindPlayerPed());
    return true;
}

// 0x4B09E0
void CEventGunAimedAt::ReportCriminalEvent(CPed* ped) {
    if (IsCriminalEvent())
        CPedType::PoliceDontCareAboutCrimesAgainstPedType(ped->m_nPedType);
}

// 0x4B0810 (lote 181A, sin validar; el listado decia CEventKnockOffBike; ranura 11 de la vtable 0x85B330): thiscall, un argumento de pila (el otro evento), `ret 4`, 138 bytes. Como el original: si quien apunta (m_AimedBy,
// +0x14) no es nulo y es el jugador (CPed::IsPlayer 0x5DF8F0): se pide la entidad origen del otro evento (ranura 10) y, si es la misma que m_AimedBy (releido), se compara prioridad propia >= la del otro (con signo; el EXE
// pide PRIMERO la del otro evento y despues la propia, ambas ranura 2); si no, el tipo del otro evento (ranura 1) 9 (EVENT_DAMAGE), 36 (EVENT_ACQUAINTANCE_PED_HATE) o 39 (EVENT_ACQUAINTANCE_PED_RESPECT) -> verdadero y
// cualquier otro -> CEvent::TakesPriorityOver (0x4ABC30). Sin jugador: el mismo >= de prioridades (otra vez primero la del otro evento).
bool CEventGunAimedAt::TakesPriorityOver(const CEvent& refEvent) {
    static_assert(EVENT_DAMAGE == 9 && EVENT_ACQUAINTANCE_PED_HATE == 36 && EVENT_ACQUAINTANCE_PED_RESPECT == 39);
    if (m_AimedBy && m_AimedBy->IsPlayer()) {
        CEntity* const sourceEntity = refEvent.GetSourceEntity();
        if (sourceEntity == m_AimedBy) {
            const int32 refPriority = refEvent.GetEventPriority();
            const int32 ownPriority = GetEventPriority();
            return ownPriority >= refPriority;
        }

        switch (refEvent.GetEventType()) {
        case EVENT_DAMAGE:
        case EVENT_ACQUAINTANCE_PED_HATE:
        case EVENT_ACQUAINTANCE_PED_RESPECT:
            return true;
        }
        return CEvent::TakesPriorityOver(refEvent);
    }
    const int32 refPriority = refEvent.GetEventPriority();
    const int32 ownPriority = GetEventPriority();
    return ownPriority >= refPriority;
}

// 0x4B7630
CEventEditableResponse* CEventGunAimedAt::CloneEditable() const noexcept {
    return new CEventGunAimedAt(m_AimedBy);
}
