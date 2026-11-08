#pragma once

#include "TaskComplexUseClosestFreeScriptedAttractor.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseClosestFreeScriptedAttractorSprint final : public CTaskComplexUseClosestFreeScriptedAttractor {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT;

    static void InjectHooks();

    CTaskComplexUseClosestFreeScriptedAttractorSprint();
    ~CTaskComplexUseClosestFreeScriptedAttractorSprint() override = default;

    CTask*    Clone()       const override;
    eTaskType GetTaskType() const override;

private: // Wrappers for hooks
    // 0x4B8850
    CTaskComplexUseClosestFreeScriptedAttractorSprint* Constructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorSprint::CTaskComplexUseClosestFreeScriptedAttractorSprint();
        return this;
    }

    // 0x4B8900
    __declspec(noinline) CTaskComplexUseClosestFreeScriptedAttractorSprint* Destructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorSprint::~CTaskComplexUseClosestFreeScriptedAttractorSprint();
        return this;
    }
    CTaskComplexUseClosestFreeScriptedAttractorSprint* ScalarDeletingDestructor(uint8 flags); // 0x4B88E0 (lote 127A, banco Fn127A): destructor con borrado (ranura 0 de la vtable 0x85B930), `ret 4`
};
