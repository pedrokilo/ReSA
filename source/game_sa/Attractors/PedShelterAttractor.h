#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedShelterAttractor final : public CPedAttractor {
public:
    static inline auto& ms_displacements = StaticRef<SArray<CVector>>(0xC0986C);

public:
    static constexpr auto Type = PED_ATTRACTOR_SHELTER;

    // 0x5EE7C0
    CPedShelterAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            5,
            1.0f,
            30000.0f,
            3000.0f,
            0.5f,
            6.28f,
            0.1f,
            0.1f
        )
    {
    }

    // 0x5EE820
    int32 GetType() const override { return Type; }

    bool IsAtHeadOfQueue(CPed* ped) override; // 0x5E9680 (lote 205A, sin validar): ranura 2 de la vtable 0x86C5B4

    // Como el original (0x5EF420): devuelve un puntero al elemento de la tabla (en eax), no un vector por valor
    static CVector& GetDisplacement(int32 pedId);
    void ComputeAttractPos(int32 pedId, CVector& outPos) override;
    void ComputeAttractHeading(int32 bQueue, float& heading) override;
    bool BroadcastDeparture(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
