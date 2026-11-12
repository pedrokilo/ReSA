/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Dummy.h"

class NOTSA_EXPORT_VTABLE CDummyPed : public CDummy {
public:
    static void InjectHooks();
};

VALIDATE_SIZE(CDummyPed, 0x38);