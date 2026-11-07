/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDeadPed
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

#include "EventDeadPed.h"

#include "Ragdoll/IKChainManager.h"

namespace {
// Cuerpo no virtual de ~CEventDeadPed del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventDeadPed del EXE (0x85AE00), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall DeadPedDestructorBody(CEventDeadPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AE00;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventDeadPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventDeadPed, 0x85AE00, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(DeadPedDestructorBody, "Destructor", 0x4ADF70);

    RH_ScopedInstall(Constructor, 0x4ADEA0);
}

// 0x4ADEA0
CEventDeadPed::CEventDeadPed(CPed* ped, bool bUnknown, uint32 deathTimeInMs) : CEventEditableResponse() {
    m_ped = ped;
    field_18 = bUnknown;
    m_deathTimeInMs = deathTimeInMs;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4ADF70
CEventDeadPed::~CEventDeadPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4ADEA0
CEventDeadPed* CEventDeadPed::Constructor(CPed* ped, bool bUnknown, uint32 deathTimeInMs) {
    this->CEventDeadPed::CEventDeadPed(ped, bUnknown, deathTimeInMs);
    return this;
}

// 0x4B4830
bool CEventDeadPed::AffectsPed(CPed* ped) {
    if (!m_ped || ped == m_ped || !ped->IsAlive())
        return false;

    if (g_ikChainMan.IsLooking(ped) && g_ikChainMan.GetLookAtEntity(ped) == m_ped)
        return false;

    if (m_ped->physicalFlags.bSubmergedInWater)
        return false;

    const CVector distance = m_ped->GetPosition() - ped->GetPosition();
    if (m_ped->bKilledByStealth) {
        const float squaredMagnitude = distance.SquaredMagnitude();
        if (DotProduct(distance, ped->GetForward()) >= 0.0f && squaredMagnitude <= sq(8.0f))
            return sq(20.0f) > squaredMagnitude;
    }
    return false;
}

// 0x4B6E70
CEventEditableResponse* CEventDeadPed::CloneEditable() const noexcept {
    return new CEventDeadPed(m_ped, field_18, m_deathTimeInMs);

}
