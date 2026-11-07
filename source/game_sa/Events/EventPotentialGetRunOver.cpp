/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialGetRunOver
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
#include "EventPotentialGetRunOver.h"

namespace {
// Cuerpo no virtual de ~CEventPotentialGetRunOver del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPotentialGetRunOver del EXE (0x85AE88), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall PotentialGetRunOverDestructorBody(CEventPotentialGetRunOver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AE88;
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventPotentialGetRunOver::InjectHooks() {
    RH_ScopedVirtualClass(CEventPotentialGetRunOver, 0x85AE88, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PotentialGetRunOverDestructorBody, "Destructor", 0x4AE270);

    RH_ScopedInstall(Constructor, 0x4AE1A0);
}

// 0x4AE1A0
CEventPotentialGetRunOver::CEventPotentialGetRunOver(CVehicle* vehicle) {
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x4AE1A0
CEventPotentialGetRunOver* CEventPotentialGetRunOver::Constructor(CVehicle* vehicle) {
    this->CEventPotentialGetRunOver::CEventPotentialGetRunOver(vehicle);
    return this;
}

// 0x4AE270
CEventPotentialGetRunOver::~CEventPotentialGetRunOver() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

bool CEventPotentialGetRunOver::AffectsPed(CPed* ped) {
    return ped->IsAlive()
       && !ped->m_pAttachedTo
       && !ped->bInVehicle
       && m_Vehicle
       && !m_Vehicle->IsBoat();
}

