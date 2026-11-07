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

// 0x4B4EE0
bool CEventGunAimedAt::AffectsPed(CPed* ped) {
    if (ped->IsPlayer())
        return false;

    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (activeTask && activeTask->GetTaskType() == TASK_COMPLEX_REACT_TO_GUN_AIMED_AT)
        return false;

    if (m_AimedBy && ped->GetIntelligence()->IsInSeeingRange(m_AimedBy->GetPosition()) && ped->IsAlive()) {
        if (CPedGroups::AreInSameGroup(ped, m_AimedBy))
            return false;
        if (!ped->IsInVehicleThatHasADriver()) {
            if (ped->m_nPedType == PED_TYPE_COP)
                CCrime::ReportCrime(eCrimeType::CRIME_AIM_GUN, ped, FindPlayerPed());
            return true;
        }
    }
    return false;
}

// 0x4B09E0
void CEventGunAimedAt::ReportCriminalEvent(CPed* ped) {
    if (IsCriminalEvent())
        CPedType::PoliceDontCareAboutCrimesAgainstPedType(ped->m_nPedType);
}

// 0x4B0810
bool CEventGunAimedAt::TakesPriorityOver(const CEvent& refEvent) {
    if (m_AimedBy && m_AimedBy->IsPlayer()) {
        if (refEvent.GetSourceEntity() == m_AimedBy)
            return GetEventPriority() >= refEvent.GetEventPriority();

        switch (refEvent.GetEventType()) {
        case EVENT_DAMAGE:
        case EVENT_ACQUAINTANCE_PED_HATE:
        case EVENT_ACQUAINTANCE_PED_RESPECT:
            return true;
        }
        return CEvent::TakesPriorityOver(refEvent);
    }
    return GetEventPriority() >= refEvent.GetEventPriority();
}

// 0x4B7630
CEventEditableResponse* CEventGunAimedAt::CloneEditable() const noexcept {
    return new CEventGunAimedAt(m_AimedBy);
}
