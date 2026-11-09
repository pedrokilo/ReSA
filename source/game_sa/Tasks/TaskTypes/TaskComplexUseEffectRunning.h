#pragma once

#include "TaskComplexUseEffect.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseEffectRunning final : public CTaskComplexUseEffect {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_EFFECT_RUNNING;

    static void InjectHooks();

    CTaskComplexUseEffectRunning(C2dEffectPedAttractor* fx, CEntity* e);
    CTaskComplexUseEffectRunning(const CTaskComplexUseEffectRunning&);
    ~CTaskComplexUseEffectRunning() override = default;
    CTaskComplexUseEffectRunning* ScalarDeletingDestructor(uint8 flags); // 0x636830 (lote 141A, banco Fn141A): destructor con borrado (ranura 0 de la vtable 0x86E594), `ret 4`

    CTask*    Clone() const override { return new CTaskComplexUseEffectRunning{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
};
