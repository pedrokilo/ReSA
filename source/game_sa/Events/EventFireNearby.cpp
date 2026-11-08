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
}



CEventFireNearby::CEventFireNearby(const CVector& position) : CEventEditableResponse() {
    m_position = position;
}

bool CEventFireNearby::AffectsPed(CPed* ped) {
    return !ped->GetTaskManager().Has<TASK_COMPLEX_EXTINGUISH_FIRES>() && ped->IsAlive();
}

// 0x4B1F10
CEventFireNearby* CEventFireNearby::Constructor(const CVector& position) {
    this->CEventFireNearby::CEventFireNearby(position);
    return this;
}
