/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventChatPartner
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
#include "EventChatPartner.h"

namespace {
// Cuerpo no virtual de ~CEventChatPartner del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventChatPartner del EXE (0x85B070), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 119A, banco Fn119A.)
void __fastcall ChatPartnerDestructorBody(CEventChatPartner* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B070;
    CEntity::SafeCleanUpRef(self->m_partner);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventChatPartner::InjectHooks() {
    RH_ScopedVirtualClass(CEventChatPartner, 0x85B070, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(ChatPartnerDestructorBody, "Destructor", 0x4AED60);

    RH_ScopedInstall(Constructor, 0x4AECD0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AEDC0); // lote 199A (sin validar): ranura 5
    RH_ScopedVMTInstall(Clone, 0x4B7210);      // lote 199A (sin validar): ranura 4
}



CEventChatPartner::CEventChatPartner(bool leadSpeaker, CPed* partner) : CEvent() {
    m_leadSpeaker = leadSpeaker;
    m_partner = partner;
    CEntity::SafeRegisterRef(m_partner);
}

CEventChatPartner::~CEventChatPartner() {
    CEntity::SafeCleanUpRef(m_partner);
}

// 0x4AEDC0 (lote 199A, sin validar; thiscall, un argumento de pila, `ret 4`, 35 bytes; ranura 5 de la vtable 0x85B070): CPed::IsAlive (0x5E0170) del peaton y companero (+0x10) no nulo. Devuelve AL (0 o 1).
bool CEventChatPartner::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_partner;
}

// 0x4B7210 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 103 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x85B070).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AECD0 (Constructor, con hook) con la bandera del hablante principal
// (byte +0xC, ampliado a cero) y el companero (+0x10), y devuelve la memoria.
CEventChatPartner* CEventChatPartner::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventChatPartner));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventChatPartner*>(memory)->Constructor(m_leadSpeaker, m_partner);
}

// 0x4AECD0
CEventChatPartner* CEventChatPartner::Constructor(bool leadSpeaker, CPed* partner) {
    this->CEventChatPartner::CEventChatPartner(leadSpeaker, partner);
    return this;
}
