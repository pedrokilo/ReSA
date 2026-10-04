/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CShopping
 * Reconstrucción: COMPLETA — 30/30 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 30/30 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Shopping.h"
#include "PedClothesDesc.h"

void CShopping::InjectHooks() {
    RH_ScopedClass(CShopping);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49C290);
    RH_ScopedInstall(ShutdownForRestart, 0x49B640);
    RH_ScopedInstall(Buy, 0x49BF70);
    RH_ScopedInstall(FindItem, 0x49AD20);
    RH_ScopedInstall(FindSection, 0x49AE70);
    RH_ScopedInstall(FindSectionInSection, 0x49AF90);
    RH_ScopedInstall(GetExtraInfo, 0x49ADE0);
    RH_ScopedInstall(GetItemIndex, 0x49AB10);
    RH_ScopedInstall(GetKey, 0x49AB30);
    RH_ScopedInstall(GetNameTag, 0x49ADA0);
    RH_ScopedInstall(GetNextSection, 0x49AF10);
    RH_ScopedInstall(GetPrice, 0x49AD50);
    RH_ScopedInstall(GetPriceSectionFromName, 0x49AAD0);
    RH_ScopedInstall(SetPlayerHasBought, 0x49B610);
    RH_ScopedInstall(HasPlayerBought, 0x49B5E0);
    RH_ScopedInstall(IncrementStat, 0x49BE70);
    RH_ScopedInstall(IncrementStat2, 0x49AFD0);
    RH_ScopedInstall(LoadPrices, 0x49B8D0);
    RH_ScopedInstall(LoadShop, 0x49BBE0);
    RH_ScopedInstall(LoadStats, 0x49B6A0);
    RH_ScopedInstall(RemoveLoadedPrices, 0x49AC90);
    RH_ScopedInstall(RemoveLoadedShop, 0x49AE30);
    RH_ScopedInstall(StoreClothesState, 0x49B200);
    RH_ScopedInstall(StoreVehicleMods, 0x49B280);
    RH_ScopedInstall(RestoreClothesState, 0x49B240);
    RH_ScopedInstall(RestoreVehicleMods, 0x49B3C0);
    RH_ScopedInstall(UpdateStats, 0x49BEF0);
    RH_ScopedInstall(Load, 0x5D3E40);
    RH_ScopedInstall(Save, 0x5D3DE0);

    // Sin hook (fuera de la lista validada): AddPriceModifier 0x49C2D0 (nombre) / 0x49BDD0 (clave),
    // RemovePriceModifier 0x49BE50 (nombre) / 0x49ACD0 (clave).

    // unused
    RH_ScopedInstall(SetCurrentProperty, 0x49B1F0);
}

/*!
 * @brief Get changing stat index from name. (Used only in CShopping::LoadStats)
 * @addr 0x49ABD0
 * @param stat Changing stat name.
 * @returns Stat index from the list provided in the function.
 */
int32 GetChangingStatIndex(const char* stat) {
    static constexpr const char* statNames[] = {"fat", "respect", "sexy", "health", "stamina", "calories"};

    // todo: use rngv::enumerate
    for (auto i = 0u; i < std::size(statNames); i++) {
        if (!strcmp(statNames[i], stat)) {
            return i;
        }
    }

    return -1;
}

// 0x49C290
void CShopping::Init() {
    rng::fill(ms_keys, 0u);
    rng::fill(ms_bHasBought, false);
    ms_numPrices = 0;
    ms_numPriceModifiers = 0;
    ms_numBuyableItems = 0u;
    gClothesHaveBeenStored = 0;

    LoadStats();
}

// 0x49B640
void CShopping::ShutdownForRestart() {
    RemoveLoadedShop();

    ms_numPriceModifiers = 0;
    rng::fill(ms_bHasBought, false);
}

// 0x49BF70
void CShopping::Buy(uint32 key, int32 extraInfo) {
    // Como el original: el índice absoluto se busca con la clave del precio encontrado (FindItem puede devolver -1).
    const auto& priceInfo = ms_prices.data()[FindItem(key)];
    const auto index = GetItemIndex(priceInfo.key);

    auto ped = FindPlayerPed();
    auto& playerInfo = FindPlayerInfo();

    auto playerClothes = ped->GetClothesDesc();
    if (gClothesHaveBeenStored) {
        playerClothes = &gStoredClothesState;
    }

    const auto price = GetPrice(key);
    playerInfo.m_nMoney -= price;

    const auto& modifiers = ms_statModifiers.data()[index].modifiers;
    IncrementStat(modifiers[0].statIndex, modifiers[0].change);
    IncrementStat(modifiers[1].statIndex, modifiers[1].change);

    const auto fPrice = static_cast<float>(price);
    CStats::ModifyStat(STAT_TOTAL_SHOPPING_BUDGET, fPrice);
    ms_bHasBought.data()[index] = true;

    switch (ms_priceSectionLoaded) {
    case PRICE_SECTION_CAR_MODS: {
        // Como el original: primero la estadística y después la mejora (0x49B0E0, inline).
        CStats::ModifyStat(STAT_CAR_MODIFICATION_BUDGET, fPrice);

        const auto veh = FindPlayerVehicle();
        const auto upgradeModel = CModelInfo::ms_modelInfoPtrs[key];
        const auto vehModel = CModelInfo::ms_modelInfoPtrs[veh->m_nModelIndex]->AsVehicleModelInfoPtr();
        veh->AddVehicleUpgrade(key);
        if (!veh->IsAutomobile()) {
            break;
        }
        const auto car = veh->AsAutomobile();

        const auto carModId = upgradeModel->CarMod;
        const auto parentId = upgradeModel->bUsesVehDummy
            ? (int32)carModId
            : vehModel->m_pVehicleStruct->m_aUpgrades.data()[carModId].m_nParentComponentId;

        switch (parentId) {
        case CAR_WHEEL_RF:
            car->FixTyre((eWheels)CAR_WHEEL_FRONT_LEFT);
            car->FixTyre((eWheels)CAR_WHEEL_FRONT_RIGHT);
            car->FixTyre((eWheels)CAR_WHEEL_REAR_LEFT);
            car->FixTyre((eWheels)CAR_WHEEL_REAR_RIGHT);
            break;
        case CAR_BUMP_FRONT:
            car->FixPanel(CAR_BUMP_FRONT, FRONT_BUMPER);
            break;
        case CAR_BUMP_REAR:
            car->FixPanel(CAR_BUMP_REAR, REAR_BUMPER);
            break;
        case CAR_BONNET:
            car->FixDoor(CAR_BONNET, DOOR_BONNET);
            break;
        case CAR_BOOT:
            car->FixDoor(CAR_BOOT, DOOR_BOOT);
            break;
        default:
            break;
        }
        break;
    }

    case PRICE_SECTION_CAR_PAINTJOBS:
        CStats::ModifyStat(STAT_CAR_MODIFICATION_BUDGET, fPrice);
        break;

    case PRICE_SECTION_CLOTHES: {
        CStats::ModifyStat(STAT_FASHION_BUDGET, fPrice);

        const auto texKey = playerClothes->m_anTextureKeys.data()[priceInfo.clothes.type];
        if (texKey) {
            UpdateStats(GetItemIndex(texKey), false);
        }
        ped->GetClothesDesc()->SetTextureAndModel(key, priceInfo.clothes.modelKey, (eClothesTexturePart)priceInfo.clothes.type);

        switch (CClothes::GetTextureDependency((eClothesTexturePart)priceInfo.clothes.type)) {
        case CLOTHES_MODEL_TORSO:
        case CLOTHES_MODEL_LEGS:
        case CLOTHES_MODEL_HEAD:
        case CLOTHES_MODEL_GLASSES:
        case CLOTHES_MODEL_HATS:
            FindPlayerWanted()->ClearWantedLevelAndGoOnParole();
            break;
        default:
            break;
        }
        break;
    }

    case PRICE_SECTION_HAIRCUTS: {
        CStats::ModifyStat(STAT_HAIRDRESSING_BUDGET, fPrice);

        const auto texKey = playerClothes->m_anTextureKeys.data()[priceInfo.clothes.type];
        if (texKey) {
            UpdateStats(GetItemIndex(texKey), false);
        }
        ped->GetClothesDesc()->SetTextureAndModel(key, priceInfo.clothes.modelKey, (eClothesTexturePart)priceInfo.clothes.type);

        FindPlayerWanted()->ClearWantedLevelAndGoOnParole();
        break;
    }

    case PRICE_SECTION_TATTOOS:
        CStats::ModifyStat(STAT_TATTOO_BUDGET, fPrice);

        ped->GetClothesDesc()->SetTextureAndModel(key, 0u, (eClothesTexturePart)priceInfo.tattoos.type1);
        break;

    case PRICE_SECTION_GIFTS:
        ped->GiveWeapon(WEAPON_FLOWERS, 0, false);
        break;

    case PRICE_SECTION_FOOD:
        // Como el original: primero la comida contada y después el presupuesto.
        CStats::ModifyStat(STAT_NUMBER_OF_MEALS_EATEN, 1.0f);
        CStats::ModifyStat(STAT_FOOD_BUDGET, fPrice);
        break;

    case PRICE_SECTION_WEAPONS:
        CStats::ModifyStat(STAT_WEAPON_BUDGET, fPrice);

        if (key == WEAPON_ARMOUR) {
            ped->m_fArmour = static_cast<float>(FindPlayerInfo().m_nMaxArmour);
        } else {
            ped->GiveWeapon((eWeaponType)key, priceInfo.weapon.ammo, false);
            ped->SetCurrentWeapon(ped->GetWeaponSlot((eWeaponType)key)); // Como el original: GetWeaponSlot (0x5DF200) + SetCurrentWeapon(slot) (0x5E61F0)
        }
        break;

    default:
        break;
    }
}

/*!
 * @brief Find item by searching through ms_prices.
 * @addr 0x49AD20 (inlined)
 * @param item Key ID of the item.
 * @returns 'Buyable' index of the item, -1 if not found
 */
int32 CShopping::FindItem(uint32 itemKey) {
    for (auto i = 0; i < ms_numPrices; i++) {
        if (ms_prices.data()[i].key == itemKey) {
            return i;
        }
    }

    return -1; // Como el original
}

/*!
 * @brief Find a section inside the file.
 * @addr 0x49AE70
 * @param file A shopping.dat-like formatted file.
 * @param The section to find
 * @returns If the section was found. The file's read pointer will be right after the section's beginning (after the `section` line)
 */
bool CShopping::FindSection(FILESTREAM file, const char* sectionName) {
    auto counter = 0;
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0')
            continue;

        if (!strncmp(line, "section", 7u)) {
            counter++;
            // Como el original: strtok (0x82244B) con estado global.
            RET_IGNORED(strtok(line, " \t"));
            const auto name = strtok(nullptr, " \t");

            if (counter == 1 && !_stricmp(sectionName, name)) {
                return true;
            }
        } else if (!strncmp(line, "end", 3u) && --counter < 0) {
            break;
        }
    }

    return false;
}

// 0x49AF90 (unused)
bool CShopping::FindSectionInSection(FILESTREAM file, const char* parentSection, const char* childSection) {
    return FindSection(file, parentSection) && FindSection(file, childSection);
}

// 0x49ADE0
int32 CShopping::GetExtraInfo(uint32 itemKey, int32 index) {
    const auto id = FindItem(itemKey);
    switch (index) {
    case 0:
        return ms_prices.data()[id].general.extra1;
    case 1:
        return ms_prices.data()[id].general.extra2;
    default:
        break;
    }

    return 0;
}

/*!
 * @brief Find item by searching through ms_keys.
 * @addr 0x49AB10
 * @param itemKey Key ID of the item.
 * @returns 'Absolute' index of the item, -1 if not found
 */
int32 CShopping::GetItemIndex(uint32 itemKey) {
    for (auto i = 0; i < (int32)NUM_ITEMS; i++) {
        if (ms_keys[i] == itemKey) {
            return i;
        }
    }

    return -1; // Como el original
}

// 0x49AB30
uint32 CShopping::GetKey(const char* modelName, ePriceSection section) {
    switch (section) {
    case PRICE_SECTION_CLOTHES:
    case PRICE_SECTION_HAIRCUTS:
    case PRICE_SECTION_TATTOOS:
        return CKeyGen::GetUppercaseKey(modelName);
    case PRICE_SECTION_WEAPONS:
        return CWeaponInfo::FindWeaponType(modelName);
    case PRICE_SECTION_CAR_PAINTJOBS:
        return PRICE_SECTION_CAR_PAINTJOBS;
    default:
        break;
    }

    int32 ret = -1;
    CModelInfo::GetModelInfo(modelName, &ret);
    return static_cast<uint32>(ret);
}

// 0x49ADA0 (inlined)
const char* CShopping::GetNameTag(uint32 itemKey) {
    return ms_prices.data()[FindItem(itemKey)].nameTag;
}

/*!
 * @brief Finds the next 'root' section name from the file
 * @addr 0x49AF10
 * @param file A shopping.dat-like formatted file.
 * @returns The file's read pointer right after the next section, nullptr if no next section is available.
 */
const char* CShopping::GetNextSection(FILESTREAM file) {
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0')
            continue;

        if (!strncmp(line, "section", 7u)) {
            // Como el original: strtok (0x82244B) con estado global.
            RET_IGNORED(strtok(line, " \t"));
            return strtok(nullptr, " \t");
        }

        if (!strncmp(line, "end", 3u)) {
            return nullptr;
        }
    }

    return nullptr;
}

// 0x49AD50
int32 CShopping::GetPrice(uint32 itemKey) {
    // Como el original: fild del precio con signo, fmul por el multiplicador del nivel actual y _ftol2 (truncado de 64 bits).
    const auto price = static_cast<int32>(ms_prices.data()[FindItem(itemKey)].price);
    const auto value = static_cast<float>(price) * gPriceMultipliers.data()[CTheZones::m_CurrLevel];
    return static_cast<int32>(static_cast<int64>(value));
}

// 0x49AAD0
ePriceSection CShopping::GetPriceSectionFromName(const char* name) {
    for (auto i = 0; i < (int32)NUM_SECTION_NAMES; i++) {
        if (!_stricmp(name, ms_sectionNames[i])) {
            return static_cast<ePriceSection>(i);
        }
    }

    return static_cast<ePriceSection>(-1); // Como el original
}

// 0x49B610
void CShopping::SetPlayerHasBought(uint32 itemKey) {
    ms_bHasBought.data()[GetItemIndex(itemKey)] = true; // Como el original: índice -1 si no existe
}

// 0x49B5E0
bool CShopping::HasPlayerBought(uint32 itemKey) {
    return ms_bHasBought.data()[GetItemIndex(itemKey)]; // Como el original: índice -1 si no existe
}

/*!
 * @brief Increments the stat from stat index
 * @addr 0x49BE70
 * @param statIndex Index of a stat list described in the function.
 * @param change Change value of the stat.
 */
void CShopping::IncrementStat(uint32 statIndex, int32 change) {
    if (statIndex == -1)
        return;

    static constexpr eStats indexStats[] = {
        STAT_FAT, STAT_CLOTHES_RESPECT, STAT_APPEARANCE,
        STAT_RIOT_MISSION_ACCOMPLISHED, STAT_STAMINA, STAT_CALORIES
    };

    IncrementStat2(indexStats[statIndex], change);
}

/*!
 * @brief Increments the stat from real stat id
 * @addr 0x49AFD0 (inlined)
 * @param stat Stat id (dword completo como el original)
 * @param change Change value of the stat.
 */
void CShopping::IncrementStat2(int32 stat, int32 change) {
    if (stat == -1) // Como el original: sale sin hacer nada
        return;

    if (stat == STAT_CLOTHES_RESPECT || stat == STAT_APPEARANCE) {
        change *= 10;
    }

    CStats::ModifyStat(static_cast<eStats>(stat), static_cast<float>(change));
}

// 0x49B8D0
void CShopping::LoadPrices(const char* sectionName) {
    const auto priceSection = GetPriceSectionFromName(sectionName);
    if (priceSection == ms_priceSectionLoaded)
        return;

    if (ms_priceSectionLoaded != PRICE_SECTION_NONE) {
        RemoveLoadedPrices();
    }
    ms_priceSectionLoaded = priceSection;
    ms_numPrices = 0;
    ms_numItemsInShop = 0;
    CTimer::Suspend();

    const auto file = CFileMgr::OpenFile("data\\shopping.dat", "r");
    if (FindSection(file, "prices")) {
        FindSection(file, sectionName);
    }

    // Como el original: las líneas vacías o de comentario no cuentan como precio.
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0')
            continue;

        if (!strncmp(line, "end", 3u))
            break;

        auto& priceInfo = ms_prices.data()[ms_numPrices];

        const auto model = strtok(line, " \t,");
        priceInfo.key = GetKey(model, ms_priceSectionLoaded);

        const auto nameTag = strtok(nullptr, " \t,");
        strncpy(priceInfo.nameTag, nameTag, 8u); // Como el original: strncpy sin terminador garantizado

        switch (ms_priceSectionLoaded) {
        case PRICE_SECTION_CLOTHES:
        case PRICE_SECTION_HAIRCUTS: {
            const auto modelName = strtok(nullptr, " \t,");
            const auto type = strtok(nullptr, " \t,");
            priceInfo.clothes.modelKey = CKeyGen::GetUppercaseKey(modelName);
            priceInfo.clothes.type = std::atoi(type);
            break;
        }
        case PRICE_SECTION_TATTOOS: {
            const auto type = strtok(nullptr, " \t,");
            const auto txtkey = strtok(nullptr, " \t,");
            priceInfo.tattoos.type1 = (type[0] == '-') ? -1 : std::atoi(type);
            priceInfo.tattoos.texKey = CKeyGen::GetUppercaseKey(txtkey);
            break;
        }
        case PRICE_SECTION_WEAPONS: {
            priceInfo.weapon.ammo = std::atoi(strtok(nullptr, " \t,"));
            break;
        }
        default:
            break;
        }

        for (auto i = 0; i < 4; i++)
            RET_IGNORED(strtok(nullptr, " \t,"));

        priceInfo.price = std::atoi(strtok(nullptr, " \t,"));
        // Como el original: recorre todos los modificadores (gana el último que coincide).
        for (auto i = 0; i < ms_numPriceModifiers; i++) {
            if (priceInfo.key == ms_priceModifiers.data()[i].key) {
                priceInfo.price = ms_priceModifiers.data()[i].price;
            }
        }
        ms_numPrices++;
    }
    CFileMgr::CloseFile(file);

    const auto animBlockIndex = CAnimManager::GetAnimationBlockIndex(ms_sectionNames.data()[ms_priceSectionLoaded]);
    if (animBlockIndex != -1) {
        CStreaming::RequestModel(IFPToModelId(animBlockIndex), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(true);
    }
    CTimer::Resume();
}

// 0x49BBE0
void CShopping::LoadShop(const char* sectionName) {
    if (!_stricmp(sectionName, ms_shopLoaded))
        return;

    strcpy(ms_shopLoaded, sectionName); // Como el original: strcpy
    CTimer::Suspend();
    ms_numItemsInShop = 0;

    if (!_stricmp("bought", sectionName)) {
        for (auto i = 0; i < ms_numPrices; i++) {
            const auto key = ms_prices.data()[i].key;
            if (ms_bHasBought.data()[GetItemIndex(key)]) {
                ms_shopContents.data()[ms_numItemsInShop++] = key;
            }
        }

        return; // Como el original: sin CTimer::Resume()
    }

    const auto file = CFileMgr::OpenFile("data\\shopping.dat", "r");
    if (FindSection(file, "shops")) {
        FindSection(file, sectionName);
    }

    char typeName[32];
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0')
            continue;

        if (!strncmp(line, "end", 3u))
            break;

        const auto type = strtok(line, " \t");
        if (!strcmp("type", type)) {
            strcpy(typeName, strtok(nullptr, " \t")); // Como el original: strcpy
            LoadPrices(typeName);
        } else if (!strcmp("item", type)) {
            // Como el original: FindPlayerVehicle solo se llama para CarMods, después de GetKey.
            const auto model = GetKey(strtok(nullptr, " \t"), ms_priceSectionLoaded);
            if (ms_priceSectionLoaded != PRICE_SECTION_CAR_MODS || IsValidModForVehicle(model, FindPlayerVehicle())) {
                ms_shopContents.data()[ms_numItemsInShop++] = model;
            }
        }
    }
    CFileMgr::CloseFile(file);
    CTimer::Resume();
}

// 0x49B6A0
void CShopping::LoadStats() {
    const auto file = CFileMgr::OpenFile("data\\shopping.dat", "r");
    FindSection(file, "prices");
    ms_numBuyableItems = 0u;

    // Como el original: GetKey está inline y en CarPaintJobs no escribe la clave, así que se repite la del
    // elemento anterior (el EXE deja la primera sin inicializar; aquí empieza en 0).
    uint32 key = 0u;
    for (auto sectionName = GetNextSection(file); sectionName; sectionName = GetNextSection(file)) {
        const auto section = GetPriceSectionFromName(sectionName);

        for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
            if (*line == '#' || *line == '\0')
                continue;

            if (!strncmp(line, "end", 3u))
                break;

            const auto model = strtok(line, " \t,");
            if (section != PRICE_SECTION_CAR_PAINTJOBS) {
                key = GetKey(model, section);
            }
            ms_keys.data()[ms_numBuyableItems] = key;
            ms_bHasBought.data()[ms_numBuyableItems] = false;

            RET_IGNORED(strtok(nullptr, " \t,"));

            if (section >= PRICE_SECTION_CLOTHES) {
                if (section <= PRICE_SECTION_TATTOOS) {
                    RET_IGNORED(strtok(nullptr, " \t,"));
                    RET_IGNORED(strtok(nullptr, " \t,"));
                } else if (section == PRICE_SECTION_WEAPONS) {
                    RET_IGNORED(strtok(nullptr, " \t,"));
                }
            }

            const auto stat0   = strtok(nullptr, " \t,");
            const auto change0 = strtok(nullptr, " \t,");
            const auto stat1   = strtok(nullptr, " \t,");
            const auto change1 = strtok(nullptr, " \t,");

            auto& modifiers = ms_statModifiers.data()[ms_numBuyableItems].modifiers;
            modifiers[0].statIndex = (int8)GetChangingStatIndex(stat0);
            modifiers[1].statIndex = (int8)GetChangingStatIndex(stat1);
            modifiers[0].change    = (int8)std::atoi(change0);
            modifiers[1].change    = (int8)std::atoi(change1);

            ms_numBuyableItems++;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x49AC90
void CShopping::RemoveLoadedPrices() {
    const auto animBlockIndex = CAnimManager::GetAnimationBlockIndex(ms_sectionNames.data()[ms_priceSectionLoaded]);
    if (animBlockIndex != -1) {
        CStreaming::SetModelIsDeletable(IFPToModelId(animBlockIndex));
    }
    ms_priceSectionLoaded = PRICE_SECTION_NONE;
}

// 0x49AE30
void CShopping::RemoveLoadedShop() {
    ms_shopLoaded[0] = '\0';
    RemoveLoadedPrices();
}

// 0x49C2D0
void CShopping::AddPriceModifier(const char* name, const char* section, int32 price) {
    AddPriceModifier(GetKey(name, GetPriceSectionFromName(section)), price);
}

// 0x49BDD0
void CShopping::AddPriceModifier(uint32 key, int32 price) {
    auto i = 0;
    for (; i < ms_numPriceModifiers; i++) {
        if (key == ms_priceModifiers.data()[i].key) {
            ms_priceModifiers.data()[i].price = price;
            break;
        }
    }

    if (i == ms_numPriceModifiers) {
        ms_priceModifiers.data()[ms_numPriceModifiers].key = key;
        ms_priceModifiers.data()[ms_numPriceModifiers].price = price;
        ms_numPriceModifiers++;
    }

    // Como el original: también actualiza el precio ya cargado.
    for (auto j = 0; j < ms_numPrices; j++) {
        if (ms_prices.data()[j].key == key) {
            ms_prices.data()[j].price = price;
            break;
        }
    }
}

// 0x49BE50
void CShopping::RemovePriceModifier(const char* name, const char* section) {
    RemovePriceModifier(GetKey(name, GetPriceSectionFromName(section)));
}

// 0x49ACD0
void CShopping::RemovePriceModifier(uint32 key) {
    auto i = 0;
    for (; i < ms_numPriceModifiers; i++) {
        if (key == ms_priceModifiers.data()[i].key) {
            break;
        }
    }

    if (i == ms_numPriceModifiers)
        return;

    // Como el original: el último ocupa el hueco.
    if (--ms_numPriceModifiers > 0) {
        ms_priceModifiers.data()[i] = ms_priceModifiers.data()[ms_numPriceModifiers];
    }
}

// 0x49B200
void CShopping::StoreClothesState() {
    memcpy(&gStoredClothesState, FindPlayerPed()->GetClothesDesc(), sizeof(CPedClothesDesc));
    gClothesHaveBeenStored = 1u;
}

// 0x49B280
void CShopping::StoreVehicleMods() {
    const auto veh = FindPlayerVehicle();
    std::copy_n(veh->m_anUpgrades.begin(), gStoredVehicleMods.size(), gStoredVehicleMods.begin());

    if (!veh->IsAutomobile())
        return;

    // Como el original: 25 estados de un byte, indexados por nodo del coche.
    const auto& damage = veh->AsAutomobile()->m_damageManager;
    for (auto i = 0; i < (int32)gComponentDamageState.size(); i++) {
        auto& state = gComponentDamageState[i];
        switch (i) {
        case CAR_WHEEL_RF:
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_FRONT_RIGHT);
            break;
        case 4: // CAR_WHEEL_RB
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_REAR_RIGHT);
            break;
        case 5: // CAR_WHEEL_LF
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_FRONT_LEFT);
            break;
        case 7: // CAR_WHEEL_LB
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_REAR_LEFT);
            break;
        case CAR_BUMP_FRONT:
            state = (eDamageState)damage.GetPanelStatus(FRONT_BUMPER);
            break;
        case CAR_BUMP_REAR:
            state = (eDamageState)damage.GetPanelStatus(REAR_BUMPER);
            break;
        case CAR_BONNET:
            state = (eDamageState)damage.GetDoorStatus(DOOR_BONNET);
            break;
        case CAR_BOOT:
            state = (eDamageState)damage.GetDoorStatus(DOOR_BOOT);
            break;
        default:
            state = DAMAGE_STATE_OK;
            break;
        }
    }
}

// 0x49B240
void CShopping::RestoreClothesState() {
    // Como el original: copia la ropa guardada sobre la del jugador.
    memcpy(FindPlayerPed()->GetClothesDesc(), &gStoredClothesState, sizeof(CPedClothesDesc));
    gClothesHaveBeenStored = 0u;
}

// 0x49B3C0
void CShopping::RestoreVehicleMods() {
    const auto veh = FindPlayerVehicle();

    for (auto i = 0u; i < gStoredVehicleMods.size(); i++) {
        auto& upgrade = veh->m_anUpgrades[i];

        if (upgrade != -1) {
            veh->RemoveVehicleUpgrade(upgrade);
        }
        upgrade = gStoredVehicleMods[i];
        if (gStoredVehicleMods[i] != -1) {
            CStreaming::RequestVehicleUpgrade(gStoredVehicleMods[i], STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY);
        }
    }

    CStreaming::LoadAllRequestedModels(false);
    veh->SetupUpgradesAfterLoad();

    for (const auto& mod : gStoredVehicleMods) {
        if (mod != -1)
            CStreaming::SetModelIsDeletable(mod);
    }

    if (!veh->IsAutomobile())
        return;

    const auto car = veh->AsAutomobile();
    auto& damage = car->m_damageManager;
    for (auto i = 0; i < (int32)gComponentDamageState.size(); i++) {
        const auto state = (int8)gComponentDamageState[i];
        // Como el original: solo estados > 0 (byte con signo); paneles y puertas también aplican el daño visual.
        switch (i) {
        case CAR_WHEEL_RF:
            if (state > 0)
                damage.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, (eCarWheelStatus)state);
            break;
        case 4: // CAR_WHEEL_RB
            if (state > 0)
                damage.SetWheelStatus(CAR_WHEEL_REAR_RIGHT, (eCarWheelStatus)state);
            break;
        case 5: // CAR_WHEEL_LF
            if (state > 0)
                damage.SetWheelStatus(CAR_WHEEL_FRONT_LEFT, (eCarWheelStatus)state);
            break;
        case 7: // CAR_WHEEL_LB
            if (state > 0)
                damage.SetWheelStatus(CAR_WHEEL_REAR_LEFT, (eCarWheelStatus)state);
            break;
        case CAR_BUMP_FRONT:
            if (state > 0) {
                damage.SetPanelStatus(FRONT_BUMPER, (ePanelDamageState)state);
                car->SetPanelDamage(FRONT_BUMPER, true);
            }
            break;
        case CAR_BUMP_REAR:
            if (state > 0) {
                damage.SetPanelStatus(REAR_BUMPER, (ePanelDamageState)state);
                car->SetPanelDamage(REAR_BUMPER, true);
            }
            break;
        case CAR_BONNET:
            if (state > 0) {
                damage.SetDoorStatus(DOOR_BONNET, (eDoorStatus)state);
                car->SetDoorDamage(DOOR_BONNET, true);
            }
            break;
        case CAR_BOOT:
            if (state > 0) {
                damage.SetDoorStatus(DOOR_BOOT, (eDoorStatus)state);
                car->SetDoorDamage(DOOR_BOOT, true);
            }
            break;
        default:
            break;
        }
    }
}

// 0x49B1F0 (unused)
void CShopping::SetCurrentProperty(CMultiBuilding* property) {
    gpCurrentProperty = property;
}

// 0x49BEF0
void CShopping::UpdateStats(size_t index, bool increment) {
    const auto& modifiers = ms_statModifiers.data()[index].modifiers; // Como el original: el índice puede ser -1
    if (increment) {
        IncrementStat(modifiers[0].statIndex, modifiers[0].change);
        IncrementStat(modifiers[1].statIndex, modifiers[1].change);
    } else {
        IncrementStat(modifiers[0].statIndex, -modifiers[0].change);
        IncrementStat(modifiers[1].statIndex, -modifiers[1].change);
    }
}

// 0x5D3E40
bool CShopping::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_numPriceModifiers);
    // Como el original: comparación sin signo y el contador se relee en cada vuelta.
    for (auto i = 0u; i < (uint32)ms_numPriceModifiers; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(ms_priceModifiers.data()[i]);
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(ms_numBuyableItems);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_bHasBought.data(), ms_numBuyableItems);
    return true;
}

// 0x5D3DE0
bool CShopping::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numPriceModifiers);
    // Como el original: comparación sin signo y el contador se relee en cada vuelta.
    for (auto i = 0u; i < (uint32)ms_numPriceModifiers; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(ms_priceModifiers.data()[i]);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(ms_numBuyableItems);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_bHasBought.data(), ms_numBuyableItems);
    return true;
}
