/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStats
 * Reconstrucción: COMPLETA — 51/51 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 51/51 entradas.
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

#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include <intrin.h>
#include <bit>

#include "Stats.h"
#include "AudioEngine.h"
#include "MenuSystem.h"
#include "Hud.h"
#include "World.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "GameLogic.h"
#include "WeaponInfo.h"
#include "GangWars.h"
#include "TagManager.h"
#include "X87Math.h"

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

// --- Residuo de registros del EXE (contrib/audit_register_residue.py, objeto Stats) ---
// Los hooks del EXE de esta clase terminan con los mismos EAX/ECX/EDX que los originales: ResidueRegsCpp (X87Math) cuando el valor es
// calculable, o la propia llamada al colaborador en la ultima posicion de la funcion (sus registros salen sin tocar). Las constantes
// de AX son las de `fnstsw ax` + `test ah,mask`: AhJe es je/jz (ZF = 1) y AhJp es jp (PF = 1, paridad par del byte `ah & mask`).
inline bool AhJe(uint32 ax, uint32 mask) { return ((ax >> 8) & mask) == 0; }
inline bool AhJp(uint32 ax, uint32 mask) { return (std::popcount((ax >> 8) & mask) & 1) == 0; }

// Tras una llamada a un colaborador de la DLL (residuo propio, no el del EXE), la version del hook conserva los registros con que sale en lugar de suponerlos:
// EAX con StatsCaptureEaxCpp y ECX/EDX con StatsCaptureEcxEdxCpp (llamadas sin argumentos justo despues; contrib/audit_register_residue.py comprueba que entre el
// colaborador y la captura solo haya `add esp`, almacenamientos en memoria y `nop`).
#define STATS_CAPTURE_REGS(eaxv, ecxv, edxv) \
    if constexpr (kExe) { \
        (eaxv) = StatsCaptureEaxCpp(); \
        const uint64 capturedEcxEdx = reinterpret_cast<uint64(__fastcall*)()>(&StatsCaptureEcxEdxCpp)(); \
        (ecxv) = static_cast<uint32>(capturedEcxEdx); \
        (edxv) = static_cast<uint32>(capturedEcxEdx >> 32); \
    }

// GetStatValue de la DLL deja pasar el EDX con que se la llama (el del EXE: el id en `lessthan`, el anterior en `morethan`): solo se capturan EAX y ECX.
#define STATS_CAPTURE_EAX_ECX(eaxv, ecxv)     if constexpr (kExe) {         (eaxv) = StatsCaptureEaxCpp();         const uint64 capturedEcxEdx = reinterpret_cast<uint64(__fastcall*)()>(&StatsCaptureEcxEdxCpp)();         (ecxv) = static_cast<uint32>(capturedEcxEdx);     }

// Salida de las plantillas <kExe>: la version del hook del EXE termina en ResidueRegsCpp (ECX, EDX, EAX); la de la clase solo devuelve EAX.
// (Siempre entre llaves: expande a un if constexpr completo.)
#define STATS_EXIT(ecx, edx, eax) \
    if constexpr (kExe) { \
        return ResidueRegsCpp((ecx), (edx), (eax)); \
    } else { \
        return (eax); \
    }

// Llamadas a colaboradores reconstruidos (void en C++) que devuelven el EDX:EAX con que salen: el EXE los deja ahi y las funciones del hook
// los reutilizan como residuo. Sin captura (version de la clase) devuelven 0. Sin comprobaciones de ejecucion para que nada toque
// EAX/EDX entre la llamada y el retorno.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint64 StatsDisplayRegs(eStatUpdateState state, eStats stat, float value) {
    if constexpr (kExe) {
        return reinterpret_cast<uint64(__cdecl*)(eStatUpdateState, eStats, float)>(&CStats::DisplayScriptStatUpdateMessage)(state, stat, value);
    } else {
        CStats::DisplayScriptStatUpdateMessage(state, stat, value);
        return 0;
    }
}
// Como StatsDisplayRegs pero devuelve ECX (mitad baja) y EDX (alta) con que sale Display: su residuo es el de su propio cuerpo (no depende de los registros de entrada).
template<bool kExe>
__declspec(safebuffers) uint64 StatsDisplayEcxEdx(eStatUpdateState state, eStats stat, float value) {
    if constexpr (kExe) {
        (void)reinterpret_cast<uint64(__cdecl*)(eStatUpdateState, eStats, float)>(&CStats::DisplayScriptStatUpdateMessage)(state, stat, value);
        return reinterpret_cast<uint64(__fastcall*)()>(&StatsCaptureEcxEdxCpp)();
    } else {
        CStats::DisplayScriptStatUpdateMessage(state, stat, value);
        return 0;
    }
}
template<bool kExe>
__declspec(safebuffers) uint64 StatsIncrementRegs(eStats stat, float value) {
    if constexpr (kExe) {
        return reinterpret_cast<uint64(__cdecl*)(eStats, float)>(&CStats::IncrementStat)(stat, value);
    } else {
        CStats::IncrementStat(stat, value);
        return 0;
    }
}
#pragma runtime_checks("", restore)

// Como el original: byte suelto de la memoria del EXE sin nombre en la clase (mov al,[direccion]).
inline uint8 ExeByte(uintptr_t address) { return *reinterpret_cast<const uint8*>(address); }
inline float ExeFloat(uintptr_t address) { return *reinterpret_cast<const float*>(address); }
} // namespace


namespace {
// Hooks de las direcciones del EXE con el residuo de registros de los originales; cada definicion esta junto al metodo de la clase.
// Los que reciben argumentos en la pila (cdecl) se declaran __fastcall sin parametros de pila: ECX y EDX de entrada llegan por registro, la
// limpieza de pila sigue siendo la del llamador y los argumentos se leen bajo la direccion de retorno (_AddressOfReturnAddress).
uint32 __cdecl StatsFindMostFavoriteExe();                                                    // 0x558FA0
float __fastcall StatsGetFatAndMuscleModifierExe(uint32 entryEcx, uint32 entryEdx);           // 0x559AF0
uint32 __fastcall StatsCheckForStatsMessageExe(uint32 entryEcx, uint32 entryEdx);             // 0x559760
int32 __cdecl StatsLoadStatUpdateConditionsExe();                                             // 0x559860
uint32 __fastcall StatsUpdateFatAndMuscleStatsExe(uint32 entryEcx, uint32 entryEdx);          // 0x55C470
uint32 __fastcall StatsUpdateStatsWhenCyclingExe(uint32 entryEcx, uint32 entryEdx);           // 0x55C780
uint32 __fastcall StatsUpdateStatsWhenSwimmingExe(uint32 entryEcx, uint32 entryEdx);          // 0x55C990
uint32 __fastcall StatsUpdateStatsWhenDrivingExe(uint32 entryEcx, uint32 entryEdx);           // 0x55CAC0
uint32 __fastcall StatsUpdateStatsWhenFlyingExe(uint32 entryEcx, uint32 entryEdx);            // 0x55CC00
uint32 __fastcall StatsUpdateStatsWhenWeaponHitExe(uint32 entryEcx, uint32 entryEdx);         // 0x55CEB0
uint32 __fastcall StatsUpdateStatsOnRespawnExe(uint32 entryEcx, uint32 entryEdx);             // 0x55CFC0
uint32 __fastcall StatsUpdateStatsAddToHealthExe(uint32 entryEcx, uint32 entryEdx);           // 0x55D030
uint32 __cdecl StatsUpdateSexAppealStatExe();                                                // 0x55BF20
}

void CStats::InjectHooks() {
    RH_ScopedClass(CStats);
    RH_ScopedCategoryGlobal();
    RH_ScopedNamedGlobalInstall(StatsCheckForStatsMessageExe, "CheckForStatsMessage", 0x559760); // Como el original: EAX/ECX/EDX de cada salida
    RH_ScopedInstall(FindCriminalRatingString, 0x55A210);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsOnRespawnExe, "UpdateStatsOnRespawn", 0x55CFC0);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsAddToHealthExe, "UpdateStatsAddToHealth", 0x55D030);
    RH_ScopedNamedGlobalInstall(StatsUpdateSexAppealStatExe, "UpdateSexAppealStat", 0x55BF20); // Como el original: ECX/EDX de la cola a CheckForStatsMessage


    RH_ScopedInstall(Init, 0x55C0C0);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsWhenCyclingExe, "UpdateStatsWhenCycling", 0x55C780);
    RH_ScopedNamedGlobalInstall(StatsUpdateFatAndMuscleStatsExe, "UpdateFatAndMuscleStats", 0x55C470);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsWhenSwimmingExe, "UpdateStatsWhenSwimming", 0x55C990);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsWhenWeaponHitExe, "UpdateStatsWhenWeaponHit", 0x55CEB0);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsWhenDrivingExe, "UpdateStatsWhenDriving", 0x55CAC0);
    RH_ScopedNamedGlobalInstall(StatsUpdateStatsWhenFlyingExe, "UpdateStatsWhenFlying", 0x55CC00);
    RH_ScopedInstall(PopulateFavoriteRadioStationList, 0x558EC0);
    RH_ScopedNamedGlobalInstall(StatsFindMostFavoriteExe, "FindMostFavoriteRadioStation", 0x558FA0); // Como el original: EAX = indice completo, ECX = maximo, EDX = 15
    RH_ScopedNamedGlobalInstall(StatsGetFatAndMuscleModifierExe, "GetFatAndMuscleModifier", 0x559AF0); // Como el original: ECX/EDX intactos
    RH_ScopedOverloadedInstall(GetStatValue, "-OG", 0x558E40, float(*)(eStats));
    RH_ScopedInstall(SetStatValue, 0x55A070);
    RH_ScopedInstall(IsStatFloat, 0x558E30);
    /*RH_ScopedInstall(GetFullFavoriteRadioStationList, 0x558F90); - different return type*/
    RH_ScopedInstall(FindCriminalRatingNumber, 0x559080);
    RH_ScopedInstall(FindLeastFavoriteRadioStation, 0x559010);
    RH_ScopedInstall(BuildStatLine, 0x559230);
    RH_ScopedInstall(ConstructStatLine, 0x55A780);
    RH_ScopedInstall(UpdateRespectStat, 0x55BC50);
    RH_ScopedInstall(GetPercentageProgress, 0x5591E0);
    RH_ScopedInstall(ConvertToMins, 0x559540);
    RH_ScopedInstall(ConvertToSecs, 0x559560);
    RH_ScopedInstall(SafeToShowThisStat, 0x559590);
    RH_ScopedInstall(CheckForThreshold, 0x5595F0);
    RH_ScopedInstall(IsStatCapped, 0x559630);
    RH_ScopedNamedGlobalInstall(StatsLoadStatUpdateConditionsExe, "LoadStatUpdateConditions", 0x559860);
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
    auto* const times = AudioEngine.GetRadioStationListenTimes();
    bool allZero = true;
    for (size_t i = 0; i < FavoriteRadioStationList.size(); ++i) {
        const int32 time = times[i];
        FavoriteRadioStationList[i] = time;
        if (time != 0) {
            allZero = false;
        }
    }
    return allZero;
}

// 0x558FA0
// The EXE returns the complete station index in EAX despite the int8 enum declaration (hook: StatsFindMostFavoriteExe).
// Ignore emergency/off (indices 0/13), keep the first strictly greater signed value.
namespace {
// Estaciones 1..12 (el EXE las recorre desenrolladas de 6 en 6); el maximo empieza en 0 y la primera estrictamente mayor con signo gana.
int32 FindMostFavoriteIndex(int32& maxOut) {
    int32 best = 0;
    int32 index = 1;
    for (int32 i = 1; i < 13; ++i) {
        const int32 time = CStats::FavoriteRadioStationList.data()[i];
        if (time > best) {
            best = time;
            index = i;
        }
    }
    maxOut = best;
    return index;
}
}

eRadioID CStats::FindMostFavoriteRadioStation() {
    int32 best;
    return static_cast<eRadioID>(FindMostFavoriteIndex(best));
}

namespace {
// Como el original (0x558FA0..0x559009): sale con EAX = indice completo, ECX = maximo (con signo) y EDX = 15 (el bucle desenrollado termina con
// edx = 15); ESI y EDI se guardan y restauran. Ningun llamador del EXE lee ECX/EDX (0x47BED3, 0x4D7184 y 0x55B35F solo usan EAX); los
// bancos con registros crudos si.
#pragma runtime_checks("", off)
uint32 __declspec(safebuffers) __cdecl StatsFindMostFavoriteExe() {
    int32 best;
    const int32 index = FindMostFavoriteIndex(best);
    return ResidueRegsCpp(static_cast<uint32>(best), 15u, static_cast<uint32>(index));
}
#pragma runtime_checks("", restore)
}

// 0x559010
int32 CStats::FindLeastFavoriteRadioStation() {
    // Como el original (0x559010..0x55907D): estaciones 1..12 (el EXE las recorre desenrolladas de 6 en 6 con la primera como minimo inicial);
    // gana la primera estrictamente menor (con signo). Solo EAX (el indice); ningun llamador del EXE lee ECX/EDX (0x55B3A3 usa solo EAX).
    int32 least = FavoriteRadioStationList.data()[1];
    int32 index = 1;
    for (int32 i = 1; i < 13; ++i) {
        const int32 time = FavoriteRadioStationList.data()[i];
        if (time < least) {
            least = time;
            index = i;
        }
    }
    return index;
}

// 0x559080
int32 CStats::FindCriminalRatingNumber() {
    // Como el original: todo en x87 (enteros cargados con fild sin redondear, cada operacion a la precision vigente), dinero de
    // CWorld::Players[PlayerInFocus] dividido entre 5000 como entero, y kills - busted*3 - hospital*3 (las visitas al hospital restan).
    // La cadena x87 completa (con los cuatro _ftol2) esta en StatsCriminalRatingCpp (X87Math).
    const int32 money = CWorld::Players[CWorld::PlayerInFocus].m_nMoney / 5000;
    return StatsCriminalRatingCpp(
        IntStat(STAT_TOTAL_LEGITIMATE_KILLS), IntStat(STAT_TIMES_BUSTED), IntStat(STAT_NUMBER_OF_HOSPITAL_VISITS),
        IntStat(STAT_HIGHEST_FIREFIGHTER_MISSION_LEVEL), IntStat(STAT_HIGHEST_PARAMEDIC_MISSION_LEVEL), money,
        IntStat(STAT_PLANES_HELICOPTERS_DESTROYED), IntStat(STAT_TOTAL_FIRES_EXTINGUISHED),
        IntStat(STAT_CRIMINALS_KILLED_ON_VIGILANTE_MISSION), IntStat(STAT_PEOPLE_SAVED_IN_AN_AMBULANCE),
        CCheat::m_bHasPlayerCheated, IntStat(STAT_TIMES_CHEATED),
        IntStat(STAT_BULLETS_FIRED), IntStat(STAT_BULLETS_THAT_HIT),
        StatTypesFloat.data()[STAT_PROGRESS_MADE], StatTypesFloat.data()[STAT_TOTAL_PROGRESS]);
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
namespace {
// --- Lote 60: BuildStatLine (0x559230) y ConstructStatLine (0x55A780) ---
// Formato de las lineas: el sprintf del CRT estatico del EXE (0x821BB5), igual que el sscanf (0x8220AD) en este archivo: el de la DLL formatea distinto en los limites
// (%.2f de NaN / inf / empates exactos). GxtCharToAscii (0x69F7E0, buffer estatico 0xC1BDD0, sin reconstruir como funcion de uso general) y la funcion que quita las marcas '~'
// del texto (0x719240) tambien van por direccion; los colaboradores reconstruidos (CText::Get, GxtCharStrcpy, AsciiToGxtChar, GetStatValue...) se llaman en C++.
using ExeSprintfFn = int(__cdecl*)(char*, const char*, ...);
template<class... Args>
int StatsExeSprintf(char* dst, const char* format, Args... args) {
    return reinterpret_cast<ExeSprintfFn>(0x821BB5)(dst, format, args...);
}
const char* StatsGxtToAscii(const GxtChar* text) {
    return reinterpret_cast<const char*(__cdecl*)(const GxtChar*, uint8)>(0x69F7E0)(text, 0);
}
void StatsStripTags(GxtChar* text) {
    reinterpret_cast<void(__cdecl*)(GxtChar*)>(0x719240)(text);
}

// Cola comun de las lineas (0x5594D7 en BuildStatLine con la clave de la linea; 0x55A8D4 en ConstructStatLine con gString): el texto de la clave a gGxtString (sin marcas) y gString2 a gGxtString2.
void StatsLineTail(const char* key) {
    const GxtChar* const text = TheText.Get(key);
    GxtCharStrcpy(gGxtString, text);
    StatsStripTags(gGxtString);
    AsciiToGxtChar(gString2, gGxtString2);
}
// Cola 0x55B68F: el texto de la clave a gGxtString2 (el llamador pone antes gGxtString[0] = 0).
void StatsLineTailKey(const char* key) {
    const GxtChar* const text = TheText.Get(key);
    GxtCharStrcpy(gGxtString2, text);
}
}

void CStats::BuildStatLine(char* line, void* pValue1, int32 metrics, void* pValue2, int32 type) {
    // Como el original (0x559230..0x55950E): sin linea no hace nada; si no, vacia gString2 y la rellena segun `type` (1 = h:mm / m:ss) o segun `metrics` (con segundo valor solo
    // 0, 1 y 3; con uno solo 0..9); al final convierte el texto de la clave a gGxtString (sin marcas) y gString2 a gGxtString2. Los punteros se leen sin comprobar (type == 1 los
    // desreferencia siempre). Los reales pasan al sprintf como double (fld + fstp qword: un sNaN sale silenciado, igual que cvtss2sd).
    if (!line) {
        return;
    }
    gString2[0] = '\0';
    if (type == 1) {
        const int32 value2 = *static_cast<int32*>(pValue2);                      // mov eax,[eax]
        const int32 value1 = *static_cast<int32*>(pValue1);                      // mov ecx,[edx]
        StatsExeSprintf(gString2, value2 < 10 ? "%d:0%d" : "%d:%d", value1, value2);   // cmp eax,0Ah; jge: 0x863EB4 si >= 10, 0x863EBC si no
    } else if (pValue2) {
        switch (metrics) {
        case 0: {   // 0x559314
            const int32 value2 = *static_cast<int32*>(pValue2);
            const int32 value1 = *static_cast<int32*>(pValue1);
            const char* const of = StatsGxtToAscii(TheText.Get("FEST_OO"));
            StatsExeSprintf(gString2, " %d %s %d", value1, of, value2);
            break;
        }
        case 1: {   // 0x5592C7
            const float value2 = *static_cast<float*>(pValue2);
            const uint32 value1Bits = *static_cast<uint32*>(pValue1);          // mov edx,[ecx] (copia cruda; se carga con fld mas tarde)
            const char* const of = StatsGxtToAscii(TheText.Get("FEST_OO"));
            StatsExeSprintf(gString2, "%.2f %s %.2f", static_cast<double>(std::bit_cast<float>(value1Bits)), of, static_cast<double>(value2));
            break;
        }
        case 3: {   // 0x55927A
            const float value2 = *static_cast<float*>(pValue2);
            const uint32 value1Bits = *static_cast<uint32*>(pValue1);
            const char* const of = StatsGxtToAscii(TheText.Get("FEST_OO"));
            StatsExeSprintf(gString2, "$%.2f %s $%.2f", static_cast<double>(std::bit_cast<float>(value1Bits)), of, static_cast<double>(value2));
            break;
        }
        default:
            break;
        }
    } else if (pValue1 && static_cast<uint32>(metrics) <= 9) {                      // test eax,eax; je ; cmp ecx,9; ja
        switch (metrics) {
        case 0:
            StatsExeSprintf(gString2, "%d", *static_cast<int32*>(pValue1));
            break;
        case 1:
            StatsExeSprintf(gString2, "%.2f", static_cast<double>(*static_cast<float*>(pValue1)));
            break;
        case 2:
            StatsExeSprintf(gString2, "%0.2f%%", static_cast<double>(*static_cast<float*>(pValue1)));
            break;
        case 3:
            StatsExeSprintf(gString2, "$%.2f", static_cast<double>(*static_cast<float*>(pValue1)));
            break;
        case 4:
            StatsExeSprintf(gString2, "%d|", *static_cast<int32*>(pValue1));
            break;
        case 5: {   // peso: el EXE pasa un double a "%dkgs" (el sprintf lee solo los 32 bits bajos del double)
            const int32 value = *static_cast<int32*>(pValue1);
            if (CLocalisation::Metric()) {
                StatsExeSprintf(gString2, "%dkgs", Stw60MulCpp(static_cast<double>(value), std::bit_cast<float>(0x3EE83E42u)));   // fild; fmul [0x863F0C]
            } else {
                StatsExeSprintf(gString2, "%dlbs", value);
            }
            break;
        }
        case 6: {   // el valor se lee como float, despues del texto de "ST_MILE"
            const uint32 valueBits = *static_cast<uint32*>(pValue1);
            const char* const mile = StatsGxtToAscii(TheText.Get("ST_MILE"));
            StatsExeSprintf(gString2, "%.2f %s", static_cast<double>(std::bit_cast<float>(valueBits)), mile);
            break;
        }
        case 7:
            StatsExeSprintf(gString2, "%.2fm", static_cast<double>(*static_cast<float*>(pValue1)));
            break;
        case 8:     // pies: metros * 3.3333333f (0x859F60) en la x87
            StatsExeSprintf(gString2, "%.2fft", Stw60MulCpp(static_cast<double>(*static_cast<float*>(pValue1)), std::bit_cast<float>(0x40555555u)));
            break;
        case 9: {
            const int32 value = *static_cast<int32*>(pValue1);
            const char* const secs = StatsGxtToAscii(TheText.Get("ST_SECS"));
            StatsExeSprintf(gString2, "%d %s", value, secs);
            break;
        }
        default:
            break;
        }
    }
    StatsLineTail(line);   // 0x5594D7
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
namespace {
// Cuerpo de CheckForStatsMessage (0x559760..0x55985F). Con kExe = true termina como el EXE: EAX/ECX/EDX de cada salida (ver STATS_EXIT);
// con kExe = false es el metodo de la clase y solo devuelve EAX.
// Salidas con registros calculables: CPad::GetPad(0) (0x53FB70) solo deja EAX = &Pads[0] sin tocar ECX/EDX y `mov cl,[eax+0x117]` /
// `mov al,[...]` solo escriben el byte bajo. Tras HelpMessageDisplayed (0x588B50, hook reconstruido) los registros son los del colaborador:
// si devuelve true se reproducen volviendo a llamar a la consulta (pura) justo antes de salir, con los registros que tenia el EXE en la
// llamada; el resto de salidas tras llamadas (GetStatValue 0x558E40, CText::Get 0x6A0050, SetHelpMessage 0x588BE0) asumen que el colaborador
// conserva ECX/EDX y que deja su resultado en EAX: esas salidas dependen del cuerpo compilado del colaborador y no son exactas con
// seguridad (ver informe).
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsCheckForStatsMessageBody(uint32 entryEcx, uint32 entryEdx) {
    const CPad* const pad = CPad::GetPad(0);                                       // push 0; call 0x53FB70
    uint32 eax = static_cast<uint32>(reinterpret_cast<uintptr_t>(pad));
    uint32 ecx = (entryEcx & 0xFFFFFF00u) | static_cast<uint8>(pad->JustOutOfFrontEnd);   // mov cl,[eax+0x117]
    uint32 edx = entryEdx;
    if (static_cast<uint8>(pad->JustOutOfFrontEnd) != 0) {                         // test cl,cl; jne 0x55985F
        STATS_EXIT(ecx, edx, eax);
    }
    eax = (eax & 0xFFFFFF00u) | ExeByte(0x8CDE56);                                 // mov al,[0x8CDE56] (bShowUpdateStats)
    if ((eax & 0xFFu) == 0) {                                                      // je 0x55985F
        STATS_EXIT(ecx, edx, eax);
    }
    eax = (eax & 0xFFFFFF00u) | ExeByte(0xB6F065);
    if ((eax & 0xFFu) != 0) {                                                      // jne 0x55985F
        STATS_EXIT(ecx, edx, eax);
    }
    eax = (eax & 0xFFFFFF00u) | ExeByte(0xB5F852);
    if ((eax & 0xFFu) != 0) {                                                      // jne 0x55985F
        STATS_EXIT(ecx, edx, eax);
    }
    // 0x55979F: call 0x588B50 (CHud::HelpMessageDisplayed); el original no toca nada despues salvo `test al,al`
    if constexpr (kExe) {
        // Sin argumentos: ECX/EDX de entrada del EXE (ecx, edx) llegan por registro (__fastcall sin pila); la consulta sale sin tocar EDX si no lo necesita.
        const uint64 help = reinterpret_cast<uint64(__fastcall*)(uint32, uint32)>(&CHud::HelpMessageDisplayed)(ecx, edx);
        if (static_cast<uint8>(help) != 0) {                                       // jne 0x55985F: EAX/ECX/EDX del colaborador
            // La consulta (pura, sin argumentos) se vuelve a llamar en posicion de cola con los ECX/EDX del EXE como argumentos __fastcall:
            // no hace falta ResidueRegsCpp ni sumidero (el auditor no admite un `mov [..],eax` tras el ayudante) y el compilador no puede
            // eliminar la llamada. La consulta sobrescribe EAX; ECX/EDX salen como el colaborador los deja.
            return reinterpret_cast<uint32(__fastcall*)(uint32, uint32)>(&CHud::HelpMessageDisplayed)(ecx, edx);
        }
        eax = static_cast<uint32>(help);
        edx = static_cast<uint32>(help >> 32);
    } else {
        if (CHud::HelpMessageDisplayed()) {
            return eax;
        }
    }
    eax = (eax & 0xFFFFFF00u) | ExeByte(0xBA82E3);                                 // mov al,[0xBA82E3]
    if ((eax & 0xFFu) != 0) {                                                      // jne 0x55985F
        STATS_EXIT(ecx, edx, eax);
    }

    // 0x5597B9: push ebx/esi/edi; edi = 0; esi = 0xB78200 (StatMessage); bl = 1. El bucle consulta TotalNumStatMessages en cada vuelta y se
    // detiene en 128 aunque el contador diga mas.
    for (uint32 i = 0; i < 128u && i < CStats::TotalNumStatMessages; ++i) {
        tStatMessage& message = CStats::StatMessage.data()[i];
        const uint8 displayed = *reinterpret_cast<const uint8*>(&message.displayed);
        eax = (eax & 0xFFFFFF00u) | displayed;                                     // mov al,[esi+2]
        if (displayed != 0) {
            continue;                                                              // jne 0x55984C
        }
        const uint8 condition = message.condition;
        eax = (eax & 0xFFFFFF00u) | condition;                                     // mov al,[esi+3]
        const uint16 statId = static_cast<uint16>(message.stat_num);
        bool show;
        if (condition != 0) {                                                      // morethan: GetStatValue(ax) >= umbral (C0 = menor/sin orden -> siguiente)
            eax = statId;                                                          // xor eax,eax; mov ax,[esi]
            const float value = CStats::GetStatValue(static_cast<eStats>(statId)); // call 0x558E40
            STATS_CAPTURE_EAX_ECX(eax, ecx);                                       // EAX/ECX con que sale GetStatValue de la DLL (EDX pasa tal cual)
            if constexpr (kExe) {                                                  // con estadistica entera la DLL deja EDX = id; con float pasa el EDX de entrada
                if (!CStats::IsStatFloat(static_cast<eStats>(statId))) {
                    edx = statId;
                }
            }
            const uint32 ax = StatsCompareFltFltAxCpp(value, message.value, 0);    // fcomp [esi+4]; fnstsw ax
            eax = (eax & 0xFFFF0000u) | (ax & 0xFFFFu);
            show = AhJe(ax, 0x01);                                                 // test ah,1; jne 0x55984C
            if (show) {
                ecx = static_cast<uint32>(reinterpret_cast<uintptr_t>(&message)) + 8; // lea ecx,[esi+8]
            }
        } else {                                                                   // lessthan: GetStatValue(dx) <= umbral (mayor/sin orden -> siguiente)
            edx = statId;                                                          // xor edx,edx; mov dx,[esi]
            const float value = CStats::GetStatValue(static_cast<eStats>(statId)); // call 0x558E40
            STATS_CAPTURE_EAX_ECX(eax, ecx);
            if constexpr (kExe) {
                if (!CStats::IsStatFloat(static_cast<eStats>(statId))) {
                    edx = statId;
                }
            }
            const uint32 ax = StatsCompareFltFltAxCpp(value, message.value, 0);    // fcomp [esi+4]; fnstsw ax
            eax = (eax & 0xFFFF0000u) | (ax & 0xFFFFu);
            show = !AhJp(ax, 0x41);                                                // test ah,41h; jp 0x55984C
            if (show) {
                eax = static_cast<uint32>(reinterpret_cast<uintptr_t>(&message)) + 8; // lea eax,[esi+8]
            }
        }
        if (!show) {
            continue;
        }
        // 0x559830: mov ecx,0xC1B340; mov [esi+2],bl; call CText::Get; push eax; call SetHelpMessage(text, 0, 0, 0); add esp,0x10; mov [0xB794D4],bl
        ecx = 0xC1B340u;
        *reinterpret_cast<uint8*>(&message.displayed) = 1;
        const GxtChar* const text = TheText.Get(message.text_id);
        STATS_CAPTURE_REGS(eax, ecx, edx);                                         // EAX = text (resultado de Get)                                         // registros con que sale CText::Get de la DLL
        CHud::SetHelpMessage(text, false, false, false);
        STATS_CAPTURE_REGS(eax, ecx, edx);                                         // y SetHelpMessage (su cuerpo fija ECX/EDX/EAX)
        *reinterpret_cast<uint8*>(&CStats::bStatUpdateMessageDisplayed) = 1;
    }
    STATS_EXIT(ecx, edx, eax);                                                     // 0x55985C: pop edi/esi/ebx; ret
}

// Hook de 0x559760: ECX y EDX de entrada llegan por registro (la funcion no tiene argumentos).
uint32 __declspec(safebuffers) __fastcall StatsCheckForStatsMessageExe(uint32 entryEcx, uint32 entryEdx) {
    return StatsCheckForStatsMessageBody<true>(entryEcx, entryEdx);
}
#pragma runtime_checks("", restore)
}

void CStats::CheckForStatsMessage() {
    StatsCheckForStatsMessageBody<false>(0u, 0u);
}

// 0x559860
namespace {
// Como el original (0x559860..0x5599A5): lee DATA\STATDISP.DAT linea a linea con el sscanf del EXE (0x8220AD, CRT estatico del juego; la
// DLL tiene otro CRT y el banco compara el estado x87 completo tras la llamada). Nada del bucle se reinicializa por linea: el id y el umbral
// (a 0 una sola vez) y los textos conservan lo leido antes si sscanf no llena el campo. La funcion termina en CFileMgr::CloseFile (0x5389D0):
// sus EAX/ECX/EDX salen sin tocar, asi que la llamada va en la ultima posicion (sin cookie de /GS ni comprobaciones de ejecucion).
#pragma runtime_checks("", off)
__declspec(noinline) __declspec(safebuffers) FILESTREAM StatsLoadConditionsRead(uint32* outCount) {
    using ExeSscanf = int(__cdecl*)(const char*, const char*, ...);
    int32 statId = 0;                 // [esp+0x14]
    float threshold = 0.0f;           // [esp+0x18]
    char  text[16] = {};              // [esp+0x24]
    char  condition[16] = {};         // [esp+0x2C]
    char  name[64] = {};              // [esp+0x38]
    uint32 count = 0;                 // [esp+0x10]

    CFileMgr::SetDir("");
    FILESTREAM file = CFileMgr::OpenFile("DATA\\STATDISP.DAT", "rb");
    CStats::TotalNumStatMessages = 0;
    char* line = CFileLoader::LoadLine(file);
    tStatMessage* record = CStats::StatMessage.data();
    for (; line != nullptr; line = CFileLoader::LoadLine(file)) {
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }
        reinterpret_cast<ExeSscanf>(0x8220AD)(line, "%d %s %s %f %s", &statId, name, condition, &threshold, text);
        *reinterpret_cast<uint16*>(record) = static_cast<uint16>(statId);        // mov word ptr [ebp],dx
        *reinterpret_cast<uint8*>(&record->displayed) = 0;
        if (std::memcmp(condition, "lessthan", 9) == 0) {                          // repe cmpsb de 9 bytes contra 0x863F64
            record->condition = static_cast<uint8>(STATMESSAGE_LESSTHAN);
        } else if (std::memcmp(condition, "morethan", 9) == 0) {                   // contra 0x863F58; si no coincide conserva el byte anterior
            record->condition = static_cast<uint8>(STATMESSAGE_MORETHAN);
        }
        std::memcpy(&record->value, &threshold, sizeof(float));
        const char* source = text;                                                  // copia hasta el NUL inclusive, sin limite (rec+8)
        char* destination = reinterpret_cast<char*>(record) + 8;
        char current;
        do {
            current = *source++;
            *destination++ = current;
        } while (current != '\0');
        ++count;
        ++record;
    }
    *outCount = count;
    return file;
}

// Los EAX/ECX/EDX de salida son los de CFileMgr::CloseFile (cola) y su ECX es una direccion de pila de fclose: depende de la profundidad del marco (medido: la
// diferencia entre el EXE y la DLL es exactamente la de los marcos). El EXE llega a CloseFile con 4 (ebp) + 4 (alineacion del banco) + 0x78 (locales) + 16 (ebx, ebp,
// esi, edi) + 4 (argumento) = 0x94 bytes bajo la direccion de retorno. Sin optimizar el marco minimo de MSVC es sub esp,0C0h (0xD4 + 4 en total), asi que esta funcion
// se optimiza por pragma (CMakeLists.txt compila Stats.cpp con /Zi y sin comprobaciones de ejecucion, como Clothes.cpp): sin ebp ni registros guardados el marco es
// `sub esp,N` y la profundidad en la llamada es N + 4, de modo que N = 0x90 = 144 (count 4 + frameFill 140). Comprobar con symbytes/dumpbin que el prologo sea
// `sub esp,90h`; si el compilador guardara registros, ajustar frameFill para que N + 4 * registros + 4 = 0x94.
#pragma optimize("gty", on)
int32 __declspec(safebuffers) __cdecl StatsLoadStatUpdateConditionsExe() {
    uint32 count = 0;
    // El compilador deja sin liberar el argumento de la llamada a StatsLoadConditionsRead (&count) hasta el final: 4 bytes mas bajo la llamada a CloseFile (medido: ECX
    // 0xEBB6A4 frente a 0xEBB6A8 con sub esp,90h). Profundidad = N + 4 (argumento retenido) + 4 (argumento de CloseFile) = 0x94, de modo que N = 0x8C = count 4 + frameFill 136.
    volatile uint8 frameFill[136];   // relleno del marco del EXE (solo reserva espacio)
    frameFill[0] = 0;
    FILESTREAM file = StatsLoadConditionsRead(&count);
    CStats::TotalNumStatMessages = count;
    return CFileMgr::CloseFile(file);
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)
}

void CStats::LoadStatUpdateConditions() {
    (void)StatsLoadStatUpdateConditionsExe();
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
    // Como el original: orden x87 y comparaciones AH, incluidos NaN y la configuracion viva 0x8CDE58..0x8CDEC0 (27 floats consecutivos que
    // el EXE no nombra); los helpers Max (0x420800) y Min (0x404330) del EXE se calculan en linea. Cadena completa en X87Math.
    return StatsFatMuscleModifierCpp(static_cast<uint32>(statMod),
                                     StatTypesFloat.data()[STAT_FAT], StatTypesFloat.data()[STAT_STAMINA],
                                     StatTypesFloat.data()[STAT_MUSCLE], StatTypesFloat.data()[STAT_MAX_HEALTH],
                                     IntStat(STAT_CYCLING_SKILL), IntStat(STAT_LUNG_CAPACITY), IntStat(STAT_BIKE_SKILL),
                                     IntStat(STAT_DRIVING_SKILL), reinterpret_cast<const float*>(0x8CDE58));
}

namespace {
// Como el original (0x559AF0): el EXE conserva ECX y EDX incluso en las ramas con las llamadas Max/Min (sus 36 llamadores lo ignoran, el
// banco DependenciesStats1 lo exige). cdecl con un argumento en la pila: ECX/EDX entran por registro y el argumento se lee bajo la direccion
// de retorno; PlayerPedResidueFloatCpp deja ECX/EDX y el resultado en ST0 sin tocar nada mas.
#pragma runtime_checks("", off)
float __declspec(safebuffers) __fastcall StatsGetFatAndMuscleModifierExe(uint32 entryEcx, uint32 entryEdx) {
    const uint32 statMod = static_cast<const uint32*>(_AddressOfReturnAddress())[1]; // [esp+4]
    const float result = CStats::GetFatAndMuscleModifier(static_cast<eStatModAbilities>(statMod));
    return PlayerPedResidueFloatCpp(entryEcx, entryEdx, result);
}
#pragma runtime_checks("", restore)
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
namespace {
// Cadena de umbrales de 0x55A280..0x55A739: el primero cuyo limite es mayor que el rating da la clave de texto (cmp eax,limite; jge siguiente).
struct RatingStep {
    int32       limit;
    const char* key;
    uint8       special; // 1: arrestos (B79034) decide entre RATNG25 y RATNG24; 2: minutos de vuelo (B790C4) deciden entre RATNG50 y RATNG49
};
constexpr RatingStep kRatingSteps[] = {
    { 20, "RATNG1", 0 },
    { 50, "RATNG2", 0 },
    { 75, "RATNG3", 0 },
    { 100, "RATNG4", 0 },
    { 120, "RATNG5", 0 },
    { 150, "RATNG6", 0 },
    { 200, "RATNG7", 0 },
    { 240, "RATNG8", 0 },
    { 270, "RATNG9", 0 },
    { 300, "RATNG10", 0 },
    { 335, "RATNG11", 0 },
    { 370, "RATNG12", 0 },
    { 400, "RATNG13", 0 },
    { 450, "RATNG14", 0 },
    { 500, "RATNG15", 0 },
    { 550, "RATNG16", 0 },
    { 600, "RATNG17", 0 },
    { 610, "RATNG18", 0 },
    { 650, "RATNG19", 0 },
    { 700, "RATNG20", 0 },
    { 850, "RATNG21", 0 },
    { 1000, "RATNG22", 0 },
    { 1005, "RATNG23", 0 },
    { 1150, "RATNG24", 0 },
    { 1300, nullptr, 1 }, // 0x514: RATNG25 con arrestos (B79034 > 0), si no RATNG24
    { 1500, "RATNG26", 0 },
    { 1700, "RATNG27", 0 },
    { 2000, "RATNG28", 0 },
    { 2100, "RATNG29", 0 },
    { 2300, "RATNG30", 0 },
    { 2500, "RATNG31", 0 },
    { 2750, "RATNG32", 0 },
    { 3000, "RATNG33", 0 },
    { 3500, "RATNG34", 0 },
    { 4000, "RATNG35", 0 },
    { 5000, "RATNG36", 0 },
    { 7500, "RATNG37", 0 },
    { 10000, "RATNG38", 0 },
    { 20000, "RATNG39", 0 },
    { 30000, "RATNG40", 0 },
    { 40000, "RATNG41", 0 },
    { 50000, "RATNG42", 0 },
    { 65000, "RATNG43", 0 },
    { 80000, "RATNG44", 0 },
    { 100000, "RATNG45", 0 },
    { 150000, "RATNG46", 0 },
    { 200000, "RATNG47", 0 },
    { 300000, "RATNG48", 0 },
    { 375000, "RATNG49", 0 },
    { 500000, nullptr, 2 }, // 0x7A120: RATNG50 si los minutos de vuelo (B790C4) son > 10, si no RATNG49
};

// Clave GXT para el rating (los textos RATNG* del EXE en 0x863FB8..0x86417C, ocho bytes cada uno).
const char* RatingKey(int32 rating) {
    if (rating < 0) { // 0x55A219..0x55A27F: rating > -500, -2000, -4000, -6000
        if (rating > -500)  return "RATNG53";
        if (rating > -2000) return "RATNG54";
        if (rating > -4000) return "RATNG55";
        if (rating > -6000) return "RATNG56";
        return "RATNG57";
    }
    for (const RatingStep& step : kRatingSteps) {
        if (rating >= step.limit) {
            continue;
        }
        if (step.special == 1) {
            return IntStat(STAT_TIMES_BUSTED) > 0 ? "RATNG25" : "RATNG24"; // fild [0xB79034]; fcomp 0.0
        }
        if (step.special == 2) {
            // fild [0xB790C4] * [0x863FD0] * [0x859044] -> _ftol2; cmp eax,10; jle
            return StatsFlightMinutesCpp(IntStat(STAT_FLIGHT_TIME), ExeFloat(0x863FD0), ExeFloat(0x859044)) > 10 ? "RATNG50" : "RATNG49";
        }
        return step.key;
    }
    if (rating < 1000000) {
        return "RATNG51";
    }
    // 0x55A740: movzx eax,[0xB7CD74]; imul eax,eax,0x190; cmp [eax+0xB7CE54],0x989680 (dinero mostrado del jugador en foco)
    return CWorld::Players[CWorld::PlayerInFocus].m_nDisplayMoney > 10000000 ? "RATNG52" : "RATNG51";
}
}

// Como el original: llama a FindCriminalRatingNumber (0x559080) y sale con un unico `call CText::Get(clave)` en la ultima posicion (ECX = TheText en cada
// salida); sus EAX/ECX/EDX salen sin tocar, asi que no hace falta ayudante de residuo. Sin comprobaciones de ejecucion ni cookie de /GS.
#pragma runtime_checks("", off)
__declspec(safebuffers) GxtChar* CStats::FindCriminalRatingString() {
    const int32 rating = FindCriminalRatingNumber();
    const char* const key = RatingKey(rating);
    return const_cast<GxtChar*>(TheText.Get(key));
}
#pragma runtime_checks("", restore)

// 0x55A780
namespace {
// Tablas de lineas de 0x55A780 (6 bytes por entrada: int16 estadistica, uint8 formato, uint8 "siempre", uint8 "especial", uint8 sin uso; terminan en -99). Estan en los datos
// del EXE y se leen vivas (el banco las sustituye para ejercitar formatos que las tablas reales no usan).
constexpr uintptr_t kStatLineTables[7] = { 0x8CD8A0, 0x8CD938, 0x8CD9B0, 0x8CD9D8, 0x8CDA70, 0x8CDAC8, 0x8CDBF8 };
constexpr uintptr_t kStatLineTableDefault = 0x8CDE08;

// 0x55B6F8: con el juego aleman se omiten las estadisticas cuyo byte bajo esta entre 167 y 209 con 0 en la tabla (las del tipo SafeToShowThisStat).
constexpr uint8 kStatLineGermanKeep[43] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };

// 0x55B784: estadistica especial (0..0xF3) -> numero de manejador (13 = sin manejador, se omite).
int32 StatLineSpecialHandler(uint32 id) {
    switch (id) {
    case 0:   return 0;
    case 143: return 1;
    case 144: return 2;
    case 145: return 3;
    case 164: return 4;
    case 169: return 5;
    case 173: return 6;
    case 174: return 7;
    case 175: return 8;
    case 213: return 9;
    case 231: return 10;
    case 241: return 11;
    case 243: return 12;
    default:  return 13;
    }
}

// Lectura cruda de la estadistica entera: fild [id*4 + 0xB78E20] (id de 16 bits sin signo; 82..119 caen en otras tablas del juego).
int32 StatLineRawInt(uint16 id) {
    return *reinterpret_cast<const int32*>(0xB78E20u + static_cast<uintptr_t>(id) * 4u);
}
// Lo que GetStatValue (0x558E40) deja en ST0 en el EXE: el float de la estadistica (fld) o el entero EXACTO (fild). La funcion de la DLL devuelve float, asi que el entero (82 o mas) se
// lee de la tabla; la llamada se hace igualmente (el EXE la hace y el banco traza las llamadas).
double StatLineValue(uint16 id) {
    const float value = CStats::GetStatValue(static_cast<eStats>(id));
    return id < CStats::FIRST_UNUSED_STAT ? static_cast<double>(value) : static_cast<double>(StatLineRawInt(id));
}
// call 0x821B40 (_ftol2) sobre ST0: palabra baja del entero truncado a 64 bits.
int32 StatLineFtol(double value) {
    return static_cast<int32>(static_cast<int64>(value));
}
// Lectura directa de 0x55A7F9 / 0x55AABF: fld [id*4 + 0xB79380] o fild [id*4 + 0xB78E20] seguido de _ftol2 (sin GetStatValue).
int32 StatLineArrayFtol(uint16 id) {
    return id < CStats::FIRST_UNUSED_STAT ? StatsFtol(CStats::StatTypesFloat.data()[id]) : StatLineRawInt(id);
}

enum class StatLineResult { Skip, Next, Done };   // Skip = 0x55B5A6 (siguiente entrada), Next = 0x55B5A5 (cuenta una linea y sigue), Done = devuelve 0

// 0x55AC1F: " %d %s %d" con "FEST_OO" (value1 de, value2); despues la cola comun.
StatLineResult StatLineOutOf(int32 value1, int32 value2) {
    gString2[0] = '\0';
    const char* const of = StatsGxtToAscii(TheText.Get("FEST_OO"));
    StatsExeSprintf(gString2, " %d %s %d", value1, of, value2);
    StatsLineTail(gString);
    return StatLineResult::Done;
}
// 0x55B60D: linea de cabecera (gString2 vacio) y cola comun.
StatLineResult StatLineHeader() {
    gString2[0] = '\0';
    StatsLineTail(gString);
    return StatLineResult::Done;
}
// 0x55A8C0 / 0x55B087 y 0x55A8D4: "%d" (o el formato dado) con un entero y cola comun.
StatLineResult StatLineInt(const char* format, int32 value) {
    gString2[0] = '\0';
    StatsExeSprintf(gString2, format, value);
    StatsLineTail(gString);
    return StatLineResult::Done;
}
StatLineResult StatLineDouble(const char* format, double value) {
    gString2[0] = '\0';
    StatsExeSprintf(gString2, format, value);
    StatsLineTail(gString);
    return StatLineResult::Done;
}
// 0x55B68F precedido de gGxtString[0] = 0.
StatLineResult StatLineKey(const char* key) {
    gGxtString[0] = 0;
    StatsLineTailKey(key);
    return StatLineResult::Done;
}
// BuildStatLine(gString, &a, 0, &b, 1) y devuelve 0.
StatLineResult StatLineClock(int32 hours, int32 minutes) {
    int32 first = hours;
    int32 second = minutes;
    CStats::BuildStatLine(gString, &first, 0, &second, 1);
    return StatLineResult::Done;
}

// Una entrada de la tabla (0x55A7E6..0x55B5A6). `count` = lineas contadas hasta ahora (esi), `wanted` = linea pedida (ebp).
StatLineResult StatLineEntry(const uint8* entry, int32& count, const int32 wanted) {
    const int16 id16 = *reinterpret_cast<const int16*>(entry);
    const uint16 id = static_cast<uint16>(id16);
    const int32 sid = id16;
    const uint8 format = entry[2];
    const uint8 always = entry[3];
    const uint8 special = entry[4];
    auto here = [&]() { return count == wanted; };

    if (always == 0) {
        // 0x55A7ED: solo se muestra si la estadistica es mayor que 0 (fcomp 0.0; test ah,41h; jne): un NaN la omite.
        const bool positive = id < CStats::FIRST_UNUSED_STAT ? CStats::StatTypesFloat.data()[id] > 0.0f : StatLineRawInt(id) > 0;
        if (!positive) {
            return StatLineResult::Skip;
        }
    }
    // 0x55A81D: clave "STAT%03d" en gString (con ceros a la izquierda hasta tres cifras).
    StatsExeSprintf(gString, sid < 10 ? "STAT00%d" : (sid < 100 ? "STAT0%d" : "STAT%d"), sid);
    if (CLocalisation::GermanGame()) {
        const uint32 index = static_cast<uint32>(static_cast<uint8>(id16)) - 167u;
        if (index <= 0x2A && kStatLineGermanKeep[index] == 0) {
            return StatLineResult::Skip;
        }
    }
    CStats::m_ThisStatIsABarChart = 0;
    if (special == 0) {
        switch (static_cast<uint32>(format) - 1u <= 9u ? format : 0) {
        case 10: {   // 0x55A8A0: barra; muestra el valor entero
            CStats::m_ThisStatIsABarChart = id16;
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineInt("%d", StatLineFtol(StatLineValue(id)));
        }
        case 5: {    // 0x55A914: peso
            if (!here()) {
                return StatLineResult::Next;
            }
            const int32 value = StatLineFtol(StatLineValue(id));
            gString2[0] = '\0';
            if (CLocalisation::Metric()) {
                StatsExeSprintf(gString2, "%dkgs", Stw60MulCpp(static_cast<double>(value), std::bit_cast<float>(0x3EE83E42u)));
            } else {
                StatsExeSprintf(gString2, "%dlbs", value);
            }
            StatsLineTail(gString);
            return StatLineResult::Done;
        }
        case 3: {    // 0x55A95E: dinero
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineDouble("$%.2f", StatLineValue(id));
        }
        case 4: {    // 0x55A97A
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineInt("%d|", StatLineFtol(StatLineValue(id)));
        }
        case 9: {    // 0x55A998: minutos : segundos
            if (!here()) {
                return StatLineResult::Next;
            }
            int32 minutes = CStats::ConvertToMins(StatLineFtol(StatLineValue(id)));
            int32 seconds = CStats::ConvertToSecs(StatLineFtol(StatLineValue(id)));
            CStats::BuildStatLine(gString, &minutes, 0, &seconds, 1);
            return StatLineResult::Done;
        }
        case 1: {    // 0x55A9EF
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineDouble("%.2f", StatLineValue(id));
        }
        case 6: {    // 0x55AA0B: millas (el valor pasa por un float en la pila)
            if (!here()) {
                return StatLineResult::Next;
            }
            const float value = static_cast<float>(StatLineValue(id));
            gString2[0] = '\0';
            const char* const mile = StatsGxtToAscii(TheText.Get("ST_MILE"));
            StatsExeSprintf(gString2, "%.2f %s", static_cast<double>(value), mile);
            StatsLineTail(gString);
            return StatLineResult::Done;
        }
        case 7: {    // 0x55AA63: metros o pies (Metric se consulta antes de comparar la linea)
            if (CLocalisation::Metric()) {
                if (!here()) {
                    return StatLineResult::Next;
                }
                return StatLineDouble("%.2fm", StatLineValue(id));
            }
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineDouble("%.2fft", Stw60MulCpp(StatLineValue(id), std::bit_cast<float>(0x40555555u)));
        }
        default: {   // 0x55AAAA (formatos 0, 2, 8 y mayores de 10): "%d" del valor leido directamente de las tablas
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineInt("%d", StatLineArrayFtol(id));
        }
        }
    }

    // --- estadisticas especiales (0x55AAD0) ---
    if (sid > 0x101) {   // 0x55AE50: tabla de 18 entradas desde 320
        switch (static_cast<uint32>(sid) - 0x140u) {
        case 0: {    // 320: tiempo jugado (ms / 60000 con la division por multiplicacion del compilador)
            const uint32 elapsed = StaticRef<uint32>(0xB7CB84);
            const int32 total = static_cast<int32>(static_cast<uint32>((static_cast<uint64>(0x45E7B273u) * elapsed) >> 32) >> 14);
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineClock(total / 60, total % 60);
        }
        case 2: {    // 322
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineOutOf(StaticRef<int32>(0xA9AD74), StaticRef<int32>(0xA9AD70));
        }
        case 3: {    // 323: banda menos favorita (0xB78E90 + 7..14, el primero estrictamente mayor sin signo)
            int32 best = 0;
            uint32 largest = 0;
            for (int32 i = 7; i < 15; ++i) {
                const uint32 kills = CStats::PedsKilledOfThisType.data()[i];
                if (kills > largest) {
                    best = i;
                    largest = kills;
                }
            }
            if (best == 0) {
                return StatLineResult::Skip;
            }
            if (here()) {
                return StatLineHeader();
            }
            const uint32 gang = static_cast<uint32>(best - 7);
            ++count;
            static constexpr const char* kGangKeys[8] = { "ST_GNG0", "ST_GNG1", "ST_GNG2", "ST_GNG3", "ST_GNG4", "ST_GNG5", "ST_GNG6", "ST_GNG7" };
            if (gang > 7) {
                return StatLineResult::Skip;
            }
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineKey(kGangKeys[gang]);
        }
        case 4: {    // 324: suma de nueve contadores (0xB78EAC..0xB78ECC), con envoltura
            if (!here()) {
                return StatLineResult::Next;
            }
            uint32 sum = 0;
            for (int32 i = 7; i < 16; ++i) {
                sum += static_cast<uint32>(CStats::PedsKilledOfThisType.data()[i]);
            }
            return StatLineInt("%d", static_cast<int32>(sum));
        }
        case 5: {    // 325
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineInt("%d", CStats::PedsKilledOfThisType.data()[20]);
        }
        case 6:      // 326: emisora mas escuchada
        case 7: {    // 327: emisora menos escuchada
            if (CStats::PopulateFavoriteRadioStationList()) {
                return StatLineResult::Skip;
            }
            if (here()) {
                return StatLineHeader();
            }
            ++count;
            char name[32];
            const eRadioID station = sid == 326 ? CStats::FindMostFavoriteRadioStation() : static_cast<eRadioID>(CStats::FindLeastFavoriteRadioStation());
            AudioEngine.GetRadioStationNameKey(station, name);
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineKey(name);
        }
        case 8: {    // 328: habilidad del arma actual
            const uintptr_t ped = reinterpret_cast<uintptr_t>(FindPlayerPed(-1));
            const int8 slot = *reinterpret_cast<const int8*>(ped + 0x718);
            const uint16 type = *reinterpret_cast<const uint16*>(ped + static_cast<intptr_t>(slot) * 0x1C + 0x5A0);
            uint8 skillSlot;                                                    // bl
            if (type == 0x20) {
                skillSlot = static_cast<uint8>(0x1C - 0x15);
            } else if (type >= 0x16 && type <= 0x20) {
                skillSlot = static_cast<uint8>(type - 0x15);
            } else {
                return StatLineResult::Skip;
            }
            if (here()) {
                gString2[0] = '\0';
                const uint16 stat = static_cast<uint16>(static_cast<uint16>(skillSlot) + 0x44);
                StatsExeSprintf(gString2, "%d", StatLineFtol(StatLineValue(stat)));
                StatsLineTail(gString);
                return StatLineResult::Done;
            }
            ++count;
            const uintptr_t ped2 = reinterpret_cast<uintptr_t>(FindPlayerPed(-1));
            const int8 slot2 = *reinterpret_cast<const int8*>(ped2 + 0x718);
            const int32 fullType = *reinterpret_cast<const int32*>(ped2 + static_cast<intptr_t>(slot2) * 0x1C + 0x5A0);
            if (fullType == 0x20) {
                skillSlot = 0xB;
                StatsExeSprintf(gString, "STWE0%d", static_cast<int32>(skillSlot));
            } else if (skillSlot < 10) {
                StatsExeSprintf(gString, "STWE00%d", static_cast<int32>(skillSlot));
            } else if (skillSlot < 100) {
                StatsExeSprintf(gString, "STWE0%d", static_cast<int32>(skillSlot));
            } else {
                StatsExeSprintf(gString, "STWE%d", static_cast<int32>(skillSlot));
            }
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineKey(gString);
        }
        case 9: {    // 329: niveles de habilidad de las armas 22..31
            if (here()) {
                return StatLineHeader();
            }
            ++count;
            for (uint16 weapon = 1; weapon < 0xB; ++weapon) {
                StatsExeSprintf(gString, weapon < 10 ? "STWE00%d" : (weapon < 100 ? "STWE0%d" : "STWE%d"), static_cast<int32>(weapon));
                const int32 type = weapon + 0x15;
                const int32 skill = static_cast<int8>(static_cast<uint8>(FindPlayerPed(-1)->GetWeaponSkill(static_cast<eWeaponType>(type))));
                StatsExeSprintf(gString2, skill == 1 ? "WS_STD" : (skill == 2 ? "WS_PRO" : "WS_POOR"));
                if (here()) {
                    GxtCharStrcpy(gGxtString, TheText.Get(gString));
                    GxtCharStrcpy(gGxtString2, TheText.Get(gString2));
                    return StatLineResult::Done;
                }
                ++count;
            }
            return StatLineResult::Skip;
        }
        case 10: {   // 330: ranking de piloto (horas : minutos de vuelo y 19 clasificaciones)
            const int32 total = Stw60ScaledFtolCpp(StaticRef<int32>(0xB790C4), std::bit_cast<float>(0x378BCF65u));
            const int32 hours = total / 60;
            const int32 minutes = total % 60;
            if (hours > 0 || minutes >= 5) {
                if (here()) {
                    return StatLineHeader();
                }
                ++count;
            }
            // 0x55B0D1: cada tramo comprueba (horas, minutos) y, si cuadra, devuelve la linea o cuenta una mas.
            const char* key = nullptr;
            if (hours <= 0) {
                if (minutes < 5) {
                    return StatLineResult::Skip;
                }
                if (minutes < 10) {
                    key = "ST_PR01";
                } else if (minutes < 20) {
                    key = "ST_PR02";
                } else if (minutes < 30) {
                    key = "ST_PR03";
                }
            }
            if (!key) {
                if (hours <= 1 && minutes < 0) {
                    key = "ST_PR04";
                } else if (hours <= 1 && minutes < 30) {
                    key = "ST_PR05";
                } else if (hours <= 2 && minutes < 0) {
                    key = "ST_PR06";
                } else if (hours <= 2 && minutes < 30) {
                    key = "ST_PR07";
                } else if (hours <= 3 && minutes < 0) {
                    key = "ST_PR08";
                } else if (hours <= 3 && minutes < 30) {
                    key = "ST_PR09";
                } else if (hours <= 4 && minutes < 0) {
                    key = "ST_PR10";
                } else if (hours <= 5 && minutes < 0) {
                    key = "ST_PR11";
                } else if (hours <= 10 && minutes < 0) {
                    key = "ST_PR12";
                } else if (hours <= 20 && minutes < 0) {
                    key = "ST_PR13";
                } else if (hours <= 25 && minutes < 0) {
                    key = "ST_PR14";
                } else if (hours <= 30 && minutes < 0) {
                    key = "ST_PR15";
                } else if (hours <= 49 && minutes < 2) {
                    key = "ST_PR16";
                } else if (hours <= 50 && minutes < 0) {
                    key = "ST_PR17";
                } else if (hours <= 100 && minutes < 0) {
                    key = "ST_PR18";
                } else {
                    key = "ST_PR19";
                }
            }
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineKey(key);
        }
        case 11:     // 331..333: bandas mas fuertes
        case 12:
        case 13: {
            const int32 gangValue = *reinterpret_cast<const int32*>(0x96A60Cu + static_cast<uintptr_t>(sid) * 4u);
            const int32 gangLabel = *reinterpret_cast<const int32*>(0x96A600u + static_cast<uintptr_t>(sid) * 4u);
            const int32 rank = sid - 331;
            if (gangValue < 0) {
                return StatLineResult::Skip;
            }
            StatsExeSprintf(gString2, "ST_GNG%d", gangValue);
            StatsExeSprintf(gString, "ST_LAB%d", rank);
            const GxtChar* const text = TheText.Get(gString);
            if (here()) {
                GxtCharStrcpy(gGxtString, text);
                StatsLineTailKey(gString2);
                return StatLineResult::Done;
            }
            ++count;
            StatsExeSprintf(gString, "%d", gangLabel);
            AsciiToGxtChar(gString, gGxtString2);
            if (here()) {
                gGxtString[0] = 0;
                GxtCharStrcpy(gGxtString2, gGxtString2);
                return StatLineResult::Done;
            }
            return StatLineResult::Next;
        }
        case 14: {   // 334: dinero perdido en el juego (0xB7940C - 0xB79414, minimo 0)
            const double lost = Stw60SubClampCpp(StaticRef<float>(0xB7940C), StaticRef<float>(0xB79414));
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineDouble("$%.2f", lost);
        }
        case 17: {   // 337: territorio bajo control
            if (!here()) {
                return StatLineResult::Next;
            }
            return StatLineDouble("%0.2f%%", Stw60MulCpp(static_cast<double>(StaticRef<float>(0x96AB9C)), std::bit_cast<float>(0x42C80000u)));
        }
        default:     // 321, 335, 336 y los que no estan en la tabla
            return StatLineResult::Skip;
        }
    }
    if (sid >= 0xFC) {   // 0x55AE07: 252..257 (progreso con las novias)
        const bool positive = id < CStats::FIRST_UNUSED_STAT ? CStats::StatTypesFloat.data()[id] > 0.0f : StatLineRawInt(id) > 0;
        if (!positive) {
            return StatLineResult::Skip;
        }
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineDouble("%0.2f%%", StatLineValue(id));
    }
    if (static_cast<uint32>(sid) > 0xF3u) {
        return StatLineResult::Skip;
    }
    switch (StatLineSpecialHandler(static_cast<uint32>(sid))) {
    case 0: {    // 0x55AB1F: porcentaje de progreso
        if (!here()) {
            return StatLineResult::Next;
        }
        const double progress = CStats::GetPercentageProgress();
        return StatLineDouble("%0.2f%%", progress);
    }
    case 1: {    // 0x55AC59: cabecera y linea de la mejor acrobacia insensata (por [0xB7905C])
        if (here()) {
            return StatLineHeader();
        }
        const int32 award = StatLineRawInt(143);
        ++count;
        static constexpr const char* kStuntKeys[8] = { "INSTUN", "PRINST", "DBINST", "DBPINS", "TRINST", "PRTRST", "QUINST", "PQUINS" };
        const uint32 which = static_cast<uint32>(award) - 1u;
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineKey(which <= 7u ? kStuntKeys[which] : "NOSTUC");
    }
    case 2:      // 0x55ABC6: saltos unicos encontrados de los hechos
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB79060u), StaticRef<int32>(0xA9A89C));
    case 3:      // 0x55ABE2
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB79064u), StaticRef<int32>(0xA9A89C));
    case 4: {    // 0x55AB02: blindaje del jugador (float de [0xB7CD98] + 0x548)
        if (!here()) {
            return StatLineResult::Next;
        }
        const uintptr_t player = StaticRef<uintptr_t>(0xB7CD98);
        return StatLineInt("%d", StatsFtol(*reinterpret_cast<const float*>(player + 0x548)));
    }
    case 5: {    // 0x55AD5F: horas : minutos de [0xB790C4]
        const int32 total = Stw60ScaledFtolCpp(StaticRef<int32>(0xB790C4), std::bit_cast<float>(0x378BCF65u));
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineClock(total / 60, total % 60);
    }
    case 6: {    // 0x55ADAD: horas : minutos de [0xB790D4] (solo si alguno es positivo)
        const int32 total = Stw60ScaledFtolCpp(StaticRef<int32>(0xB790D4), std::bit_cast<float>(0x378BCF65u));
        const int32 hours = total / 60;
        const int32 minutes = total % 60;
        if (!(hours > 0 || minutes > 0)) {
            return StatLineResult::Skip;
        }
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineClock(hours, minutes);
    }
    case 7:      // 0x55AB58: [0xB790D8] de 12
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB790D8u), 12);
    case 8:      // 0x55AB74: [0xB790DC] de 25
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB790DCu), 25);
    case 9:      // 0x55AB3C: [0xB79174] de 30
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB79174u), 30);
    case 10:     // 0x55AB90: [0xB791BC] de [0xB791C0]
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB791BCu), *reinterpret_cast<const int32*>(0xB791C0u));
    case 11:     // 0x55ABFE: [0xB791E4] de [0xB791E8]
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB791E4u), *reinterpret_cast<const int32*>(0xB791E8u));
    case 12:     // 0x55ABAB: [0xB791EC] de [0xB791F0]
        if (!here()) {
            return StatLineResult::Next;
        }
        return StatLineOutOf(*reinterpret_cast<const int32*>(0xB791ECu), *reinterpret_cast<const int32*>(0xB791F0u));
    default:
        return StatLineResult::Skip;
    }
}
}

int32 CStats::ConstructStatLine(int32 arg0, uint8 arg1) {
    // Como el original (0x55A780..0x55B6D3): recorre la tabla de lineas elegida por arg1 (0..6; mayores usan la 0x8CDE08) hasta la entrada -99 contando las lineas que se muestran;
    // la que coincide con arg0 se compone en gString / gString2 / gGxtString / gGxtString2 y se devuelve 0; si arg0 queda fuera devuelve el numero de lineas. Ningun llamador del EXE
    // lee ECX/EDX (0x57494E y 0x57DFDA usan solo EAX).
    const uint8* const table = reinterpret_cast<const uint8*>(arg1 <= 6 ? kStatLineTables[arg1] : kStatLineTableDefault);
    int32 count = 0;
    for (uint16 index = 0; *reinterpret_cast<const int16*>(table + static_cast<size_t>(index) * 6) != -99; ++index) {
        switch (StatLineEntry(table + static_cast<size_t>(index) * 6, count, arg0)) {
        case StatLineResult::Done:
            return 0;
        case StatLineResult::Next:
            ++count;
            break;
        case StatLineResult::Skip:
            break;
        }
    }
    return count;
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
    // Como el original (0x55BC50..0x55BF1B). Los ultimos valores mostrados estan en 0x8CDEC4 (bajo) y 0x8CDEC8 (alto); -99.0f = sin valor. Con arg0 distinto de cero solo los reinicia.
    // La aritmetica va en la x87 de 24 bits (Stw60RespectPartialCpp / Stw60RespectFinishCpp en X87Math): la primera mitad hasta los dos float guardados en pila, la segunda tras
    // GetPercentageTagged (0x49CDA0). Ningun llamador del EXE lee EAX/ECX/EDX (0x442ADF y 0x573393 no los usan).
    auto& shownLow = StaticRef<float>(0x8CDEC4);
    auto& shownHigh = StaticRef<float>(0x8CDEC8);
    constexpr float NO_VALUE = -99.0f;                                            // 0x86430C

    if (arg0) {
        shownHigh = NO_VALUE;
        shownLow = NO_VALUE;
        return;
    }

    float partial, moneyPart;
    Stw60RespectPartialCpp(StatTypesFloat.data()[STAT_RESPECT], IntStat(STAT_RESPECT_MISSION), IntStat(STAT_RESPECT_MISSION_TOTAL), StatTypesFloat.data()[STAT_GIRLFRIEND_RESPECT],
                           CGangWars::TerritoryUnderControlPercentage,
                           CWorld::Players[CWorld::PlayerInFocus].m_nMoney, &partial, &moneyPart);
    const float muscle = StatTypesFloat.data()[STAT_MUSCLE];                      // [0xB793DC] (copia cruda a la pila)
    const float clothesRespect = StatTypesFloat.data()[STAT_CLOTHES_RESPECT];     // [0xB79488]
    const int32 tagged = CTagManager::GetPercentageTagged();
    float respect = Stw60RespectFinishCpp(tagged, moneyPart, partial, muscle, clothesRespect);
    if (CCheat::m_aCheatsActive[CHEAT_MAX_RESPECT]) {
        respect = 1000.0f;
    }

    if (StatsFtol(shownHigh) == StatsFtol(respect)) {
        return;
    }
    StatTypesFloat.data()[STAT_TOTAL_RESPECT] = respect;                         // [0xB79480]
    CheckForStatsMessage();

    if (!(respect > shownLow) && !(shownHigh < 2.0f)) {
        // 0x55BDFA: baja (o igual, o NaN) y el maximo mostrado ya es >= 2 (o NaN)
        if (shownHigh != NO_VALUE && shownLow != NO_VALUE) {
            if (CheckForThreshold(&shownLow, respect) || respect < 2.0f) {
                DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_TOTAL_RESPECT, respect);
            }
        }
        if (respect > shownHigh) {
            shownLow = respect;
            shownHigh = respect;
        } else {
            shownHigh = respect;
        }
        return;
    }

    // 0x55BE97: sube (o el maximo mostrado es < 2)
    if (shownHigh != NO_VALUE && shownLow != NO_VALUE) {
        if (CheckForThreshold(&shownLow, respect) || shownHigh < 2.0f) {
            DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_TOTAL_RESPECT, respect);
        }
    }
    if (respect < shownHigh) {
        shownLow = respect;
    }
    shownHigh = respect;
}

// 0x55BF20
namespace {
// Como el original (0x55BF20..0x55C09C, 108 instrucciones). Dos mitades:
//  - Aritmetica x87: va entera en X87Math (SexAppealHalfCpp, SexAppealNearCpp, SexAppealVehiclePartCpp, SexAppealFinishCpp); aqui no hay ninguna operacion
//    float/double (Stats.cpp se compila con SSE: cada una cambiaria los MXCSR/FSW que el EXE deja). La comparacion que ramifica (fsqrt, fcomp [35.0f] y su
//    `test ah,5; jp`) va DENTRO de SexAppealNearCpp: asi la pila x87 queda, tambien en sus registros vacios, como la deja el EXE (el banco compara el FNSAVE entero).
//  - Colaboradores y residuo de registros: la funcion termina con `jmp 0x559760` (CheckForStatsMessage) y, como el original, le llegan el ECX/EDX con
//    que los dejo la ultima instruccion que los escribio:
//      * sin cache de vehiculo (mov ecx,[0xB79530] = 0; je 0x55C048): ECX = 0 y EDX = el que deja el primer FindPlayerVehicle (0x56E0D0);
//      * distancia >= 35 o NaN (jp 0x55C048): los de FindPlayerCoors (0x56E010);
//      * distancia < 35: ECX = movsx(modelo del vehiculo) y EDX = ms_modelInfoPtrs[ECX] (mov ecx,[0xB79530] ... movsx ecx,word [ecx+0x22]; mov edx,[ecx*4+0xA9B0C8]).
//    Los de los colaboradores se capturan con StatsCaptureEcxEdxCpp justo despues de la llamada (como STATS_CAPTURE_REGS); EAX no hace falta: lo pisa CPad::GetPad.
//    El segundo FindPlayerVehicle y RegisterReference nunca llegan al final: el ECX se recarga de la cache y el EDX lo pisa FindPlayerCoors o el modelo.
#define STATS_SEX_APPEAL_CAPTURE(ecxv, edxv) \
    if constexpr (kExe) { \
        const uint64 capturedEcxEdx = reinterpret_cast<uint64(__fastcall*)()>(&StatsCaptureEcxEdxCpp)(); \
        (ecxv) = static_cast<uint32>(capturedEcxEdx); \
        (edxv) = static_cast<uint32>(capturedEcxEdx >> 32); \
    }

// Tabla de 11 bytes de 0x55C0B4 (movzx eax,byte [eax+0x55C0B4]) para la clase de vehiculo - 1 (0..10); el resto (ja 0x55C040) usa el indice 3. Indices del
// salto de 0x55C0A4: 0 = x0.1f (0x55C038), 1 = sin factor (0x55C04C), 2 = x0.3f (0x55C030), 3 = x0.5f (0x55C040).
constexpr uint8 kSexAppealFactorTable[11] = { 0, 1, 1, 2, 0, 2, 0, 3, 1, 2, 0 };

#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 StatsSexAppealBody() {
    // 0x55BF23: fld [0xB794C0] (STAT_APPEARANCE); fmul 0.5f; fstp [esp+0xC]. [esp+8] (la parte del vehiculo) vale 0.0f.
    uint32 halfBits = 0;
    SexAppealHalfCpp(CStats::StatTypesFloat.data()[STAT_APPEARANCE], &halfBits);
    [[maybe_unused]] uint32 ecx = 0;
    [[maybe_unused]] uint32 edx = 0;
    CVehicle*& cache = *reinterpret_cast<CVehicle**>(0xB79530);                    // ultimo vehiculo del jugador (referencia registrada)

    CVehicle* const vehicle = FindPlayerVehicle(-1, false);                        // 0x55BF3F: push 0; push -1; call 0x56E0D0
    STATS_SEX_APPEAL_CAPTURE(ecx, edx);                                            // EDX/ECX de FindPlayerVehicle (llegan al final si no hay cache)
    if (vehicle) {
        CVehicle* const current = FindPlayerVehicle(-1, false);                    // 0x55BF4F: segunda llamada identica
        cache = current;                                                            // mov [0xB79530],eax (antes del RegisterReference)
        static_cast<CEntity*>(current)->RegisterReference(reinterpret_cast<CEntity**>(&cache)); // mov ecx,eax; push 0xB79530; call 0x571B70
    }

    CVehicle* const target = cache;                                                // 0x55BF68: mov ecx,[0xB79530]
    double vehiclePart = 0.0;                                                      // [esp] = 0.0f (0x55C048: fld [esp])
    if (!target) {                                                                 // 0x55BF70: je 0x55C048
        ecx = 0;
    } else {
        // 0x55BF76: esi = matriz ? &matriz->pos (+0x30) : &placeable->pos (+4); el pop esi posterior deja ESI como estaba
        const uintptr_t targetAddress = reinterpret_cast<uintptr_t>(target);
        const uint32 matrix = *reinterpret_cast<const uint32*>(targetAddress + 0x14);
        const float* const position = reinterpret_cast<const float*>(matrix ? static_cast<uintptr_t>(matrix) + 0x30u : targetAddress + 4u);
        const CVector player = FindPlayerCoors(-1);                                // 0x55BF8B: push -1; push &tmp; call 0x56E010
        STATS_SEX_APPEAL_CAPTURE(ecx, edx);                                        // ECX/EDX de FindPlayerCoors (llegan al final si la distancia no es < 35)
        // 0x55BF90..0x55BFB9: fsqrt(dy*dy + dx*dx) sin redondear a float; fcomp [0x85F070] (35.0f); fnstsw ax; test ah,5; jp 0x55C048
        if (SexAppealNearCpp(position, &player.x) != 0) {                          // solo sigue con distancia < 35 (NaN y >= 35 saltan)
            // 0x55BFBF: mov ecx,[0xB79530] (se relee; no cambia); salud = [ecx+0x4C0]; modelo = movsx word [ecx+0x22]
            const uintptr_t currentAddress = reinterpret_cast<uintptr_t>(cache);
            const float health = *reinterpret_cast<const float*>(currentAddress + 0x4C0);
            const int32 modelId = static_cast<int16>(*reinterpret_cast<const uint16*>(currentAddress + 0x22));      // movsx ecx,word [ecx+0x22]
            const uint32 modelInfo = reinterpret_cast<const uint32*>(0xA9B0C8)[modelId];                           // mov edx,[ecx*4+0xA9B0C8] (sin comprobar el rango)
            const int32 vehicleClass = *reinterpret_cast<const int8*>(static_cast<uintptr_t>(modelInfo) + 0x4D);   // movsx eax,byte [edx+0x4D]
            const uint32 classIndex = static_cast<uint32>(vehicleClass) - 1u;                                       // dec eax; cmp eax,0Ah; ja 0x55C040
            const uint32 factorIndex = classIndex > 10u ? 3u : kSexAppealFactorTable[classIndex];
            vehiclePart = SexAppealVehiclePartCpp(health, factorIndex);
            ecx = static_cast<uint32>(modelId);
            edx = modelInfo;
        }
    }
    // 0x55C04C: x0.5f + mitad de la apariencia, limites [0, 1000], cheat (byte 0x969180 = CHEAT_MAX_SEX_APPEAL) -> 1000; fstp [0xB793E4] (STAT_SEX_APPEAL)
    SexAppealFinishCpp(vehiclePart, halfBits, ExeByte(0x969180) != 0 ? 1u : 0u);

    // 0x55C09C: jmp 0x559760
    if constexpr (kExe) {
        return StatsCheckForStatsMessageExe(ecx, edx);
    } else {
        CStats::CheckForStatsMessage();
        return 0;
    }
}

// Hook de 0x55BF20: sin argumentos. El llamador (CGameLogic::Update, 0x442ADF) no lee ningun registro; ECX/EDX los fija la cola a CheckForStatsMessage.
uint32 __declspec(safebuffers) __cdecl StatsUpdateSexAppealStatExe() {
    return StatsSexAppealBody<true>();
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateSexAppealStat() {
    StatsSexAppealBody<false>();
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
namespace {
// Como el original (0x55C470..0x55C65A). Las dos salidas (`pop ecx; ret`) dejan ECX = el de entrada: la primera tras DisplayScriptStatUpdateMessage
// (EAX/EDX = los del colaborador, capturados con StatsDisplayRegs) y la segunda tras _ftol2 (EAX/EDX de StatsStepMsRegsCpp). Ninguna lee AX.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsFatAndMuscleBody(uint32 entryEcx, uint32 entryEdx, uint32 value) {
    float* const reaction = CStats::StatReactionValue.data();
    float* const stats = CStats::StatTypesFloat.data();
    uint8& messageState = *reinterpret_cast<uint8*>(0xB79534); // estado de los mensajes de grasa/musculo

    // fild [0xB794FC] (+2^32 si el bit alto esta puesto); fld [0xB78FB8]; fmul 1000.0f; fcompp; jp 0x55C5B1
    if (reaction[STAT_TIMELIMIT_FAT_ADJUST] * 1000.0f < CounterAsX87(CStats::m_FatCounter)) {
        CStats::m_FatCounter = 0;
        if (stats[STAT_FAT] > 0.0f) {                                    // 0x55C4AF: fcomp 0.0; jne 0x55C556
            const float decFat = reaction[STAT_DEC_FAT];                 // [0xB78F6C]
            if (decFat > 0.0f) {                                         // 0x55C4C6
                const float lowered = stats[STAT_FAT] - decFat;          // 0x55C4D3: fcom 0.0; je (> 0): conserva; si no, 0
                stats[STAT_FAT] = lowered > 0.0f ? lowered : 0.0f;
                CStats::CheckForStatsMessage();
            }
            CStats::IncrementStat(STAT_MUSCLE, reaction[STAT_INC_BODY_MUSCLE]);          // push [0xB78F34]; push 0x17
            if (messageState == 1) {
                messageState = 2;
                CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_MUSCLE, reaction[STAT_INC_BODY_MUSCLE]);
            } else {
                messageState = 1;
                CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_FAT, reaction[STAT_DEC_FAT]);
            }
        } else {                                                         // 0x55C556
            const float decMuscle = reaction[STAT_DEC_BODY_MUSCLE];      // [0xB78F70]
            if (decMuscle > 0.0f) {
                const float lowered = stats[STAT_MUSCLE] - decMuscle;
                stats[STAT_MUSCLE] = lowered > 0.0f ? lowered : 0.0f;
                CStats::CheckForStatsMessage();
            }
            messageState = 3;
            CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_MUSCLE, reaction[STAT_DEC_BODY_MUSCLE]);
        }
    } else {                                                             // 0x55C5B1: ftol(paso * 0.02f * 1000.0f) * value / 10 sin signo
        const uint32 ticks = static_cast<uint32>(TimeStepMsInt());
        CStats::m_FatCounter += (ticks * value) / 10u;                   // imul edx,[esp+8]; mul 0xCCCCCCCD; shr edx,3
    }

    // 0x55C5E5: contador de salud maxima
    if (reaction[STAT_TIMELIMIT_MAX_HEALTH] * 1000.0f < CounterAsX87(CStats::m_MaxHealthCounter)) {
        CStats::IncrementStat(STAT_MAX_HEALTH, reaction[STAT_INC_MAX_HEALTH]);
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_INCREASE, STAT_MAX_HEALTH, reaction[STAT_INC_MAX_HEALTH]);
        CStats::m_MaxHealthCounter = 0;
        STATS_EXIT(entryEcx, static_cast<uint32>(display >> 32), static_cast<uint32>(display)); // pop ecx; ret
    }
    // 0x55C63C
    const Ftol2Regs ftol = StatsStepMsRegsCpp(CTimer::GetTimeStep(), entryEcx);
    CStats::m_MaxHealthCounter += ftol.eax;                              // add [0xB7950C],eax
    STATS_EXIT(entryEcx, ftol.edx, ftol.eax);                            // pop ecx; ret
}

// Hook de 0x55C470: cdecl con un argumento en la pila ([esp+4] = value); ECX/EDX de entrada por registro.
uint32 __declspec(safebuffers) __fastcall StatsUpdateFatAndMuscleStatsExe(uint32 entryEcx, uint32 entryEdx) {
    const uint32 value = static_cast<const uint32*>(_AddressOfReturnAddress())[1];
    return StatsFatAndMuscleBody<true>(entryEcx, entryEdx, value);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateFatAndMuscleStats(uint32 value) {
    StatsFatAndMuscleBody<false>(0u, 0u, value);
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
namespace {
// Como el original (0x55C780..0x55C98C). La primera parte (tasa de ejercicio) y la segunda (habilidad) dejan EAX/ECX/EDX en cada colaborador
// (UpdateFatAndMuscleStats, _ftol2, DisplayScriptStatUpdateMessage). Las salidas son:
//  - Display de la habilidad (0x55C89B..0x55C8A0): la llamada va en ultima posicion y sus registros salen sin tocar (pop esi; pop ebx; ret).
//  - rama del techo (0x55C97F..0x55C987): EAX = ftol(ceil(paso * multiplicador)), ECX = contador + EAX, EDX de _ftol2.
//  - salida por velocidad (0x55C98A): EAX = parte alta del contador de habilidad + AX de la ultima comparacion (TOP = 6 y banderas
//    acumuladas de las operaciones x87 de esta llamada, que se repiten aqui con StatsSpeedSumSqCpp/StatsCompareDblDblAxCpp), ECX/EDX = los que
//    dejo la primera parte (ftol de la tasa); si en la primera parte hubo Display, su ECX no es calculable y se supone conservado (ver informe).
// El segundo argumento (puntero al BMX) se sobrescribe con el tick truncado en la rama del techo (mov [esp+10h],eax): `bmxSlot` es ese hueco.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsCyclingBody(uint32 entryEcx, uint32 entryEdx, uint32 sprintArgument, volatile uint32* bmxSlot) {
    float* const reaction = CStats::StatReactionValue.data();
    const uint8     sprint = static_cast<uint8>(sprintArgument);       // mov bl,[esp+8]
    const uintptr_t bmx    = *bmxSlot;                                 // mov esi,[esp+0x10]
    uint32 ecx = entryEcx;
    uint32 edx = entryEdx;

    // 0x55C78A: fld [bmx+0x49C]; fabs; fcomp 0.0; fnstsw ax; test ah,41h; je (> 0)
    const uint32 axSteer = StatsAbsVsZeroAxCpp(*reinterpret_cast<const float*>(bmx + 0x49C));
    bool  exercise = true;
    float rate     = 0.0f;
    if (AhJe(axSteer, 0x41)) {
        rate = sprint != 0 ? reaction[STAT_EXERCISE_RATE_CYCLE_SPRINT] : reaction[STAT_EXERCISE_RATE_CYCLE]; // [0xB78F7C] : [0xB78F78]
    } else if (sprint != 0) {
        rate = reaction[STAT_EXERCISE_RATE_CYCLE_SPRINT];
    } else {
        exercise = false;                                              // je 0x55C843
    }
    if (exercise) {
        // call _ftol2; push eax; call 0x55C470: _ftol2 deja ECX/EDX y 0x55C470 sale con el ECX con que se la llama (pop ecx).
        const Ftol2Regs rateRegs = Ftol2RegsCpp(rate, ecx);
        ecx = rateRegs.ecx;
        edx = rateRegs.edx;
        CStats::UpdateFatAndMuscleStats(rateRegs.eax);
        const double counter = StatsCounterX87Cpp(CStats::m_CycleStaminaCounter);   // fild [0xB794DC] (+2^32)
        const double limit   = StatsScaledLimitCpp(reaction[STAT_TIMELIMIT_CYCLE_STAMINA]); // fld [0xB78F94]; fmul 1000.0f
        const uint32 ax      = StatsCompareDblDblAxCpp(limit, counter, 0);          // fcompp; fnstsw ax
        if (!AhJp(ax, 0x05)) {
            CStats::m_CycleStaminaCounter = 0;
            CStats::IncrementStat(STAT_STAMINA, reaction[STAT_INC_CYCLE_STAMINA]);
            const float shown = reaction[STAT_INC_CYCLE_STAMINA];
            ecx = std::bit_cast<uint32>(shown);                                     // mov ecx,[0xB78F10]: el ECX con que se llega a Display
            const uint64 display = StatsDisplayEcxEdx<kExe>(STAT_UPDATE_INCREASE, STAT_STAMINA, shown);
            if constexpr (kExe) {                                                   // Display sale con el ECX/EDX de su propio cuerpo
                ecx = static_cast<uint32>(display);
                edx = static_cast<uint32>(display >> 32);
            }
        } else {
            const Ftol2Regs ftol = StatsStepMsRegsCpp(CTimer::GetTimeStep(), ecx);  // 0x55C826
            CStats::m_CycleStaminaCounter += ftol.eax;
            ecx = ftol.ecx;
            edx = ftol.edx;
        }
    }

    // 0x55C843: mov eax,[0xB794E0]; fild [0xB794E0]; fld [0xB78FAC]; fmul 1000.0f; fcompp; test ah,5; jp 0x55C8A1
    const uint32 skillCounter = CStats::m_CycleSkillCounter;
    const uint32 skillHigh    = skillCounter & 0xFFFF0000u;
    const double skillLimit   = StatsScaledLimitCpp(reaction[STAT_TIMELIMIT_CYCLE_SKILL]);
    const uint32 axSkill      = StatsCompareDblDblAxCpp(skillLimit, StatsCounterX87Cpp(skillCounter), 0);
    if (!AhJp(axSkill, 0x05)) {
        CStats::m_CycleSkillCounter = 0;
        CStats::IncrementStat(STAT_CYCLING_SKILL, reaction[STAT_INC_CYCLE_SKILL]);
        if constexpr (kExe) {   // call Display; add esp,0x14; pop esi; pop ebx; ret: los registros del colaborador salen sin tocar
            return reinterpret_cast<uint32(__cdecl*)(eStatUpdateState, eStats, float)>(&CStats::DisplayScriptStatUpdateMessage)(STAT_UPDATE_INCREASE, STAT_CYCLING_SKILL, reaction[STAT_INC_CYCLE_SKILL]);
        } else {
            CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_CYCLING_SKILL, reaction[STAT_INC_CYCLE_SKILL]);
            return 0;
        }
    }

    // 0x55C8A1: velocidad al cuadrado del BMX ([bmx+0x44..0x4C]) contra 0.05^2 (si [bmx+0x804] < 2) y contra 0.2^2
    const float* const velocity = reinterpret_cast<const float*>(bmx + 0x44);
    float multiplier = 0.0f;
    bool  moving     = false;
    if (*reinterpret_cast<const uint8*>(bmx + 0x804) < 2) {
        const double lowSquare = StatsSquareCpp(ExeFloat(0x863E58));                 // 0.05f
        const uint32 axLow     = StatsCompareDblDblAxCpp(lowSquare, StatsSpeedSumSqCpp(velocity), 2);
        if (!AhJp(axLow, 0x05)) {
            multiplier = ExeFloat(0x863E64);                                         // 3.0f
            moving     = true;
        }
    }
    if (!moving) {                                                                   // 0x55C8E6
        const double square  = StatsSquareCpp(ExeFloat(0x863E54));                   // 0.2f
        const uint32 axSpeed = StatsCompareDblDblAxCpp(square, StatsSpeedSumSqCpp(velocity), 2);
        if (AhJp(axSpeed, 0x05)) {                                                   // jp 0x55C98A: pop esi; pop ebx; ret
            STATS_EXIT(ecx, edx, skillHigh | (axSpeed & 0xFFFFu));
        }
        multiplier = sprint != 0 ? ExeFloat(0x863E60) : ExeFloat(0x863E5C);          // 1.5f : 1.0f
    }
    // 0x55C92C: fcom 0.0; fnstsw ax; test ah,41h; jne 0x55C988 (los multiplicadores son constantes positivas: no se toma)
    const uint32 axMultiplier = StatsCompareFltFltAxCpp(multiplier, 0.0f, 1);
    if (!AhJe(axMultiplier, 0x41)) {
        STATS_EXIT(ecx, edx, skillHigh | (axMultiplier & 0xFFFFu));
    }
    // 0x55C939: ticks = ftol(paso * 0.02f * 1000.0f) guardados en el hueco del argumento; ftol(ceil(ticks * multiplicador))
    const Ftol2Regs ticksRegs = StatsStepMsRegsCpp(CTimer::GetTimeStep(), ecx);
    *bmxSlot = ticksRegs.eax;                                                        // mov [esp+0x10],eax
    const Ftol2Regs ceilRegs  = StatsCeilRegsCpp(static_cast<int32>(ticksRegs.eax), multiplier, ticksRegs.ecx);
    const uint32 newCounter   = CStats::m_CycleSkillCounter + ceilRegs.eax;          // mov ecx,[0xB794E0]; add ecx,eax
    CStats::m_CycleSkillCounter = newCounter;
    STATS_EXIT(newCounter, ceilRegs.edx, ceilRegs.eax);                              // pop esi; mov [0xB794E0],ecx; pop ebx; ret
}

// Hook de 0x55C780: cdecl (bool, CBmx*) en la pila; [esp+8] es el hueco que la funcion sobrescribe.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsWhenCyclingExe(uint32 entryEcx, uint32 entryEdx) {
    volatile uint32* const arguments = static_cast<volatile uint32*>(_AddressOfReturnAddress()) + 1;
    return StatsCyclingBody<true>(entryEcx, entryEdx, arguments[0], &arguments[1]);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsWhenCycling(bool arg0, CBmx* bmx) {
    uint32 slot = static_cast<uint32>(reinterpret_cast<uintptr_t>(bmx));
    StatsCyclingBody<false>(0u, 0u, arg0 ? 1u : 0u, &slot);
}

// 0x55C990
namespace {
// Como el original (0x55C990..0x55CAB4). Todas las salidas son `pop ecx; ret` (ECX = el de entrada); EAX/EDX salen del ultimo colaborador de la
// primera parte (Display o _ftol2) o de la segunda, y si arg0 es 0 el EXE sale tras `mov al,[esp+8]` (solo cambia AL). Ninguna lee AX.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsSwimmingBody(uint32 entryEcx, uint32 entryEdx, uint32 underwaterArgument, uint32 sprintArgument) {
    float* const reaction = CStats::StatReactionValue.data();
    const uint8  arg0 = static_cast<uint8>(underwaterArgument);            // [esp+4]
    const uint8  arg1 = static_cast<uint8>(sprintArgument);                // [esp+8]

    // 0x55C991: tasa 0xB78F84 si arg0 o arg1, si no 0xB78F80
    const float rate = (arg0 != 0 || arg1 != 0) ? reaction[STAT_EXERCISE_RATE_SWIM_SPRINT] : reaction[STAT_EXERCISE_RATE_SWIM];
    CStats::UpdateFatAndMuscleStats(static_cast<uint32>(StatsFtol(rate)));  // call _ftol2; push eax; call 0x55C470

    uint32 eax;
    uint32 edx;
    // fild [0xB794E4] (+2^32); fld [0xB78F98]; fmul 1000.0f; fcompp; test ah,5; jp 0x55CA14
    if (reaction[STAT_TIMELIMIT_SWIM_STAMINA] * 1000.0f < CounterAsX87(CStats::m_SwimStaminaCounter)) {
        CStats::m_SwimStaminaCounter = 0;
        CStats::IncrementStat(STAT_STAMINA, reaction[STAT_INC_SWIM_STAMINA]);
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_INCREASE, STAT_STAMINA, reaction[STAT_INC_SWIM_STAMINA]);
        eax = static_cast<uint32>(display);
        edx = static_cast<uint32>(display >> 32);
    } else {
        const Ftol2Regs ftol = StatsStepMsRegsCpp(CTimer::GetTimeStep(), entryEcx);   // 0x55CA14
        CStats::m_SwimStaminaCounter += ftol.eax;
        eax = ftol.eax;
        edx = ftol.edx;
    }

    if (arg0 == 0) {                                                       // 0x55CA31: mov al,[esp+8]; test al,al; je 0x55CAB3
        STATS_EXIT(entryEcx, edx, eax & 0xFFFFFF00u);
    }
    // 0x55CA39: contador bajo el agua
    if (reaction[STAT_TIMELIMIT_BREATH_UNDERWATER] * 1000.0f < CounterAsX87(CStats::m_SwimUnderWaterCounter)) {
        CStats::m_SwimUnderWaterCounter = 0;
        CStats::IncrementStat(STAT_LUNG_CAPACITY, reaction[STAT_INC_BREATH_UNDERWATER]);
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_INCREASE, STAT_LUNG_CAPACITY, reaction[STAT_INC_BREATH_UNDERWATER]);
        STATS_EXIT(entryEcx, static_cast<uint32>(display >> 32), static_cast<uint32>(display));
    }
    const Ftol2Regs ftol = StatsStepMsRegsCpp(CTimer::GetTimeStep(), entryEcx);       // 0x55CA96
    CStats::m_SwimUnderWaterCounter += ftol.eax;
    STATS_EXIT(entryEcx, ftol.edx, ftol.eax);
}

// Hook de 0x55C990: cdecl (bool, bool) en la pila; el llamador (0x68A1A7) empuja EAX/ECX con los bytes altos sin definir.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsWhenSwimmingExe(uint32 entryEcx, uint32 entryEdx) {
    const uint32* const arguments = static_cast<const uint32*>(_AddressOfReturnAddress()) + 1;
    return StatsSwimmingBody<true>(entryEcx, entryEdx, arguments[0], arguments[1]);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsWhenSwimming(bool arg0, bool arg1) {
    StatsSwimmingBody<false>(0u, 0u, arg0 ? 1u : 0u, arg1 ? 1u : 0u);
}

// 0x55CAC0
namespace {
// Como el original (0x55CAC0..0x55CBFE). El EXE guarda el contador convertido a float en el hueco de su argumento ([esp+8] tras `push ecx`:
// fstp dword [esp+8]) y los llamadores (0x571482/0x571492) lo descartan, pero el banco lo observa: `vehicleSlot` es ese hueco. Salidas
// (todas con ECX = el de entrada, `pop ecx`):
//  - Display (0x55CB34): EAX/EDX del colaborador (capturados con StatsDisplayRegs).
//  - velocidad <= 0.2 (0x55CBFD): EAX = parte alta del contador + AX de la ultima comparacion (fcomp pop: TOP = 0), EDX = vehiculo.
//  - _ftol2 (0x55CBBC / 0x55CBFC): EAX/EDX de la segunda conversion (StatsAccumulateRegsCpp).
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsDrivingBody(uint32 entryEcx, uint32 entryEdx, volatile uint32* vehicleSlot) {
    float* const reaction = CStats::StatReactionValue.data();
    const uint32 vehicle = *vehicleSlot;                                   // mov edx,[esp+8]
    const uintptr_t vehicleAddress = vehicle;
    // mov eax,[vehicle+0x590]; xor ecx,ecx; test eax,eax; jne; mov ecx,edx
    const uint32 wheelsVehicle = *reinterpret_cast<const uint32*>(vehicleAddress + 0x590) == 0 ? vehicle : 0u;

    const uint32 counter     = CStats::m_DrivingCounter;                   // mov eax,[0xB794EC]; fild [0xB794EC] (+2^32)
    const float  counterFloat = static_cast<float>(StatsCounterX87Cpp(counter)); // fstp dword [esp+8]
    *vehicleSlot = std::bit_cast<uint32>(counterFloat);
    // fld [0xB78FA4]; fmul 1000.0f; fcomp dword [esp+8]; fnstsw ax; test ah,5; jp 0x55CB36
    const double limit = StatsScaledLimitCpp(reaction[STAT_TIMELIMIT_DRIVING_SKILL]);
    const uint32 axCounter = StatsCompareDblFltAxCpp(limit, counterFloat, 0);
    if (!AhJp(axCounter, 0x05)) {
        CStats::m_DrivingCounter = 0;
        CStats::IncrementStat(STAT_DRIVING_SKILL, reaction[STAT_INC_DRIVING_SKILL]);
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_INCREASE, STAT_DRIVING_SKILL, reaction[STAT_INC_DRIVING_SKILL]);
        STATS_EXIT(entryEcx, static_cast<uint32>(display >> 32), static_cast<uint32>(display));
    }

    // 0x55CB36: velocidad = sqrt((x*x + y*y) + z*z) con x,y,z = [vehiculo+0x44..0x4C]; fcom [0x863E70] (0.8f); test ah,41h; je 0x55CBBE
    const double speed = StatsSpeedCpp(reinterpret_cast<const float*>(vehicleAddress + 0x44));
    const uint32 axFast = StatsCompareDblFltAxCpp(speed, ExeFloat(0x863E70), 1);
    bool fast = AhJe(axFast, 0x41);
    if (!fast && wheelsVehicle != 0 && *reinterpret_cast<const uint8*>(static_cast<uintptr_t>(wheelsVehicle) + 0x960) == 0) {
        fast = true;                                                       // sin ruedas en contacto: je 0x55CBBE
    }
    float multiplier;
    if (!fast) {
        // 0x55CB72: fcomp [0x863E68] (0.2f); test ah,41h; jne 0x55CBFD
        const uint32 axSlow = StatsCompareDblFltAxCpp(speed, ExeFloat(0x863E68), 0);
        if (!AhJe(axSlow, 0x41)) {
            STATS_EXIT(entryEcx, vehicle, (counter & 0xFFFF0000u) | (axSlow & 0xFFFFu));
        }
        multiplier = ExeFloat(0x863E6C);                                   // 0.5f
    } else {
        multiplier = ExeFloat(0x863E74);                                   // 1.5f (fstp st(0) descarta la velocidad)
    }
    // ftol(paso * 0.02f * 1000.0f) -> fild (+2^32 si es negativo) * multiplicador + contador (float) -> ftol -> [0xB794EC]
    const Ftol2Regs ticks = StatsStepMsRegsCpp(CTimer::GetTimeStep(), wheelsVehicle);
    const Ftol2Regs total = StatsAccumulateRegsCpp(static_cast<int32>(ticks.eax), multiplier, counterFloat, ticks.ecx);
    CStats::m_DrivingCounter = total.eax;
    // `mov [esp],eax` tras el primer _ftol2 sobrescribe el ECX guardado: el `pop ecx` final devuelve los ticks.
    STATS_EXIT(ticks.eax, total.edx, total.eax);
}

// Hook de 0x55CAC0: cdecl (CVehicle*) en la pila; el hueco se sobrescribe.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsWhenDrivingExe(uint32 entryEcx, uint32 entryEdx) {
    volatile uint32* const arguments = static_cast<volatile uint32*>(_AddressOfReturnAddress()) + 1;
    return StatsDrivingBody<true>(entryEcx, entryEdx, arguments);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsWhenDriving(CVehicle* vehicle) {
    uint32 slot = static_cast<uint32>(reinterpret_cast<uintptr_t>(vehicle));
    StatsDrivingBody<false>(0u, 0u, &slot);
}

// 0x55CC00
namespace {
// Como el original (0x55CC00..0x55CD51). Igual que la de conducir: el contador convertido a float se guarda en el hueco del argumento. Salidas
// (todas con `pop ecx`: ECX = el de entrada):
//  - con conductor ([vehiculo+0x590] != 0): EAX = ese valor, EDX = el de entrada.
//  - Display: EAX/EDX del colaborador.
//  - con ruedas en contacto ([vehiculo+0x960] != 0): EAX = parte alta del contador + AH de la comparacion del contador + AL = esa byte.
//  - velocidad <= 0.5 (0x55CD50): EAX = parte alta del puntero a la matriz + AX de la comparacion (TOP = 0), EDX = el de entrada.
//  - _ftol2: EAX/EDX de la segunda conversion.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsFlyingBody(uint32 entryEcx, uint32 entryEdx, volatile uint32* vehicleSlot) {
    float* const reaction = CStats::StatReactionValue.data();
    const uint32 vehicle = *vehicleSlot;                                   // mov ecx,[esp+8]
    const uintptr_t vehicleAddress = vehicle;
    const uint32 driver = *reinterpret_cast<const uint32*>(vehicleAddress + 0x590);
    if (driver != 0) {                                                     // jne 0x55CD50
        STATS_EXIT(entryEcx, entryEdx, driver);
    }

    const uint32 counter      = CStats::m_FlyingCounter;                   // mov eax,[0xB794F0]; fild [0xB794F0] (+2^32)
    const float  counterFloat = static_cast<float>(StatsCounterX87Cpp(counter)); // fstp dword [esp+8]
    *vehicleSlot = std::bit_cast<uint32>(counterFloat);
    // fld [0xB78FA8]; fmul 1000.0f; fcomp dword [esp+8]; fnstsw ax; test ah,5; jp 0x55CC76
    const double limit = StatsScaledLimitCpp(reaction[STAT_TIMELIMIT_FLYING_SKILL]);
    const uint32 axCounter = StatsCompareDblFltAxCpp(limit, counterFloat, 0);
    if (!AhJp(axCounter, 0x05)) {
        CStats::m_FlyingCounter = 0;
        CStats::IncrementStat(STAT_FLYING_SKILL, reaction[STAT_INC_FLYING_SKILL]);
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_INCREASE, STAT_FLYING_SKILL, reaction[STAT_INC_FLYING_SKILL]);
        STATS_EXIT(entryEcx, static_cast<uint32>(display >> 32), static_cast<uint32>(display));
    }

    // 0x55CC76: mov al,[vehiculo+0x960]; test al,al; jne 0x55CD50
    const uint8 contactWheels = *reinterpret_cast<const uint8*>(vehicleAddress + 0x960);
    if (contactWheels != 0) {
        STATS_EXIT(entryEcx, entryEdx, (counter & 0xFFFF0000u) | (axCounter & 0xFF00u) | contactWheels);
    }
    // 0x55CC84: velocidad = sqrt((x*x + y*y) + z*z); fcom [0x863E80] (1.3f); test ah,41h; je 0x55CD11
    const double speed = StatsSpeedCpp(reinterpret_cast<const float*>(vehicleAddress + 0x44));
    const uint32 axFast = StatsCompareDblFltAxCpp(speed, ExeFloat(0x863E80), 1);
    bool fast = AhJe(axFast, 0x41);
    float multiplier = 0.0f;
    if (!fast) {
        // mov eax,[vehiculo+0x14]; fld [eax+0x28]; fcomp 0.0; test ah,5; jnp 0x55CD11 (componente < 0)
        const uint32 matrix = *reinterpret_cast<const uint32*>(vehicleAddress + 0x14);
        const uint32 axMatrix = StatsCompareFltFltAxCpp(*reinterpret_cast<const float*>(static_cast<uintptr_t>(matrix) + 0x28), 0.0f, 1);
        if (!AhJp(axMatrix, 0x05)) {
            fast = true;
        } else {
            // fcomp [0x863E78] (0.5f); test ah,41h; jne 0x55CD50
            const uint32 axSlow = StatsCompareDblFltAxCpp(speed, ExeFloat(0x863E78), 0);
            if (!AhJe(axSlow, 0x41)) {
                STATS_EXIT(entryEcx, entryEdx, (matrix & 0xFFFF0000u) | (axSlow & 0xFFFFu));
            }
            multiplier = ExeFloat(0x863E7C);                               // 0.5f
        }
    }
    if (fast) {
        multiplier = ExeFloat(0x863E84);                                   // 1.5f (fstp st(0) descarta la velocidad)
    }
    const Ftol2Regs ticks = StatsStepMsRegsCpp(CTimer::GetTimeStep(), vehicle);
    const Ftol2Regs total = StatsAccumulateRegsCpp(static_cast<int32>(ticks.eax), multiplier, counterFloat, ticks.ecx);
    CStats::m_FlyingCounter = total.eax;
    // `mov [esp],eax` tras el primer _ftol2 sobrescribe el ECX guardado: el `pop ecx` final devuelve los ticks.
    STATS_EXIT(ticks.eax, total.edx, total.eax);
}

// Hook de 0x55CC00: cdecl (CVehicle*) en la pila; el hueco se sobrescribe.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsWhenFlyingExe(uint32 entryEcx, uint32 entryEdx) {
    volatile uint32* const arguments = static_cast<volatile uint32*>(_AddressOfReturnAddress()) + 1;
    return StatsFlyingBody<true>(entryEcx, entryEdx, arguments);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsWhenFlying(CVehicle* vehicle) {
    uint32 slot = static_cast<uint32>(reinterpret_cast<uintptr_t>(vehicle));
    StatsFlyingBody<false>(0u, 0u, &slot);
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
namespace {
// Como el original (0x55CEB0..0x55CF93). El EXE escribe el valor de la habilidad (float) en el ECX guardado ([esp+8] tras `push ecx`), no en el
// argumento: todas las salidas (`pop ecx`) dejan ECX = los bits de ese float. EAX/EDX salen de los colaboradores (IsCoopGameGoingOn, IncrementStat,
// DisplayScriptStatUpdateMessage, capturados con uint64) o del contador:
//  - juego cooperativo: EAX/EDX de IsCoopGameGoingOn (0x441390).
//  - habilidad >= 1000: EAX = parte alta de EAX del colaborador + AX de la comparacion (TOP = 0), EDX = el del colaborador.
//  - arma distinta de la ultima: EAX = el valor anterior de [0xB79514], EDX = el de IncrementStat.
//  - Display: EAX/EDX de DisplayScriptStatUpdateMessage; si no, EAX = contador + 1 y EDX = el de IncrementStat.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsWeaponHitBody(uint32 entryEcx, uint32 entryEdx, uint32 weaponType) {
    // 0x55CEB7: push eax; call 0x743CD0 (CWeaponInfo::GetSkillStatIndex); mov esi,eax
    const uint32 statIndex = static_cast<uint32>(CWeaponInfo::GetSkillStatIndex(static_cast<eWeaponType>(weaponType)));
    const uint16 statWord  = static_cast<uint16>(statIndex);               // cmp si,52h; movzx ecx,si
    const uint32 slot      = statIndex - 0x45u;                            // lea edi,[esi-45h]
    float skill;
    if (statWord < 0x52) {
        skill = CStats::StatTypesFloat.data()[statWord];                   // fld [ecx*4+0xB79380]
    } else {
        skill = StatsIntToFloatCpp(*reinterpret_cast<const int32*>(0xB78E20 + statWord * 4u)); // fild [edx*4+0xB78E20]; fstp dword
    }
    const uint32 skillBits = std::bit_cast<uint32>(skill);                 // fstp dword [esp+8]: el ECX guardado

    const uint64 coop = reinterpret_cast<uint64(__cdecl*)()>(&CGameLogic::IsCoopGameGoingOn)();   // call 0x441390
    const uint32 coopEax = static_cast<uint32>(coop);
    const uint32 coopEdx = static_cast<uint32>(coop >> 32);
    if (static_cast<uint8>(coopEax) != 0) {                                // jne 0x55CF90
        STATS_EXIT(skillBits, coopEdx, coopEax);
    }
    // fld [esp+8]; fcomp [0x858C4C] (1000.0f); fnstsw ax; test ah,5; jp 0x55CF90
    const uint32 axSkill = StatsCompareFltFltAxCpp(skill, 1000.0f, 0);
    if (AhJp(axSkill, 0x05)) {
        STATS_EXIT(skillBits, coopEdx, (coopEax & 0xFFFF0000u) | (axSkill & 0xFFFFu));
    }
    // 0x55CF07: push [slot*4+0xB78F40]; push esi; call IncrementStat
    const uintptr_t incrementAddress = 0xB78F40u + static_cast<uintptr_t>(slot) * 4u;
    const float incrementValue = ExeFloat(incrementAddress);
    const uint64 increment = StatsIncrementRegs<kExe>(static_cast<eStats>(statIndex), incrementValue);
    // IncrementStat sale al instante con value <= 0 o NaN y entonces conserva el EDX con que se la llama (el de IsCoopGameGoingOn en el EXE).
    const uint32 incrementEdx = !(incrementValue > 0.0f) ? coopEdx : static_cast<uint32>(increment >> 32);
    const uint32 lastWeapon = CStats::m_LastWeaponTypeFired;               // mov eax,[0xB79514]
    if (lastWeapon != slot) {                                              // cmp eax,edi; je 0x55CF3E
        CStats::m_LastWeaponTypeFired = slot;
        CStats::m_WeaponCounter = 0;
        STATS_EXIT(skillBits, incrementEdx, lastWeapon);
    }
    // 0x55CF3E: fild [0xB79504] (+2^32); fcom [slot*4+0xB78FC8]; fnstsw ax; test ah,41h; jne 0x55CF82
    const uint32 weaponCounter = CStats::m_WeaponCounter;
    const double counter = StatsCounterX87Cpp(weaponCounter);
    const uint32 axReach = StatsCompareDblFltAxCpp(counter, ExeFloat(0xB78FC8u + static_cast<uintptr_t>(slot) * 4u), 1);
    if (AhJe(axReach, 0x41)) {
        // fmul [ebx] (ebx = &[slot*4+0xB78F40]); push ecx; fstp dword [esp]; push esi; push 1; call Display
        const float shown = UnsignedTimesFloatCpp(weaponCounter, ExeFloat(incrementAddress));
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_INCREASE, static_cast<eStats>(statIndex), shown);
        CStats::m_WeaponCounter = 0;
        STATS_EXIT(skillBits, static_cast<uint32>(display >> 32), static_cast<uint32>(display));
    }
    // 0x55CF82: mov eax,[0xB79504]; fstp st(0); inc eax; mov [0xB79504],eax
    const uint32 next = weaponCounter + 1u;
    CStats::m_WeaponCounter = next;
    STATS_EXIT(skillBits, incrementEdx, next);
}

// Hook de 0x55CEB0: cdecl (eWeaponType) en la pila; el llamador (0x73B853) la limpia con add esp,4.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsWhenWeaponHitExe(uint32 entryEcx, uint32 entryEdx) {
    const uint32 weaponType = static_cast<const uint32*>(_AddressOfReturnAddress())[1];
    return StatsWeaponHitBody<true>(entryEcx, entryEdx, weaponType);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsWhenWeaponHit(eWeaponType weaponType) {
    StatsWeaponHitBody<false>(0u, 0u, static_cast<uint32>(weaponType));
}

// 0x55CFA0
void CStats::UpdateStatsWhenFighting() {
    UpdateFatAndMuscleStats(static_cast<uint32>(StatsFtol(StatReactionValue[STAT_EXERCISE_RATE_FIGHT]))); // Como el original: ftol
}

// 0x55CFC0
namespace {
// Como el original (0x55CFC0..0x55D02A). Salidas (`pop ecx; ret`, ECX = el de entrada):
//  - contador + 1 (0x55D023) y fin sin reducir salud (0x55D017): EDX = el de entrada y EAX = parte alta del contador + AX de `fnstsw ax`
//    (condiciones C0/C2/C3 de la comparacion mas las banderas x87 acumuladas, TOP = 0), que StatsCounterVsFloatAxCpp/StatsCompareFltFltAxCpp ejecutan con
//    las mismas operaciones x87 (fild, fadd 2^32, fcomp);
//  - tras Increment + Display (0x55D014): EAX/EDX de DisplayScriptStatUpdateMessage.
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsRespawnBody(uint32 entryEcx, uint32 entryEdx) {
    float* const reaction = CStats::StatReactionValue.data();
    const uint32 counter = CStats::m_DeathCounter;                         // mov eax,[0xB79508]
    const uint32 high = counter & 0xFFFF0000u;
    // fild [0xB79508] (+2^32 si el bit alto esta puesto); fcomp [0xB78FF4]; fnstsw ax; test ah,41h; jne 0x55D023
    const uint32 axCounter = StatsCounterVsFloatAxCpp(counter, reaction[STAT_TIMELIMIT_DEATH_HEALTH]);
    if (!AhJe(axCounter, 0x41)) {
        CStats::m_DeathCounter = counter + 1u;                             // inc dword ptr [0xB79508]
        STATS_EXIT(entryEcx, entryEdx, high | (axCounter & 0xFFFFu));
    }
    // 0x55CFE3: fld [0xB793E0]; fcomp [0x85A700] (400.0f); fnstsw ax; test ah,41h; jne 0x55D017
    const uint32 axHealth = StatsCompareFltFltAxCpp(CStats::StatTypesFloat.data()[STAT_MAX_HEALTH], 400.0f, 0);
    if (AhJe(axHealth, 0x41)) {
        // mov ecx,[0xB78F74]; push ecx; push 0x18; call 0x55C180; mov edx,[0xB78F74]; push edx; push 0x18; push 0; call 0x55B980; add esp,0x14
        CStats::IncrementStat(STAT_MAX_HEALTH, reaction[STAT_DEC_MAX_HEALTH]);
        const uint64 display = StatsDisplayRegs<kExe>(STAT_UPDATE_DECREASE, STAT_MAX_HEALTH, reaction[STAT_DEC_MAX_HEALTH]);
        CStats::m_DeathCounter = 0;
        STATS_EXIT(entryEcx, static_cast<uint32>(display >> 32), static_cast<uint32>(display));
    }
    CStats::m_DeathCounter = 0;                                            // 0x55D017
    STATS_EXIT(entryEcx, entryEdx, high | (axHealth & 0xFFFFu));
}

// Hook de 0x55CFC0: sin argumentos; ECX y EDX de entrada por registro.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsOnRespawnExe(uint32 entryEcx, uint32 entryEdx) {
    return StatsRespawnBody<true>(entryEcx, entryEdx);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsOnRespawn() {
    StatsRespawnBody<false>(0u, 0u);
}

// 0x55D030
namespace {
// Como el original (0x55D030..0x55D087): sobrescribe su argumento con la suma ([esp+4] = contador + argumento, que el banco observa) y la guarda en
// el contador. Salidas: EAX = parte alta de la suma + AX de la comparacion (ECX/EDX = los de entrada), o la llamada a Display en la ultima posicion
// (ret sin pop: EAX/ECX/EDX del colaborador salen sin tocar; el `mov [0xB79510],0` posterior se hace antes de Display, que no lo lee).
#pragma runtime_checks("", off)
template<bool kExe>
__declspec(safebuffers) uint32 __fastcall StatsAddToHealthBody(uint32 entryEcx, uint32 entryEdx, volatile uint32* argument) {
    float* const reaction = CStats::StatReactionValue.data();
    const uint32 sum = CStats::m_AddToHealthCounter + *argument;           // mov eax,[0xB79510]; add eax,[esp+4]
    *argument = sum;                                                       // mov [esp+4],eax
    CStats::m_AddToHealthCounter = sum;
    // fild [esp+4] (+2^32 si el bit alto esta puesto); fcomp [0xB78FF8]; fnstsw ax; test ah,41h; jne 0x55D087
    const uint32 axSum = StatsCounterVsFloatAxCpp(sum, reaction[STAT_TIMELIMIT_ADD_TO_HEALTH]);
    if (!AhJe(axSum, 0x41)) {
        STATS_EXIT(entryEcx, entryEdx, (sum & 0xFFFF0000u) | (axSum & 0xFFFFu));
    }
    // mov eax,[0xB78F3C]; push eax; push 0x18; call 0x55C180; mov ecx,[0xB78F3C]; push ecx; push 0x18; push 1; call 0x55B980; add esp,0x14
    CStats::IncrementStat(STAT_MAX_HEALTH, reaction[STAT_INC_MAX_HEALTH]);
    CStats::m_AddToHealthCounter = 0;
    if constexpr (kExe) {
        return reinterpret_cast<uint32(__cdecl*)(eStatUpdateState, eStats, float)>(&CStats::DisplayScriptStatUpdateMessage)(STAT_UPDATE_INCREASE, STAT_MAX_HEALTH, reaction[STAT_INC_MAX_HEALTH]);
    } else {
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_MAX_HEALTH, reaction[STAT_INC_MAX_HEALTH]);
        return 0;
    }
}

// Hook de 0x55D030: cdecl (uint32) en la pila; [esp+4] se sobrescribe y los llamadores (0x456610, 0x55C34A) lo limpian con add esp,4.
uint32 __declspec(safebuffers) __fastcall StatsUpdateStatsAddToHealthExe(uint32 entryEcx, uint32 entryEdx) {
    volatile uint32* const arguments = static_cast<volatile uint32*>(_AddressOfReturnAddress()) + 1;
    return StatsAddToHealthBody<true>(entryEcx, entryEdx, arguments);
}
#pragma runtime_checks("", restore)
}

void CStats::UpdateStatsAddToHealth(uint32 addToHealth) {
    uint32 argument = addToHealth;
    StatsAddToHealthBody<false>(0u, 0u, &argument);
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

#undef STATS_EXIT
#undef STATS_CAPTURE_REGS
#undef STATS_CAPTURE_EAX_ECX

// IMPRESCINDIBLE: dejar al final del archivo y sin `restore`. MSVC instancia las plantillas <kExe> (cuerpos StatsXxxBody<true/false>) al final de la unidad de
// traduccion e ignora el estado de #pragma runtime_checks vigente donde estan definidas, llamadas o instanciadas explicitamente: con /RTC1 (Debug) emiten
// `call _RTC_CheckStackVars` (que pone ECX = EBP, pisando el residuo de ECX), `push edx`/`pop edx` alrededor y `add esp,N; cmp ebp,esp; call __RTC_CheckEsp`
// tras la llamada a ResidueRegsCpp. Con el pragma activo en el estado final de la unidad, las instancias salen sin comprobaciones.
#pragma runtime_checks("", off)
