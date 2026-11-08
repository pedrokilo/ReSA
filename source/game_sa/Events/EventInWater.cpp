#include "StdInc.h"
#include "EventInWater.h"

namespace {
// Cuerpo no virtual de ~CEventInWater del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall InWaterDestructorBody(CEventInWater* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventInWater::InjectHooks() {
    RH_ScopedVirtualClass(CEventInWater, 0x85B4C8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(InWaterDestructorBody, "Destructor", 0x4B13C0);

    RH_ScopedInstall(Constructor, 0x4B1370);
}



CEventInWater::CEventInWater(float acceleration) {
    m_acceleration = acceleration;
}

bool CEventInWater::AffectsPed(CPed* ped) {
    CTask* task = ped->GetTaskManager().GetActiveTask();
    if (!ped->IsPlayer() && task && task->GetTaskType() == TASK_COMPLEX_IN_WATER)
        return false;
    return ped->IsAlive();
}

bool CEventInWater::TakesPriorityOver(const CEvent& refEvent) {
    switch (refEvent.GetEventType()) {
    case EVENT_KNOCK_OFF_BIKE:
    case EVENT_DAMAGE:
    case EVENT_STUCK_IN_AIR:
        if (m_acceleration > 1.0f) {
            return true;
        }
        break;
    }
    return CEvent::TakesPriorityOver(refEvent);
}

// 0x4B1370
CEventInWater* CEventInWater::Constructor(float acceleration) {
    this->CEventInWater::CEventInWater(acceleration);
    return this;
}
