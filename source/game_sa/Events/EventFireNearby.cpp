/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventFireNearby
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
#include "EventFireNearby.h"

namespace {
// Cuerpo no virtual de ~CEventFireNearby del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall FireNearbyDestructorBody(CEventFireNearby* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventFireNearby::InjectHooks() {
    RH_ScopedVirtualClass(CEventFireNearby, 0x85B6E8, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(FireNearbyDestructorBody, "Destructor", 0x4B1F80);

    RH_ScopedInstall(Constructor, 0x4B1F10);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1F90); // lote 184A (sin validar): ranura 5
}



CEventFireNearby::CEventFireNearby(const CVector& position) : CEventEditableResponse() {
    m_position = position;
}

// 0x4B1F90 (lote 184A, sin validar; el listado decia CEventPotentialWalkIntoFire; ranura 5 de la vtable 0x85B6E8; thiscall, un puntero en la pila, `ret 4`, 45 bytes): si el gestor de tareas del peaton (inteligencia +0x47C, +4)
// tiene una tarea activa de tipo 0x25C (TASK_COMPLEX_EXTINGUISH_FIRES) con CTaskManager::FindActiveTaskByType (0x681740, con hook) devuelve falso; si no, devuelve CPed::IsAlive (0x5E0170, con hook; ECX = peaton).
static_assert(TASK_COMPLEX_EXTINGUISH_FIRES == 0x25C, "el EXE empuja 0x25C a FindActiveTaskByType");
bool CEventFireNearby::AffectsPed(CPed* ped) {
    if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_EXTINGUISH_FIRES)) {
        return false;
    }
    return ped->IsAlive();
}

// 0x4B1F10
CEventFireNearby* CEventFireNearby::Constructor(const CVector& position) {
    this->CEventFireNearby::CEventFireNearby(position);
    return this;
}
