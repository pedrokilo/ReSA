/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CClothes
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Clothes.h"
#include "ClothesBuilder.h"
#include "PedClothesDesc.h"

auto& PlayerClothes = StaticRef<CPedClothesDesc>(0xBC1C78);

static eClothesModelPart __cdecl ClothesGetTextureDependency(eClothesTexturePart texturePart) {
    return CClothes::GetTextureDependency(texturePart);
}

// Como el original: 0x5A7EA0 solo toca eax; sus llamadores del EXE (CClothes::Init 0x5A80D0,
// CPedClothesDesc::SetTextureAndModel 0x5A8050/0x5A8080, 0x5A8626) conservan ecx y edx a través de la llamada.
static __declspec(naked) void ClothesGetTextureDependencyKeepRegs() {
    __asm {
        push ecx
        push edx
        push dword ptr [esp+12]
        call ClothesGetTextureDependency
        add  esp, 4
        pop  edx
        pop  ecx
        ret
    }
}

void CClothes::InjectHooks() {
    RH_ScopedClass(CClothes);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x5A80D0);
    RH_ScopedInstall(LoadClothesFile, 0x5A7B30);
    RH_ScopedInstall(ConstructPedModel, 0x5A81E0);
    RH_ScopedInstall(RequestMotionGroupAnims, 0x5A8120);
    RH_ScopedInstall(RebuildPlayerIfNeeded, 0x5A8390);
    RH_ScopedInstall(RebuildPlayer, 0x5A82C0);
    RH_ScopedInstall(RebuildCutscenePlayer, 0x5A8270);
    RH_ScopedNamedGlobalInstall(ClothesGetTextureDependencyKeepRegs, "GetTextureDependency", 0x5A7EA0); // Envoltorio que conserva ecx y edx
    RH_ScopedInstall(GetDependentTexture, 0x5A7F30);
    RH_ScopedInstall(GetPlayerMotionGroupToLoad, 0x5A7FB0);
    RH_ScopedInstall(GetDefaultPlayerMotionGroup, 0x5A81B0);
}

// 0x5A80D0
void CClothes::Init() {
    for (int32 part = 0; part < CLOTHES_TEXTURE_TOTAL; part++) {
        eClothesModelPart modelPart = GetTextureDependency(static_cast<eClothesTexturePart>(part));
        // Como el original: las claves quedan a -1 (edx = 0xFFFFFFFF), no a 0.
        PlayerClothes.m_anTextureKeys[part] = (uint32)(-1);
        if (modelPart != CLOTHES_MODEL_UNAVAILABLE)
            PlayerClothes.m_anModelKeys[modelPart] = (uint32)(-1);
    }
    ms_numRuleTags = 0;
    ms_clothesImageId = CStreaming::AddImageToList("MODELS\\PLAYER.IMG", false);
    LoadClothesFile();
}

// 0x5A7A20
int32 GetClothesModelFromName(const char* name) {
    if (!strcmp(name, "torso"))    return CLOTHES_MODEL_TORSO;
    if (!strcmp(name, "head"))     return CLOTHES_MODEL_HEAD;
    if (!strcmp(name, "hands"))    return CLOTHES_MODEL_HANDS;
    if (!strcmp(name, "legs"))     return CLOTHES_MODEL_LEGS;
    if (!strcmp(name, "feet"))     return CLOTHES_MODEL_SHOES;
    if (!strcmp(name, "necklace")) return CLOTHES_MODEL_NECKLACE;
    if (!strcmp(name, "watch"))    return CLOTHES_MODEL_BRACELET;
    if (!strcmp(name, "glasses"))  return CLOTHES_MODEL_GLASSES;
    if (!strcmp(name, "hat"))      return CLOTHES_MODEL_HATS;
    if (!strcmp(name, "extra1"))   return CLOTHES_MODEL_SPECIAL;

    return CLOTHES_MODEL_TORSO;
}

// 0x5A7B30
void CClothes::LoadClothesFile() {
    char delimiters[] = " \t,"; // Como el original: cadena local para strtok
    // Como el original: si la etiqueta no se reconoce se reutiliza la de la línea anterior
    // (en el EXE el primer valor es basura de la pila).
    uint32 ruleTag       = 0;
    bool   isRuleStarted = false;

    ms_numRuleTags = 0; // Como el original: también se reinicia aquí
    auto* file = CFileMgr::OpenFile("DATA\\CLOTHES.DAT", "r");

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }

        if (!isRuleStarted) {
            if (!strncmp("rule", line, 4)) {
                isRuleStarted = true;
            }
            continue;
        }
        if (!strncmp("end", line, 3)) {
            isRuleStarted = false;
            continue;
        }

        char* const strTag = strtok(line, delimiters);
        if (strTag == nullptr) {
            continue;
        }

        // Como el original: comparación sin distinguir mayúsculas (_stricmp), en este orden.
        constexpr struct { const char* name; eClothRule rule; } map[]{
            {"cuts",         eClothRule::TAG_CUTS},
            {"setc",         eClothRule::TAG_SETC},
            {"tex",          eClothRule::TAG_TEX},
            {"hide",         eClothRule::TAG_HIDE},
            {"ignore",       eClothRule::TAG_IGNORE},
            {"endignore",    eClothRule::TAG_END_IGNORE},
            {"exclusive",    eClothRule::TAG_EXCLUSIVE},
            {"endexclusive", eClothRule::TAG_END_EXCLUSIVE},
        };
        for (const auto& [name, rule] : map) {
            if (!_stricmp(strTag, name)) {
                ruleTag = static_cast<uint32>(rule);
                break;
            }
        }
        AddRule(ruleTag);

        // Como el original: siempre se leen los dos argumentos siguientes antes de mirar la etiqueta.
        char* const arg1 = strtok(nullptr, delimiters);
        char* const arg2 = strtok(nullptr, delimiters);
        switch (static_cast<eClothRule>(ruleTag)) {
        case eClothRule::TAG_CUTS:
        case eClothRule::TAG_TEX: {
            AddRule(CKeyGen::GetUppercaseKey(arg1));
            AddRule(CKeyGen::GetUppercaseKey(arg2));
            break;
        }
        case eClothRule::TAG_SETC: {
            char* const arg3 = strtok(nullptr, delimiters);
            char* const arg4 = strtok(nullptr, delimiters);
            AddRule(CKeyGen::GetUppercaseKey(arg1));
            AddRule(GetClothesModelFromName(arg2));
            AddRule(!strcmp(arg3, "-") ? 0 : CKeyGen::GetUppercaseKey(arg3));
            AddRule(!strcmp(arg4, "-") ? 0 : CKeyGen::GetUppercaseKey(arg4));
            break;
        }
        case eClothRule::TAG_HIDE: {
            AddRule(CKeyGen::GetUppercaseKey(arg1));
            AddRule(GetClothesModelFromName(arg2));
            break;
        }
        case eClothRule::TAG_END_IGNORE:
        case eClothRule::TAG_IGNORE:
        case eClothRule::TAG_END_EXCLUSIVE:
        case eClothRule::TAG_EXCLUSIVE:
            AddRule(CKeyGen::GetUppercaseKey(arg1));
            break;
        default:
            break;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x5A81E0
void CClothes::ConstructPedModel(uint32 modelId, CPedClothesDesc& newClothes, const CPedClothesDesc* oldClothes, bool bCutscenePlayer) {
    CTimer::Suspend();

    auto modelInfo = CModelInfo::GetModelInfo(modelId)->AsPedModelInfoPtr();
    auto txd = CTxdStore::ms_pTxdPool->GetAt(modelInfo->m_nTxdIndex);
    auto skinnedClump = CClothesBuilder::CreateSkinnedClump(modelInfo->GetRpClump(), txd->m_pRwDictionary, newClothes, oldClothes, bCutscenePlayer);
    if (skinnedClump) {
        RequestMotionGroupAnims();
        modelInfo->AddTexDictionaryRef();
        modelInfo->DeleteRwObject();
        modelInfo->SetClump(skinnedClump);
        modelInfo->RemoveTexDictionaryRef();
        CStreaming::LoadAllRequestedModels(true);
    }

    CTimer::Resume();
}

// 0x5A8120
void CClothes::RequestMotionGroupAnims() {
    const auto group = GetPlayerMotionGroupToLoad();
    const auto fatIndex = CAnimManager::GetAnimationBlockIndex("fat");
    const auto muscularIndex = CAnimManager::GetAnimationBlockIndex("muscular");

    if (group == ANIM_GROUP_FAT) {
        CStreaming::RequestModel(IFPToModelId(fatIndex), STREAMING_GAME_REQUIRED | STREAMING_PRIORITY_REQUEST);
        CStreaming::SetModelIsDeletable(IFPToModelId(muscularIndex));
    } else {
        if (group == ANIM_GROUP_MUSCULAR) {
            CStreaming::RequestModel(IFPToModelId(muscularIndex), STREAMING_GAME_REQUIRED | STREAMING_PRIORITY_REQUEST);
            CStreaming::SetModelIsDeletable(IFPToModelId(fatIndex));
        } else {
            CStreaming::SetModelIsDeletable(IFPToModelId(muscularIndex));
            CStreaming::SetModelIsDeletable(IFPToModelId(fatIndex));
        }
    }
}

// 0x5A8390
void CClothes::RebuildPlayerIfNeeded(CPlayerPed* player) {
    const auto& fat = player->GetPlayerData()->m_pPedClothesDesc->m_fFatStat;
    const auto& muscle = player->GetPlayerData()->m_pPedClothesDesc->m_fMuscleStat;

    if (CStats::GetStatValue(STAT_FAT) != fat || CStats::GetStatValue(STAT_MUSCLE) != muscle) {
        RebuildPlayer(player, 0);
    }
}

// 0x5A82C0
void CClothes::RebuildPlayer(CPlayerPed* player, bool bIgnoreFatAndMuscle) {
    auto assoc = RpAnimBlendClumpExtractAssociations(player->GetRpClump());
    auto task = player->GetIntelligence()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK);
    if (task)
        task->MakeAbortable(player, ABORT_PRIORITY_IMMEDIATE, nullptr);

    player->DeleteRwObject();
    CWorld::Remove(player);
    if (!bIgnoreFatAndMuscle) {
        player->GetPlayerData()->m_pPedClothesDesc->m_fFatStat = CStats::GetStatValue(STAT_FAT);
        player->GetPlayerData()->m_pPedClothesDesc->m_fMuscleStat = CStats::GetStatValue(STAT_MUSCLE);
    }

    ConstructPedModel(player->GetModelIndex(), *player->GetPlayerData()->m_pPedClothesDesc, &PlayerClothes, 0);
    player->Dress();
    RpAnimBlendClumpGiveAssociations(player->GetRpClump(), assoc);
    PlayerClothes = *player->GetPlayerData()->m_pPedClothesDesc;
}

// 0x5A8270
void CClothes::RebuildCutscenePlayer(CPlayerPed* player, int32 modelId) {
    const auto& clothesDesc    = player->GetPlayerData()->m_pPedClothesDesc;
    clothesDesc->m_fFatStat    = CStats::GetStatValue(STAT_FAT);
    clothesDesc->m_fMuscleStat = CStats::GetStatValue(STAT_MUSCLE);
    ConstructPedModel(modelId, *clothesDesc, nullptr, true);
}

// 0x5A7EA0
eClothesModelPart CClothes::GetTextureDependency(eClothesTexturePart texturePart) {
    switch (texturePart) {
    case CLOTHES_TEXTURE_TORSO:    return CLOTHES_MODEL_TORSO;
    case CLOTHES_TEXTURE_HEAD:     return CLOTHES_MODEL_HEAD;
    case CLOTHES_TEXTURE_LEGS:     return CLOTHES_MODEL_LEGS;
    case CLOTHES_TEXTURE_SHOES:    return CLOTHES_MODEL_SHOES;
    case CLOTHES_TEXTURE_NECKLACE: return CLOTHES_MODEL_NECKLACE;
    case CLOTHES_TEXTURE_BRACELET: return CLOTHES_MODEL_BRACELET;
    case CLOTHES_TEXTURE_GLASSES:  return CLOTHES_MODEL_GLASSES;
    case CLOTHES_TEXTURE_HATS:     return CLOTHES_MODEL_HATS;
    case CLOTHES_TEXTURE_SPECIAL:  return CLOTHES_MODEL_SPECIAL;
    default:                       return CLOTHES_MODEL_UNAVAILABLE;
    }
}

// 0x5A7F30
eClothesTexturePart CClothes::GetDependentTexture(eClothesModelPart modelPart) {
    switch (modelPart) {
    case CLOTHES_MODEL_TORSO:    return CLOTHES_TEXTURE_TORSO;
    case CLOTHES_MODEL_HEAD:     return CLOTHES_TEXTURE_HEAD;
    case CLOTHES_MODEL_LEGS:     return CLOTHES_TEXTURE_LEGS;
    case CLOTHES_MODEL_SHOES:    return CLOTHES_TEXTURE_SHOES;
    case CLOTHES_MODEL_NECKLACE: return CLOTHES_TEXTURE_NECKLACE;
    case CLOTHES_MODEL_BRACELET: return CLOTHES_TEXTURE_BRACELET;
    case CLOTHES_MODEL_GLASSES:  return CLOTHES_TEXTURE_GLASSES;
    case CLOTHES_MODEL_HATS:     return CLOTHES_TEXTURE_HATS;
    case CLOTHES_MODEL_SPECIAL:  return CLOTHES_TEXTURE_SPECIAL;
    default:                     return CLOTHES_TEXTURE_UNAVAILABLE;
    }
}

// 0x5A7FB0
AssocGroupId CClothes::GetPlayerMotionGroupToLoad() {
    // Como el original: vuelve a leer las estadísticas en cada comparación; con NaN no se cumple ninguna.
    if (CStats::GetStatValue(STAT_FAT) > 500.0f) {
        const auto muscle = CStats::GetStatValue(STAT_MUSCLE);
        if (CStats::GetStatValue(STAT_FAT) > muscle)
            return ANIM_GROUP_FAT;
    }

    if (CStats::GetStatValue(STAT_MUSCLE) > 500.0f)
        return ANIM_GROUP_MUSCULAR;

    return ANIM_GROUP_PLAYER;
}

// 0x5A81B0
AssocGroupId CClothes::GetDefaultPlayerMotionGroup() {
    AssocGroupId group = GetPlayerMotionGroupToLoad();
    if (group == ANIM_GROUP_PLAYER)
        return ANIM_GROUP_PLAYER;

    // Como el original: CAnimManager::ms_aAnimAssocGroups (0xB4EA34) indexado sin comprobar el rango.
    CAnimBlock* animBlock = StaticRef<CAnimBlendAssocGroup*>(0xB4EA34)[group].m_AnimBlock;
    if (!animBlock || !animBlock->IsLoaded)
        return ANIM_GROUP_PLAYER;

    return group;
}
