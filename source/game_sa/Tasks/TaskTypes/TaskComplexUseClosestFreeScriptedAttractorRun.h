#pragma once

#include "TaskComplexUseClosestFreeScriptedAttractor.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseClosestFreeScriptedAttractorRun final : public CTaskComplexUseClosestFreeScriptedAttractor {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN;

    static void InjectHooks();

    CTaskComplexUseClosestFreeScriptedAttractorRun();
    ~CTaskComplexUseClosestFreeScriptedAttractorRun() override = default;

    CTask*    Clone()       const override { return new CTaskComplexUseClosestFreeScriptedAttractorRun{}; } // 0x4B87B0: a fresh task, nothing copied
    eTaskType GetTaskType() const override { return Type; }

private: // Wrappers for hooks
    // 0x4B8790
    CTaskComplexUseClosestFreeScriptedAttractorRun* Constructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorRun::CTaskComplexUseClosestFreeScriptedAttractorRun();
        return this;
    }

    // 0x4B8840
    __declspec(noinline) CTaskComplexUseClosestFreeScriptedAttractorRun* Destructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorRun::~CTaskComplexUseClosestFreeScriptedAttractorRun();
        return this;
    }
    CTaskComplexUseClosestFreeScriptedAttractorRun* ScalarDeletingDestructor(uint8 flags); // 0x4B8820 (lote 127A, banco Fn127A): destructor con borrado (ranura 0 de la vtable 0x85B904), `ret 4`
};
