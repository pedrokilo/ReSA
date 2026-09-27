/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Building.h"

/**
 * Building cuyo RpClump requiere actualización de frame y HAnim antes del render.
 */
class NOTSA_EXPORT_VTABLE CAnimatedBuilding : public CBuilding {
public:
    CAnimatedBuilding() : CBuilding() {}

public:
    static void InjectHooks();

    void ProcessControl() override;
    void ProcessCollision() override;
    void PreRender() override;

private:
};

VALIDATE_SIZE(CAnimatedBuilding, 0x38);
