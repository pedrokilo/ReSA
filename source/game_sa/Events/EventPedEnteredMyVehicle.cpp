#include "StdInc.h"
#include "EventPedEnteredMyVehicle.h"

namespace {
// Cuerpo no virtual de ~CEventPedEnteredMyVehicle del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPedEnteredMyVehicle del EXE (0x85AFE8), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 119A, banco Fn119A.)
void __fastcall PedEnteredMyVehicleDestructorBody(CEventPedEnteredMyVehicle* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AFE8;
    CEntity::SafeCleanUpRef(self->m_PedThatEntered);
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventPedEnteredMyVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedEnteredMyVehicle, 0x85AFE8, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedEnteredMyVehicleDestructorBody, "Destructor", 0x4AEB90);

    RH_ScopedInstall(Constructor, 0x4AEAC0);
}



CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) :
    m_Vehicle{vehicle},
    m_PedThatEntered{pedThatEntered},
    m_TargetDoor{targetDoor}
{
    // 0x4AEAC0: el EXE registra las dos referencias sin comprobar que sean no nulas (con `this` nulo 0x571B70 lee [this+0x36] y falla); se conserva (Lote 119A, banco Fn119A).
    m_PedThatEntered->RegisterReference(reinterpret_cast<CEntity**>(&m_PedThatEntered));
    m_Vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_Vehicle));
}

CEventPedEnteredMyVehicle::~CEventPedEnteredMyVehicle() {
    CEntity::SafeCleanUpRef(m_PedThatEntered);
    CEntity::SafeCleanUpRef(m_Vehicle);
}

bool CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive()
        && ped->m_pVehicle
        && ped->bInVehicle
        && ped->m_pVehicle == m_Vehicle
        && m_PedThatEntered;
}

// 0x4AEAC0
CEventPedEnteredMyVehicle* CEventPedEnteredMyVehicle::Constructor(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) {
    this->CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(pedThatEntered, vehicle, targetDoor);
    return this;
}
