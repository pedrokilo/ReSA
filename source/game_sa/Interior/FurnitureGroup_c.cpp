/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FurnitureGroup_c
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "FurnitureGroup_c.h"
#include "FurnitureManager_c.h"

/**
 * Registra las operaciones de agrupación y consulta de mobiliario interior.
 */
void FurnitureGroup_c::InjectHooks() {
    RH_ScopedClass(FurnitureGroup_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(Init, 0x5910A0);
    RH_ScopedInstall(Exit, 0x5910B0);
    RH_ScopedInstall(AddSubGroup, 0x5910E0);
    RH_ScopedInstall(GetFurniture, 0x591130);
    RH_ScopedInstall(GetRandomId, 0x591170);
    RH_ScopedInstall(AddFurniture, 0x5C0230);
}

// 0x5910A0
int32 FurnitureGroup_c::Init() {
    return 1;
}

// 0x5910B0
// Vacía cada subgrupo antes de retirar sus nodos de la lista del grupo.
void FurnitureGroup_c::Exit() {
    rng::for_each(m_subGroupsList, &FurnitureSubGroup_c::Exit);
    m_subGroupsList.RemoveAll();
}

// 0x5910E0
/**
 * Obtiene un FurnitureSubGroup_c del almacén global y lo configura para este grupo.
 *
 * El grupo no reserva memoria dinámica para el subgrupo: FurnitureManager_c controla
 * el almacén fijo del que procede y esta lista sólo mantiene el enlace.
 */
bool FurnitureGroup_c::AddSubGroup(int32 subGroupId, int32 minWidth, int32 minDepth, int32 maxWidth, int32 maxDepth, uint8 canPlaceInFrontOfWindow, bool isTall, bool canSteal) {
    // Como el original: NewSubGroup en línea con el contador de 16 bits sin signo de 0xBAB37C (mov/inc dx, jae)
    auto& numSubGroups  = StaticRef<uint16>(0xBAB37C);
    auto& subGroupStore = StaticRef<std::array<FurnitureSubGroup_c, 128>>(0xBAD3F8);
    if (numSubGroups >= subGroupStore.size()) {
        return false;
    }
    const auto sg = &subGroupStore.data()[numSubGroups++];

    sg->m_SubGroupId = subGroupId;
    reinterpret_cast<uint8&>(sg->m_bCanPlaceInFrontOfWindow) = canPlaceInFrontOfWindow; // Como el original: copia el byte tal cual (asignar a bool lo normalizaba a 0/1)
    sg->m_bIsTall = isTall;
    sg->m_bCanSteal = canSteal;

    m_subGroupsList.AddItem(sg);

    return true;
}

// 0x591130
Furniture_c* FurnitureGroup_c::GetFurniture(int32 subGroupId, int16 furnitureId, uint8 wealth) {
    if (const auto sg = GetSubGroup(subGroupId)) {
        return sg->GetFurniture(furnitureId, wealth);
    }
    return nullptr;
}

// 0x591170
int32 FurnitureGroup_c::GetRandomId(int32 subGroupId, uint8 rating) {
    if (const auto sg = GetSubGroup(subGroupId)) {
        return sg->GetRandomId(rating);
    }
    return -1;
}

// 0x5C0230
bool FurnitureGroup_c::AddFurniture(int32 subGroupId, uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) {
    // Como el original: añade a todos los subgrupos con ese ID (no para en el primero) y siempre devuelve true
    for (auto& sg : m_subGroupsList) {
        if (sg.m_SubGroupId == subGroupId) {
            sg.AddFurniture(modelId, id, wealthMin, wealthMax, maxAng);
        }
    }
    return true;
}

// NOTSA
// Busca linealmente por ID dentro de la pequeña lista de subgrupos pertenecientes al grupo.
FurnitureSubGroup_c* FurnitureGroup_c::GetSubGroup(int32 subGroupId) {
    for (auto& sg : m_subGroupsList) {
        if (sg.m_SubGroupId == subGroupId) {
            return &sg;
        }
    }
    return nullptr;
}
