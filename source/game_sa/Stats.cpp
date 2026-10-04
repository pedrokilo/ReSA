/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStats
 * Reconstrucción: COMPLETA — 32/32 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 32/32 entradas.
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

#include "Stats.h"
#include "MenuSystem.h"
#include "Hud.h"
#include "World.h"
#include "FileMgr.h"
#include "FileLoader.h"

namespace {
// Como el original: _ftol2 (0x821B40) trunca a 64 bits y se queda con la palabra baja.
int32 StatsFtol(float v) { return static_cast<int32>(static_cast<int64>(v)); }

// Como el original: las estadísticas enteras se indexan con 0xB78E20 + stat*4 sin comprobar el rango
// (stat 82..119 lee memoria anterior a StatTypesInt).
int32& IntStat(eStats stat) { return CStats::StatTypesInt.data()[static_cast<int32>(stat) - static_cast<int32>(CStats::FIRST_INT_STAT)]; }

// Como el original: un contador sin signo cargado con fild (+2^32 si el bit alto está puesto, sumado en x87
// a 24 bits); exacto mientras el valor cabe en 31 bits.
double CounterAsX87(uint32 c) {
    return static_cast<int32>(c) >= 0 ? static_cast<double>(c) : static_cast<double>(static_cast<float>(static_cast<double>(c)));
}

// Como el original: ftol(CTimer::ms_fTimeStep * 0.02f (0x858B38) * 1000.0f) en milisegundos enteros.
int32 TimeStepMsInt() { return StatsFtol(CTimer::GetTimeStep() * 0.02f * 1000.0f); }

// Como el original: RegisterFastestTime (0x55A0B0) y RegisterBestPosition (0x55A160) tienen el mismo cuerpo:
// si la estadística vale 0 se pone el valor; si no, se queda el menor (NaN -> el nuevo valor). Las enteras se
// guardan como int16 con signo (ftol + movsx).
void RegisterLowerStat(eStats stat, int32 value) {
    if (CStats::IsStatFloat(stat)) {
        float& s = CStats::StatTypesFloat[stat];
        if (s == 0.0f) {
            s = static_cast<float>(value);
        } else {
            const float t = static_cast<float>(value);
            s = (s < t) ? s : t;
        }
    } else {
        int32& s = IntStat(stat);
        if (s == 0) {
            s = static_cast<int16>(value);
        } else {
            const float t = static_cast<float>(value);
            s = (static_cast<double>(s) < static_cast<double>(t)) ? static_cast<int16>(s) : static_cast<int16>(StatsFtol(t));
        }
    }
    CStats::CheckForStatsMessage();
}
} // namespace

void CStats::InjectHooks() {
    RH_ScopedClass(CStats);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x55C0C0);
    RH_ScopedOverloadedInstall(GetStatValue, "-OG", 0x558E40, float(*)(eStats));
    RH_ScopedInstall(SetStatValue, 0x55A070);
    RH_ScopedInstall(IsStatFloat, 0x558E30);
    /*RH_ScopedInstall(GetFullFavoriteRadioStationList, 0x558F90); - different return type*/
    RH_ScopedInstall(FindCriminalRatingNumber, 0x559080);
    RH_ScopedInstall(GetPercentageProgress, 0x5591E0);
    RH_ScopedInstall(ConvertToMins, 0x559540);
    RH_ScopedInstall(ConvertToSecs, 0x559560);
    RH_ScopedInstall(SafeToShowThisStat, 0x559590);
    RH_ScopedInstall(CheckForThreshold, 0x5595F0);
    RH_ScopedInstall(IsStatCapped, 0x559630);
    RH_ScopedInstall(LoadActionReactionStats, 0x5599B0);
    RH_ScopedInstall(FindMaxNumberOfGroupMembers, 0x559A50);
    RH_ScopedInstall(ProcessReactionStatsOnDecrement, 0x559730);
    RH_ScopedInstall(DecrementStat, 0x559FA0);
    RH_ScopedInstall(SetNewRecordStat, 0x55C410);
    RH_ScopedInstall(RegisterFastestTime, 0x55A0B0);
    RH_ScopedInstall(RegisterBestPosition, 0x55A160);
    RH_ScopedInstall(ProcessReactionStatsOnIncrement, 0x55B900);
    RH_ScopedInstall(DisplayScriptStatUpdateMessage, 0x55B980);
    RH_ScopedInstall(IncrementStat, 0x55C180);
    RH_ScopedInstall(UpdateStatsWhenSprinting, 0x55C660);
    RH_ScopedInstall(UpdateStatsWhenRunning, 0x55C6F0);
    RH_ScopedInstall(UpdateStatsWhenOnMotorBike, 0x55CD60);
    RH_ScopedInstall(UpdateStatsWhenFighting, 0x55CFA0);
    RH_ScopedInstall(ModifyStat, 0x55D090);

    // unused
    RH_ScopedInstall(GetStatID, 0x558DE0);
    RH_ScopedInstall(GetTimesMissionAttempted, 0x558E70);
    RH_ScopedInstall(RegisterMissionAttempted, 0x558E80);
    RH_ScopedInstall(RegisterMissionPassed, 0x558EA0);

    RH_ScopedInstall(Load, 0x5D3BF0);
    RH_ScopedInstall(Save, 0x5D3B40);
}

// 0x55C0C0
void CStats::Init() {
    // Como el original: mismo orden de escrituras y llamadas que el EXE.
    std::ranges::fill(StatTypesFloat, 0.0f);
    bStatUpdateMessageDisplayed = false;
    CTimer::m_snTimeInMilliseconds = 0;
    std::ranges::fill(StatTypesInt, 0);

    StatTypesFloat[STAT_MAX_HEALTH] = 569.0f;
    CheckForStatsMessage();
    StatTypesFloat[STAT_STAMINA] = 100.0f;
    CheckForStatsMessage();

    std::ranges::fill(PedsKilledOfThisType, 0);
    std::ranges::fill(TimesMissionAttempted, 0);
    std::ranges::fill(LastMissionPassedName, 0);

    PopulateFavoriteRadioStationList();
    LoadActionReactionStats();
    LoadStatUpdateConditions();

    m_SprintStaminaCounter = 0;
    m_CycleStaminaCounter = 0;
    m_SwimStaminaCounter = 0;
    m_DrivingCounter = 0;
    m_FlyingCounter = 0;
    m_BoatCounter = 0;
    m_BikeCounter = 0;
    m_FatCounter = 0;
    m_RunningCounter = 0;
}

// 0x558E40
float CStats::GetStatValue(eStats stat) {
    if (!IsStatFloat(stat)) { // int32
        // Como el original: sin comprobar stat >= FIRST_INT_STAT (fild de 0xB78E20 + stat*4).
        return static_cast<float>(IntStat(stat));
    }

    return StatTypesFloat[stat];
}

// 0x55A070
void CStats::SetStatValue(eStats stat, float value) {
    if (IsStatFloat(stat)) {
        StatTypesFloat[stat] = value;
    } else { // int32
        // Como el original: ftol y movsx ax -> solo se guardan 16 bits con signo.
        IntStat(stat) = static_cast<int16>(StatsFtol(value));
    }
    CheckForStatsMessage();
}

// 0x558E30
bool CStats::IsStatFloat(eStats stat) {
    return stat < FIRST_UNUSED_STAT;
}

// 0x558EC0
bool CStats::PopulateFavoriteRadioStationList() {
    return plugin::CallAndReturn<bool, 0x558EC0>();
}

// 0x558FA0
eRadioID CStats::FindMostFavoriteRadioStation() {
    return plugin::CallAndReturn<eRadioID, 0x558FA0>();
}

// 0x559010
int32 CStats::FindLeastFavoriteRadioStation() {
    return plugin::CallAndReturn<int32, 0x559010>();
}

// 0x559080
int32 CStats::FindCriminalRatingNumber() {
    // Como el original: todo en x87 (enteros cargados con fild sin redondear, cada operación a la precisión
    // vigente), dinero de CWorld::Players[PlayerInFocus] dividido entre 5000 como entero, y
    // kills - busted*3 - hospital*3 (las visitas al hospital restan).
    int32 kills    = IntStat(STAT_TOTAL_LEGITIMATE_KILLS);
    int32 busted   = IntStat(STAT_TIMES_BUSTED);
    int32 hospital = IntStat(STAT_NUMBER_OF_HOSPITAL_VISITS);
    int32 fireLvl  = IntStat(STAT_HIGHEST_FIREFIGHTER_MISSION_LEVEL);
    int32 paraLvl  = IntStat(STAT_HIGHEST_PARAMEDIC_MISSION_LEVEL);
    int32 money    = CWorld::Players[CWorld::PlayerInFocus].m_nMoney / 5000;
    int32 planes   = IntStat(STAT_PLANES_HELICOPTERS_DESTROYED);
    int32 firesOut = IntStat(STAT_TOTAL_FIRES_EXTINGUISHED);
    int32 vigilant = IntStat(STAT_CRIMINALS_KILLED_ON_VIGILANTE_MISSION);
    int32 savedAmb = IntStat(STAT_PEOPLE_SAVED_IN_AN_AMBULANCE);
    float k3 = 3.0f, k10 = 10.0f, k30 = 30.0f, k500 = 500.0f, k1000 = 1000.0f;
    float sum;
    __asm {
        fild  kills
        fild  busted
        fmul  k3
        fsubp st(1), st
        fild  hospital
        fmul  k3
        fsubp st(1), st
        fild  fireLvl
        fild  paraLvl
        faddp st(1), st
        fmul  k10
        faddp st(1), st
        fiadd money
        fild  planes
        fmul  k30
        faddp st(1), st
        fild  firesOut
        faddp st(1), st
        fild  vigilant
        faddp st(1), st
        fild  savedAmb
        faddp st(1), st
        fstp  sum
    }
    int32 value = StatsFtol(sum);

    int32 cheated = IntStat(STAT_TIMES_CHEATED);
    if (!CCheat::m_bHasPlayerCheated && cheated <= 0) {
        value = std::max(value, 0);
    } else {
        float tmpCheat;
        __asm {
            fild  value
            fild  cheated
            fmul  k10
            fsubp st(1), st
            fstp  tmpCheat
        }
        value = StatsFtol(tmpCheat);
        if (value <= -10000) {
            value = -10000;
        }
    }

    // Como el original: solo con más de 100 balas (estricto); hits/balas*500 + valor en x87 y luego ftol.
    int32 bullets = IntStat(STAT_BULLETS_FIRED);
    if (bullets > 100) {
        int32 hits = IntStat(STAT_BULLETS_THAT_HIT);
        float tmpBullets;
        __asm {
            fild  bullets
            fild  hits
            fdiv  st, st(1)
            fmul  k500
            fiadd value
            fstp  tmpBullets
            fstp  st(0)
        }
        value = StatsFtol(tmpBullets);
    }

    // Como el original: GetPercentageProgress en línea: hecho/total*1000 + valor (nada si el total es 0).
    float made = StatTypesFloat[STAT_PROGRESS_MADE], total = StatTypesFloat[STAT_TOTAL_PROGRESS];
    if (total == 0.0f) {
        return value;
    }
    float tmpProgress;
    __asm {
        fld   made
        fdiv  total
        fmul  k1000
        fiadd value
        fstp  tmpProgress
    }
    return StatsFtol(tmpProgress);
}

// 0x5591E0
float CStats::GetPercentageProgress() {
    // Como el original: STAT_PROGRESS_MADE / STAT_TOTAL_PROGRESS * 100, 0 si el total es 0, máximo 100.
    if (StatTypesFloat[STAT_TOTAL_PROGRESS] == 0.0f) {
        return 0.0f;
    }
    const float progress = StatTypesFloat[STAT_PROGRESS_MADE] / StatTypesFloat[STAT_TOTAL_PROGRESS] * 100.0f;
    return (100.0f < progress) ? 100.0f : progress;
}

// 0x559230
void CStats::BuildStatLine(char* line, void* pValue1, int32 metrics, void* pValue2, int32 type) {
    plugin::Call<0x559230, char*, void*, int32, void*, int32>(line, pValue1, metrics, pValue2, type);
}

// 0x559540
int32 CStats::ConvertToMins(int32 statValue) {
    if (statValue > 59)
        return (statValue - 60) / 60 + 1;
    return 0;
}

// 0x559560
int32 CStats::ConvertToSecs(int32 statValue) {
    int32 seconds = statValue;
    if (statValue > 59)
        seconds = -60 - 60 * ((statValue - 60) / 60) + statValue;
    if (seconds < 0)
        seconds = -seconds;
    return seconds;
}

// 0x559590
bool CStats::SafeToShowThisStat(eStats stat) {
    if (!CLocalisation::GermanGame()) {
        return true;
    }

    // Como el original: la tabla de saltos usa solo el byte bajo de la estadística.
    switch (static_cast<uint8>(stat)) {
    case STAT_RAMPAGES_ATTEMPTED:
    case STAT_RAMPAGES_PASSED:
    case STAT_TOTAL_LEGITIMATE_KILLS:
    case STAT_HIGHEST_CIVILIAN_PEDS_KILLED_ON_RAMPAGE:
    case STAT_HIGHEST_POLICE_PEDS_KILLED_ON_RAMPAGE:
    case STAT_HIGHEST_CIVILIAN_VEHICLES_DESTROYED_ON_RAMPAGE:
    case STAT_HIGHEST_POLICE_VEHICLES_DESTROYED_ON_RAMPAGE:
    case STAT_HIGHEST_NUMBER_OF_TANKS_DESTROYED_ON_RAMPAGE:
        return false;
    default:
        return true;
    }
}

// 0x5595F0
bool CStats::CheckForThreshold(float* pValue, float range) {
    // Como el original: comparaciones estrictas; con NaN no se cambia nada y devuelve false.
    if (*pValue + 40.0f < range || *pValue - 40.0f > range) {
        *pValue = range;
        return true;
    }
    return false;
}

// 0x559630
bool CStats::IsStatCapped(eStats stat) {
    switch (stat) {
    case STAT_GIRLFRIEND_RESPECT:
    case STAT_CLOTHES_RESPECT:
    case STAT_FITNESS_RESPECT:
    case STAT_FAT:
    case STAT_STAMINA:
    case STAT_MUSCLE:
    case STAT_MAX_HEALTH:
    case STAT_SEX_APPEAL:
    case STAT_PISTOL_SKILL:
    case STAT_SILENCED_PISTOL_SKILL:
    case STAT_DESERT_EAGLE_SKILL:
    case STAT_SHOTGUN_SKILL:
    case STAT_SAWN_OFF_SHOTGUN_SKILL:
    case STAT_COMBAT_SHOTGUN_SKILL:
    case STAT_MACHINE_PISTOL_SKILL:
    case STAT_SMG_SKILL:
    case STAT_AK_47_SKILL:
    case STAT_M4_SKILL:
    case STAT_RIFLE_SKILL:
    case STAT_APPEARANCE:
    case STAT_ARMOR:
    case STAT_ENERGY:
    case STAT_DRIVING_SKILL:
    case STAT_FLYING_SKILL:
    case STAT_LUNG_CAPACITY:
    case STAT_BIKE_SKILL:
    case STAT_LUCK:
    case STAT_HORSESHOES_COLLECTED:
    case STAT_TOTAL_HORSESHOES:
    case STAT_OYSTERS_COLLECTED:
    case STAT_TOTAL_OYSTERS:
    case STAT_CYCLING_SKILL:
        return true;
    }
    return false;
}

// 0x559760
void CStats::CheckForStatsMessage() {
    plugin::Call<0x559760>();
}

// 0x559860
void CStats::LoadStatUpdateConditions() {
    plugin::Call<0x559860>();
}

// 0x5599B0
void CStats::LoadActionReactionStats() {
    // Como el original: id y valor se inicializan a 0 una sola vez y conservan el valor anterior si
    // sscanf("%d %s %f") no los lee; no se comprueba el resultado ni el rango del id.
    float reactValue = 0.0f;
    int32 reactId = 0;
    char  name[80];

    CFileMgr::SetDir("");

    auto* file = CFileMgr::OpenFile("DATA\\AR_STATS.DAT", "rb");

    for (char* line = CFileLoader::LoadLine(file); line != nullptr; line = CFileLoader::LoadLine(file)) {
        if (line[0] != '#' && line[0] != '\0') {
            (void)sscanf(line, "%d %s %f", &reactId, name, &reactValue);

            StatReactionValue.data()[reactId] = reactValue;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x559A50
int32 CStats::FindMaxNumberOfGroupMembers() {
    float respect = StatTypesFloat[STAT_TOTAL_RESPECT];

    if (respect < 10.0f)
        return 0;
    if (respect < 60.0f)
        return 2;
    if (respect < 160.0f)
        return 3;
    if (respect < 330.0f)
        return 4;
    if (respect < 540.0f)
        return 5;
    if (respect < 800.0f)
        return 6;

    return 7;
}

// 0x559AF0
float CStats::GetFatAndMuscleModifier(eStatModAbilities statMod) {
    return plugin::CallAndReturn<float, 0x559AF0, eStatModAbilities>(statMod);
}

// 0x559730
void CStats::ProcessReactionStatsOnDecrement(eStats stat) {
    // Como el original: compara solo el byte bajo de la estadística.
    if (static_cast<uint8>(stat) == STAT_ENERGY && IntStat(STAT_ENERGY) < 0)
        DecrementStat(STAT_FAT, 23.0f);
}

// 0x559FA0
void CStats::DecrementStat(eStats stat, float value) {
    // Como el original: sale si value <= 0 o NaN.
    if (!(value > 0.0f))
        return;

    // Como el original: no pasa por SetStatValue (un solo CheckForStatsMessage al final); las enteras restan
    // ftol(value) truncado a int16 con signo; el resultado se limita a > 0 (NaN -> 0).
    if (IsStatFloat(stat)) {
        const float newValue = StatTypesFloat[stat] - value;
        StatTypesFloat[stat] = (newValue > 0.0f) ? newValue : 0.0f;
    } else {
        int32& s = IntStat(stat);
        s -= static_cast<int16>(StatsFtol(value));
        s = (s > 0) ? s : 0;
    }

    ProcessReactionStatsOnDecrement(stat);
    CheckForStatsMessage();
}

// 0x55C410
void CStats::SetNewRecordStat(eStats stat, float value) {
    // Como el original: se escribe siempre max(actual, value) (NaN -> value), las enteras como int16 con signo,
    // y un solo CheckForStatsMessage.
    if (IsStatFloat(stat)) {
        const float current = StatTypesFloat[stat];
        StatTypesFloat[stat] = (current > value) ? current : value;
    } else {
        int32& s = IntStat(stat);
        s = (static_cast<double>(s) > static_cast<double>(value)) ? static_cast<int16>(s) : static_cast<int16>(StatsFtol(value));
    }

    CheckForStatsMessage();
}

// 0x55A0B0
void CStats::RegisterFastestTime(eStats stat, int32 fastestTime) {
    // Como el original: guarda el menor tiempo (o el primero si la estadística vale 0), no el mayor.
    RegisterLowerStat(stat, fastestTime);
}

// 0x55A160
void CStats::RegisterBestPosition(eStats stat, int32 position) {
    // Como el original: mismo cuerpo que RegisterFastestTime (se queda la menor posición).
    RegisterLowerStat(stat, position);
}

// 0x55A210
GxtChar* CStats::FindCriminalRatingString() {
    return plugin::CallAndReturn<GxtChar*, 0x55A210>();
}

// 0x55A780
int32 CStats::ConstructStatLine(int32 arg0, uint8 arg1) {
    return plugin::CallAndReturn<int32, 0x55A780, int32, uint8>(arg0, arg1);
}

// 0x55B900
void CStats::ProcessReactionStatsOnIncrement(eStats stat) {
    // Como el original: switch sobre el byte bajo de la estadística.
    switch (static_cast<uint8>(stat)) {
    case STAT_STAMINA:
    case STAT_LUNG_CAPACITY:
        if (IntStat(STAT_ENERGY) < 0) {
            // Como el original: grasa - 23 limitada a > 0 (NaN -> 0).
            const float fat = StatTypesFloat[STAT_FAT] - 23.0f;
            StatTypesFloat[STAT_FAT] = (fat > 0.0f) ? fat : 0.0f;
            CheckForStatsMessage();
        }
        break;
    case STAT_ENERGY: {
        const int32 energy = IntStat(STAT_ENERGY);
        if (energy > 1000)
            IncrementStat(STAT_FAT, static_cast<float>(static_cast<int64>(energy) - 1000));
        break;
    }
    default:
        break;
    }
}

// 0x55B980
void CStats::DisplayScriptStatUpdateMessage(eStatUpdateState state, eStats stat, float value) {
    if (CPad::GetPad(0)->JustOutOfFrontEnd
        || !bShowUpdateStats
        || TheCamera.m_bWideScreenOn
        || CHud::HelpMessageDisplayed()
        || bStatUpdateMessageDisplayed
        || CMenuSystem::num_menus_in_use)
    {
        bStatUpdateMessageDisplayed = false;

        return;
    }

    if (IsStatCapped(stat) && GetStatValue(stat) >= 1000.0f)
        return;

    switch (stat) {
    case STAT_FAT:
    case STAT_STAMINA:
    case STAT_MUSCLE:
    case STAT_MAX_HEALTH:
    case STAT_SEX_APPEAL:
    case STAT_PISTOL_SKILL:
    case STAT_SILENCED_PISTOL_SKILL:
    case STAT_DESERT_EAGLE_SKILL:
    case STAT_SHOTGUN_SKILL:
    case STAT_SAWN_OFF_SHOTGUN_SKILL:
    case STAT_COMBAT_SHOTGUN_SKILL:
    case STAT_MACHINE_PISTOL_SKILL:
    case STAT_SMG_SKILL:
    case STAT_AK_47_SKILL:
    case STAT_M4_SKILL:
    case STAT_RIFLE_SKILL:
    case STAT_GAMBLING:
    case STAT_DRIVING_SKILL:
    case STAT_ARMOR:
    case STAT_ENERGY:
        if (!(value <= 1.0f)) // Como el original: NaN también muestra el mensaje
            CHud::SetHelpMessageStatUpdate(state, stat, value, 1000.0f);
        break;

    case STAT_TOTAL_RESPECT:
        CHud::SetHelpMessageStatUpdate(state, stat, value, 1000.0f);
        break;

    case STAT_FLYING_SKILL:
    case STAT_LUNG_CAPACITY:
    case STAT_BIKE_SKILL:
    case STAT_CYCLING_SKILL:
    case STAT_LUCK:
        if (!(value <= 1.0f)) // Como el original: NaN también muestra el mensaje
            CHud::SetHelpMessageStatUpdate(state, stat, value, 1000.0f);
        break;

    case STAT_PROGRESS_WITH_DENISE:
    case STAT_PROGRESS_WITH_MICHELLE:
    case STAT_PROGRESS_WITH_HELENA:
    case STAT_PROGRESS_WITH_BARBARA:
    case STAT_PROGRESS_WITH_KATIE:
    case STAT_PROGRESS_WITH_MILLIE:
        CHud::SetHelpMessageStatUpdate(state, stat, value, 100.0f);
        break;

    case STAT_PIMPING_LEVEL:
        CHud::SetHelpMessageStatUpdate(state, STAT_PIMPING_LEVEL, value, 10.0f);
        break;

    case STAT_GANG_STRENGTH: {
        // Como el original: FindPlayerPed se llama de nuevo para leer el límite y FindMaxNumberOfGroupMembers
        // otra vez cuando es el menor (comparación con signo contra el byte del límite).
        if (FindPlayerPed()) {
            const CPlayerPed* player = FindPlayerPed();
            float maxGroup;
            if (FindMaxNumberOfGroupMembers() < static_cast<int32>(player->GetPlayerData()->m_nScriptLimitToGangSize)) {
                maxGroup = static_cast<float>(FindMaxNumberOfGroupMembers());
            } else {
                maxGroup = static_cast<float>(FindPlayerPed()->GetPlayerData()->m_nScriptLimitToGangSize);
            }
            CHud::SetHelpMessageStatUpdate(state, stat, value, maxGroup);
        }
        break;
    }
    default:
        return;
    }
}

// 0x55BC50
void CStats::UpdateRespectStat(uint8 arg0) {
    plugin::Call<0x55BC50, uint8>(arg0);
}

// 0x55BF20
void CStats::UpdateSexAppealStat() {
    plugin::Call<0x55BF20>();
}

// 0x55C180
void CStats::IncrementStat(eStats stat, float value)
{
    // Como el original: sale si value <= 0 o NaN.
    if (!(value > 0.0f))
        return;

    if (IsStatFloat(stat)) { // float
        // Como el original: value + stat; con tope min(x, 1000) donde NaN -> 1000.
        const float newValue = value + StatTypesFloat[stat];
        StatTypesFloat[stat] = newValue;

        if (IsStatCapped(stat))
            StatTypesFloat[stat] = (newValue < 1000.0f) ? newValue : 1000.0f;

        ProcessReactionStatsOnIncrement(stat);
        CheckForStatsMessage();

        return;
    }

    // Como el original: el jugador es CWorld::Players[CWorld::PlayerInFocus] (no FindPlayerPed).
    CPlayerInfo& playerInfo = CWorld::Players[CWorld::PlayerInFocus];

    switch (stat) {
    case STAT_CALORIES: {
        // Como el original: diferencia de salud truncada a entero con ftol.
        const int32 healthDiff = StatsFtol(static_cast<float>(playerInfo.m_nMaxHealth) - playerInfo.m_pPed->m_fHealth);

        IncrementStat(STAT_RIOT_MISSION_ACCOMPLISHED, value);

        if (static_cast<double>(value) > static_cast<double>(healthDiff)) {
            const float excess = static_cast<float>(static_cast<double>(value) - static_cast<double>(healthDiff));
            IncrementStat(STAT_FAT, excess * 0.5f);
        }
        break;
    }
    case STAT_RIOT_MISSION_ACCOMPLISHED: {
        // Como el original: comer. Las horas sin comer (int16 con signo) bajan value/2 limitadas a [0, 36]
        // (NaN -> 36) y se guardan; si value >= la salud que falta, value pasa a ser esa diferencia y las horas 0.
        auto& hoursDidntEat = reinterpret_cast<int16&>(playerInfo.m_nNumHoursDidntEat);
        const float hours = static_cast<float>(hoursDidntEat) - value * 0.5f;
        float clamped;
        if (0.0f > hours) {
            clamped = 0.0f;
        } else {
            clamped = (hours < 36.0f) ? hours : 36.0f;
        }
        hoursDidntEat = static_cast<int16>(StatsFtol(clamped));

        const int32 healthDiff = StatsFtol(static_cast<float>(playerInfo.m_nMaxHealth) - playerInfo.m_pPed->m_fHealth);
        if (!(static_cast<double>(value) < static_cast<double>(healthDiff))) {
            value = static_cast<float>(healthDiff);
            hoursDidntEat = 0;
        }

        playerInfo.m_pPed->m_fHealth = value + playerInfo.m_pPed->m_fHealth;
        UpdateStatsAddToHealth(static_cast<uint32>(StatsFtol(value)));
        break;
    }
    default: {
        // Como el original: se suma ftol(value) truncado a int16 con signo; tope 1000 si está limitada.
        int32& s = IntStat(stat);
        s += static_cast<int16>(StatsFtol(value));

        if (IsStatCapped(stat))
            s = (s < 1000) ? s : 1000;
        break;
    }
    }

    ProcessReactionStatsOnIncrement(stat);
    CheckForStatsMessage();
}

// 0x55C470
void CStats::UpdateFatAndMuscleStats(uint32 value) {
    plugin::Call<0x55C470, uint32>(value);
}

// 0x55C660
void CStats::UpdateStatsWhenSprinting() {
    // Como el original: ftol del ritmo; contador sin signo cargado con fild (+2^32); con NaN suma el paso.
    UpdateFatAndMuscleStats(static_cast<uint32>(StatsFtol(StatReactionValue[STAT_EXERCISE_RATE_SPRINT])));
    if (static_cast<double>(StatReactionValue[STAT_TIMELIMIT_SPRINT_STAMINA] * 1000.0f) < CounterAsX87(m_SprintStaminaCounter)) {
        m_SprintStaminaCounter = 0;
        IncrementStat(STAT_STAMINA, StatReactionValue[STAT_INC_SPRINT_STAMINA]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_STAMINA, StatReactionValue[STAT_INC_SPRINT_STAMINA]);
    } else {
        m_SprintStaminaCounter += static_cast<uint32>(TimeStepMsInt());
    }
}

// 0x55C6F0
void CStats::UpdateStatsWhenRunning() {
    // Como el original: igual que UpdateStatsWhenSprinting con los valores de correr.
    UpdateFatAndMuscleStats(static_cast<uint32>(StatsFtol(StatReactionValue[STAT_EXERCISE_RATE_RUN])));
    if (static_cast<double>(StatReactionValue[STAT_TIMELIMIT_RUNNING] * 1000.0f) < CounterAsX87(m_RunningCounter)) {
        m_RunningCounter = 0;
        IncrementStat(STAT_STAMINA, StatReactionValue[STAT_INC_RUNNING]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_STAMINA, StatReactionValue[STAT_INC_RUNNING]);
    } else {
        m_RunningCounter += static_cast<uint32>(TimeStepMsInt());
    }
}

// 0x55C780
void CStats::UpdateStatsWhenCycling(bool arg0, CBmx* bmx) {
    plugin::Call<0x55C780, bool, CBmx*>(arg0, bmx);
}

// 0x55C990
void CStats::UpdateStatsWhenSwimming(bool arg0, bool arg1) {
    plugin::Call<0x55C990, bool, bool>(arg0, arg1);
}

// 0x55CAC0
void CStats::UpdateStatsWhenDriving(CVehicle* vehicle) {
    plugin::Call<0x55CAC0, CVehicle*>(vehicle);
}

// 0x55CC00
void CStats::UpdateStatsWhenFlying(CVehicle* vehicle) {
    plugin::Call<0x55CC00, CVehicle*>(vehicle);
}

// 0x55CD60
void CStats::UpdateStatsWhenOnMotorBike(CBike* bike) {
    // Como el original: el contador se redondea a float; con NaN en el límite se va a la rama de velocidad.
    const auto bikeCounter = static_cast<float>(CounterAsX87(m_BikeCounter));
    if (!(StatReactionValue[STAT_TIMELIMIT_MOTORBIKE_SKILL] * 1000.0f < bikeCounter)) {
        const float bikeMoveSpeed = bike->m_vecMoveSpeed.Magnitude();
        // Como el original: el paso de tiempo se trunca a ms enteros (ftol) y se vuelve a cargar sin signo.
        const double stepMs = CounterAsX87(static_cast<uint32>(TimeStepMsInt()));

        if (bikeMoveSpeed > 0.6f || bike->m_nNoOfContactWheels < 3u && bikeMoveSpeed > 0.1f)
            m_BikeCounter = static_cast<uint32>(StatsFtol(static_cast<float>(stepMs * 1.5) + bikeCounter));
        else if (bikeMoveSpeed > 0.2f)
            m_BikeCounter = static_cast<uint32>(StatsFtol(static_cast<float>(stepMs * 0.5) + bikeCounter));
    } else {
        m_BikeCounter = 0;
        IncrementStat(STAT_BIKE_SKILL, StatReactionValue[STAT_INC_MOTORBIKE_SKILL]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_BIKE_SKILL, StatReactionValue[STAT_INC_MOTORBIKE_SKILL]);
    }
}

// 0x55CEB0
void CStats::UpdateStatsWhenWeaponHit(eWeaponType weaponType) {
    plugin::Call<0x55CEB0, eWeaponType>(weaponType);
}

// 0x55CFA0
void CStats::UpdateStatsWhenFighting() {
    UpdateFatAndMuscleStats(static_cast<uint32>(StatsFtol(StatReactionValue[STAT_EXERCISE_RATE_FIGHT]))); // Como el original: ftol
}

// 0x55CFC0
void CStats::UpdateStatsOnRespawn() {
    plugin::Call<0x55CFC0>();
}

// 0x55D030
void CStats::UpdateStatsAddToHealth(uint32 addToHealth) {
    plugin::Call<0x55D030, uint32>(addToHealth);
}

// 0x55D090
void CStats::ModifyStat(eStats stat, float value) {
    // Como el original: negativo o NaN -> DecrementStat(-value).
    if (!(value >= 0.0f)) {
        CStats::DecrementStat(stat, -value);
    } else {
        CStats::IncrementStat(stat, value);
    }
}

// 0x5D3B40
bool CStats::Save() {
    IncrementStat(STAT_TOTAL_LEGITIMATE_KILLS, GetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT));
    SetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT, 0.0f);

    CGenericGameStorage::SaveDataToWorkBuffer(StatTypesFloat);
    CGenericGameStorage::SaveDataToWorkBuffer(StatTypesInt);
    CGenericGameStorage::SaveDataToWorkBuffer(PedsKilledOfThisType);
    CGenericGameStorage::SaveDataToWorkBuffer(LastMissionPassedName);
    PopulateFavoriteRadioStationList(); // Como el original: se recalcula la lista antes de guardarla
    CGenericGameStorage::SaveDataToWorkBuffer(FavoriteRadioStationList);
    CGenericGameStorage::SaveDataToWorkBuffer(TimesMissionAttempted);
    // TODO: NOTSA: CGenericGameStorage::SaveDataToWorkBuffer(StatMessage);
    for (auto& statMessage : StatMessage) {
        CGenericGameStorage::SaveDataToWorkBuffer(statMessage.displayed);
    }
    return true;
}

// 0x5D3BF0
bool CStats::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(StatTypesFloat);
    CGenericGameStorage::LoadDataFromWorkBuffer(StatTypesInt);
    CGenericGameStorage::LoadDataFromWorkBuffer(PedsKilledOfThisType);
    CGenericGameStorage::LoadDataFromWorkBuffer(LastMissionPassedName);
    CGenericGameStorage::LoadDataFromWorkBuffer(FavoriteRadioStationList);
    CGenericGameStorage::LoadDataFromWorkBuffer(TimesMissionAttempted);
    // TODO: NOTSA: CGenericGameStorage::LoadDataFromWorkBuffer(StatMessage);
    for (auto& statMessage : StatMessage) {
        CGenericGameStorage::LoadDataFromWorkBuffer(statMessage.displayed);
    }
    return true;
}

// Unused
// 0x558DE0
char* CStats::GetStatID(eStats stat) {
    // Como el original: sprintf (sin el relleno 0xFE de sprintf_s en depuración).
    if (!IsStatFloat(stat)) // int32
        std::sprintf(gString, "stat_i_%d", static_cast<int32>(stat));
    else
        std::sprintf(gString, "stat_f_%d", static_cast<int32>(stat));

    return gString;
}

// Unused
// 0x558E70
int8 CStats::GetTimesMissionAttempted(uint8 missionId) {
    return static_cast<int8>(TimesMissionAttempted.data()[missionId]); // Como el original: sin comprobar el rango
}

// Unused
// 0x558E80
void CStats::RegisterMissionAttempted(uint8 missionId) {
    // Como el original: missionId hasta 255 sin comprobar el rango (std::array abortaría en depuración).
    if (TimesMissionAttempted.data()[missionId] != -1) {
        TimesMissionAttempted.data()[missionId]++;
    }
}

// Unused
// 0x558EA0
void CStats::RegisterMissionPassed(uint8 missionId) {
    TimesMissionAttempted.data()[missionId] = -1; // Como el original: sin comprobar el rango
}
