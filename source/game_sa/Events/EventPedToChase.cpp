#include "StdInc.h"

#include "EventPedToChase.h"

namespace {
// Cuerpo no virtual de ~CEventPedToChase del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPedToChase del EXE (0x85B138), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall PedToChaseDestructorBody(CEventPedToChase* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B138;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventPedToChase::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedToChase, 0x85B138, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedToChaseDestructorBody, "Destructor", 0x4AF1C0);

    RH_ScopedInstall(Constructor, 0x4AF130);
}

// 0x4AF130
CEventPedToChase::CEventPedToChase(CPed* ped) {
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AF130
CEventPedToChase* CEventPedToChase::Constructor(CPed* ped) {
    this->CEventPedToChase::CEventPedToChase(ped);
    return this;
}

// 0x4AF1C0
CEventPedToChase::~CEventPedToChase() {
    CEntity::SafeCleanUpRef(m_ped);
}
