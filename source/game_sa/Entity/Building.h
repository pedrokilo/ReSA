/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssE2.h"

#include "Entity.h"

/**
 * Entidad estática de tipo BUILDING almacenada en el pool dedicado.
 *
 * Nace con colisión habilitada y puede cambiar de model index liberando previamente
 * el recurso RenderWare del modelo anterior.
 */
class NOTSA_EXPORT_VTABLE CBuilding : public CEntity {
public:
    CBuilding();
    static void* operator new(unsigned size);
    static void operator delete(void* data);

public:
    static void InjectHooks();

    void ReplaceWithNewModel(int32 newModelIndex);
};

VALIDATE_SIZE(CBuilding, 0x38);

bool IsBuildingPointerValid(CBuilding* building);

// in entity.cpp
static auto& gBuildings = DatosExe::Bss::bB71804;

