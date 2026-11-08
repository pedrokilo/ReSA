/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventChatPartner
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
}



CEventChatPartner::CEventChatPartner(bool leadSpeaker, CPed* partner) : CEvent() {
    m_leadSpeaker = leadSpeaker;
    m_partner = partner;
    CEntity::SafeRegisterRef(m_partner);
}

CEventChatPartner::~CEventChatPartner() {
    CEntity::SafeCleanUpRef(m_partner);
}

bool CEventChatPartner::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_partner;
}

// 0x4AECD0
CEventChatPartner* CEventChatPartner::Constructor(bool leadSpeaker, CPed* partner) {
    this->CEventChatPartner::CEventChatPartner(leadSpeaker, partner);
    return this;
}
