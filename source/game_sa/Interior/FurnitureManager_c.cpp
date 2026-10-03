/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FurnitureManager_c
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "FurnitureManager_c.h"


/**
 * Registra el ciclo de vida, carga y consultas del catálogo global de mobiliario.
 */
void FurnitureManager_c::InjectHooks() {
    RH_ScopedClass(FurnitureManager_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(GetGroupId, 0x5BFB40);
    RH_ScopedInstall(GetSubGroupId, 0x5BFBF0);
    RH_ScopedInstall(Exit, 0x5911A0);
    RH_ScopedInstall(GetFurniture, 0x5911E0);
    RH_ScopedInstall(GetRandomId, 0x591220);
    RH_ScopedInstall(LoadFurniture, 0x5C0280);
    RH_ScopedInstall(Init, 0x5C04A0);
}

// 0x5C04A0
/**
 * Reinicia los stores, enlaza los FurnitureItem preasignados e inicializa grupos.
 * Después carga data\\furnitur.dat para poblar la jerarquía grupo/subgrupo/item.
 */
bool FurnitureManager_c::Init() {
    // Como el original: los contadores son words (mov word ptr)
    g_currSubGroupId = 0;
    g_currFurnitureId = 0;

    for (auto& i : m_FurnitureItem) {
        m_FurnitureList.AddItem(&i);
    }
    
    rng::for_each(m_Groups, &FurnitureGroup_c::Init);

    LoadFurniture();

    return true;
}

// 0x5911A0
void FurnitureManager_c::Exit() {
    m_FurnitureList.RemoveAll();
    rng::for_each(m_Groups, &FurnitureGroup_c::Exit); // Como el original: Exit de cada grupo en línea
}

// NOTSA
// Entrega secuencialmente una entrada del store fijo; nullptr indica agotamiento.
FurnitureSubGroup_c* FurnitureManager_c::NewSubGroup() {
    return g_currSubGroupId < std::size(g_subGroupStore)
        ? &g_subGroupStore[g_currSubGroupId++]
        : nullptr;
}

// 0x5C0280
/**
 * Parsea data\\furnitur.dat manteniendo el contexto del GROUP y SUBGROUP actuales.
 *
 * Las líneas ITEM se resuelven a model IDs y se delegan al subgrupo vigente. Los
 * objetos resultantes viven en stores fijos, por lo que el parser no transfiere
 * ownership mediante asignaciones individuales.
 */
void FurnitureManager_c::LoadFurniture() {
    auto file = CFileMgr::OpenFile("data\\furnitur.dat", "rb");

    /**
    * GROUP:    <group name>
    * SUBGROUP: <sub-group name> <min width> <min depth> <max width> <max depth> <can place in front of Window> <is tall> <is stealable>
    * ITEM:	    <model name> <item id> <min wealth> <max wealth> <max angle>
    * <Items....>
    *
    * A sub-group can have multiple items.
    * A group can't direcly have items, only a subgroup can.
    **/

    // Como el original: guarda solo los IDs del grupo (-1 al inicio) y del subgrupo actuales, no valida
    // los resultados de sscanf ni de las búsquedas, ignora etiquetas desconocidas y añade los ITEM
    // a través del grupo (todos los subgrupos con ese ID).
    int32 grpId = -1;
    int32 subGrpId = 0; // El EXE lo deja sin inicializar (solo importa con un ITEM antes de un SUBGROUP); evita el aviso /RTCu de Debug
    while (const auto l = CFileLoader::LoadLine(file)) {
        if (*l == '#' || *l == 0) { // Skip empty and comment lines
            continue;
        }

        char tag[96];
        sscanf(l, "%s", tag);

        if (!strcmp(tag, "GROUP:")) {
            char groupName[64];
            sscanf(l, "%s %s", tag, groupName);
            grpId = GetGroupId(groupName);
        } else if (!strcmp(tag, "SUBGROUP:")) {
            char  subGroupName[64];
            int32 minWidth, minDepth, maxWidth, maxDepth, canPlaceInFrontOfWindow, isTall, canSteal;
            sscanf(l, "%s %s %d %d %d %d %d %d %d", tag, subGroupName, &minWidth, &minDepth, &maxWidth, &maxDepth, &canPlaceInFrontOfWindow, &isTall, &canSteal);
            subGrpId = GetSubGroupId(subGroupName);
            m_Groups.data()[grpId].AddSubGroup(subGrpId, minWidth, minDepth, maxWidth, maxDepth, (uint8)canPlaceInFrontOfWindow, isTall != 0, canSteal != 0);
        } else if (!strcmp(tag, "ITEM:")) {
            char  modelName[64];
            int32 id, wealthMin, wealthMax, maxAng;
            int32 modelIndex;
            sscanf(l, "%s %s %d %d %d %d", tag, modelName, &id, &wealthMin, &wealthMax, &maxAng);
            if (CModelInfo::GetModelInfo(modelName, &modelIndex)) {
                m_Groups.data()[grpId].AddFurniture(subGrpId, (uint16)modelIndex, (int16)id, (uint8)wealthMin, (uint8)wealthMax, (uint8)maxAng);
            }
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x5BFB40
int32 FurnitureManager_c::GetGroupId(const char* name) {
    static const auto groups = notsa::make_mapping<std::string_view, int32>({
        {"IT_SHOP",    0},
        {"IT_OFFICE",  1},
        {"IT_LOUNGE",  2},
        {"IT_BEDROOM", 3},
        {"IT_KITCHEN", 4},
        {"IT_MISC",    8},
    });
    return notsa::find_value_or(groups, name, -1);
}

// 0x5BFBF0
int32 FurnitureManager_c::GetSubGroupId(const char* name) {
    static const auto subGroups = notsa::make_mapping<std::string_view, int32>({
        // IT_SHOP
        {"SHOP_UNIT1_L",          0 },
        { "SHOP_UNIT1_R",         1 },
        { "SHOP_UNIT1_M",         2 },
        { "SHOP_UNIT2_L",         3 },
        { "SHOP_UNIT2_R",         4 },
        { "SHOP_UNIT2_M",         5 },
        { "SHOP_UNIT3_L",         6 },
        { "SHOP_UNIT3_R",         7 },
        { "SHOP_UNIT3_M",         8 },
        { "SHOP_UNIT4_L",         9 },
        { "SHOP_UNIT4_R",         10},
        { "SHOP_UNIT4_M",         11},
        { "SHOP_CHECKOUTS",       12},
        { "SHOP_BASKETS",         13},
        { "SHOP_LIGHTS",          14},

        // IT_OFFICE
        { "OFFICE_DESKS",         0 },
        { "OFFICE_CHAIRS",        1 },
        { "OFFICE_WATERCOOLERS",  2 },
        { "OFFICE_FILINGCABS",    3 },

        // IT_LOUNGE
        { "LOUNGE_SOFAS",         0 },
        { "LOUNGE_CHAIRS",        1 },
        { "LOUNGE_TVSTANDS",      2 },
        { "LOUNGE_TVS",           3 },
        { "LOUNGE_TABLES",        4 },
        { "LOUNGE_LAMPS",         5 },
        { "LOUNGE_HIFI_TABLES",   6 },
        { "LOUNGE_VIDEOS",        7 },
        { "LOUNGE_HIFIS",         8 },
        { "LOUNGE_CONSOLES",      9 },

        // IT_BEDROOM
        { "BEDROOM_BEDS",         0 },
        { "BEDROOM_TABLES",       1 },
        { "BEDROOM_DRAWERS",      2 },
        { "BEDROOM_WARDROBES",    3 },

        // IT_KITCHEN
        { "KITCHEN_UNIT_L",       0 },
        { "KITCHEN_UNIT_M",       1 },
        { "KITCHEN_UNIT_R",       2 },
        { "KITCHEN_UNIT_SINKS",   3 },
        { "KITCHEN_UNIT_FRIDGES", 4 },
        { "KITCHEN_UNIT_OVENS",   5 },
        { "KITCHEN_UNIT_WASHERS", 6 },
        { "KITCHEN_UNIT_CORNERS", 7 },
        { "KITCHEN_MICROWAVES",   8 },
        { "KITCHEN_TOASTERS",     9 },
        { "KITCHEN_TVS",          10},

        // IT_MISC
        { "MISC_PLANTS",          0 },
        { "MISC_LIGHTS",          1 },
        { "MISC_RUGS",            2 },
        { "MISC_PLATES",          3 },
        { "MISC_FOOD",            4 },
        { "MISC_MAGAZINES",       5 },
        { "MISC_CLOTHES",         6 },
    });
    return notsa::find_value_or(subGroups, name, -1);
}

// 0x5911E0
Furniture_c* FurnitureManager_c::GetFurniture(int32 groupId, int32 subGroupId, int16 id, uint8 wealth) {
    // Como el original: sin comprobar el rango de groupId
    return m_Groups.data()[groupId].GetFurniture(subGroupId, id, wealth);
}

// 0x591220
int32 FurnitureManager_c::GetRandomId(int32 groupId, int32 subGroupId, uint8 wealth) {
    return m_Groups.data()[groupId].GetRandomId(subGroupId, wealth);
}
