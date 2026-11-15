/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPedToFlee
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

#include "EventPedToFlee.h"

namespace {
// Cuerpo no virtual de ~CEventPedToFlee del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPedToFlee del EXE (0x85B178), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall PedToFleeDestructorBody(CEventPedToFlee* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B178;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventPedToFlee::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedToFlee, 0x85B178, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedToFleeDestructorBody, "Destructor", 0x4AF2D0);

    RH_ScopedInstall(Constructor, 0x4AF240);
    RH_ScopedVMTInstall(Clone, 0x4B73D0); // lote 199A (sin validar): ranura 4
}

// 0x4AF240
CEventPedToFlee::CEventPedToFlee(CPed* ped) {
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AF240
CEventPedToFlee* CEventPedToFlee::Constructor(CPed* ped) {
    this->CEventPedToFlee::CEventPedToFlee(ped);
    return this;
}

// 0x4B73D0 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x85B178).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x10 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AF240 (Constructor, con hook) con el peaton (+0xC) y devuelve la memoria.
CEvent* CEventPedToFlee::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventPedToFlee));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventPedToFlee*>(memory)->Constructor(m_ped);
}

// 0x4AF2D0
CEventPedToFlee::~CEventPedToFlee() {
    CEntity::SafeCleanUpRef(m_ped);
}
