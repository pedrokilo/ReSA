#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedStepAttractor : public  CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_STEP;

    // 0x5EEBD0
    CPedStepAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            1,
            1.0f,
            30000.0f,
            3000.0f,
            0.125f,
            0.1f,
            0.1f,
            0.1f
        )
    {
    }

    // 0x5EEC30
    int32 GetType() const override { return Type; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // Envolturas para los hooks (lote 221A, sin validar)
    CPedStepAttractor* Constructor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState); // 0x5EEBD0
    __declspec(noinline) void Destructor();                                               // 0x5EEC90
    CPedStepAttractor* ScalarDeletingDestructor(uint8 flags);                                           // 0x5EEC40
};
