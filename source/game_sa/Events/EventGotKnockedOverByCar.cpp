/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGotKnockedOverByCar
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

#include "EventGotKnockedOverByCar.h"

namespace {
// Cuerpo no virtual de ~CEventGotKnockedOverByCar del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventGotKnockedOverByCar del EXE (0x85B618), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall GotKnockedOverByCarDestructorBody(CEventGotKnockedOverByCar* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B618;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventGotKnockedOverByCar::InjectHooks() {
    RH_ScopedVirtualClass(CEventGotKnockedOverByCar, 0x85B618, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(GotKnockedOverByCarDestructorBody, "Destructor", 0x4B1C10);

    RH_ScopedInstall(Constructor, 0x4B1B60);
}

// 0x4B1B60
CEventGotKnockedOverByCar::CEventGotKnockedOverByCar(CVehicle* vehicle) {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

// 0x4B1B60
CEventGotKnockedOverByCar* CEventGotKnockedOverByCar::Constructor(CVehicle* vehicle) {
    this->CEventGotKnockedOverByCar::CEventGotKnockedOverByCar(vehicle);
    return this;
}

// 0x4B1C10
CEventGotKnockedOverByCar::~CEventGotKnockedOverByCar() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B1C70
bool CEventGotKnockedOverByCar::AffectsPed(CPed* ped) {
    if (m_vehicle && !ped->IsPlayer())
        return ped->IsAlive();
    return false;
}

// 0x4B7960
CEventEditableResponse* CEventGotKnockedOverByCar::CloneEditable() const noexcept {
    return new CEventGotKnockedOverByCar(m_vehicle);
}
