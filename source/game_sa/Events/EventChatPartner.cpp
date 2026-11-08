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
