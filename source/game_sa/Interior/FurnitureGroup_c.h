#pragma once

#include "Base.h"

#include "List_c.h"

class Furniture_c;
class FurnitureSubGroup_c;

/**
 * Agrupa FurnitureSubGroup_c de una misma categoría de interior.
 *
 * Actúa como punto de despacho para búsquedas por subGroupId. Los subgrupos proceden
 * del almacén fijo administrado por FurnitureManager_c y se enlazan aquí sin cambiar
 * su ubicación en memoria.
 */
class FurnitureGroup_c {
public:
    static void InjectHooks();

    FurnitureGroup_c() = default;  // 0x590DE0
    ~FurnitureGroup_c() = default; // 0x590DF0

    int32 Init();
    void Exit();
    bool AddSubGroup(int32 subGroupId, int32 minWidth, int32 minDepth, int32 maxWidth, int32 maxDepth, uint8 canPlaceInFrontOfWindow, bool isTall, bool canSteal);
    Furniture_c* GetFurniture(int32 subGroupId, int16 furnitureId, uint8 rating);
    int32 GetRandomId(int32 subGroupId, uint8 rating);
    bool AddFurniture(int32 subGroupId, uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng);
    FurnitureSubGroup_c* GetSubGroup(int32 subGroupId);

private:
    TList_c<FurnitureSubGroup_c> m_subGroupsList; //!< Subgrupos activos pertenecientes a esta categoría.
};
VALIDATE_SIZE(FurnitureGroup_c, 0xC);
