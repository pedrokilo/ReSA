#include "StdInc.h"

#include "EventDraggedOutCar.h"

namespace {
// Cuerpo no virtual de ~CEventDraggedOutCar del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventDraggedOutCar del EXE (0x85AD30), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall DraggedOutCarDestructorBody(CEventDraggedOutCar* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AD30;
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    CEntity::SafeCleanUpRef(self->m_CarJacker);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventDraggedOutCar::InjectHooks() {
    RH_ScopedVirtualClass(CEventDraggedOutCar, 0x85AD30, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(DraggedOutCarDestructorBody, "Destructor", 0x4AD330);

    RH_ScopedInstall(Constructor, 0x4AD250);
}

// 0x4AD250
CEventDraggedOutCar::CEventDraggedOutCar(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat) : CEventEditableResponse() {
    m_CarJacker = carjacker;
    m_Vehicle   = vehicle;
    m_IsDriverSeat = IsDriverSeat;
    CEntity::SafeRegisterRef(m_Vehicle);
    CEntity::SafeRegisterRef(m_CarJacker);
}

// 0x4AD330
CEventDraggedOutCar::~CEventDraggedOutCar() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    CEntity::SafeCleanUpRef(m_CarJacker);
}

// 0x4AD250
CEventDraggedOutCar* CEventDraggedOutCar::Constructor(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat) {
    this->CEventDraggedOutCar::CEventDraggedOutCar(vehicle, carjacker, IsDriverSeat);
    return this;
}

// 0x4AD3A0
bool CEventDraggedOutCar::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_CarJacker;
}

// 0x4AD3C0
bool CEventDraggedOutCar::AffectsPedGroup(CPedGroup* pedGroup) {
    return FindPlayerPed() == pedGroup->GetMembership().GetLeader();
}

// 0x4B6DC0
CEventEditableResponse* CEventDraggedOutCar::CloneEditable() const noexcept {
    return new CEventDraggedOutCar(m_Vehicle, m_CarJacker, m_IsDriverSeat);
}
