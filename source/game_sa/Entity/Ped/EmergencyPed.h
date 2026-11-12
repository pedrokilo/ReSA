/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEmergencyPed : public CPed {
public:
    CEmergencyPed(ePedType pedType, uint32 modelIndex);
    ~CEmergencyPed() override = default;

    void MedicAI();
    void FiremanAI();
    void ProcessControl() override;
    bool InRange(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEmergencyPed* Constructor(ePedType pedType, uint32 modelIndex); // 0x5DE340 (lote 178A, sin validar)
};

VALIDATE_SIZE(CEmergencyPed, 0x79C);
