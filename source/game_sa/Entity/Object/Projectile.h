/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Object.h"

class NOTSA_EXPORT_VTABLE CProjectile : public CObject {
public:
    CProjectile(int32 modelId);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // Envoltura del hook del constructor CProjectile(int32) (0x5A4030, thiscall, ret 4): devuelve this en EAX como el EXE.
    auto Constructor(int32 modelId) { this->CProjectile::CProjectile(modelId); return this; }
};

VALIDATE_SIZE(CProjectile, 0x17C);
