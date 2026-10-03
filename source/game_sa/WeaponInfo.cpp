/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWeaponInfo
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
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

#include "WeaponInfo.h"

void CWeaponInfo::InjectHooks() {
    RH_ScopedClass(CWeaponInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FindWeaponFireType, 0x5BCF30);
    RH_ScopedInstall(LoadWeaponData, 0x5BE670);
    RH_ScopedInstall(Initialise, 0x5BF750);
    RH_ScopedInstall(Shutdown, 0x743C50);
    RH_ScopedOverloadedInstall(GetWeaponInfo, "original", 0x743C60, CWeaponInfo*(*)(eWeaponType, eWeaponSkill));
    RH_ScopedInstall(GetSkillStatIndex, 0x743CD0);
    RH_ScopedInstall(FindWeaponType, 0x743D10);
    RH_ScopedInstall(GetCrouchReloadAnimationID, 0x685700);
    RH_ScopedInstall(GetTargetHeadRange, 0x743D50);
    RH_ScopedInstall(GetWeaponReloadTime, 0x743D70);
}

// 0x5BF750
void CWeaponInfo::Initialise() {
    for (auto& info : aWeaponInfo) {
        info.m_nWeaponFire = WEAPON_FIRE_MELEE;
        info.m_fTargetRange = 0.0f;
        info.m_fWeaponRange = 0.0f;
        info.m_nModelId1 = MODEL_INVALID;
        info.m_nModelId2 = MODEL_INVALID;
        info.m_nSlot = -1;
        info.m_eAnimGroup = ANIM_GROUP_DEFAULT;
        info.m_nAmmoClip = 0;
        info.m_nDamage = 0;
        info.m_vecFireOffset = CVector();
        info.m_nSkillLevel = eWeaponSkill::STD;
        info.m_nReqStatLevel = 0;
        info.m_fAccuracy = 1.0f;
        info.m_fMoveSpeed = 1.0f;
        info.m_fAnimLoopStart = 0.0f;
        info.m_fAnimLoopEnd = 0.0f;
        info.m_fAnimLoopFire = 0.0f;
        info.m_fAnimLoop2Start = 0.0f;
        info.m_fAnimLoop2End = 0.0f;
        info.m_fAnimLoop2Fire = 0.0f;
        info.m_fBreakoutTime = 0.0f;
        info.m_Speed = 0.0f;
        info.m_fRadius = 0.0f;
        info.m_fLifespan = 0.0f;
        info.m_fSpread = 0.0f;
        info.m_nAimOffsetIndex = 0;
        info.m_nFlags = 0;

        // Melee data
        info.m_nBaseCombo = MELEE_COMBO_UNARMED_1;
        info.m_nNumCombos = 1;
    }

    for (auto& offset : ms_WeaponAimOffsets) {
        offset.AimX = 0.0f;
        offset.AimZ = 0.0f;
        offset.DuckX = 0.0f;
        offset.DuckZ = 0.0f;
        offset.RLoadA = 0;
        offset.RLoadB = 0;
        offset.CrouchRLoadA = 0;
        offset.CrouchRLoadB = 0;
    }

    LoadWeaponData();
}

// 0x743C50
void CWeaponInfo::Shutdown() {
    // NOP
}

// 0x5BCF30
eWeaponFire CWeaponInfo::FindWeaponFireType(const char* name) {
    static constexpr struct { std::string_view name; eWeaponFire wf; } mapping[]{
        { "MELEE",       WEAPON_FIRE_MELEE       },
        { "INSTANT_HIT", WEAPON_FIRE_INSTANT_HIT },
        { "PROJECTILE",  WEAPON_FIRE_PROJECTILE  },
        { "AREA_EFFECT", WEAPON_FIRE_AREA_EFFECT },
        { "CAMERA",      WEAPON_FIRE_CAMERA      },
        { "USE",         WEAPON_FIRE_USE         },
    };
    if (const auto it = rng::find(mapping, name, [](const auto& e) { return e.name; }); it != std::end(mapping))
        return it->wf;

    return WEAPON_FIRE_INSTANT_HIT;
}

// Check if weapon has skill stats
// NOTSA
bool CWeaponInfo::TypeHasSkillStats(eWeaponType wt) {
    return wt >= FIRST_WEAPON_WITH_SKILLS && wt <= LAST_WEAPON_WITH_SKILLS;
}

//! @notsa
bool CWeaponInfo::TypeIsWeapon(eWeaponType type) {
    return type < WEAPON_LAST_WEAPON;
}

// Get weapon info index for this type and with this skill
// NOTSA
uint32 CWeaponInfo::GetWeaponInfoIndex(eWeaponType wt, eWeaponSkill skill) {
    assert(TypeIsWeapon(wt) || (skill == eWeaponSkill::STD && (wt >= WEAPON_RAMMEDBYCAR && wt <= WEAPON_FLARE))); // Damage events also have their weapon info entries

    const auto GetNonSTDSkillLevelIndex = [wt](uint32 i) {
        assert(TypeHasSkillStats(wt));
        return WEAPON_LAST_WEAPON           // Offset for std skill level
            + wt - FIRST_WEAPON_WITH_SKILLS // Offset for this weapon [relative to the other weapons with skill levels]
            + i * NUM_WEAPONS_WITH_SKILL;   // Offset for skill level
    };

    switch (skill) {
    case eWeaponSkill::POOR: return GetNonSTDSkillLevelIndex(0);
    case eWeaponSkill::STD:  return (uint32)wt;
    case eWeaponSkill::PRO:  return GetNonSTDSkillLevelIndex(1);
    case eWeaponSkill::COP:  return GetNonSTDSkillLevelIndex(2);
    default:                 NOTSA_UNREACHABLE("Invalid weapon skill");
    }
}

// NOTSA
void CWeaponInfo::StreamModelsForWeapon(eStreamingFlags streamingFlags) {
    for (auto modelId : GetModels()) {
        if (modelId != MODEL_INVALID) {
            CStreaming::RequestModel(modelId, streamingFlags | STREAMING_PRIORITY_REQUEST);
        }
    }
    CStreaming::LoadAllRequestedModels(true);
}

// NOTSA
auto GetBaseComboByName(const char* name) {
    static constexpr std::pair<std::string_view, eMeleeCombo> mapping[]{
        { "UNARMED",     MELEE_COMBO_UNARMED_1 },
        { "BBALLBAT",    MELEE_COMBO_BBALLBAT  },
        { "KNIFE",       MELEE_COMBO_KNIFE     },
        { "GOLFCLUB",    MELEE_COMBO_GOLFCLUB  },
        { "SWORD",       MELEE_COMBO_SWORD     },
        { "CHAINSAW",    MELEE_COMBO_CHAINSAW  },
        { "DILDO",       MELEE_COMBO_DILDO     },
        { "FLOWERS",     MELEE_COMBO_FLOWERS   },
    };
    if (const auto it = rng::find(mapping, name, [](const auto& e) { return e.first; }); it != std::end(mapping))
        return it->second;

    return eMeleeCombo::MELEE_COMBO_UNARMED_1;
}

// Como el original: busqueda en linea de un grupo de animacion por nombre (strcmp, distingue mayusculas)
// sobre los ms_numAnimAssocDefinitions grupos registrados. Devuelve el indice donde se detuvo el bucle:
// el del grupo encontrado o, si no se encuentra, el numero de grupos (0 si no hay ninguno).
static int32 FindAnimGroupIndexByName(const char* name, bool& found) {
    found = false;
    int32 i = 0;
    for (; i < StaticRef<int32>(0xB4EA28); i++) { // CAnimManager::ms_numAnimAssocDefinitions (privado), con signo como el EXE
        if (!strcmp(name, CAnimManager::GetAnimGroupName((AssocGroupId)i))) {
            found = true;
            break;
        }
    }
    return i;
}

// 0x5BE670
void CWeaponInfo::LoadWeaponData() {
    // Como el original: 1/30 es el float del EXE (0x858F10); 50, 0.1, 0.02 y 0.006 (0x858B40, 0x858B1C,
    // 0x858B38, 0x859CDC) son los floats mas cercanos
    constexpr float FRAME_TO_SECONDS = std::bit_cast<float>(0x3D088889u);

    // Como el original: fin del bucle = floor((fin - inicio) * 50 + 0.1) * 0.02 - 0.006 + inicio (orden del x87)
    const auto FixAnimLoopEnd = [](float start, float& end) {
        const float steps = (end - start) * 50.0f + 0.1f;
        end = (float)std::floor((double)steps) * 0.02f - 0.006f + start;
    };

    auto f = CFileMgr::OpenFile("DATA\\WEAPON.DAT", "rb");
    for (auto line = CFileLoader::LoadLine(f); line; line = CFileLoader::LoadLine(f)) {
        // Como el original: se saltan las lineas vacias y los comentarios
        if (line[0] == '\0' || line[0] == '#')
            continue;

        switch ((uint8)line[0]) { // Gotta cast it because of `case 163`
        case 163: { // MELEE DATA
            char weaponName[32], fireTypeName[32], baseComboName[16], stealthAnimGrpName[32];
            weaponName[0] = fireTypeName[0] = baseComboName[0] = stealthAnimGrpName[0] = '\0';
            float targetRange, weaponRange;
            int32 modelId1, modelId2, slot, numCombos;
            uint32 flags;

            RET_IGNORED(sscanf_s(line,
                "%*s %s %s %f %f %d %d %d %s %d %x %s",
                SCANF_S_STR(weaponName),
                SCANF_S_STR(fireTypeName),
                &targetRange,
                &weaponRange,
                &modelId1,
                &modelId2,
                &slot,
                SCANF_S_STR(baseComboName),
                &numCombos,
                &flags,
                SCANF_S_STR(stealthAnimGrpName)
            ));

            const auto wType = FindWeaponType(weaponName);
            auto& wi = aWeaponInfo[(int32)wType];
            wi.m_nWeaponFire = FindWeaponFireType(fireTypeName);
            wi.m_fTargetRange = targetRange;
            wi.m_fWeaponRange = weaponRange;
            wi.m_nModelId1 = modelId1;
            wi.m_nModelId2 = modelId2;
            wi.m_nSlot = slot;
            wi.m_nNumCombos = (uint8)numCombos;
            wi.m_nBaseCombo = GetBaseComboByName(baseComboName);
            wi.m_nFlags = flags;

            // Como el original: el grupo solo se asigna si no empieza por "null" y aparece en la tabla
            if (strncmp(stealthAnimGrpName, "null", 4) != 0) {
                bool found;
                const auto group = FindAnimGroupIndexByName(stealthAnimGrpName, found);
                if (found)
                    wi.m_eAnimGroup = (AssocGroupId)group;
            }

            if (modelId1 > 0)
                CModelInfo::GetModelInfo(modelId1)->AsWeaponModelInfoPtr()->SetWeaponInfo(wType);
            break;
        }
        case '$': { // Gun data
            char weaponName[32], fireTypeName[32], animGrpName[32];
            weaponName[0] = fireTypeName[0] = animGrpName[0] = '\0';
            float targetRange, weaponRange;
            int32 modelId1, modelId2, slot;
            int32 ammo, dmg;
            CVector offset;
            int32 skill, reqStatLevelForSkill;
            float accuracy, moveSpeed;
            int32 loop1Start, loop1End, loop1Fire, loop2Start, loop2End, loop2Fire;
            int32 breakoutTime;
            uint32 flags;
            // Como el original: solo los cuatro campos opcionales se ponen a cero antes de leer
            float speed = 0.0f, radius = 0.0f, lifespan = 0.0f, spread = 0.0f;

            RET_IGNORED(sscanf_s(line,
                "%*s %s %s %f %f %d %d %d %s %d %d %f %f %f %d %d %f %f %d %d %d %d %d %d %d %x %f %f %f %f",
                SCANF_S_STR(weaponName),
                SCANF_S_STR(fireTypeName),
                &targetRange, &weaponRange,
                &modelId1, &modelId2,
                &slot,
                SCANF_S_STR(animGrpName),
                &ammo,
                &dmg,
                &offset.x, &offset.y, &offset.z,
                &skill,
                &reqStatLevelForSkill,
                &accuracy,
                &moveSpeed,
                &loop1Start, &loop1End, &loop1Fire,
                &loop2Start, &loop2End, &loop2Fire,
                &breakoutTime,
                &flags,
                &speed,    // optional
                &radius,   // optional
                &lifespan, // optional
                &spread    // optional
            ));

            const auto weaponType = FindWeaponType(weaponName);
            // Como el original: las armas sin niveles de habilidad guardan la habilidad 1 (STD)
            if (!TypeHasSkillStats(weaponType))
                skill = (int32)eWeaponSkill::STD;
            auto& wi = *GetWeaponInfo(weaponType, (eWeaponSkill)skill);
            wi.m_nWeaponFire = FindWeaponFireType(fireTypeName);
            wi.m_fTargetRange = targetRange;
            wi.m_fWeaponRange = weaponRange;
            wi.m_nModelId1 = modelId1;
            wi.m_nModelId2 = modelId2;
            wi.m_nSlot = slot;
            wi.m_nAmmoClip = (uint16)ammo;
            wi.m_nDamage = (uint16)dmg;
            wi.m_vecFireOffset = offset;
            reinterpret_cast<int32&>(wi.m_nSkillLevel) = skill;
            wi.m_nReqStatLevel = (uint32)reqStatLevelForSkill;
            wi.m_fAccuracy = accuracy;
            wi.m_fMoveSpeed = moveSpeed;
            wi.m_nFlags = flags;
            wi.m_Speed = speed;
            wi.m_fRadius = radius;
            wi.m_fLifespan = lifespan;
            wi.m_fSpread = spread;
            // Como el original: fild * (1/30) en lugar de dividir entre 30
            wi.m_fAnimLoopStart = (float)loop1Start * FRAME_TO_SECONDS;
            wi.m_fAnimLoopEnd = (float)loop1End * FRAME_TO_SECONDS;
            wi.m_fAnimLoopFire = (float)loop1Fire * FRAME_TO_SECONDS;
            wi.m_fAnimLoop2Start = (float)loop2Start * FRAME_TO_SECONDS;
            wi.m_fAnimLoop2End = (float)loop2End * FRAME_TO_SECONDS;
            wi.m_fAnimLoop2Fire = (float)loop2Fire * FRAME_TO_SECONDS;
            wi.m_fBreakoutTime = (float)breakoutTime * FRAME_TO_SECONDS;

            if (strncmp(animGrpName, "null", 4) != 0) {
                bool found;
                const auto group = FindAnimGroupIndexByName(animGrpName, found);
                if (found)
                    wi.m_eAnimGroup = (AssocGroupId)group;
            }

            if ((int32)wi.m_eAnimGroup >= ANIM_GROUP_PYTHON && (int32)wi.m_eAnimGroup <= ANIM_GROUP_SPRAYCAN) {
                wi.m_nAimOffsetIndex = (int16)((int32)wi.m_eAnimGroup - ANIM_GROUP_PYTHON);
            }

            const bool setModelWeapon = skill == (int32)eWeaponSkill::STD && weaponType != eWeaponType::WEAPON_DETONATOR;

            FixAnimLoopEnd(wi.m_fAnimLoopStart, wi.m_fAnimLoopEnd);
            FixAnimLoopEnd(wi.m_fAnimLoop2Start, wi.m_fAnimLoop2End);

            if (setModelWeapon && modelId1 > 0) {
                CModelInfo::GetModelInfo(modelId1)->AsWeaponModelInfoPtr()->SetWeaponInfo(weaponType);
            }
            break;
        }
        case '%': { // Gun aiming offsets
            char stealthAnimGrp[32];
            stealthAnimGrp[0] = '\0';
            float aimX, aimZ, duckX, duckZ;
            int32 RLoadA, RLoadB, crouchRLoadA, crouchRLoadB;

            RET_IGNORED(sscanf_s(line, "%*s %s %f %f %f %f %d %d %d %d", SCANF_S_STR(stealthAnimGrp), &aimX, &aimZ, &duckX, &duckZ, &RLoadA, &RLoadB, &crouchRLoadA, &crouchRLoadB));

            // Como el original: sin comprobar si el grupo existe (si no aparece se usa el numero de grupos)
            bool found;
            auto& ao = ms_WeaponAimOffsets.data()[FindAnimGroupIndexByName(stealthAnimGrp, found) - ANIM_GROUP_PYTHON];
            ao.AimX = aimX;
            ao.AimZ = aimZ;
            ao.DuckX = duckX;
            ao.DuckZ = duckZ;
            ao.RLoadA = (int16)RLoadA;
            ao.RLoadB = (int16)RLoadB;
            ao.CrouchRLoadA = (int16)crouchRLoadA;
            ao.CrouchRLoadB = (int16)crouchRLoadB;
            break;
        }
        default: {
            // Como el original: solo una primera palabra que empiece por ENDWEAPONDATA termina la lectura
            char word[512];
            word[0] = '\0';
            RET_IGNORED(sscanf_s(line, "%s", SCANF_S_STR(word)));
            if (!strncmp(word, "ENDWEAPONDATA", 13)) {
                CFileMgr::CloseFile(f);
                return;
            }
            break;
        }
        }
    }
    CFileMgr::CloseFile(f);
}

// 0x743C60
CWeaponInfo* CWeaponInfo::GetWeaponInfo(eWeaponType weaponID, eWeaponSkill skill) {
    // Como el original: indice truncado a int16 (movsx ax) y, con una habilidad desconocida, la entrada 47
    int32 index;
    switch (skill) {
    case eWeaponSkill::POOR: index = (int32)weaponID + 25; break;
    case eWeaponSkill::STD:  index = (int32)weaponID;      break;
    case eWeaponSkill::PRO:  index = (int32)weaponID + 36; break;
    case eWeaponSkill::COP:  index = (int32)weaponID + 47; break;
    default:                 index = 47;                   break;
    }
    return &aWeaponInfo[(int16)index];
}

// 0x743CD0
eStats CWeaponInfo::GetSkillStatIndex(eWeaponType wt) {
    switch (wt) {
    case WEAPON_PISTOL:          return STAT_PISTOL_SKILL;
    case WEAPON_PISTOL_SILENCED: return STAT_SILENCED_PISTOL_SKILL;
    case WEAPON_DESERT_EAGLE:    return STAT_DESERT_EAGLE_SKILL;
    case WEAPON_SHOTGUN:         return STAT_SHOTGUN_SKILL;
    case WEAPON_SAWNOFF_SHOTGUN: return STAT_SAWN_OFF_SHOTGUN_SKILL;
    case WEAPON_SPAS12_SHOTGUN:  return STAT_COMBAT_SHOTGUN_SKILL;
    case WEAPON_MICRO_UZI:       return STAT_MACHINE_PISTOL_SKILL;
    case WEAPON_MP5:             return STAT_SMG_SKILL;
    case WEAPON_AK47:            return STAT_AK_47_SKILL;
    case WEAPON_M4:              return STAT_M4_SKILL;
    case WEAPON_TEC9:            return STAT_MACHINE_PISTOL_SKILL;
    default:                     return (eStats)-1; // Como el original: -1 para armas sin niveles de habilidad
    }
}

// 0x743D10
eWeaponType CWeaponInfo::FindWeaponType(const char* type) {
    // From 8D6150 (ms_aWeaponNames)
    constexpr const char* names[]{
        "UNARMED",
        "BRASSKNUCKLE",
        "GOLFCLUB",
        "NIGHTSTICK",
        "KNIFE",
        "BASEBALLBAT",
        "SHOVEL",
        "POOLCUE",
        "KATANA",
        "CHAINSAW",
        "DILDO1",
        "DILDO2",
        "VIBE1",
        "VIBE2",
        "FLOWERS",
        "CANE",
        "GRENADE",
        "TEARGAS",
        "MOLOTOV",
        "ROCKET",
        "ROCKET_HS",
        "FREEFALL_BOMB",
        "PISTOL",
        "PISTOL_SILENCED",
        "DESERT_EAGLE",
        "SHOTGUN",
        "SAWNOFF",
        "SPAS12",
        "MICRO_UZI",
        "MP5",
        "AK47",
        "M4",
        "TEC9",
        "COUNTRYRIFLE",
        "SNIPERRIFLE",
        "RLAUNCHER",
        "RLAUNCHER_HS",
        "FTHROWER",
        "MINIGUN",
        "SATCHEL_CHARGE",
        "DETONATOR",
        "SPRAYCAN",
        "EXTINGUISHER",
        "CAMERA",
        "NIGHTVISION",
        "INFRARED",
        "PARACHUTE",
        "", // Yeah, empty string. Unsure why.
        "ARMOUR",
    };
    for (auto i = 0u; i < std::size(names); i++) {
        if (!_stricmp(names[i], type)) {
            return (eWeaponType)i;
        }
    }
    return eWeaponType::WEAPON_UNARMED; // Como el original: 0 si no se encuentra
}

// 0x685700
AnimationId CWeaponInfo::GetCrouchReloadAnimationID() const {
    return flags.bCrouchFire && flags.bReload ? ANIM_ID_CROUCHRELOAD : ANIM_ID_WALK;
}

// 0x743D50
float CWeaponInfo::GetTargetHeadRange() const {
    // Como el original: fild del dword con signo (+2) y dos fmul en el x87 (el producto exacto se redondea una sola vez,
    // tambien con habilidades que no caben en 24 bits); 0.04 es el float de 0x872CE8
    int32 skillPlus2 = (int32)(m_nSkillLevel.get_underlying() + 2u);
    float rangeValue = m_fWeaponRange;
    float factor = 0.04f;
    float result;
    __asm {
        fild  skillPlus2
        fmul  rangeValue
        fmul  factor
        fstp  result
    }
    return result;
}

// 0x743D70
uint32 CWeaponInfo::GetWeaponReloadTime() const {
    if (flags.bReload)
        return flags.bTwinPistol ? 2000u : 1000u;

    if (flags.bLongReload)
        return 1000u;

    // Como el original: indice con signo (movsx), recargas leidas sin signo (movzx) y se devuelve la primera
    // de A, CrouchA, B que supere 400 tras sumar 100; si ninguna, CrouchB + 100 o 400
    const auto& ao = ms_WeaponAimOffsets.data()[m_nAimOffsetIndex];
    for (const int32 reload : { (int32)(uint16)ao.RLoadA, (int32)(uint16)ao.CrouchRLoadA, (int32)(uint16)ao.RLoadB }) {
        if (reload + 100 > 400)
            return (uint32)(reload + 100);
    }
    const int32 last = (int32)(uint16)ao.CrouchRLoadB + 100;
    return last > 400 ? (uint32)last : 400u;
}