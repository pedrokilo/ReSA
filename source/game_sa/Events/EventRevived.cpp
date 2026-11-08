#include "StdInc.h"
#include "EventRevived.h"

namespace {
// Cuerpo no virtual de ~CEventRevived del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall RevivedDestructorBody(CEventRevived* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventRevived::InjectHooks() {
    RH_ScopedVirtualClass(CEventRevived, 0x85B030, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(RevivedDestructorBody, "Destructor", 0x4AECA0);

    RH_ScopedInstall(Constructor, 0x4AEC50);
}



bool CEventRevived::AffectsPed(CPed* ped) {
    return !ped->IsCreatedByMission() && !ped->IsAlive();
}

// 0x4AEC50
CEventRevived* CEventRevived::Constructor() {
    this->CEventRevived::CEventRevived();
    return this;
}
