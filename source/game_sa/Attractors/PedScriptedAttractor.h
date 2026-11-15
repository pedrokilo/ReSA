#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedScriptedAttractor final : public CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_SCRIPTED;

    // 0x5EEA30
    CPedScriptedAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
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

    // 0x5EEA90
    ePedAttractorType GetType() const override { return Type; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // Envolturas para los hooks (lote 221A, sin validar)
    CPedScriptedAttractor* Constructor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState); // 0x5EEA30
    __declspec(noinline) void Destructor();                                               // 0x5EEAF0
    CPedScriptedAttractor* ScalarDeletingDestructor(uint8 flags);                                           // 0x5EEAA0
};
