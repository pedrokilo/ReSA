#pragma once

#include "TaskComplexUseEffect.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseEffectSprinting final : public CTaskComplexUseEffect {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_EFFECT_SPRINTING;

    static void InjectHooks();

    CTaskComplexUseEffectSprinting(C2dEffectPedAttractor* fx, CEntity* e);
    CTaskComplexUseEffectSprinting(const CTaskComplexUseEffectSprinting&);
    ~CTaskComplexUseEffectSprinting() override = default;
    CTaskComplexUseEffectSprinting* ScalarDeletingDestructor(uint8 flags); // 0x636910 (lote 141A, banco Fn141A): destructor con borrado (ranura 0 de la vtable 0x86E5C0), `ret 4`

    CTask*    Clone() const override { return new CTaskComplexUseEffectSprinting{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
};
