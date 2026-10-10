/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCheat
 * Reconstrucción: COMPLETA — 72/72 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 72/72 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Cheat.h"
#include "PedType.h"
#include "PedClothesDesc.h"

#include "Events/EventAcquaintancePedHate.h"
#include "TaskSimpleJetPack.h"
#include "PostEffects.h"
#include "Hud.h"
#include <CarGenerator.h>

/*
 * Interesting links:
 *
 * https://youtube.com/watch?v=W_eFZ4HzU7Q GTA SA - Alternative Cheats - Feat. Badger Goodger
 * https://youtube.com/watch?v=MVpMTw0rWoc GTA SA - New Secret Cheats - Feat. Spoofer
 * https://youtube.com/watch?v=L97xXbFnFWM GTA SA - Bugs that break your save game - Feat. BadgerGoodger
 *
 */

bool CCheat::m_bShowMappings;
uint32 CCheat::m_nLastScriptBypassTime;

// NOTSA
struct Cheat {
    DWORD   installAddress;
    void*   method;
    const   std::string methodName;
    uint32  hash;
    eCheats type;
};

const auto cheats = std::to_array<Cheat>({
        { 0x4385b0,  CCheat::WeaponCheat1, "WeaponCheat1", 0xde4b237d, CHEAT_WEAPON_SET1 },
        { 0x438890,  CCheat::WeaponCheat2, "WeaponCheat2", 0xb22a28d1, CHEAT_WEAPON_SET2 },
        { 0x438b30,  CCheat::WeaponCheat3, "WeaponCheat3", 0x5a783fae, CHEAT_WEAPON_SET3 },
        { 0x438e40,  CCheat::MoneyArmourHealthCheat, "MoneyArmourHealthCheat", 0xeeccea2b, CHEAT_HEALTH_ARMOR_250K },
        { 0x438e90,  CCheat::WantedLevelUpCheat, "WantedLevelUpCheat", 0x42af1e28, CHEAT_WANTED_LEVEL_2STARS },
        { 0x438f20,  CCheat::WantedLevelDownCheat, "WantedLevelDownCheat", 0x555fc201, CHEAT_CLEAR_WANTED_LEVEL },
        { 0x438f40,  CCheat::SunnyWeatherCheat, "SunnyWeatherCheat", 0x2a845345, CHEAT_SUNNY_WEATHER },
        { 0x438f50,  CCheat::ExtraSunnyWeatherCheat, "ExtraSunnyWeatherCheat", 0xe1ef01ea, CHEAT_VERY_SUNNY_WEATHER },
        { 0x438f60,  CCheat::CloudyWeatherCheat, "CloudyWeatherCheat", 0x771b83fc, CHEAT_OVERCAST_WEATHER },
        { 0x438f70,  CCheat::RainyWeatherCheat, "RainyWeatherCheat", 0x5bf12848, CHEAT_RAINY_WEATHER },
        { 0x438f80,  CCheat::FoggyWeatherCheat, "FoggyWeatherCheat", 0x44453a17, CHEAT_FOGGY_WEATHER },
        { 0x0, nullptr, "", 0xfcff1d08, CHEAT_FASTER_CLOCK },
        { 0x438f90,  CCheat::FastTimeCheat, "FastTimeCheat", 0xb69e8532, CHEAT_FASTER_GAMEPLAY },
        { 0x438fc0,  CCheat::SlowTimeCheat, "SlowTimeCheat", 0x8b828076, CHEAT_SLOWER_GAMEPLAY },
        { 0x439b20,  CCheat::MayhemCheat, "MayhemCheat", 0xdd6ed9e9, CHEAT_PEDS_ATTACK_OTHER_WITH_GOLFCLUB },
        { 0x439c70,  CCheat::EverybodyAttacksPlayerCheat, "EverybodyAttacksPlayerCheat", 0xa290fd8c, CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD },
        { 0x0, nullptr, "", 0x3484b5a7, CHEAT_EVERYONE_ARMED },
        { 0x43a4a0,  CCheat::TankCheat, "TankCheat", 0x43db914e, CHEAT_SPAWN_RHINO },
        { 0x43a4b0,  CCheat::StockCarCheat, "StockCarCheat", 0xdbc0dd65, CHEAT_SPAWN_BLOODRINGBANGER },
        { 0x43a4c0,  CCheat::StockCar2Cheat, "StockCar2Cheat", 0x00000000, CHEAT_SPAWN_RANCHER },
        { 0x43a4d0,  CCheat::StockCar3Cheat, "StockCar3Cheat", 0xd08a30fe, CHEAT_SPAWN_HOTRINGA },
        { 0x43a4e0,  CCheat::StockCar4Cheat, "StockCar4Cheat", 0x37bf1b4e, CHEAT_SPAWN_HOTRINGB },
        { 0x43a4f0,  CCheat::HearseCheat, "HearseCheat", 0xb5d40866, CHEAT_SPAWN_ROMERO },
        { 0x43a500,  CCheat::LovefistCheat, "LovefistCheat", 0xe63b0d99, CHEAT_SPAWN_STRETCH },
        { 0x43a510,  CCheat::TrashmasterCheat, "TrashmasterCheat", 0x675b8945, CHEAT_SPAWN_TRASHMASTER },
        { 0x43a520,  CCheat::GolfcartCheat, "GolfcartCheat", 0x4987d5ee, CHEAT_SPAWN_CADDY },
        { 0x439d80,  CCheat::BlowUpCarsCheat, "BlowUpCarsCheat", 0x2e8f84e8, CHEAT_BLOW_UP_ALL_CARS },
        { 0x0, nullptr, "", 0x1a9aa3d6, CHEAT_INVISIBLE_CAR },
        { 0x0, nullptr, "", 0xe842f3bc, CHEAT_PERFECT_HANDLING },
        { 0x438ff0,  CCheat::SuicideCheat, "SuicideCheat", 0x0d5c6a4e, CHEAT_SUICIDE },
        { 0x0, nullptr, "", 0x74d4fcb1, CHEAT_BGREEN_LIGHTS_CHEAT },
        { 0x0, nullptr, "", CHEAT_AGGRESSIVE_DRIVERS },
        { 0x4390d0,  CCheat::PinkCarsCheat, "PinkCarsCheat", 0x66516ebc, CHEAT_PINK_TRAFFIC },
        { 0x4390f0,  CCheat::BlackCarsCheat, "BlackCarsCheat", 0x4b137e45, CHEAT_BLACK_TRAFFIC },
        { 0x0, nullptr, "", 0x00000000, CHEAT_CARS_ON_WATER },
        { 0x0, nullptr, "", 0x78520e33, CHEAT_BOATS_FLY },
        { 0x439110,  CCheat::FatCheat, "FatCheat", 0x3a577325, CHEAT_FAT_PLAYER },
        { 0x439150,  CCheat::MuscleCheat, "MuscleCheat", 0xd4966d59, CHEAT_MAX_MUSCLE },
        { 0x439190,  CCheat::SkinnyCheat, "SkinnyCheat", 0x5fd1b49d, CHEAT_SKINNY_PLAYER },
        { 0x4391d0,  CCheat::ElvisLivesCheat, "ElvisLivesCheat", 0xa7613f99, CHEAT_ELVIS_IS_EVERYWHERE },
        { 0x439dd0,  CCheat::VillagePeopleCheat, "VillagePeopleCheat", 0x1792d871, CHEAT_PEDS_ATTACK_YOU_WITH_ROCKETS },
        { 0x439230,  CCheat::BeachPartyCheat, "BeachPartyCheat", 0xcbc579df, CHEAT_BEACH_PARTY },
        { 0x439360,  CCheat::GangsCheat, "GangsCheat", 0x4fedccff, CHEAT_GANGMEMBERS_EVERYWHERE },
        { 0x4393d0,  CCheat::GangLandCheat, "GangLandCheat", CHEAT_GANGS_CONTROLS_THE_STREETS },
        { 0x439e50,  CCheat::NinjaCheat, "NinjaCheat", 0x44b34866, CHEAT_NINJA_THEME },
        { 0x4393f0,  CCheat::LoveConquersAllCheat, "LoveConquersAllCheat", 0x2ef877db, CHEAT_SLUT_MAGNET },
        { 0x4394b0,  CCheat::AllCarsAreShitCheat, "AllCarsAreShitCheat", 0x2781e797, CHEAT_CHEAP_TRAFFIC },
        { 0x4394e0,  CCheat::AllCarsAreGreatCheat, "AllCarsAreGreatCheat", 0x2bc1a045, CHEAT_FAST_TRAFFIC },
        { 0x0, nullptr, "", 0xb2afe368, CHEAT_CARS_FLY },
        { 0x0, nullptr, "", 0xfa8dd45b, CHEAT_HUGE_BUNNY_HOP },
        { 0x43a530,  CCheat::FlyboyCheat, "FlyboyCheat", 0x8ded75bd, CHEAT_SPAWN_HYDRA },
        { 0x43a540,  CCheat::VortexCheat, "VortexCheat", 0x1a5526bc, CHEAT_SPAWN_VORTEX },
        { 0x0, nullptr, "", 0xa48a770b, CHEAT_SMASH_N_BOOM },
        { 0x0, nullptr, "", 0xb07d3b32, CHEAT_ALL_CARS_HAVE_NITRO },
        { 0x0, nullptr, "", 0x80c1e54b, CHEAT_CARS_FLOAT_AWAY_WHEN_HIT },
        { 0x439510,  CCheat::MidnightCheat, "MidnightCheat", CHEAT_ALWAYS_MIDNIGHT },
        { 0x439540,  CCheat::DuskCheat, "DuskCheat", 0x5dad0087, CHEAT_STOP_GAME_CLOCK_ORANGE_SKY },
        { 0x439570,  CCheat::StormCheat, "StormCheat", 0x7f80b950, CHEAT_THUNDER_STORM },
        { 0x439590,  CCheat::SandstormCheat, "SandstormCheat", 0x6c0fa650, CHEAT_SAND_STORM },
        { 0x4395a0,  CCheat::PredatorCheat, "PredatorCheat", 0xf46f2fa4, CHEAT_UNUSED },
        { 0x0, nullptr, "", 0x70164385, CHEAT_MEGAJUMP },
        { 0x0, nullptr, "", 0x00000000, CHEAT_INFINITE_HEALTH },
        { 0x0, nullptr, "", 0x885d0b50, CHEAT_INFINITE_OXYGEN },
        { 0x4395b0,  CCheat::ParachuteCheat, "ParachuteCheat", 0x151bdcb3, CHEAT_GET_PARACHUTE },
        { 0x439600,  CCheat::JetpackCheat, "JetpackCheat", 0xadfa640a, CHEAT_GET_JETPACK },
        { 0x4396c0,  CCheat::NotWantedCheat, "NotWantedCheat", 0xe57f96ce, CHEAT_I_DO_AS_I_PLEASE },
        { 0x4396f0,  CCheat::WantedCheat, "WantedCheat", 0x040cf761, CHEAT_SIX_WANTED_STARS },
        { 0x0, nullptr, "", 0xe1b33eb9, CHEAT_MEGA_PUNCH },
        { 0x0, nullptr, "", 0xfeda77f7, CHEAT_NEVER_GET_HUNGRY },
        { 0x439710,  CCheat::RiotCheat, "RiotCheat", 0x8ca870dd, CHEAT_RIOT_MODE },
        { 0x439720,  CCheat::FunhouseCheat, "FunhouseCheat", 0x9a629401, CHEAT_FUNHOUSE_THEME },
        { 0x439880,  CCheat::AdrenalineCheat, "AdrenalineCheat", 0xf53ef5a5, CHEAT_ADRENALINE_MODE },
        { 0x0, nullptr, "", 0xf2aa0c1d, CHEAT_INFINITE_AMMO },
        { 0x4398d0,  CCheat::DrivebyCheat, "DrivebyCheat", 0xf36345a8, CHEAT_WEAPON_AIMING_WHILE_DRIVING },
        { 0x0, nullptr, "", 0x8990d5e1, CHEAT_REDUCED_TRAFFIC },
        { 0x439f60,  CCheat::CountrysideInvasionCheat, "CountrysideInvasionCheat", 0xb7013b1b, CHEAT_COUNTRY_TRAFFIC },
        { 0x0, nullptr, "", 0xcaec94ee, CHEAT_WANNA_BE_IN_MY_GANG },
        { 0x0, nullptr, "", 0x31f0c3cc, CHEAT_NO_ONE_CAN_STOP_US },
        { 0x0, nullptr, "", 0xb3b3e72a, CHEAT_ROCKET_MAYHEM },
        { 0x0, nullptr, "", 0xc25cdbff, CHEAT_MAX_RESPECT },
        { 0x0, nullptr, "", 0xd5cf4eff, CHEAT_MAX_SEX_APPEAL },
        { 0x439930,  CCheat::StaminaCheat, "StaminaCheat", 0x680416b1, CHEAT_MAX_STAMINA },
        { 0x439940,  CCheat::WeaponSkillsCheat, "WeaponSkillsCheat", 0xcf5fda18, CHEAT_HITMAN_LEVEL_FOR_ALL_WEAPONS },
        { 0x4399d0,  CCheat::VehicleSkillsCheat, "VehicleSkillsCheat", 0xf01286e9, CHEAT_MAX_DRIVING_SKILLS },
        { 0x43a550,  CCheat::ApacheCheat, "ApacheCheat", 0xa841cc0a, CHEAT_SPAWN_HUNTER },
        { 0x43a560,  CCheat::QuadCheat, "QuadCheat", 0x31ea09cf, CHEAT_SPAWN_QUAD },
        { 0x43a570,  CCheat::TankerCheat, "TankerCheat", 0xe958788a, CHEAT_SPAWN_TANKER_TRUCK },
        { 0x43a660,  CCheat::DozerCheat, "DozerCheat", 0x02c83a7c, CHEAT_SPAWN_DOZER },
        { 0x43a670,  CCheat::StuntPlaneCheat, "StuntPlaneCheat", 0xe49c3ed4, CHEAT_SPAWN_STUNT_PLANE },
        { 0x43a680,  CCheat::MonsterTruckCheat, "MonsterTruckCheat", 0x171ba8cc, CHEAT_SPAWN_MONSTER },
        { 0x0, nullptr, "", 0x86988dae, CHEAT_PROSTITUTES_PAY_YOU },
        { 0x0, nullptr, "", 0x2bdd2fa1, CHEAT_ALL_TAXIS_NITRO },
});

// 0x438480
void CCheat::AddToCheatString(char LastPressedKey) {
    if (CCutsceneMgr::ms_running) {
        return;
    }

    // Shift stuff to the right
    for (auto i = CHEAT_STRING_SIZE - 1 - 1; i >= 1; --i) {
        m_CheatString[i] = m_CheatString[i - 1];
    }
    m_CheatString[0] = LastPressedKey;
    m_CheatString[CHEAT_STRING_SIZE - 1] = 0;

    uint32 strLen = strlen(m_CheatString);
    if (strLen < CHEAT_MIN_HASH_SIZE) {
        return;
    }

    // Como el original (0x4384E0..0x4384ED): copia local de la cadena, incluido el 0 final
    char buf[CHEAT_STRING_SIZE];
    {
        size_t k = 0;
        char   c;
        do {
            c = m_CheatString[k];
            buf[k] = c;
            k++;
        } while (c != 0);
    }

    // Como el original (0x438500..0x438527): prueba primero la cadena completa y va acortando por el final (hasta 6)
    for (int32 len = (int32)strLen; len >= CHEAT_MIN_HASH_SIZE; --len) {
        const uint32 hash = CKeyGen::GetKey(buf); // 0x53CF00

        // Como el original (0x438510): se queda con el primer indice que coincide
        for (int32 j = 0; j < TOTAL_CHEATS; ++j) {
            if ((uint32)m_aCheatHashKeys[j] != hash) {
                continue;
            }

            if (m_aCheatsActive[j]) {
                // deactivated
                CHud::SetHelpMessage(TheText.Get("CHEAT8"), true, false, false);
            } else {
                // activated
                CHud::SetHelpMessage(TheText.Get("CHEAT1"), true, false, false);
            }

            CStats::IncrementStat(STAT_TIMES_CHEATED, 1.0f);

            // Como el original (0x438576): el indicador se pone ANTES de ejecutar el truco; sin pasar por ApplyCheat
            m_bHasPlayerCheated = true;
            if (auto* const func = m_aCheatFunctions[j]) {
                func();
            } else {
                m_aCheatsActive[j] = !m_aCheatsActive[j];
            }
            m_CheatString[0] = '\0';
            return;
        }

        buf[len - 1] = 0; // 0x43851F
    }
}

// Activates the cheat without changing statistics
// unknown name
// 0x438370
void CCheat::ApplyCheat(eCheats cheat) {
    auto func = m_aCheatFunctions[cheat];
    if (func) {
        return func();
    } else {
        return Toggle(cheat);
    }
}

// 0x438450
void CCheat::ResetCheats() {
    ZoneScoped;

    memset(&m_aCheatsActive, 0, sizeof(m_aCheatsActive));
    CWeather::ReleaseWeather();
    CTimer::ResetTimeScale();
    m_CheatString[0] = '\0';
    m_bHasPlayerCheated = false;
}

// 0x439AF0
void CCheat::DoCheats() {
    ZoneScoped;

    for (auto key = 0; key < 256; ++key) {
        if (CPad::GetPad(0)->IsStandardKeyJustPressed(key)) {
            AddToCheatString(key);
        }
    }
}

// 0x439880
void CCheat::AdrenalineCheat() {
    Toggle(CHEAT_ADRENALINE_MODE);
    if (IsActive(CHEAT_ADRENALINE_MODE)) {
        CPickups::GivePlayerGoodiesWithPickUpMI(ModelIndices::MI_PICKUP_ADRENALINE, 0);
    } else {
        // Como el original (0x4398A8..0x4398BE): solo limpia el bit 1 (m_bAdrenaline) del dword [m_pPlayerData+0x34]
        FindPlayerPed()->GetPlayerData()->m_bAdrenaline = false;
    }
}

// 0x4394e0
void CCheat::AllCarsAreGreatCheat() {
    Toggle(CHEAT_FAST_TRAFFIC);
    if (IsActive(CHEAT_FAST_TRAFFIC)) {
        HandleSpecialCheats(CHEAT_FAST_TRAFFIC);
        CStreaming::ReclassifyLoadedCars();
    }
}

// 0x4394b0
void CCheat::AllCarsAreShitCheat() {
    Toggle(CHEAT_CHEAP_TRAFFIC);
    if (IsActive(CHEAT_CHEAP_TRAFFIC)) {
        HandleSpecialCheats(CHEAT_CHEAP_TRAFFIC);
        CStreaming::ReclassifyLoadedCars();
    }
}

// 0x439230
void CCheat::BeachPartyCheat() {
    Toggle(CHEAT_BEACH_PARTY);
    if (IsActive(CHEAT_BEACH_PARTY)) {
        int32 peds[] = {
            MODEL_BMYBE, MODEL_HMYBE, MODEL_WFYBE, MODEL_BFYBE,
            MODEL_HFYBE, UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);

        CPlayerPed* player = FindPlayerPed();
        // Como el original (0x439289..): no hay llamada previa para limpiar CLOTHES_TEXTURE_SPECIAL
        player->GetClothesDesc()->SetTextureAndModel("torso", nullptr, CLOTHES_TEXTURE_TORSO);
        player->GetClothesDesc()->SetTextureAndModel("shortskhaki", "shorts", CLOTHES_TEXTURE_LEGS);
        player->GetClothesDesc()->SetTextureAndModel("flipflop", "flipflop", CLOTHES_TEXTURE_SHOES);
        player->GetClothesDesc()->SetTextureAndModel("glasses04dark", "glasses04", CLOTHES_TEXTURE_GLASSES);

        if (player->m_nPedState != PEDSTATE_DRIVING) {
            CClothes::RebuildPlayer(player, false);
        }

        ExtraSunnyWeatherCheat();
        HandleSpecialCheats(CHEAT_BEACH_PARTY);
        CStreaming::ReclassifyLoadedCars();
    }
}

// 0x4390f0
void CCheat::BlackCarsCheat() {
    Toggle(CHEAT_BLACK_TRAFFIC);
    if (IsActive(CHEAT_BLACK_TRAFFIC)) {
        Disable(CHEAT_PINK_TRAFFIC);
    }
}

// 0x439d80
void CCheat::BlowUpCarsCheat() {
    // Como el original (0x439D80..0x439DBE): recorre el pool de vehiculos del ultimo slot al primero
    auto* const pool = GetVehiclePool();
    for (auto i = pool->GetSize(); i > 0; --i) {
        if (auto* const veh = pool->GetAt(i - 1)) {
            veh->BlowUpCar(nullptr, false);
        }
    }
}

// 0x439f60
void CCheat::CountrysideInvasionCheat() {
    Toggle(CHEAT_COUNTRY_TRAFFIC);
    if (IsActive(CHEAT_COUNTRY_TRAFFIC)) {
        int32 peds[] = {
            MODEL_CWFOFR, MODEL_CWFOHB, MODEL_CWFYFR1, MODEL_CWFYHB,
            MODEL_CWMOFR, MODEL_CWMOHB1, MODEL_CWMOHB2, MODEL_CWMYFR,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);

        CPlayerPed* player = FindPlayerPed();
        player->GetClothesDesc()->SetTextureAndModel("timberfawn", "bask1", CLOTHES_TEXTURE_SHOES);
        player->GetClothesDesc()->SetTextureAndModel("captruck", "captruck", CLOTHES_TEXTURE_HATS);
        player->GetClothesDesc()->SetTextureAndModel("countrytr", "countrytr", CLOTHES_TEXTURE_SPECIAL);

        if (player->m_nPedState != PEDSTATE_DRIVING) {
            CClothes::RebuildPlayer(player, false);
        }
        if (IsActive(CHEAT_BEACH_PARTY)) {
            BeachPartyCheat();
        }
        if (IsActive(CHEAT_CHEAP_TRAFFIC)) {
            Disable(CHEAT_CHEAP_TRAFFIC);
        }
        if (IsActive(CHEAT_FAST_TRAFFIC)) {
            Disable(CHEAT_FAST_TRAFFIC);
        }
        if (IsActive(CHEAT_FUNHOUSE_THEME)) {
            FunhouseCheat();
        }

        CStreaming::ReclassifyLoadedCars();
    }
}

// 0x4398d0
void CCheat::DrivebyCheat() {
    Toggle(CHEAT_WEAPON_AIMING_WHILE_DRIVING);

    CPlayerPed *player = FindPlayerPed();
    if (IsActive(CHEAT_WEAPON_AIMING_WHILE_DRIVING) && player->m_aWeapons[WEAPON_KNIFE].m_Type == WEAPON_UNARMED) {
        player->GiveDelayedWeapon(WEAPON_MICRO_UZI, 150);
        player->SetCurrentWeapon(WEAPON_MICRO_UZI);
    }
}

// 0x439540
void CCheat::DuskCheat() {
    Toggle(CHEAT_STOP_GAME_CLOCK_ORANGE_SKY);
    if (IsActive(CHEAT_STOP_GAME_CLOCK_ORANGE_SKY)) { // Como el original (0x43954C..0x439555): pone las 21:00 al ACTIVAR
        CClock::ms_nGameClockHours = 21;
        CClock::ms_nGameClockMinutes = 0;
        CClock::ms_nGameClockSeconds = 0;
    }
}

// 0x4391d0
void CCheat::ElvisLivesCheat() {
    Toggle(CHEAT_ELVIS_IS_EVERYWHERE);
    if (IsActive(CHEAT_ELVIS_IS_EVERYWHERE)) {
        int32 peds[] = {
            MODEL_VHMYELV, MODEL_VBMYELV, MODEL_VIMYELV, UNLOAD_MODEL,
            UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);
    }
}

// 0x439c70
void CCheat::EverybodyAttacksPlayerCheat() {
    Toggle(CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD);
    if (IsActive(CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD)) {
        // Como el original (0x439CA1..0x439D5A): recorre el pool de peds del ultimo slot al primero
        auto* const pool = GetPedPool();
        for (auto i = pool->GetSize(); i > 0; --i) {
            auto* const ped = pool->GetAt(i - 1);
            if (!ped || ped->IsPlayer())
                continue;

            ped->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(PED_TYPE_PLAYER1));

            CEventAcquaintancePedHate event(FindPlayerPed());
            event.m_TaskId = TASK_COMPLEX_KILL_PED_ON_FOOT;
            ped->GetEventGroup().Add(&event, false);
        }
    }
}

// 0x438f90
void CCheat::FastTimeCheat() {
    if (CTimer::GetTimeScale() < 4.0f) {
        CTimer::SetTimeScale(CTimer::GetTimeScale() * 2.0f);
    }
}

// 0x438fc0
void CCheat::SlowTimeCheat() {
    if (CTimer::GetTimeScale() > 0.25f) {
        CTimer::SetTimeScale(CTimer::GetTimeScale() / 2.0f);
    }
}

// 0x439110
void CCheat::FatCheat() {
    CStats::SetStatValue(STAT_FAT, 1000.0f);

    CPlayerPed *player = FindPlayerPed();
    if (player->m_nPedState != PEDSTATE_DRIVING) {
        CClothes::RebuildPlayer(player, false);
    }
}

// 0x439720
void CCheat::FunhouseCheat() {
    Toggle(CHEAT_FUNHOUSE_THEME);
    if (IsActive(CHEAT_FUNHOUSE_THEME)) {
        // Como el original (0x43973B..0x439871): la rama activa carga los peds, viste al jugador y activa la distorsion
        int32 peds[] = {
            MODEL_WMYBELL, MODEL_WFYBURG, MODEL_WMOICE, UNLOAD_MODEL,
            UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);

        CPlayerPed* player = FindPlayerPed();
        player->GetClothesDesc()->SetTextureAndModel("torso", "torso", CLOTHES_TEXTURE_TORSO);
        player->GetClothesDesc()->SetTextureAndModel("legsheart", "legs", CLOTHES_TEXTURE_LEGS);
        player->GetClothesDesc()->SetTextureAndModel("timberhike", "bask1", CLOTHES_TEXTURE_SHOES);
        player->GetClothesDesc()->SetTextureAndModel("groucho", "grouchos", CLOTHES_TEXTURE_GLASSES);
        player->GetClothesDesc()->SetTextureAndModel("hairpink", "head", CLOTHES_TEXTURE_HEAD);

        if (player->m_nPedState != PEDSTATE_DRIVING) {
            CClothes::RebuildPlayer(player, false);
        }

        HandleSpecialCheats(CHEAT_FUNHOUSE_THEME);
        CStreaming::ReclassifyLoadedCars();
        ExtraSunnyWeatherCheat();
        CPostEffects::m_bHeatHazeFX = true;
    } else {
        // Como el original (0x439875): al desactivar solo se apaga la distorsion
        CPostEffects::m_bHeatHazeFX = false;
    }
}

// 0x4393d0
void CCheat::GangLandCheat() {
    Toggle(CHEAT_GANGS_CONTROLS_THE_STREETS);
    CPopulation::m_bOnlyCreateRandomGangMembers = IsActive(CHEAT_GANGS_CONTROLS_THE_STREETS);
}

// 0x439360
void CCheat::GangsCheat() {
    Toggle(CHEAT_GANGMEMBERS_EVERYWHERE);
    if (IsActive(CHEAT_GANGMEMBERS_EVERYWHERE)) {
        int32 peds[] = {
            MODEL_BALLAS1, MODEL_BALLAS2, MODEL_BALLAS3, MODEL_FAM1,
            MODEL_FAM2, MODEL_LSV1, MODEL_LSV2, MODEL_LSV3,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);
    }
}

// Handles BeachParty, Funhouse, AllCarsAreGreat, AllCarsAreCheap cheats toggling
// 0x439A10
void CCheat::HandleSpecialCheats(eCheats cheat) {
    if (IsActive(CHEAT_BEACH_PARTY) && cheat != CHEAT_BEACH_PARTY) {
        BeachPartyCheat();
    }
    if (IsActive(CHEAT_CHEAP_TRAFFIC) && cheat != CHEAT_CHEAP_TRAFFIC) {
        Disable(CHEAT_CHEAP_TRAFFIC);
    }
    if (IsActive(CHEAT_FAST_TRAFFIC) && cheat != CHEAT_FAST_TRAFFIC) {
        Disable(CHEAT_FAST_TRAFFIC);
    }
    if (IsActive(CHEAT_FUNHOUSE_THEME) && cheat != CHEAT_FUNHOUSE_THEME) {
        FunhouseCheat();
    }
    if (IsActive(CHEAT_COUNTRY_TRAFFIC) && cheat != CHEAT_COUNTRY_TRAFFIC) {
        CountrysideInvasionCheat();
    }
}

// 0x438D60
void CCheat::HealthCheat() {
    // Como el original (0x438D60..0x438D83): usa la vida maxima del jugador 0 (byte global 0xB7CEE7), no la del ped
    CPlayerPed* player = FindPlayerPed();
    player->m_fHealth = static_cast<float>(CWorld::Players[0].m_nMaxHealth);

    CVehicle* vehicle = FindPlayerVehicle();
    if (!vehicle) {
        return;
    }

    vehicle->m_fHealth = 1000.0f;
    if (vehicle->IsBike()) {
        vehicle->AsBike()->m_BlowUpTimer = 0.0f;
        vehicle->AsBike()->Fix();
    } else if (vehicle->IsAutomobile()) {
        vehicle->AsAutomobile()->m_fBurnTimer = 0.0f;
        vehicle->AsAutomobile()->Fix();
    }
}

// 0x439600
void CCheat::JetpackCheat() {
    auto player = FindPlayerPed();
    CTaskSimpleJetPack* task = player->GetIntelligence()->GetTaskJetPack();
    if (!task) {
        auto jetpackTask = new CTaskSimpleJetPack(nullptr, 10, 0, nullptr);
        CEventScriptCommand event(TASK_PRIMARY_PRIMARY, jetpackTask, false);
        player->GetEventGroup().Add(&event, false);
    }
}

// 0x4393f0
void CCheat::LoveConquersAllCheat() {
    Toggle(CHEAT_SLUT_MAGNET);
    if (IsActive(CHEAT_SLUT_MAGNET)) {
        int32 peds[] = {
            MODEL_BMYPIMP, MODEL_BFYPRO, MODEL_HFYPRO, MODEL_SWFOPRO,
            MODEL_SBFYPRO, MODEL_VWFYPRO, MODEL_VHFYPRO, UNLOAD_MODEL,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);

        CPlayerPed* player = FindPlayerPed();
        player->GetClothesDesc()->SetTextureAndModel("gimpleg", "gimpleg", CLOTHES_TEXTURE_SPECIAL);
        if (player->m_nPedState != PEDSTATE_DRIVING) {
            CClothes::RebuildPlayer(player, false);
        }
    }
}

// 0x439B20
void CCheat::MayhemCheat() {
    Toggle(CHEAT_PEDS_ATTACK_OTHER_WITH_GOLFCLUB);
    if (IsActive(CHEAT_PEDS_ATTACK_OTHER_WITH_GOLFCLUB)) {
        for (uint32 pedType = PED_TYPE_CIVMALE; pedType <= PED_TYPE_PROSTITUTE; pedType++) {
            CPedType::SetPedTypeAsAcquaintance(ACQUAINTANCE_HATE, static_cast<ePedType>(pedType), 0xFFFFF);
        }

        // Como el original (0x439B6E..0x439C5A): recorre el pool de peds del ultimo slot al primero
        auto* const pool = GetPedPool();
        for (auto i = pool->GetSize(); i > 0; --i) {
            auto* const ped = pool->GetAt(i - 1);
            if (!ped || ped->IsPlayer())
                continue;

            for (uint32 pedType_1 = PED_TYPE_CIVMALE; pedType_1 <= PED_TYPE_PROSTITUTE; ++pedType_1) {
                ped->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(static_cast<ePedType>(pedType_1)));
            }
            if (CPed* closestPed = ped->GetIntelligence()->GetPedScanner().GetClosestPedInRange()) {
                CEventAcquaintancePedHate event(closestPed);
                event.m_TaskId = TASK_COMPLEX_KILL_PED_ON_FOOT;
                ped->GetEventGroup().Add(&event, false);
            }
        }
    }
    // Como el original (0x439B4A): al desactivar no hace nada mas (sin intercambio de tablas de tipos ni baja de eventos)
}

// 0x439510
void CCheat::MidnightCheat() {
    Toggle(CHEAT_ALWAYS_MIDNIGHT);
    if (IsActive(CHEAT_ALWAYS_MIDNIGHT)) {
        CClock::ms_nGameClockHours = 0;
        CClock::ms_nGameClockMinutes = 0;
        CClock::ms_nGameClockSeconds = 0;
    }
}

// 0x438e40
void CCheat::MoneyArmourHealthCheat() {
    // Como el original (0x438E41..0x438E80): dinero del jugador en foco, armadura maxima del jugador 0 (0xB7CEE8)
    CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 250000;
    CPlayerPed* player = FindPlayerPed();
    player->m_fArmour = static_cast<float>(CWorld::Players[0].m_nMaxArmour);
    HealthCheat();
}

// 0x439150
void CCheat::MuscleCheat() {
    CPlayerPed* player = FindPlayerPed();

    CStats::SetStatValue(STAT_MUSCLE, 1000.0f);
    if (player->m_nPedState != PEDSTATE_DRIVING) {
        CClothes::RebuildPlayer(player, false);
    }
}

// 0x439e50
void CCheat::NinjaCheat() {
    Toggle(CHEAT_NINJA_THEME);
    if (IsActive(CHEAT_NINJA_THEME)) {
        // Como el original (0x439E6E..0x439F48): la rama ACTIVA carga los peds, apaga los otros temas y da la katana
        int32 peds[] = {
            MODEL_TRIADA, MODEL_TRIADB, MODEL_TRIBOSS, UNLOAD_MODEL,
            UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);

        if (IsActive(CHEAT_BEACH_PARTY)) {
            BeachPartyCheat();
        }
        if (IsActive(CHEAT_CHEAP_TRAFFIC)) {
            Disable(CHEAT_CHEAP_TRAFFIC);
        }
        if (IsActive(CHEAT_FAST_TRAFFIC)) {
            Disable(CHEAT_FAST_TRAFFIC);
        }
        if (IsActive(CHEAT_FUNHOUSE_THEME)) {
            FunhouseCheat();
        }
        if (IsActive(CHEAT_COUNTRY_TRAFFIC)) {
            CountrysideInvasionCheat();
        }
        CStreaming::ReclassifyLoadedCars();
        if (!IsActive(CHEAT_BLACK_TRAFFIC)) {
            Enable(CHEAT_BLACK_TRAFFIC);
            Disable(CHEAT_PINK_TRAFFIC);
        }

        CStreaming::RequestModel(MODEL_KATANA, STREAMING_GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);

        CPlayerPed* player = FindPlayerPed();
        player->GiveWeapon(WEAPON_KATANA, 0, true);
        player->SetCurrentWeapon(WEAPON_KATANA);
    } else {
        // Como el original (0x439F4D): al desactivar solo se quita el trafico negro
        if (IsActive(CHEAT_BLACK_TRAFFIC)) {
            Disable(CHEAT_BLACK_TRAFFIC);
        }
    }
}

// 0x4396c0
void CCheat::NotWantedCheat() {
    CPlayerPed* player = FindPlayerPed();
    player->CheatWantedLevel(eWantedLevel::WANTED_CLEAN);
    Toggle(CHEAT_I_DO_AS_I_PLEASE); // Como el original (0x4396C0..0x4396E2): no toca bWantedByPolice
}

// 0x4395b0
void CCheat::ParachuteCheat() {
    CStreaming::RequestModel(MODEL_GUN_PARA, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    CPlayerPed* player = FindPlayerPed();
    player->GiveWeapon(WEAPON_PARACHUTE, 0, true);
    player->SetCurrentWeapon(WEAPON_PARACHUTE);
}

// 0x4390d0
void CCheat::PinkCarsCheat() {
    Toggle(CHEAT_PINK_TRAFFIC);
    if (IsActive(CHEAT_PINK_TRAFFIC)) {
        Disable(CHEAT_BLACK_TRAFFIC);
    }
}

// 0x439710
void CCheat::RiotCheat() {
    Toggle(CHEAT_RIOT_MODE);
}

// 0x439190
void CCheat::SkinnyCheat() {
    CStats::SetStatValue(STAT_FAT, 0.0f);
    CStats::SetStatValue(STAT_MUSCLE, 0.0f);

    CPlayerPed* player = FindPlayerPed();
    if (player->m_nPedState != PEDSTATE_DRIVING) {
        CClothes::RebuildPlayer(player, false);
    }
}

// 0x439930
void CCheat::StaminaCheat() {
    CStats::SetStatValue(STAT_STAMINA, 1000.0f);
}

// 0x439570
void CCheat::StormCheat() {
    RainyWeatherCheat();
    CWeather::Rain = 1.0f;
    CWeather::Wind = 1.0f;
}

// 0x43a570
void CCheat::TankerCheat() {
    CVehicle* vehicle = VehicleCheat(MODEL_PETRO);
    if (!vehicle)
        return;

    CStreaming::RequestModel(MODEL_PETROTR, 0);
    CStreaming::LoadAllRequestedModels(false);

    if (!CStreaming::GetInfo(MODEL_PETROTR).IsLoaded())
        return;

    auto* trailer = new CTrailer(MODEL_PETROTR, RANDOM_VEHICLE);
    if (!trailer)
        return; // Como el original (0x43A5F3: test esi,esi / je): sin memoria en el pool no se hace nada mas
    trailer->SetPosn(vehicle->GetPosition());
    trailer->SetOrientation(0.0f, 0.0f, std::bit_cast<float>(0x405F66F3u)); // Como el original (0x43A616): literal exacto de 200 grados
    trailer->SetStatus(STATUS_ABANDONED);
    CWorld::Add(trailer);
    trailer->SetTowLink(vehicle, true);
}

// 0x43A0B0
CVehicle* CCheat::VehicleCheat(eModelID modelId) {
    const auto player = FindPlayerPed();
    if (player->GetAreaCode() != AREA_CODE_NORMAL_WORLD) {
        return nullptr;
    }

    // Como el original (0x43A0E2..0x43A105): si hay mas de 50 vehiculos vivos en el pool no crea nada
    {
        auto* const pool = GetVehiclePool();
        int32 numUsed = 0;
        for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
            if (!pool->IsFreeSlotAtIndex(i)) {
                numUsed++;
            }
        }
        if (numUsed > 50) {
            return nullptr;
        }
    }

    // Como el original (0x43A128): las banderas de streaming se leen ANTES de pedir el modelo
    const auto streamFlagsBefore = CStreaming::GetInfo(modelId).GetFlags();

    CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    if (!CStreaming::IsModelLoaded(modelId)) {
        return nullptr;
    }

    if (!(streamFlagsBefore & STREAMING_GAME_REQUIRED)) { // 0x43A14E: test bl,2
        CStreaming::SetModelIsDeletable(modelId);
        CStreaming::SetModelTxdIsDeletable(modelId);
    }

    // Como el original (0x43A162..0x43A32D): tabla de saltos por tipo de vehiculo del modelo; tren, helicoptero y avion
    // falsos y cualquier otro tipo caen en CAutomobile (no se usa CCarGenerator::CreateVehicle)
    CVehicle* vehicle;
    switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
    case VEHICLE_TYPE_MTRUCK:
        vehicle = new CMonsterTruck(modelId, RANDOM_VEHICLE);
        break;
    case VEHICLE_TYPE_QUAD:
        vehicle = new CQuadBike(modelId, RANDOM_VEHICLE);
        break;
    case VEHICLE_TYPE_HELI:
        vehicle = new CHeli(modelId, RANDOM_VEHICLE);
        break;
    case VEHICLE_TYPE_PLANE:
        vehicle = new CPlane(modelId, RANDOM_VEHICLE);
        break;
    case VEHICLE_TYPE_BOAT:
        vehicle = new CBoat(modelId, RANDOM_VEHICLE);
        break;
    case VEHICLE_TYPE_BIKE: {
        auto* const bike = new CBike(modelId, RANDOM_VEHICLE);
        bike->bikeFlags.bOnSideStand = true; // 0x43A269: or byte [eax+0x614],10h
        vehicle = bike;
        break;
    }
    case VEHICLE_TYPE_BMX: {
        auto* const bmx = new CBmx(modelId, RANDOM_VEHICLE);
        bmx->bikeFlags.bOnSideStand = true; // 0x43A29B
        vehicle = bmx;
        break;
    }
    case VEHICLE_TYPE_TRAILER:
        vehicle = new CTrailer(modelId, RANDOM_VEHICLE);
        break;
    default:
        vehicle = new CAutomobile(modelId, RANDOM_VEHICLE, true);
        break;
    }
    if (!vehicle) {
        return nullptr;
    }

    const float radius = vehicle->GetModelInfo()->GetColModel()->GetBoundRadius();
    const auto  rotZ   = player->m_fCurrentRotation + std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4: pi/2 exacto del exe
    const auto  pos    = player->GetPosition() + (radius + 2.0f) * player->GetForward();

    vehicle->SetPosn(pos);
    vehicle->SetOrientation(0.0f, 0.0f, rotZ);
    vehicle->SetStatus(STATUS_ABANDONED);
    vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
    CWorld::Add(vehicle);
    CTheScripts::ClearSpaceForMissionEntity(pos, vehicle);

    // Como el original (0x43A426..0x43A456): bici/moto -> CBike::PlaceOnRoadProperly; barco -> nada; el resto -> CAutomobile::PlaceOnRoadProperly
    if (vehicle->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        vehicle->AsBike()->PlaceOnRoadProperly();
    } else if (vehicle->m_nVehicleType != VEHICLE_TYPE_BOAT) {
        vehicle->AsAutomobile()->PlaceOnRoadProperly();
    }

    return vehicle;
}

// 0x43a550
void CCheat::ApacheCheat() {
    VehicleCheat(MODEL_HUNTER);
}

// 0x43a660
void CCheat::DozerCheat() {
    VehicleCheat(MODEL_DOZER);
}

// 0x43a530
void CCheat::FlyboyCheat() {
    VehicleCheat(MODEL_HYDRA);
}

// 0x43a520
void CCheat::GolfcartCheat() {
    VehicleCheat(MODEL_CADDY);
}

// 0x43a4f0
void CCheat::HearseCheat() {
    VehicleCheat(MODEL_ROMERO);
}

// 0x43a500
void CCheat::LovefistCheat() {
    VehicleCheat(MODEL_STRETCH);
}

// 0x43a680
void CCheat::MonsterTruckCheat() {
    VehicleCheat(MODEL_MONSTERA);
}

// unused
// 0x4395a0
void CCheat::PredatorCheat() {
    // Como el original: el cuerpo es solo un `ret`
}

// 0x43a560
void CCheat::QuadCheat() {
    VehicleCheat(MODEL_QUAD);
}
// 0x43a4b0
void CCheat::StockCarCheat() {
    VehicleCheat(MODEL_BLOODRA);
}

// 0x43a4c0
void CCheat::StockCar2Cheat() {
    VehicleCheat(MODEL_RNCHLURE);
}

// 0x43a4d0
void CCheat::StockCar3Cheat() {
    VehicleCheat(MODEL_HOTRINA);
}

// 0x43a4e0
void CCheat::StockCar4Cheat() {
    VehicleCheat(MODEL_HOTRINB);
}

// 0x43a670
void CCheat::StuntPlaneCheat() {
    VehicleCheat(MODEL_STUNT);
}

// 0x43a4a0
void CCheat::TankCheat() {
    VehicleCheat(MODEL_RHINO);
}

// 0x43a510
void CCheat::TrashmasterCheat() {
    VehicleCheat(MODEL_TRASH);
}

// 0x43a540
void CCheat::VortexCheat() {
    VehicleCheat(MODEL_VORTEX);
}

// 0x4399d0
void CCheat::VehicleSkillsCheat() {
    CStats::SetStatValue(STAT_DRIVING_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_FLYING_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_BIKE_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_CYCLING_SKILL, 1000.0f);
}

// 0x439c70
void CCheat::VillagePeopleCheat() {
    Toggle(CHEAT_PEDS_ATTACK_YOU_WITH_ROCKETS);
    if (IsActive(CHEAT_PEDS_ATTACK_YOU_WITH_ROCKETS)) {
        // Como el original (0x439DE7..0x439E39): la rama ACTIVA carga los peds y activa EVERYONE_ARMED
        int32 peds[] = {
            MODEL_WMYCON, MODEL_CWMYFR, MODEL_ARMY, MODEL_LAPDM1,
            MODEL_BIKERB, UNLOAD_MODEL, UNLOAD_MODEL, UNLOAD_MODEL,
        };
        CStreaming::StreamPedsIntoRandomSlots(peds);
        EverybodyAttacksPlayerCheat();
        Enable(CHEAT_EVERYONE_ARMED);
    } else {
        Disable(CHEAT_EVERYONE_ARMED);
        EverybodyAttacksPlayerCheat();
    }
}

// 0x4396f0
void CCheat::WantedCheat() {
    CPlayerPed* player = FindPlayerPed();
    player->CheatWantedLevel(eWantedLevel::WANTED_LEVEL_6);
}

// 0x438f20
void CCheat::WantedLevelDownCheat() {
    CPlayerPed* player = FindPlayerPed();
    player->CheatWantedLevel(eWantedLevel::WANTED_CLEAN);
}

// 0x438e90
void CCheat::WantedLevelUpCheat() {
    CPlayerPed* player = FindPlayerPed(); // Como el original (0x438E90): sin comprobar el puntero

    const auto level = player->GetWantedLevel();
    player->CheatWantedLevel((eWantedLevel)(std::min(+level + +eWantedLevel::WANTED_LEVEL_2, +eWantedLevel::WANTED_LEVEL_6)));
}

// refactored
// 0x4385b0
void CCheat::WeaponCheat1() {
    static constexpr eModelID weapons[] = {
        MODEL_BRASSKNUCKLE, MODEL_BAT,       MODEL_MOLOTOV, MODEL_COLT45,
        MODEL_CHROMEGUN,    MODEL_MICRO_UZI, MODEL_AK47,    MODEL_CUNTGUN,
        MODEL_ROCKETLA,     MODEL_SPRAYCAN
    };
    std::ranges::for_each(weapons, [](auto model) { CStreaming::RequestModel(model, STREAMING_GAME_REQUIRED); });
    CStreaming::LoadAllRequestedModels(false);

    // Como el original (0x438637..0x438824): el primer jugador es el del foco (-1) y el segundo recibe su propio juego
    CPlayerPed* player = FindPlayerPed(-1);
    player->GiveWeaponSet1();

    CPlayerPed* player1 = FindPlayerPed(1);
    if (player1) {
        player1->GiveWeaponSet1();
    }

    // Como el original (0x438825..0x43887F): 9 modelos, en este orden, y sin MODEL_COLT45
    static constexpr eModelID deletable[] = {
        MODEL_BRASSKNUCKLE, MODEL_MICRO_UZI, MODEL_BAT,      MODEL_MOLOTOV, MODEL_CHROMEGUN,
        MODEL_AK47,         MODEL_CUNTGUN,   MODEL_ROCKETLA, MODEL_SPRAYCAN
    };
    std::ranges::for_each(deletable, [](auto model) { CStreaming::SetModelIsDeletable(model); });
}

// refactored
// 0x438890
void CCheat::WeaponCheat2() {
    static constexpr eModelID weapons[] = {
        MODEL_KNIFECUR, MODEL_GRENADE, MODEL_DESERT_EAGLE, MODEL_SAWNOFF,
        MODEL_TEC9,     MODEL_M4,      MODEL_SNIPER,       MODEL_FLAME,
        MODEL_FIRE_EX
    };
    std::ranges::for_each(weapons, [](auto model) { CStreaming::RequestModel(model, STREAMING_GAME_REQUIRED); });
    CStreaming::LoadAllRequestedModels(false);

    CPlayerPed* player = FindPlayerPed(-1); // Como el original (0x43890D): jugador en foco
    player->GiveWeaponSet2();

    CPlayerPed* player1 = FindPlayerPed(1);
    if (player1) {
        player1->GiveWeaponSet2();
    }

    std::ranges::for_each(weapons, [](auto model) { CStreaming::SetModelIsDeletable(model); });
}

// refactored
// 0x438b30
void CCheat::WeaponCheat3() {
    static constexpr eModelID weapons[] = {
        MODEL_CHNSAW, MODEL_SILENCED, MODEL_SHOTGSPA, MODEL_MP5LNG,
        MODEL_M4,     MODEL_HEATSEEK, MODEL_SATCHEL,  MODEL_BOMB
    };
    std::ranges::for_each(weapons, [](auto model) { CStreaming::RequestModel(model, STREAMING_GAME_REQUIRED); });
    CStreaming::LoadAllRequestedModels(false);

    CPlayerPed* player = FindPlayerPed(-1); // Como el original (0x438B9D): jugador en foco
    player->GiveWeaponSet3();

    CPlayerPed* player1 = FindPlayerPed(1);
    if (player1) {
        player1->GiveWeaponSet3();
    }

    std::ranges::for_each(weapons, [](auto model) { CStreaming::SetModelIsDeletable(model); });
}

// todo: add WEAPON_NIGHTVISION, WEAPON_INFRARED
// Android
void CCheat::WeaponCheat4() {
    const eModelID weapons[] = { MODEL_MINIGUN, MODEL_GUN_DILDO2 };
    std::ranges::for_each(weapons, [](auto model) { CStreaming::RequestModel(model, STREAMING_GAME_REQUIRED); });
    CStreaming::LoadAllRequestedModels(false);

    auto* player = FindPlayerPed(0);
    player->GiveWeapon(WEAPON_MINIGUN, 500, true);
    player->GiveWeapon(WEAPON_DILDO2, 0, true);

    auto* player1 = FindPlayerPed(1);
    if (player1) {
        player->GiveWeapon(WEAPON_MINIGUN, 500, true);
        player->GiveWeapon(WEAPON_DILDO2, 0, true);
    }

    std::ranges::for_each(weapons, [](auto model) { CStreaming::SetModelIsDeletable(model); });
}

// 0x439940
void CCheat::WeaponSkillsCheat() {
    CStats::SetStatValue(STAT_PISTOL_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_SILENCED_PISTOL_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_DESERT_EAGLE_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_SHOTGUN_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_SAWN_OFF_SHOTGUN_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_COMBAT_SHOTGUN_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_MACHINE_PISTOL_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_SMG_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_AK_47_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_M4_SKILL, 1000.0f);
    CStats::SetStatValue(STAT_RIFLE_SKILL, 1000.0f);
}

// 0x438f60
void CCheat::CloudyWeatherCheat() {
    CWeather::ForceWeatherNow(WEATHER_CLOUDY_LA);
}

// 0x438f50
void CCheat::ExtraSunnyWeatherCheat() {
    CWeather::ForceWeatherNow(WEATHER_EXTRASUNNY_LA);
}

// 0x438f80
void CCheat::FoggyWeatherCheat() {
    CWeather::ForceWeatherNow(WEATHER_FOGGY_SF);
}

// 0x438f70
void CCheat::RainyWeatherCheat() {
    CWeather::ForceWeatherNow(WEATHER_RAINY_COUNTRYSIDE);
}

// 0x439590
void CCheat::SandstormCheat() {
    CWeather::ForceWeatherNow(WEATHER_SANDSTORM_DESERT);
}

// 0x438f40
void CCheat::SunnyWeatherCheat() {
    CWeather::ForceWeatherNow(WEATHER_SUNNY_LA);
}

// 0x438ff0
void CCheat::SuicideCheat() {
    CPedDamageResponseCalculator damageCalculator(nullptr, 1000.0f, WEAPON_UNARMED, PED_PIECE_TORSO, false);
    CEventDamage damageEvent(nullptr, CTimer::GetTimeInMS(), WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    CPlayerPed* player = FindPlayerPed();
    if (damageEvent.AffectsPed(player))
        damageCalculator.ComputeDamageResponse(player, damageEvent.m_damageResponse, true);
    else
        damageEvent.m_damageResponse.m_bDamageCalculated = true;
    player->GetEventGroup().Add(&damageEvent, false);
}

// 0x407410
bool CCheat::IsZoneStreamingAllowed() {
    return IsActive(CHEAT_ELVIS_IS_EVERYWHERE)
        || IsActive(CHEAT_PEDS_ATTACK_YOU_WITH_ROCKETS)
        || IsActive(CHEAT_BEACH_PARTY)
        || IsActive(CHEAT_GANGMEMBERS_EVERYWHERE)
        || IsActive(CHEAT_NINJA_THEME)
        || IsActive(CHEAT_SLUT_MAGNET)
        || IsActive(CHEAT_FUNHOUSE_THEME)
        || IsActive(CHEAT_COUNTRY_TRAFFIC);
}

// Android
void CCheat::TimeTravelCheat() {
    CClock::SetGameClock(CClock::GetGameClockHours() + 4, CClock::GetGameClockMinutes(), CClock::GetGameClockDays());
}

// Android
void CCheat::TheGamblerCheat() {
    CStats::SetStatValue(eStats::STAT_GAMBLING, 1000.0f);
}

void CCheat::BigHeadCheat() {
    Toggle(CHEAT_BIG_HEAD);
}

void CCheat::ThinBodyCheat() {
    Toggle(CHEAT_THIN_BODY);
}

// **** DEBUG STUFF ****

// Android
void CCheat::TogglePlayerInvincibility() {
    CPlayerPed::bDebugPlayerInvincible ^= true;
}

// Android
void CCheat::ToggleShowTargeting() {
    CPlayerPed::bDebugTargeting ^= true;
}

// Android
void CCheat::ToggleShowTapToTarget() {
    CPlayerPed::bDebugTapToTarget ^= true;
}

// Android
void CCheat::ShowMappingsCheat() {
    m_bShowMappings ^= true;
}

// Android
void CCheat::ScriptBypassCheat() {
    m_nLastScriptBypassTime = CTimer::GetTimeInMS();
}

void CCheat::ProcessAllCheats() {
    ProcessDebugCarCheats();
    ProcessDebugMissionSkip();
    ProcessCheats();
    ProcessCheatMenu();
    ProcessWeaponSlotCheats();
}

void CCheat::ProcessDebugCarCheats() {

}

void CCheat::ProcessDebugMissionSkip() {

}

void CCheat::ProcessCheats() {

}

void CCheat::ProcessCheatMenu() {

}

enum eCheatWeaponSlot {
    SLOT_MELEE,
    SLOT_HANDGUN,
    SLOT_SMG,
    SLOT_SHOTGUN,
    SLOT_ASSAULT_RIFLES,
    SLOT_LONG_RIFLES,
    SLOT_THROWN,
    SLOT_HEAVY_ARTILLERY,
    SLOT_EQUIPMENT,
    SLOT_OTHER,
};

static int32 g_CheatWeaponSlot = -1;
static bool  g_bDisplayCheatWeaponSlot = true;

// Android
void CCheat::ProcessWeaponSlotCheats() {
    if (!g_bDisplayCheatWeaponSlot)
        return;

    CTimer::StartUserPause();
    switch (g_CheatWeaponSlot) {
    case SLOT_MELEE:
        // HandleSlotMelee();
        break;
    case SLOT_HANDGUN:
        // HandleSlotHandguns();
        break;
    case SLOT_SMG:
        // HandleSlotSubmachineGuns();
        break;
    case SLOT_SHOTGUN:
        // HandleSlotShotguns();
        break;
    case SLOT_ASSAULT_RIFLES:
        // HandleSlotAssaultRifles();
        break;
    case SLOT_LONG_RIFLES:
        // HandleSlotLongRifles();
        break;
    case SLOT_THROWN:
        // HandleSlotThrow();
        break;
    case SLOT_HEAVY_ARTILLERY:
        // HandleSlotArtillery();
        break;
    case SLOT_EQUIPMENT:
        // HandleSlotEquipment();
        break;
    case SLOT_OTHER:
        // HandleSlotOther();
        break;
    default:
        break;
    }
    CTimer::EndUserPause();
}

// Can be realized through CMenuSystem
// Android
void CCheat::WeaponSlotCheat() {
    // SLOT_MELEE           "NIGHTSTICK" "BRASS KNUCKLES" "POOL CUE" "BASEBALL BAT" "CHAINSAW" "KATANA" "GOLF CLUB" "SHOVEL" "KNIFE"
    // SLOT_HANDGUN         "DESERT EAGLE" "SLIENCED 9MM"
    // SLOT_SMG             "TEC-9" "MICRO SMG"
    // SLOT_SHOTGUN         "SHOTGUN" "SPAS"
    // SLOT_ASSAULT_RIFLES  "AK47" "M4"
    // SLOT_LONG_RIFLES     "RIFLE" "SNIPER RIFLE"
    // SLOT_THROWN          "GRENADE" "MOLOTOV COCKTAIL" "C4" "TEAR GAS"
    // SLOT_HEAVY_ARTILLERY "MINI GUN" "FLAME THROWER" "HS ROCKET LAUNCHER" "ROCKET LAUNCHER"
    // SLOT_EQUIPMENT       "NIGHT-VISION GOGGLES" "FIRE EXTINGUISHER" "SPRAY CAN" "PARACHUTE" "CAMERA" "THERMAL GOGGLES"
    // SLOT_OTHER           "VIBRA2" "DILDO1" "CANE" "DILDO2" "FLOWERS" "VIBRA1"
}

void CCheat::InjectHooks() {
    RH_ScopedClass(CCheat);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddToCheatString, 0x438480);
    RH_ScopedInstall(HandleSpecialCheats, 0x439A10);
    RH_ScopedInstall(DoCheats, 0x439AF0);
    RH_ScopedInstall(ResetCheats, 0x438450);
    RH_ScopedInstall(IsZoneStreamingAllowed, 0x407410);
    RH_ScopedInstall(ApplyCheat, 0x438370);
    RH_ScopedInstall(VehicleCheat, 0x43A0B0);

    for (auto& cheat: cheats) {
        if (cheat.installAddress == 0x0) {
            continue;
        }

        // La macro RH_ScopedNamedGlobalInstall usa `&fn`: con `cheat.method` (un void* con la direccion de la funcion) daria la direccion del
        // CAMPO de la tabla (datos no ejecutables) y no la de la funcion. Se pasa el puntero a la funcion directamente.
        ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, cheat.methodName, cheat.installAddress, cheat.method);

        for (auto& cheatFunc: CCheat::m_aCheatFunctions) {
            if (reinterpret_cast<unsigned long>(cheatFunc) == cheat.installAddress) {
                cheatFunc = static_cast<void (*)()>(cheat.method);
            }
        }
    }
}

