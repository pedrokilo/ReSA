/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Entity.h"

/**
 * Entidad ligera del mundo almacenada en CDummyPool.
 *
 * Comparte la infraestructura de CEntity pero evita el coste de CPhysical cuando
 * sólo se necesita una representación estática/dummy para streaming.
 */
class NOTSA_EXPORT_VTABLE CDummy : public CEntity {
public:
    CDummy();
    ~CDummy() override = default;

    static void* operator new(size_t size);
    static void  operator delete(void* obj);

public:
    static void InjectHooks();
};
VALIDATE_SIZE(CDummy, 0x38);

bool IsDummyPointerValid(CDummy* dummy);
