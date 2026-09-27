#pragma once

#include "Base.h"

#include "ListItem_c.h"

class Furniture_c;

/**
 * Conjunto enlazado de Furniture_c que comparten reglas de colocación y categoría.
 *
 * Las entradas Furniture_c proceden de un almacén global fijo; esta clase conserva
 * únicamente la lista y las restricciones heredadas por cada elemento añadido.
 */
class FurnitureSubGroup_c : public ListItem_c<FurnitureSubGroup_c> {
public:
    static void InjectHooks();

    FurnitureSubGroup_c() = default;  // 0x590E20
    ~FurnitureSubGroup_c() = default; // 0x590E70

    bool AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng);
    Furniture_c* GetFurniture(int16 id, uint8 wealth);
    int32 GetRandomId(uint8 wealth);
    void Exit();

public:
    int32                m_SubGroupId;               //!< ID local dentro del FurnitureGroup_c.
    TList_c<Furniture_c> m_Furnitures;               //!< Mobiliario compatible enlazado desde el almacén global.
    bool                 m_bCanPlaceInFrontOfWindow; //!< Regla copiada a cada Furniture_c añadido.
    bool                 m_bIsTall;                  //!< Clasificación de altura usada al colocar interiores.
    bool                 m_bCanSteal;                //!< Habilita comportamiento de objeto robable y afecta dimensiones.
};
VALIDATE_SIZE(FurnitureSubGroup_c, 0x1C);
