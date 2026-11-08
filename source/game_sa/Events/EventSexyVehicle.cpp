#include "StdInc.h"
#include "EventSexyVehicle.h"

namespace {
// Cuerpo no virtual de ~CEventSexyVehicle del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventSexyVehicle del EXE (0x85B0F8), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 119A, banco Fn119A.)
void __fastcall SexyVehicleDestructorBody(CEventSexyVehicle* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B0F8;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventSexyVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventSexyVehicle, 0x85B0F8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(SexyVehicleDestructorBody, "Destructor", 0x4AF0A0);

    RH_ScopedInstall(Constructor, 0x4AF010);
}



CEventSexyVehicle::CEventSexyVehicle(CVehicle* vehicle) : CEvent() {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventSexyVehicle::~CEventSexyVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

bool CEventSexyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_vehicle;
}

// 0x4AF010
CEventSexyVehicle* CEventSexyVehicle::Constructor(CVehicle* vehicle) {
    this->CEventSexyVehicle::CEventSexyVehicle(vehicle);
    return this;
}
