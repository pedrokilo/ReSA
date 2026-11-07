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

// 0x4AF2D0
CEventPedToFlee::~CEventPedToFlee() {
    CEntity::SafeCleanUpRef(m_ped);
}
