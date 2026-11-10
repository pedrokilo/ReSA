/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once









class CRideAnimData {
public:
    AssocGroupId AnimGroup{};
    float        BarSteerAngle{};
    float        LeanAngle{};
    float        DesiredLeanAngle{};
    float        LeanFwd{};
    float        AnimLeanLeft{};
    float        AnimLeanFwd{};

public:
    static void InjectHooks();

    CRideAnimData() = default; // 0x6D0B10 (lote 153A, banco Fn153A: RideAnimData.cpp)
    CRideAnimData(AssocGroupId animGroup) : AnimGroup(animGroup) {} // NOTSA
};
VALIDATE_SIZE(CRideAnimData, 0x1C);
