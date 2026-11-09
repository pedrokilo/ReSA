#include "StdInc.h"

#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"

#include "TaskComplexObserveTrafficLights.h"
#include "TaskSimpleAchieveHeading.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

static_assert(int32(TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING) == 0xE1); // 0x631940: mov eax,0E1h

namespace {
// 0x631910 (lote 141A, banco Fn141A): cuerpo del constructor de CTaskComplexObserveTrafficLightsAndAchieveHeading del EXE (thiscall, dos argumentos de pila, `ret 8`, 34 bytes). Base
// CTaskComplex (0x61A3B0, con hook); +0xC = primer argumento (duracion), vtable 0x86DFC4 y +0x10 = segundo argumento copiado como dword (sin pasar por el x87); devuelve this.
CTaskComplexObserveTrafficLightsAndAchieveHeading* __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) {
    CTaskComplexBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes + 0xC) = durationInMs;
    *reinterpret_cast<uint32*>(bytes) = 0x86DFC4;
    *reinterpret_cast<uint32*>(bytes + 0x10) = headingBits;
    return self;
}
} // namespace

void CTaskComplexObserveTrafficLightsAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexObserveTrafficLightsAndAchieveHeading, 0x86DFC4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(ObserveTrafficLightsAndAchieveHeadingConstructorBody, "Constructor", 0x631910); // lote 141A (banco Fn141A)

    RH_ScopedVMTInstall(GetTaskType, 0x631940); // lote 141A (banco Fn141A): mov eax,0E1h
    RH_ScopedVMTInstall(Clone, 0x636490);
    RH_ScopedVMTInstall(ControlSubTask, 0x631AD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631AC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631A70);
    RH_ScopedVMTInstall(MakeAbortable, 0x631950);
}

// 0x631910
CTaskComplexObserveTrafficLightsAndAchieveHeading::CTaskComplexObserveTrafficLightsAndAchieveHeading(int32 durationInMs, float targetHeading) : CTaskComplex() {
    m_nDurationInMs = durationInMs;
    m_fTargetHeading = targetHeading;
}

// 0x636490
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::Clone() const {
    return new CTaskComplexObserveTrafficLightsAndAchieveHeading(m_nDurationInMs, m_fTargetHeading);
}

// 0x631AD0
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x631AC0
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}

// 0x631950
bool CTaskComplexObserveTrafficLightsAndAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x631A70
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS);
    default:
        return nullptr;
    }
}

// 0x631990
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask(eTaskType taskType) const {
    switch (taskType) {
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS:
        return new CTaskComplexObserveTrafficLights; // Como el original: sin value-init, no escribe el relleno +0x16..+0x17
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(m_fTargetHeading, 0.5f, 0.2f);
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
    }
}
