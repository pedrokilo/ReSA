/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDeadPed
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
// 0x4B5E00 (lote 204A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable de CEventDeadPed; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado del EXE. Como el original: llama al cuerpo del destructor
// (0x4ADF70, con hook propio) y, con el bit 0 del BYTE de banderas, libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (0x4B5630, con hook: indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CEventDeadPed* __fastcall DeadPedScalarDeletingDestructor(CEventDeadPed* self, void* /*edx*/, uint8 flags) {
    DeadPedDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventDeadPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventDeadPed, 0x85AE00, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(DeadPedDestructorBody, "Destructor", 0x4ADF70);
    RH_ScopedNamedGlobalInstall(DeadPedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5E00); // lote 204A (sin validar)

    RH_ScopedInstall(Constructor, 0x4ADEA0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B6E70); // lote 202A (sin validar): ranura 16
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

// 0x4B6E70 (lote 202A, sin validar; el listado decia CEventVehicleCollision; ranura 16 (CloneEditable) de la vtable 0x85AE00; thiscall, sin argumentos, `ret`, 107 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x20 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4ADEA0 (Constructor, con hook) con (peaton +0x14, byte +0x18 ampliado a cero, hora de la muerte +0x1C) y devuelve el evento nuevo.
CEventEditableResponse* CEventDeadPed::CloneEditable() const noexcept {
    auto* const object = static_cast<CEventDeadPed*>(CEvent::operator new(sizeof(CEventDeadPed)));
    if (!object) {
        return nullptr;
    }
    return object->Constructor(m_ped, field_18, m_deathTimeInMs);
}
