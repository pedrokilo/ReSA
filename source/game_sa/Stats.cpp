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
#include "AudioEngine.h"
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


namespace {
__declspec(naked) void Ui8_Bridge_0053FB70() {
    __asm {
        push 053FB70h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_00588B50() {
    __asm {
        push 0588B50h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_00558E40() {
    __asm {
        push 0558E40h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_006A0050() {
    __asm {
        push 06A0050h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_00588BE0() {
    __asm {
        push 0588BE0h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_00559080() {
    __asm {
        push 0559080h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_00821B40() {
    __asm {
        push 0821B40h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_0055C180() {
    __asm {
        push 055C180h
        ret
    }
}
__declspec(naked) void Ui8_Bridge_0055B980() {
    __asm {
        push 055B980h
        ret
    }
}
}

void CStats::InjectHooks() {
    RH_ScopedClass(CStats);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(CheckForStatsMessage, 0x559760);
    RH_ScopedInstall(FindCriminalRatingString, 0x55A210);
    RH_ScopedInstall(UpdateStatsOnRespawn, 0x55CFC0);
    RH_ScopedInstall(UpdateStatsAddToHealth, 0x55D030);


    RH_ScopedInstall(Init, 0x55C0C0);
    RH_ScopedInstall(UpdateStatsWhenCycling, 0x55C780);
    RH_ScopedInstall(UpdateFatAndMuscleStats, 0x55C470);
    RH_ScopedInstall(UpdateStatsWhenSwimming, 0x55C990);
    RH_ScopedInstall(UpdateStatsWhenWeaponHit, 0x55CEB0);
    RH_ScopedInstall(UpdateStatsWhenDriving, 0x55CAC0);
    RH_ScopedInstall(UpdateStatsWhenFlying, 0x55CC00);
    RH_ScopedInstall(PopulateFavoriteRadioStationList, 0x558EC0);
    RH_ScopedInstall(FindMostFavoriteRadioStation, 0x558FA0);
    RH_ScopedInstall(GetFatAndMuscleModifier, 0x559AF0);
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
    RH_ScopedInstall(LoadStatUpdateConditions, 0x559860);
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
// The EXE returns the complete station index in EAX despite the int8 enum declaration.
// Ignore emergency/off (indices 0/13), keep the first strictly greater signed value.
__declspec(naked) eRadioID CStats::FindMostFavoriteRadioStation() {
    __asm {
        xor ecx, ecx
        mov eax, 1
        mov edx, 1
    nextStation:
        cmp dword ptr [edx*4+0B78E58h], ecx
        jle keepStation
        mov ecx, dword ptr [edx*4+0B78E58h]
        mov eax, edx
    keepStation:
        inc edx
        cmp edx, 13
        jl nextStation
        ret
    }
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
__declspec(naked) void CStats::CheckForStatsMessage() {
    __asm {
// 0x559760
        push        0
// 0x559762
        call Ui8_Bridge_0053FB70
// 0x559767
        mov         cl,byte ptr [eax+00000117h]
// 0x55976D
        add         esp,4
// 0x559770
        test        cl,cl
// 0x559772
        jne Ui8_CheckForStatsMessage_55985F
// 0x559778
        mov         al,byte ptr ds:[008CDE56h]
// 0x55977D
        test        al,al
// 0x55977F
        je Ui8_CheckForStatsMessage_55985F
// 0x559785
        mov         al,byte ptr ds:[00B6F065h]
// 0x55978A
        test        al,al
// 0x55978C
        jne Ui8_CheckForStatsMessage_55985F
// 0x559792
        mov         al,byte ptr ds:[00B5F852h]
// 0x559797
        test        al,al
// 0x559799
        jne Ui8_CheckForStatsMessage_55985F
// 0x55979F
        call Ui8_Bridge_00588B50
// 0x5597A4
        test        al,al
// 0x5597A6
        jne Ui8_CheckForStatsMessage_55985F
// 0x5597AC
        mov         al,byte ptr ds:[00BA82E3h]
// 0x5597B1
        test        al,al
// 0x5597B3
        jne Ui8_CheckForStatsMessage_55985F
// 0x5597B9
        push        ebx
// 0x5597BA
        push        esi
// 0x5597BB
        push        edi
// 0x5597BC
        xor         edi,edi
// 0x5597BE
        mov         esi,0B78200h
// 0x5597C3
        mov         bl,1
// 0x5597C5
        jmp Ui8_CheckForStatsMessage_5597D0
// 0x5597C7
        lea         esp,[esp+00000000h]
// 0x5597CE
        mov         edi,edi
Ui8_CheckForStatsMessage_5597D0:
        cmp         edi,dword ptr ds:[00B794D0h]
// 0x5597D6
        jae Ui8_CheckForStatsMessage_55985C
// 0x5597DC
        mov         al,byte ptr [esi+2]
// 0x5597DF
        test        al,al
// 0x5597E1
        jne Ui8_CheckForStatsMessage_55984C
// 0x5597E3
        mov         al,byte ptr [esi+3]
// 0x5597E6
        test        al,al
// 0x5597E8
        je Ui8_CheckForStatsMessage_55980E
// 0x5597EA
        xor         eax,eax
// 0x5597EC
        mov         ax,word ptr [esi]
// 0x5597EF
        push        eax
// 0x5597F0
        call Ui8_Bridge_00558E40
// 0x5597F5
        fcomp       dword ptr [esi+4]
// 0x5597F8
        add         esp,4
// 0x5597FB
        fnstsw      ax
// 0x5597FD
        test        ah,1
// 0x559800
        jne Ui8_CheckForStatsMessage_55984C
// 0x559802
        push        0
// 0x559804
        push        0
// 0x559806
        push        0
// 0x559808
        lea         ecx,[esi+8]
// 0x55980B
        push        ecx
// 0x55980C
        jmp Ui8_CheckForStatsMessage_559830
Ui8_CheckForStatsMessage_55980E:
        xor         edx,edx
// 0x559810
        mov         dx,word ptr [esi]
// 0x559813
        push        edx
// 0x559814
        call Ui8_Bridge_00558E40
// 0x559819
        fcomp       dword ptr [esi+4]
// 0x55981C
        add         esp,4
// 0x55981F
        fnstsw      ax
// 0x559821
        test        ah,41h
// 0x559824
        jp Ui8_CheckForStatsMessage_55984C
// 0x559826
        push        0
// 0x559828
        push        0
// 0x55982A
        push        0
// 0x55982C
        lea         eax,[esi+8]
// 0x55982F
        push        eax
Ui8_CheckForStatsMessage_559830:
        mov         ecx,0C1B340h
// 0x559835
        mov         byte ptr [esi+2],bl
// 0x559838
        call Ui8_Bridge_006A0050
// 0x55983D
        push        eax
// 0x55983E
        call Ui8_Bridge_00588BE0
// 0x559843
        add         esp,10h
// 0x559846
        mov         byte ptr ds:[00B794D4h],bl
Ui8_CheckForStatsMessage_55984C:
        inc         edi
// 0x55984D
        add         esi,10h
// 0x559850
        cmp         edi,80h
// 0x559856
        jb Ui8_CheckForStatsMessage_5597D0
Ui8_CheckForStatsMessage_55985C:
        pop         edi
// 0x55985D
        pop         esi
// 0x55985E
        pop         ebx
Ui8_CheckForStatsMessage_55985F:
        ret
    }
}

// 0x559860
// Tail entry bridge: preserves registers, flags and physical arguments.
__declspec(naked) static void StatsFile15EntrySetDir() {
    __asm { push 05387D0h }
    __asm { ret }
}

// Tail entry bridge: preserves registers, flags and physical arguments.
__declspec(naked) static void StatsFile15EntryOpenFile() {
    __asm { push 0538900h }
    __asm { ret }
}

// Tail entry bridge: preserves registers, flags and physical arguments.
__declspec(naked) static void StatsFile15EntryLoadLine() {
    __asm { push 0536F80h }
    __asm { ret }
}

// Tail entry bridge: preserves registers, flags and physical arguments.
__declspec(naked) static void StatsFile15EntryCloseFile() {
    __asm { push 05389D0h }
    __asm { ret }
}

// Tail entry bridge: preserves registers, flags and physical arguments.
__declspec(naked) static void StatsFile15EntrySscanf() {
    __asm { push 08220ADh }
    __asm { ret }
}

__declspec(naked) void CStats::LoadStatUpdateConditions() {
    __asm {
        // 0x559860
        push ebp
        // 0x559861
        mov ebp,esp
        // 0x559863
        and esp,0FFFFFFF8h
        // 0x559866
        sub esp,78h
        // 0x559869
        push ebx
        // 0x55986A
        push ebp
        // 0x55986B
        push esi
        // 0x55986C
        push edi
        // 0x55986D
        xor ebx,ebx
        // 0x55986F
        push 858B54h
        // 0x559874
        mov dword ptr [esp+1Ch],0
        // 0x55987C
        mov dword ptr [esp+18h],ebx
        // 0x559880
        call StatsFile15EntrySetDir
        // 0x559885
        push 859C6Ch
        // 0x55988A
        push 863F80h
        // 0x55988F
        call StatsFile15EntryOpenFile
        // 0x559894
        mov esi,eax
        // 0x559896
        push esi
        // 0x559897
        mov dword ptr [esp+30h],esi
        // 0x55989B
        mov dword ptr ds:[00B794D0h],ebx
        // 0x5598A1
        mov dword ptr [esp+20h],ebx
        // 0x5598A5
        call StatsFile15EntryLoadLine
        // 0x5598AA
        add esp,10h
        // 0x5598AD
        cmp eax,ebx
        // 0x5598AF
        je StatsFile15_55998B
        // 0x5598B5
        mov ecx,8
        // 0x5598BA
        lea edx,[esp+24h]
        // 0x5598BE
        sub ecx,edx
        // 0x5598C0
        mov ebp,0B78200h
        // 0x5598C5
        mov dword ptr [esp+1Ch],ecx
        // 0x5598C9
        lea esp,[esp+00000000h]
        StatsFile15_5598D0:
        mov cl,byte ptr [eax]
        // 0x5598D2
        cmp cl,23h
        // 0x5598D5
        je StatsFile15_55997A
        // 0x5598DB
        cmp cl,bl
        // 0x5598DD
        je StatsFile15_55997A
        // 0x5598E3
        lea ecx,[esp+24h]
        // 0x5598E7
        push ecx
        // 0x5598E8
        lea edx,[esp+1Ch]
        // 0x5598EC
        push edx
        // 0x5598ED
        lea ecx,[esp+34h]
        // 0x5598F1
        push ecx
        // 0x5598F2
        lea edx,[esp+44h]
        // 0x5598F6
        push edx
        // 0x5598F7
        lea ecx,[esp+24h]
        // 0x5598FB
        push ecx
        // 0x5598FC
        push 863F70h
        // 0x559901
        push eax
        // 0x559902
        call StatsFile15EntrySscanf
        // 0x559907
        mov dx,word ptr [esp+30h]
        // 0x55990C
        add esp,1Ch
        // 0x55990F
        mov edi,863F64h
        // 0x559914
        lea esi,[esp+2Ch]
        // 0x559918
        mov ecx,9
        // 0x55991D
        xor eax,eax
        // 0x55991F
        repe cmpsb
        // 0x559921
        mov word ptr [ebp],dx
        // 0x559925
        mov byte ptr [ebp+2],bl
        // 0x559928
        jne StatsFile15_55992F
        // 0x55992A
        mov byte ptr [ebp+3],bl
        // 0x55992D
        jmp StatsFile15_559947
        StatsFile15_55992F:
        mov edi,863F58h
        // 0x559934
        lea esi,[esp+2Ch]
        // 0x559938
        mov ecx,9
        // 0x55993D
        xor edx,edx
        // 0x55993F
        repe cmpsb
        // 0x559941
        jne StatsFile15_559947
        // 0x559943
        mov byte ptr [ebp+3],1
        StatsFile15_559947:
        mov eax,dword ptr [esp+18h]
        // 0x55994B
        mov ecx,dword ptr [esp+1Ch]
        // 0x55994F
        mov dword ptr [ebp+4],eax
        // 0x559952
        lea eax,[esp+24h]
        // 0x559956
        lea edx,[ecx+ebp]
        // 0x559959
        lea esp,[esp+00000000h]
        StatsFile15_559960:
        mov cl,byte ptr [eax]
        // 0x559962
        mov byte ptr [edx+eax],cl
        // 0x559965
        inc eax
        // 0x559966
        cmp cl,bl
        // 0x559968
        jne StatsFile15_559960
        // 0x55996A
        mov ecx,dword ptr [esp+10h]
        // 0x55996E
        mov esi,dword ptr [esp+20h]
        // 0x559972
        inc ecx
        // 0x559973
        mov dword ptr [esp+10h],ecx
        // 0x559977
        add ebp,10h
        StatsFile15_55997A:
        push esi
        // 0x55997B
        call StatsFile15EntryLoadLine
        // 0x559980
        add esp,4
        // 0x559983
        cmp eax,ebx
        // 0x559985
        jne StatsFile15_5598D0
        StatsFile15_55998B:
        mov edx,dword ptr [esp+10h]
        // 0x55998F
        push esi
        // 0x559990
        mov dword ptr ds:[00B794D0h],edx
        // 0x559996
        call StatsFile15EntryCloseFile
        // 0x55999B
        add esp,4
        // 0x55999E
        pop edi
        // 0x55999F
        pop esi
        // 0x5599A0
        pop ebp
        // 0x5599A1
        pop ebx
        // 0x5599A2
        mov esp,ebp
        // 0x5599A4
        pop ebp
        // 0x5599A5
        ret
    }
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

// Como el original: Max/Min devuelven el segundo argumento con NaN; retornan en ST0.
static __declspec(naked) float __cdecl StatsModifierMaxExe(float, float) {
    __asm {
        fld dword ptr [esp+4]
        fcomp dword ptr [esp+8]
        fnstsw ax
        test ah,41h
        jne statsMaxSecond
        fld dword ptr [esp+4]
        ret
    statsMaxSecond:
        fld dword ptr [esp+8]
        ret
    }
}
static __declspec(naked) float __cdecl StatsModifierMinExe(float, float) {
    __asm {
        fld dword ptr [esp+4]
        fcomp dword ptr [esp+8]
        fnstsw ax
        test ah,5
        jp statsMinSecond
        fld dword ptr [esp+4]
        ret
    statsMinSecond:
        fld dword ptr [esp+8]
        ret
    }
}

static float __cdecl StatsModifierBody(eStatModAbilities statMod) {
    // Como el original: orden x87 y comparaciones AH, incluidos NaN y configuración viva 0x8CDE58..C0.
    float result;
    __asm {
        mov eax, statMod
        fld         dword ptr ds:[00858624h]
        cmp         eax,0Fh
        ja          statsMod_00559F58
        cmp eax, 0
        je statsMod_00559B0A
        cmp eax, 1
        je statsMod_00559B41
        cmp eax, 2
        je statsMod_00559BB3
        cmp eax, 3
        je statsMod_00559C25
        cmp eax, 4
        je statsMod_00559C61
        cmp eax, 5
        je statsMod_00559CC5
        cmp eax, 6
        je statsMod_00559D62
        cmp eax, 7
        je statsMod_00559DC9
        cmp eax, 8
        je statsMod_00559DE4
        cmp eax, 9
        je statsMod_00559E05
        cmp eax, 10
        je statsMod_00559E1A
        cmp eax, 11
        je statsMod_00559E23
        cmp eax, 12
        je statsMod_00559E4F
        cmp eax, 13
        je statsMod_00559E7B
        cmp eax, 14
        je statsMod_00559EA7
        cmp eax, 15
        je statsMod_00559EF9
        jmp statsMod_00559F58
    statsMod_00559B0A:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fcomp       dword ptr ds:[008594DCh]
        fnstsw      ax
        test        ah,41h
        je          statsMod_00559CBE
        fld         dword ptr ds:[00B793D4h]
        fcomp       dword ptr ds:[0085A700h]
        fnstsw      ax
        test        ah,41h
        je          statsMod_00559EF2
        fld         dword ptr ds:[00858B50h]
        jmp statsMod_finish
    statsMod_00559B41:
        fstp        st(0)
        push        ecx
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fstp        dword ptr [esp]
        push        0
        call StatsModifierMaxExe
        fmul        dword ptr ds:[008CDEBCh]
        add         esp,8
        fadd        dword ptr ds:[00858624h]
        fld         dword ptr ds:[00B793DCh]
        fsub        dword ptr ds:[008CDEB8h]
        fmul        dword ptr ds:[008CDEB4h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEB8h]
        fdivp       st(1),st
        faddp       st(1),st
        fcom        dword ptr ds:[00858CB0h]
        fnstsw      ax
        test        ah,5
        jp          statsMod_00559F58
        fstp        st(0)
        fld         dword ptr ds:[00858CB0h]
        jmp statsMod_finish
    statsMod_00559BB3:
        fstp        st(0)
        push        ecx
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fstp        dword ptr [esp]
        push        0
        call StatsModifierMaxExe
        fmul        dword ptr ds:[008CDEB0h]
        add         esp,8
        fadd        dword ptr ds:[00858624h]
        fld         dword ptr ds:[00B793DCh]
        fsub        dword ptr ds:[008CDEB8h]
        fmul        dword ptr ds:[008CDEACh]
    statsMod_00559BFB:
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEB8h]
        fdivp       st(1),st
        faddp       st(1),st
        fcom        dword ptr ds:[00858C98h]
        fnstsw      ax
        test        ah,5
        jp          statsMod_00559F58
        fstp        st(0)
        fld         dword ptr ds:[00858C98h]
        jmp statsMod_finish
    statsMod_00559C25:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fmul        dword ptr ds:[008CDEA8h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fadd        dword ptr ds:[00858624h]
        fld         dword ptr ds:[00B793DCh]
        fsub        dword ptr ds:[008CDEB8h]
        fmul        dword ptr ds:[008CDEA4h]
        jmp         statsMod_00559BFB
    statsMod_00559C61:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fmul        dword ptr ds:[008CDEA0h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fadd        dword ptr ds:[00858624h]
        fld         dword ptr ds:[00B793DCh]
        fsub        dword ptr ds:[008CDEB8h]
        fmul        dword ptr ds:[008CDE9Ch]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEB8h]
        fdivp       st(1),st
        faddp       st(1),st
        fcom        dword ptr ds:[00858CA0h]
        fnstsw      ax
        test        ah,41h
        jne         statsMod_00559F58
    statsMod_00559CBC:
        fstp        st(0)
    statsMod_00559CBE:
        fld         dword ptr ds:[00858CA0h]
        jmp statsMod_finish
    statsMod_00559CC5:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fmul        dword ptr ds:[008CDE98h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fadd        dword ptr ds:[00858624h]
        fld         dword ptr ds:[00B793DCh]
        fsub        dword ptr ds:[008CDEB8h]
        fmul        dword ptr ds:[008CDE94h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEB8h]
        fdivp       st(1),st
        faddp       st(1),st
        fld         dword ptr ds:[00B793D8h]
        fmul        dword ptr ds:[00858CDCh]
        fmul        dword ptr ds:[008CDE90h]
        faddp       st(1),st
        fild        dword ptr ds:[00B791B8h]
        fmul        dword ptr ds:[00858CDCh]
        fmul        dword ptr ds:[008CDE8Ch]
        faddp       st(1),st
        fcom        dword ptr ds:[00858CA0h]
        fnstsw      ax
        test        ah,41h
        je          statsMod_00559CBC
        fcom        dword ptr ds:[00858C84h]
        fnstsw      ax
        test        ah,5
        jp          statsMod_00559F58
        fstp        st(0)
        fld         dword ptr ds:[00858C84h]
        jmp statsMod_finish
    statsMod_00559D62:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fmul        dword ptr ds:[008CDE88h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fadd        dword ptr ds:[00858624h]
        fild        dword ptr ds:[00B791B8h]
        fmul        dword ptr ds:[00858CDCh]
        fmul        dword ptr ds:[008CDE84h]
        faddp       st(1),st
        fcom        dword ptr ds:[00858CA0h]
        fnstsw      ax
        test        ah,41h
        je          statsMod_00559CBC
        fcom        dword ptr ds:[00858B8Ch]
        fnstsw      ax
        test        ah,5
        jp          statsMod_00559F58
        fstp        st(0)
        fld         dword ptr ds:[00858B8Ch]
        jmp statsMod_finish
    statsMod_00559DC9:
        fstp        st(0)
        fld         dword ptr ds:[00B793D8h]
        fmul        dword ptr ds:[00858CDCh]
        fmul        dword ptr ds:[008CDE80h]
        fadd        dword ptr ds:[00858A28h]
        jmp statsMod_finish
    statsMod_00559DE4:
        fstp        st(0)
        fild        dword ptr ds:[00B791A4h]
        fadd        dword ptr ds:[00B793D8h]
        fmul        dword ptr ds:[00859978h]
        fmul        dword ptr ds:[008CDE7Ch]
        fadd        dword ptr ds:[00858C4Ch]
        jmp statsMod_finish
    statsMod_00559E05:
        fstp        st(0)
        fld         dword ptr ds:[00B793E0h]
        fmul        dword ptr ds:[00858CDCh]
        fmul        dword ptr ds:[008CDE78h]
        jmp statsMod_finish
    statsMod_00559E1A:
        fstp        st(0)
        fld         dword ptr ds:[008CDE78h]
        jmp statsMod_finish
    statsMod_00559E23:
        fstp        st(0)
        push        ecx
        fild        dword ptr ds:[00B791B4h]
        fmul        dword ptr ds:[00858CDCh]
        fstp        dword ptr [esp]
        push        3F800000h
        call StatsModifierMinExe
        fmul        dword ptr ds:[008CDE74h]
        add         esp,8
        fadd        dword ptr ds:[00858624h]
        jmp statsMod_finish
    statsMod_00559E4F:
        fstp        st(0)
        push        ecx
        fild        dword ptr ds:[00B791B4h]
        fmul        dword ptr ds:[00858CDCh]
        fstp        dword ptr [esp]
        push        3F800000h
        call StatsModifierMinExe
        fmul        dword ptr ds:[008CDE70h]
        add         esp,8
        fadd        dword ptr ds:[00858624h]
        jmp statsMod_finish
    statsMod_00559E7B:
        fstp        st(0)
        push        ecx
        fild        dword ptr ds:[00B791B4h]
        fmul        dword ptr ds:[00858CDCh]
        fstp        dword ptr [esp]
        push        3F800000h
        call StatsModifierMinExe
        fmul        dword ptr ds:[008CDE6Ch]
        add         esp,8
        fadd        dword ptr ds:[00858624h]
        jmp statsMod_finish
    statsMod_00559EA7:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fmul        dword ptr ds:[008CDE68h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fild        dword ptr ds:[00B790A0h]
        fmul        dword ptr ds:[00858CDCh]
        fmul        dword ptr ds:[008CDE64h]
        fadd        dword ptr ds:[00858624h]
        faddp       st(1),st
        fcom        dword ptr ds:[00858624h]
        fnstsw      ax
        test        ah,41h
        jne         statsMod_00559F58
        fstp        st(0)
    statsMod_00559EF2:
        fld         dword ptr ds:[00858624h]
        jmp statsMod_finish
    statsMod_00559EF9:
        fstp        st(0)
        fld         dword ptr ds:[00B793D4h]
        fsub        dword ptr ds:[008CDEC0h]
        fmul        dword ptr ds:[008CDE60h]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEC0h]
        fdivp       st(1),st
        fadd        dword ptr ds:[00858624h]
        fld         dword ptr ds:[00B793DCh]
        fsub        dword ptr ds:[008CDEB8h]
        fmul        dword ptr ds:[008CDE5Ch]
        fld         dword ptr ds:[00858C4Ch]
        fsub        dword ptr ds:[008CDEB8h]
        fdivp       st(1),st
        faddp       st(1),st
        fcom        dword ptr ds:[008CDE58h]
        fnstsw      ax
        test        ah,5
        jp          statsMod_00559F58
        fstp        st(0)
        fld         dword ptr ds:[008CDE58h]
    statsMod_00559F58:
        jmp statsMod_finish
    statsMod_finish:
        fstp result
    }
    return result;
}

// 0x559AF0: el EXE conserva ECX/EDX incluso en las ramas con llamadas Max/Min.
__declspec(naked) float CStats::GetFatAndMuscleModifier(eStatModAbilities) {
    __asm {
        push ecx
        push edx
        push dword ptr [esp+12]
        call StatsModifierBody
        add esp,4
        pop edx
        pop ecx
        ret
    }
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
__declspec(naked) GxtChar* CStats::FindCriminalRatingString() {
    __asm {
// 0x55A210
        call Ui8_Bridge_00559080
// 0x55A215
        test        eax,eax
// 0x55A217
        jge Ui8_FindCriminalRatingString_55A280
// 0x55A219
        cmp         eax,0FFFFFE0Ch
// 0x55A21E
        jle Ui8_FindCriminalRatingString_55A230
// 0x55A220
        push        86417Ch
// 0x55A225
        mov         ecx,0C1B340h
// 0x55A22A
        call Ui8_Bridge_006A0050
// 0x55A22F
        ret
Ui8_FindCriminalRatingString_55A230:
        cmp         eax,0FFFFF830h
// 0x55A235
        jle Ui8_FindCriminalRatingString_55A247
// 0x55A237
        push        864174h
// 0x55A23C
        mov         ecx,0C1B340h
// 0x55A241
        call Ui8_Bridge_006A0050
// 0x55A246
        ret
Ui8_FindCriminalRatingString_55A247:
        cmp         eax,0FFFFF060h
// 0x55A24C
        jle Ui8_FindCriminalRatingString_55A25E
// 0x55A24E
        push        86416Ch
// 0x55A253
        mov         ecx,0C1B340h
// 0x55A258
        call Ui8_Bridge_006A0050
// 0x55A25D
        ret
Ui8_FindCriminalRatingString_55A25E:
        cmp         eax,0FFFFE890h
// 0x55A263
        mov         ecx,0C1B340h
// 0x55A268
        jle Ui8_FindCriminalRatingString_55A275
// 0x55A26A
        push        864164h
// 0x55A26F
        call Ui8_Bridge_006A0050
// 0x55A274
        ret
Ui8_FindCriminalRatingString_55A275:
        push        86415Ch
// 0x55A27A
        call Ui8_Bridge_006A0050
// 0x55A27F
        ret
Ui8_FindCriminalRatingString_55A280:
        cmp         eax,14h
// 0x55A283
        jge Ui8_FindCriminalRatingString_55A295
// 0x55A285
        push        864154h
// 0x55A28A
        mov         ecx,0C1B340h
// 0x55A28F
        call Ui8_Bridge_006A0050
// 0x55A294
        ret
Ui8_FindCriminalRatingString_55A295:
        cmp         eax,32h
// 0x55A298
        jge Ui8_FindCriminalRatingString_55A2AA
// 0x55A29A
        push        86414Ch
// 0x55A29F
        mov         ecx,0C1B340h
// 0x55A2A4
        call Ui8_Bridge_006A0050
// 0x55A2A9
        ret
Ui8_FindCriminalRatingString_55A2AA:
        cmp         eax,4Bh
// 0x55A2AD
        jge Ui8_FindCriminalRatingString_55A2BF
// 0x55A2AF
        push        864144h
// 0x55A2B4
        mov         ecx,0C1B340h
// 0x55A2B9
        call Ui8_Bridge_006A0050
// 0x55A2BE
        ret
Ui8_FindCriminalRatingString_55A2BF:
        cmp         eax,64h
// 0x55A2C2
        jge Ui8_FindCriminalRatingString_55A2D4
// 0x55A2C4
        push        86413Ch
// 0x55A2C9
        mov         ecx,0C1B340h
// 0x55A2CE
        call Ui8_Bridge_006A0050
// 0x55A2D3
        ret
Ui8_FindCriminalRatingString_55A2D4:
        cmp         eax,78h
// 0x55A2D7
        jge Ui8_FindCriminalRatingString_55A2E9
// 0x55A2D9
        push        864134h
// 0x55A2DE
        mov         ecx,0C1B340h
// 0x55A2E3
        call Ui8_Bridge_006A0050
// 0x55A2E8
        ret
Ui8_FindCriminalRatingString_55A2E9:
        cmp         eax,96h
// 0x55A2EE
        jge Ui8_FindCriminalRatingString_55A300
// 0x55A2F0
        push        86412Ch
// 0x55A2F5
        mov         ecx,0C1B340h
// 0x55A2FA
        call Ui8_Bridge_006A0050
// 0x55A2FF
        ret
Ui8_FindCriminalRatingString_55A300:
        cmp         eax,0C8h
// 0x55A305
        jge Ui8_FindCriminalRatingString_55A317
// 0x55A307
        push        864124h
// 0x55A30C
        mov         ecx,0C1B340h
// 0x55A311
        call Ui8_Bridge_006A0050
// 0x55A316
        ret
Ui8_FindCriminalRatingString_55A317:
        cmp         eax,0F0h
// 0x55A31C
        jge Ui8_FindCriminalRatingString_55A32E
// 0x55A31E
        push        86411Ch
// 0x55A323
        mov         ecx,0C1B340h
// 0x55A328
        call Ui8_Bridge_006A0050
// 0x55A32D
        ret
Ui8_FindCriminalRatingString_55A32E:
        cmp         eax,10Eh
// 0x55A333
        jge Ui8_FindCriminalRatingString_55A345
// 0x55A335
        push        864114h
// 0x55A33A
        mov         ecx,0C1B340h
// 0x55A33F
        call Ui8_Bridge_006A0050
// 0x55A344
        ret
Ui8_FindCriminalRatingString_55A345:
        cmp         eax,12Ch
// 0x55A34A
        jge Ui8_FindCriminalRatingString_55A35C
// 0x55A34C
        push        86410Ch
// 0x55A351
        mov         ecx,0C1B340h
// 0x55A356
        call Ui8_Bridge_006A0050
// 0x55A35B
        ret
Ui8_FindCriminalRatingString_55A35C:
        cmp         eax,14Fh
// 0x55A361
        jge Ui8_FindCriminalRatingString_55A373
// 0x55A363
        push        864104h
// 0x55A368
        mov         ecx,0C1B340h
// 0x55A36D
        call Ui8_Bridge_006A0050
// 0x55A372
        ret
Ui8_FindCriminalRatingString_55A373:
        cmp         eax,172h
// 0x55A378
        jge Ui8_FindCriminalRatingString_55A38A
// 0x55A37A
        push        8640FCh
// 0x55A37F
        mov         ecx,0C1B340h
// 0x55A384
        call Ui8_Bridge_006A0050
// 0x55A389
        ret
Ui8_FindCriminalRatingString_55A38A:
        cmp         eax,190h
// 0x55A38F
        jge Ui8_FindCriminalRatingString_55A3A1
// 0x55A391
        push        8640F4h
// 0x55A396
        mov         ecx,0C1B340h
// 0x55A39B
        call Ui8_Bridge_006A0050
// 0x55A3A0
        ret
Ui8_FindCriminalRatingString_55A3A1:
        cmp         eax,1C2h
// 0x55A3A6
        jge Ui8_FindCriminalRatingString_55A3B8
// 0x55A3A8
        push        8640ECh
// 0x55A3AD
        mov         ecx,0C1B340h
// 0x55A3B2
        call Ui8_Bridge_006A0050
// 0x55A3B7
        ret
Ui8_FindCriminalRatingString_55A3B8:
        cmp         eax,1F4h
// 0x55A3BD
        jge Ui8_FindCriminalRatingString_55A3CF
// 0x55A3BF
        push        8640E4h
// 0x55A3C4
        mov         ecx,0C1B340h
// 0x55A3C9
        call Ui8_Bridge_006A0050
// 0x55A3CE
        ret
Ui8_FindCriminalRatingString_55A3CF:
        cmp         eax,226h
// 0x55A3D4
        jge Ui8_FindCriminalRatingString_55A3E6
// 0x55A3D6
        push        8640DCh
// 0x55A3DB
        mov         ecx,0C1B340h
// 0x55A3E0
        call Ui8_Bridge_006A0050
// 0x55A3E5
        ret
Ui8_FindCriminalRatingString_55A3E6:
        cmp         eax,258h
// 0x55A3EB
        jge Ui8_FindCriminalRatingString_55A3FD
// 0x55A3ED
        push        8640D4h
// 0x55A3F2
        mov         ecx,0C1B340h
// 0x55A3F7
        call Ui8_Bridge_006A0050
// 0x55A3FC
        ret
Ui8_FindCriminalRatingString_55A3FD:
        cmp         eax,262h
// 0x55A402
        jge Ui8_FindCriminalRatingString_55A414
// 0x55A404
        push        8640CCh
// 0x55A409
        mov         ecx,0C1B340h
// 0x55A40E
        call Ui8_Bridge_006A0050
// 0x55A413
        ret
Ui8_FindCriminalRatingString_55A414:
        cmp         eax,28Ah
// 0x55A419
        jge Ui8_FindCriminalRatingString_55A42B
// 0x55A41B
        push        8640C4h
// 0x55A420
        mov         ecx,0C1B340h
// 0x55A425
        call Ui8_Bridge_006A0050
// 0x55A42A
        ret
Ui8_FindCriminalRatingString_55A42B:
        cmp         eax,2BCh
// 0x55A430
        jge Ui8_FindCriminalRatingString_55A442
// 0x55A432
        push        8640BCh
// 0x55A437
        mov         ecx,0C1B340h
// 0x55A43C
        call Ui8_Bridge_006A0050
// 0x55A441
        ret
Ui8_FindCriminalRatingString_55A442:
        cmp         eax,352h
// 0x55A447
        jge Ui8_FindCriminalRatingString_55A459
// 0x55A449
        push        8640B4h
// 0x55A44E
        mov         ecx,0C1B340h
// 0x55A453
        call Ui8_Bridge_006A0050
// 0x55A458
        ret
Ui8_FindCriminalRatingString_55A459:
        cmp         eax,3E8h
// 0x55A45E
        jge Ui8_FindCriminalRatingString_55A470
// 0x55A460
        push        8640ACh
// 0x55A465
        mov         ecx,0C1B340h
// 0x55A46A
        call Ui8_Bridge_006A0050
// 0x55A46F
        ret
Ui8_FindCriminalRatingString_55A470:
        cmp         eax,3EDh
// 0x55A475
        jge Ui8_FindCriminalRatingString_55A487
// 0x55A477
        push        8640A4h
// 0x55A47C
        mov         ecx,0C1B340h
// 0x55A481
        call Ui8_Bridge_006A0050
// 0x55A486
        ret
Ui8_FindCriminalRatingString_55A487:
        cmp         eax,47Eh
// 0x55A48C
        jge Ui8_FindCriminalRatingString_55A49E
// 0x55A48E
        push        86409Ch
// 0x55A493
        mov         ecx,0C1B340h
// 0x55A498
        call Ui8_Bridge_006A0050
// 0x55A49D
        ret
Ui8_FindCriminalRatingString_55A49E:
        cmp         eax,514h
// 0x55A4A3
        jge Ui8_FindCriminalRatingString_55A4D3
// 0x55A4A5
        fild        dword ptr ds:[00B79034h]
// 0x55A4AB
        mov         ecx,0C1B340h
// 0x55A4B0
        fcomp       dword ptr ds:[00858B50h]
// 0x55A4B6
        fnstsw      ax
// 0x55A4B8
        test        ah,41h
// 0x55A4BB
        jne Ui8_FindCriminalRatingString_55A4C8
// 0x55A4BD
        push        864094h
// 0x55A4C2
        call Ui8_Bridge_006A0050
// 0x55A4C7
        ret
Ui8_FindCriminalRatingString_55A4C8:
        push        86409Ch
// 0x55A4CD
        call Ui8_Bridge_006A0050
// 0x55A4D2
        ret
Ui8_FindCriminalRatingString_55A4D3:
        cmp         eax,5DCh
// 0x55A4D8
        jge Ui8_FindCriminalRatingString_55A4EA
// 0x55A4DA
        push        86408Ch
// 0x55A4DF
        mov         ecx,0C1B340h
// 0x55A4E4
        call Ui8_Bridge_006A0050
// 0x55A4E9
        ret
Ui8_FindCriminalRatingString_55A4EA:
        cmp         eax,6A4h
// 0x55A4EF
        jge Ui8_FindCriminalRatingString_55A501
// 0x55A4F1
        push        864084h
// 0x55A4F6
        mov         ecx,0C1B340h
// 0x55A4FB
        call Ui8_Bridge_006A0050
// 0x55A500
        ret
Ui8_FindCriminalRatingString_55A501:
        cmp         eax,7D0h
// 0x55A506
        jge Ui8_FindCriminalRatingString_55A518
// 0x55A508
        push        86407Ch
// 0x55A50D
        mov         ecx,0C1B340h
// 0x55A512
        call Ui8_Bridge_006A0050
// 0x55A517
        ret
Ui8_FindCriminalRatingString_55A518:
        cmp         eax,834h
// 0x55A51D
        jge Ui8_FindCriminalRatingString_55A52F
// 0x55A51F
        push        864074h
// 0x55A524
        mov         ecx,0C1B340h
// 0x55A529
        call Ui8_Bridge_006A0050
// 0x55A52E
        ret
Ui8_FindCriminalRatingString_55A52F:
        cmp         eax,8FCh
// 0x55A534
        jge Ui8_FindCriminalRatingString_55A546
// 0x55A536
        push        86406Ch
// 0x55A53B
        mov         ecx,0C1B340h
// 0x55A540
        call Ui8_Bridge_006A0050
// 0x55A545
        ret
Ui8_FindCriminalRatingString_55A546:
        cmp         eax,9C4h
// 0x55A54B
        jge Ui8_FindCriminalRatingString_55A55D
// 0x55A54D
        push        864064h
// 0x55A552
        mov         ecx,0C1B340h
// 0x55A557
        call Ui8_Bridge_006A0050
// 0x55A55C
        ret
Ui8_FindCriminalRatingString_55A55D:
        cmp         eax,0ABEh
// 0x55A562
        jge Ui8_FindCriminalRatingString_55A574
// 0x55A564
        push        86405Ch
// 0x55A569
        mov         ecx,0C1B340h
// 0x55A56E
        call Ui8_Bridge_006A0050
// 0x55A573
        ret
Ui8_FindCriminalRatingString_55A574:
        cmp         eax,0BB8h
// 0x55A579
        jge Ui8_FindCriminalRatingString_55A58B
// 0x55A57B
        push        864054h
// 0x55A580
        mov         ecx,0C1B340h
// 0x55A585
        call Ui8_Bridge_006A0050
// 0x55A58A
        ret
Ui8_FindCriminalRatingString_55A58B:
        cmp         eax,0DACh
// 0x55A590
        jge Ui8_FindCriminalRatingString_55A5A2
// 0x55A592
        push        86404Ch
// 0x55A597
        mov         ecx,0C1B340h
// 0x55A59C
        call Ui8_Bridge_006A0050
// 0x55A5A1
        ret
Ui8_FindCriminalRatingString_55A5A2:
        cmp         eax,0FA0h
// 0x55A5A7
        jge Ui8_FindCriminalRatingString_55A5B9
// 0x55A5A9
        push        864044h
// 0x55A5AE
        mov         ecx,0C1B340h
// 0x55A5B3
        call Ui8_Bridge_006A0050
// 0x55A5B8
        ret
Ui8_FindCriminalRatingString_55A5B9:
        cmp         eax,1388h
// 0x55A5BE
        jge Ui8_FindCriminalRatingString_55A5D0
// 0x55A5C0
        push        86403Ch
// 0x55A5C5
        mov         ecx,0C1B340h
// 0x55A5CA
        call Ui8_Bridge_006A0050
// 0x55A5CF
        ret
Ui8_FindCriminalRatingString_55A5D0:
        cmp         eax,1D4Ch
// 0x55A5D5
        jge Ui8_FindCriminalRatingString_55A5E7
// 0x55A5D7
        push        864034h
// 0x55A5DC
        mov         ecx,0C1B340h
// 0x55A5E1
        call Ui8_Bridge_006A0050
// 0x55A5E6
        ret
Ui8_FindCriminalRatingString_55A5E7:
        cmp         eax,2710h
// 0x55A5EC
        jge Ui8_FindCriminalRatingString_55A5FE
// 0x55A5EE
        push        86402Ch
// 0x55A5F3
        mov         ecx,0C1B340h
// 0x55A5F8
        call Ui8_Bridge_006A0050
// 0x55A5FD
        ret
Ui8_FindCriminalRatingString_55A5FE:
        cmp         eax,4E20h
// 0x55A603
        jge Ui8_FindCriminalRatingString_55A615
// 0x55A605
        push        864024h
// 0x55A60A
        mov         ecx,0C1B340h
// 0x55A60F
        call Ui8_Bridge_006A0050
// 0x55A614
        ret
Ui8_FindCriminalRatingString_55A615:
        cmp         eax,7530h
// 0x55A61A
        jge Ui8_FindCriminalRatingString_55A62C
// 0x55A61C
        push        86401Ch
// 0x55A621
        mov         ecx,0C1B340h
// 0x55A626
        call Ui8_Bridge_006A0050
// 0x55A62B
        ret
Ui8_FindCriminalRatingString_55A62C:
        cmp         eax,9C40h
// 0x55A631
        jge Ui8_FindCriminalRatingString_55A643
// 0x55A633
        push        864014h
// 0x55A638
        mov         ecx,0C1B340h
// 0x55A63D
        call Ui8_Bridge_006A0050
// 0x55A642
        ret
Ui8_FindCriminalRatingString_55A643:
        cmp         eax,0C350h
// 0x55A648
        jge Ui8_FindCriminalRatingString_55A65A
// 0x55A64A
        push        86400Ch
// 0x55A64F
        mov         ecx,0C1B340h
// 0x55A654
        call Ui8_Bridge_006A0050
// 0x55A659
        ret
Ui8_FindCriminalRatingString_55A65A:
        cmp         eax,0FDE8h
// 0x55A65F
        jge Ui8_FindCriminalRatingString_55A671
// 0x55A661
        push        864004h
// 0x55A666
        mov         ecx,0C1B340h
// 0x55A66B
        call Ui8_Bridge_006A0050
// 0x55A670
        ret
Ui8_FindCriminalRatingString_55A671:
        cmp         eax,13880h
// 0x55A676
        jge Ui8_FindCriminalRatingString_55A688
// 0x55A678
        push        863FFCh
// 0x55A67D
        mov         ecx,0C1B340h
// 0x55A682
        call Ui8_Bridge_006A0050
// 0x55A687
        ret
Ui8_FindCriminalRatingString_55A688:
        cmp         eax,186A0h
// 0x55A68D
        jge Ui8_FindCriminalRatingString_55A69F
// 0x55A68F
        push        863FF4h
// 0x55A694
        mov         ecx,0C1B340h
// 0x55A699
        call Ui8_Bridge_006A0050
// 0x55A69E
        ret
Ui8_FindCriminalRatingString_55A69F:
        cmp         eax,249F0h
// 0x55A6A4
        jge Ui8_FindCriminalRatingString_55A6B6
// 0x55A6A6
        push        863FECh
// 0x55A6AB
        mov         ecx,0C1B340h
// 0x55A6B0
        call Ui8_Bridge_006A0050
// 0x55A6B5
        ret
Ui8_FindCriminalRatingString_55A6B6:
        cmp         eax,30D40h
// 0x55A6BB
        jge Ui8_FindCriminalRatingString_55A6CD
// 0x55A6BD
        push        863FE4h
// 0x55A6C2
        mov         ecx,0C1B340h
// 0x55A6C7
        call Ui8_Bridge_006A0050
// 0x55A6CC
        ret
Ui8_FindCriminalRatingString_55A6CD:
        cmp         eax,493E0h
// 0x55A6D2
        jge Ui8_FindCriminalRatingString_55A6E4
// 0x55A6D4
        push        863FDCh
// 0x55A6D9
        mov         ecx,0C1B340h
// 0x55A6DE
        call Ui8_Bridge_006A0050
// 0x55A6E3
        ret
Ui8_FindCriminalRatingString_55A6E4:
        cmp         eax,5B8D8h
// 0x55A6E9
        jge Ui8_FindCriminalRatingString_55A6FB
// 0x55A6EB
        push        863FD4h
// 0x55A6F0
        mov         ecx,0C1B340h
// 0x55A6F5
        call Ui8_Bridge_006A0050
// 0x55A6FA
        ret
Ui8_FindCriminalRatingString_55A6FB:
        cmp         eax,7A120h
// 0x55A700
        jge Ui8_FindCriminalRatingString_55A739
// 0x55A702
        fild        dword ptr ds:[00B790C4h]
// 0x55A708
        fmul        dword ptr ds:[00863FD0h]
// 0x55A70E
        fmul        dword ptr ds:[00859044h]
// 0x55A714
        call Ui8_Bridge_00821B40
// 0x55A719
        cmp         eax,0Ah
// 0x55A71C
        mov         ecx,0C1B340h
// 0x55A721
        jle Ui8_FindCriminalRatingString_55A72E
// 0x55A723
        push        863FC8h
// 0x55A728
        call Ui8_Bridge_006A0050
// 0x55A72D
        ret
Ui8_FindCriminalRatingString_55A72E:
        push        863FD4h
// 0x55A733
        call Ui8_Bridge_006A0050
// 0x55A738
        ret
Ui8_FindCriminalRatingString_55A739:
        cmp         eax,0F4240h
// 0x55A73E
        jl Ui8_FindCriminalRatingString_55A769
// 0x55A740
        movzx       eax,byte ptr ds:[00B7CD74h]
// 0x55A747
        imul        eax,eax,190h
// 0x55A74D
        cmp         dword ptr [eax+00B7CE54h],989680h
// 0x55A757
        jle Ui8_FindCriminalRatingString_55A769
// 0x55A759
        push        863FC0h
// 0x55A75E
        mov         ecx,0C1B340h
// 0x55A763
        call Ui8_Bridge_006A0050
// 0x55A768
        ret
Ui8_FindCriminalRatingString_55A769:
        push        863FB8h
// 0x55A76E
        mov         ecx,0C1B340h
// 0x55A773
        call Ui8_Bridge_006A0050
// 0x55A778
        ret
    }
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

namespace {
__declspec(naked) void VehicleStats10Increment() { __asm { push 00055C180h } __asm { ret } }
__declspec(naked) void VehicleStats10Display() { __asm { push 00055B980h } __asm { ret } }
__declspec(naked) void VehicleStats10Ftol() { __asm { push 000821B40h } __asm { ret } }
__declspec(naked) void VehicleStats10CheckMessage() { __asm { push 000559760h } __asm { ret } }
__declspec(naked) void VehicleStats10FatMuscle() { __asm { push 00055C470h } __asm { ret } }
__declspec(naked) void VehicleStats10SkillIndex() { __asm { push 000743CD0h } __asm { ret } }
__declspec(naked) void VehicleStats10Coop() { __asm { push 000441390h } __asm { ret } }
}

__declspec(naked) void CStats::UpdateFatAndMuscleStats(uint32 value) {
    __asm {
        // 0x55C470
        push ecx
        // 0x55C471
        mov eax,dword ptr ds:[000B794FCh]
        // 0x55C476
        fild dword ptr ds:[000B794FCh]
        // 0x55C47C
        test eax,eax
        // 0x55C47E
        jge VehicleStats10_UpdateFatAndMuscleStats_55C486
        // 0x55C480
        fadd dword ptr ds:[000858C54h]
VehicleStats10_UpdateFatAndMuscleStats_55C486:
        fld dword ptr ds:[000B78FB8h]
        // 0x55C48C
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C492
        fcompp
        // 0x55C494
        fnstsw ax
        // 0x55C496
        test ah,5
        // 0x55C499
        jp VehicleStats10_UpdateFatAndMuscleStats_55C5B1
        // 0x55C49F
        fld dword ptr ds:[000B793D4h]
        // 0x55C4A5
        mov dword ptr ds:[000B794FCh],0
        // 0x55C4AF
        fcomp dword ptr ds:[000858B50h]
        // 0x55C4B5
        fnstsw ax
        // 0x55C4B7
        test ah,041h
        // 0x55C4BA
        jne VehicleStats10_UpdateFatAndMuscleStats_55C556
        // 0x55C4C0
        fld dword ptr ds:[000B78F6Ch]
        // 0x55C4C6
        fcomp dword ptr ds:[000858B50h]
        // 0x55C4CC
        fnstsw ax
        // 0x55C4CE
        test ah,041h
        // 0x55C4D1
        jne VehicleStats10_UpdateFatAndMuscleStats_55C4FF
        // 0x55C4D3
        fld dword ptr ds:[000B793D4h]
        // 0x55C4D9
        fsub dword ptr ds:[000B78F6Ch]
        // 0x55C4DF
        fcom dword ptr ds:[000858B50h]
        // 0x55C4E5
        fnstsw ax
        // 0x55C4E7
        test ah,041h
        // 0x55C4EA
        je VehicleStats10_UpdateFatAndMuscleStats_55C4F4
        // 0x55C4EC
        fstp st(0)
        // 0x55C4EE
        fld dword ptr ds:[000858B50h]
VehicleStats10_UpdateFatAndMuscleStats_55C4F4:
        fstp dword ptr ds:[000B793D4h]
        // 0x55C4FA
        call VehicleStats10CheckMessage
VehicleStats10_UpdateFatAndMuscleStats_55C4FF:
        mov ecx,dword ptr ds:[000B78F34h]
        // 0x55C505
        push ecx
        // 0x55C506
        push 017h
        // 0x55C508
        call VehicleStats10Increment
        // 0x55C50D
        mov al,byte ptr ds:[000B79534h]
        // 0x55C512
        add esp,8
        // 0x55C515
        cmp al,1
        // 0x55C517
        jne VehicleStats10_UpdateFatAndMuscleStats_55C537
        // 0x55C519
        mov eax,dword ptr ds:[000B78F34h]
        // 0x55C51E
        push eax
        // 0x55C51F
        push 017h
        // 0x55C521
        push 1
        // 0x55C523
        mov byte ptr ds:[000B79534h],2
        // 0x55C52A
        call VehicleStats10Display
        // 0x55C52F
        add esp,00Ch
        // 0x55C532
        jmp VehicleStats10_UpdateFatAndMuscleStats_55C5E5
VehicleStats10_UpdateFatAndMuscleStats_55C537:
        mov edx,dword ptr ds:[000B78F6Ch]
        // 0x55C53D
        push edx
        // 0x55C53E
        push 015h
        // 0x55C540
        push 0
        // 0x55C542
        mov byte ptr ds:[000B79534h],1
        // 0x55C549
        call VehicleStats10Display
        // 0x55C54E
        add esp,00Ch
        // 0x55C551
        jmp VehicleStats10_UpdateFatAndMuscleStats_55C5E5
VehicleStats10_UpdateFatAndMuscleStats_55C556:
        fld dword ptr ds:[000B78F70h]
        // 0x55C55C
        fcomp dword ptr ds:[000858B50h]
        // 0x55C562
        fnstsw ax
        // 0x55C564
        test ah,041h
        // 0x55C567
        jne VehicleStats10_UpdateFatAndMuscleStats_55C595
        // 0x55C569
        fld dword ptr ds:[000B793DCh]
        // 0x55C56F
        fsub dword ptr ds:[000B78F70h]
        // 0x55C575
        fcom dword ptr ds:[000858B50h]
        // 0x55C57B
        fnstsw ax
        // 0x55C57D
        test ah,041h
        // 0x55C580
        je VehicleStats10_UpdateFatAndMuscleStats_55C58A
        // 0x55C582
        fstp st(0)
        // 0x55C584
        fld dword ptr ds:[000858B50h]
VehicleStats10_UpdateFatAndMuscleStats_55C58A:
        fstp dword ptr ds:[000B793DCh]
        // 0x55C590
        call VehicleStats10CheckMessage
VehicleStats10_UpdateFatAndMuscleStats_55C595:
        mov ecx,dword ptr ds:[000B78F70h]
        // 0x55C59B
        push ecx
        // 0x55C59C
        push 017h
        // 0x55C59E
        push 0
        // 0x55C5A0
        mov byte ptr ds:[000B79534h],3
        // 0x55C5A7
        call VehicleStats10Display
        // 0x55C5AC
        add esp,00Ch
        // 0x55C5AF
        jmp VehicleStats10_UpdateFatAndMuscleStats_55C5E5
VehicleStats10_UpdateFatAndMuscleStats_55C5B1:
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55C5B7
        fmul dword ptr ds:[000858B38h]
        // 0x55C5BD
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C5C3
        call VehicleStats10Ftol
        // 0x55C5C8
        mov edx,eax
        // 0x55C5CA
        imul edx,dword ptr [esp+8]
        // 0x55C5CF
        mov eax,00CCCCCCCDh
        // 0x55C5D4
        mul edx
        // 0x55C5D6
        mov eax,dword ptr ds:[000B794FCh]
        // 0x55C5DB
        shr edx,3
        // 0x55C5DE
        add eax,edx
        // 0x55C5E0
        mov dword ptr ds:[000B794FCh],eax
VehicleStats10_UpdateFatAndMuscleStats_55C5E5:
        mov eax,dword ptr ds:[000B7950Ch]
        // 0x55C5EA
        fild dword ptr ds:[000B7950Ch]
        // 0x55C5F0
        test eax,eax
        // 0x55C5F2
        jge VehicleStats10_UpdateFatAndMuscleStats_55C5FA
        // 0x55C5F4
        fadd dword ptr ds:[000858C54h]
VehicleStats10_UpdateFatAndMuscleStats_55C5FA:
        fld dword ptr ds:[000B78FC4h]
        // 0x55C600
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C606
        fcompp
        // 0x55C608
        fnstsw ax
        // 0x55C60A
        test ah,5
        // 0x55C60D
        jp VehicleStats10_UpdateFatAndMuscleStats_55C63C
        // 0x55C60F
        mov ecx,dword ptr ds:[000B78F3Ch]
        // 0x55C615
        push ecx
        // 0x55C616
        push 018h
        // 0x55C618
        call VehicleStats10Increment
        // 0x55C61D
        mov edx,dword ptr ds:[000B78F3Ch]
        // 0x55C623
        push edx
        // 0x55C624
        push 018h
        // 0x55C626
        push 1
        // 0x55C628
        call VehicleStats10Display
        // 0x55C62D
        add esp,014h
        // 0x55C630
        mov dword ptr ds:[000B7950Ch],0
        // 0x55C63A
        pop ecx
        // 0x55C63B
        ret
VehicleStats10_UpdateFatAndMuscleStats_55C63C:
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55C642
        fmul dword ptr ds:[000858B38h]
        // 0x55C648
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C64E
        call VehicleStats10Ftol
        // 0x55C653
        add dword ptr ds:[000B7950Ch],eax
        // 0x55C659
        pop ecx
        // 0x55C65A
        ret
    }
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
__declspec(naked) void VehicleStats11Fat() { __asm { push 00055C470h } __asm { ret } }
__declspec(naked) void VehicleStats11Increment() { __asm { push 00055C180h } __asm { ret } }
__declspec(naked) void VehicleStats11Display() { __asm { push 00055B980h } __asm { ret } }
__declspec(naked) void VehicleStats11Ftol() { __asm { push 000821B40h } __asm { ret } }
__declspec(naked) void VehicleStats11Ceil() { __asm { push 000823820h } __asm { ret } }
}

__declspec(naked) void CStats::UpdateStatsWhenCycling(bool arg0, CBmx* bmx) {
    __asm {
        // 0x55C780
        push ebx
        // 0x55C781
        mov bl,byte ptr [esp+8]
        // 0x55C785
        push esi
        // 0x55C786
        mov esi,dword ptr [esp+010h]
        // 0x55C78A
        fld dword ptr [esi+00000049Ch]
        // 0x55C790
        fabs
        // 0x55C792
        fcomp dword ptr ds:[000858B50h]
        // 0x55C798
        fnstsw ax
        // 0x55C79A
        test ah,041h
        // 0x55C79D
        je VehicleStats11_Cycling_55C7AF
        // 0x55C79F
        test bl,bl
        // 0x55C7A1
        je VehicleStats11_Cycling_55C843
        // 0x55C7A7
        fld dword ptr ds:[000B78F7Ch]
        // 0x55C7AD
        jmp VehicleStats11_Cycling_55C7C1
VehicleStats11_Cycling_55C7AF:
        test bl,bl
        // 0x55C7B1
        je VehicleStats11_Cycling_55C7BB
        // 0x55C7B3
        fld dword ptr ds:[000B78F7Ch]
        // 0x55C7B9
        jmp VehicleStats11_Cycling_55C7C1
VehicleStats11_Cycling_55C7BB:
        fld dword ptr ds:[000B78F78h]
VehicleStats11_Cycling_55C7C1:
        call VehicleStats11Ftol
        // 0x55C7C6
        push eax
        // 0x55C7C7
        call VehicleStats11Fat
        // 0x55C7CC
        fild dword ptr ds:[000B794DCh]
        // 0x55C7D2
        mov eax,dword ptr ds:[000B794DCh]
        // 0x55C7D7
        add esp,4
        // 0x55C7DA
        test eax,eax
        // 0x55C7DC
        jge VehicleStats11_Cycling_55C7E4
        // 0x55C7DE
        fadd dword ptr ds:[000858C54h]
VehicleStats11_Cycling_55C7E4:
        fld dword ptr ds:[000B78F94h]
        // 0x55C7EA
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C7F0
        fcompp
        // 0x55C7F2
        fnstsw ax
        // 0x55C7F4
        test ah,5
        // 0x55C7F7
        jp VehicleStats11_Cycling_55C826
        // 0x55C7F9
        mov ecx,dword ptr ds:[000B78F10h]
        // 0x55C7FF
        push ecx
        // 0x55C800
        push 016h
        // 0x55C802
        mov dword ptr ds:[000B794DCh],0
        // 0x55C80C
        call VehicleStats11Increment
        // 0x55C811
        mov edx,dword ptr ds:[000B78F10h]
        // 0x55C817
        push edx
        // 0x55C818
        push 016h
        // 0x55C81A
        push 1
        // 0x55C81C
        call VehicleStats11Display
        // 0x55C821
        add esp,014h
        // 0x55C824
        jmp VehicleStats11_Cycling_55C843
VehicleStats11_Cycling_55C826:
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55C82C
        fmul dword ptr ds:[000858B38h]
        // 0x55C832
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C838
        call VehicleStats11Ftol
        // 0x55C83D
        add dword ptr ds:[000B794DCh],eax
VehicleStats11_Cycling_55C843:
        mov eax,dword ptr ds:[000B794E0h]
        // 0x55C848
        fild dword ptr ds:[000B794E0h]
        // 0x55C84E
        test eax,eax
        // 0x55C850
        jge VehicleStats11_Cycling_55C858
        // 0x55C852
        fadd dword ptr ds:[000858C54h]
VehicleStats11_Cycling_55C858:
        fld dword ptr ds:[000B78FACh]
        // 0x55C85E
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C864
        fcompp
        // 0x55C866
        fnstsw ax
        // 0x55C868
        test ah,5
        // 0x55C86B
        jp VehicleStats11_Cycling_55C8A1
        // 0x55C86D
        mov ecx,dword ptr ds:[000B78F28h]
        // 0x55C873
        push ecx
        // 0x55C874
        push 00E6h
        // 0x55C879
        mov dword ptr ds:[000B794E0h],0
        // 0x55C883
        call VehicleStats11Increment
        // 0x55C888
        mov edx,dword ptr ds:[000B78F28h]
        // 0x55C88E
        push edx
        // 0x55C88F
        push 00E6h
        // 0x55C894
        push 1
        // 0x55C896
        call VehicleStats11Display
        // 0x55C89B
        add esp,014h
        // 0x55C89E
        pop esi
        // 0x55C89F
        pop ebx
        // 0x55C8A0
        ret
VehicleStats11_Cycling_55C8A1:
        cmp byte ptr [esi+000000804h],2
        // 0x55C8A8
        jae VehicleStats11_Cycling_55C8E6
        // 0x55C8AA
        fld dword ptr [esi+04Ch]
        // 0x55C8AD
        fld dword ptr [esi+048h]
        // 0x55C8B0
        fld dword ptr [esi+044h]
        // 0x55C8B3
        fld st(0)
        // 0x55C8B5
        fmul st,st(1)
        // 0x55C8B7
        fld st(2)
        // 0x55C8B9
        fmul st,st(3)
        // 0x55C8BB
        faddp st(1),st
        // 0x55C8BD
        fld st(3)
        // 0x55C8BF
        fmul st,st(4)
        // 0x55C8C1
        faddp st(1),st
        // 0x55C8C3
        fld dword ptr ds:[000863E58h]
        // 0x55C8C9
        fmul dword ptr ds:[000863E58h]
        // 0x55C8CF
        fcompp
        // 0x55C8D1
        fstp st(0)
        // 0x55C8D3
        fnstsw ax
        // 0x55C8D5
        fstp st(0)
        // 0x55C8D7
        test ah,5
        // 0x55C8DA
        fstp st(0)
        // 0x55C8DC
        jp VehicleStats11_Cycling_55C8E6
        // 0x55C8DE
        fld dword ptr ds:[000863E64h]
        // 0x55C8E4
        jmp VehicleStats11_Cycling_55C92C
VehicleStats11_Cycling_55C8E6:
        fld dword ptr [esi+04Ch]
        // 0x55C8E9
        fld dword ptr [esi+048h]
        // 0x55C8EC
        fld dword ptr [esi+044h]
        // 0x55C8EF
        fld st(0)
        // 0x55C8F1
        fmul st,st(1)
        // 0x55C8F3
        fld st(2)
        // 0x55C8F5
        fmul st,st(3)
        // 0x55C8F7
        faddp st(1),st
        // 0x55C8F9
        fld st(3)
        // 0x55C8FB
        fmul st,st(4)
        // 0x55C8FD
        faddp st(1),st
        // 0x55C8FF
        fld dword ptr ds:[000863E54h]
        // 0x55C905
        fmul dword ptr ds:[000863E54h]
        // 0x55C90B
        fcompp
        // 0x55C90D
        fstp st(0)
        // 0x55C90F
        fnstsw ax
        // 0x55C911
        fstp st(0)
        // 0x55C913
        test ah,5
        // 0x55C916
        fstp st(0)
        // 0x55C918
        jp VehicleStats11_Cycling_55C98A
        // 0x55C91A
        test bl,bl
        // 0x55C91C
        je VehicleStats11_Cycling_55C926
        // 0x55C91E
        fld dword ptr ds:[000863E60h]
        // 0x55C924
        jmp VehicleStats11_Cycling_55C92C
VehicleStats11_Cycling_55C926:
        fld dword ptr ds:[000863E5Ch]
VehicleStats11_Cycling_55C92C:
        fcom dword ptr ds:[000858B50h]
        // 0x55C932
        fnstsw ax
        // 0x55C934
        test ah,041h
        // 0x55C937
        jne VehicleStats11_Cycling_55C988
        // 0x55C939
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55C93F
        fmul dword ptr ds:[000858B38h]
        // 0x55C945
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C94B
        call VehicleStats11Ftol
        // 0x55C950
        test eax,eax
        // 0x55C952
        mov dword ptr [esp+010h],eax
        // 0x55C956
        fild dword ptr [esp+010h]
        // 0x55C95A
        jge VehicleStats11_Cycling_55C962
        // 0x55C95C
        fadd dword ptr ds:[000858C54h]
VehicleStats11_Cycling_55C962:
        fmulp st(1),st
        // 0x55C964
        sub esp,8
        // 0x55C967
        fstp qword ptr [esp]
        // 0x55C96A
        call VehicleStats11Ceil
        // 0x55C96F
        add esp,8
        // 0x55C972
        call VehicleStats11Ftol
        // 0x55C977
        mov ecx,dword ptr ds:[000B794E0h]
        // 0x55C97D
        add ecx,eax
        // 0x55C97F
        pop esi
        // 0x55C980
        mov dword ptr ds:[000B794E0h],ecx
        // 0x55C986
        pop ebx
        // 0x55C987
        ret
VehicleStats11_Cycling_55C988:
        fstp st(0)
VehicleStats11_Cycling_55C98A:
        pop esi
        // 0x55C98B
        pop ebx
        // 0x55C98C
        ret
    }
}

// 0x55C990
__declspec(naked) void CStats::UpdateStatsWhenSwimming(bool arg0, bool arg1) {
    __asm {
        // 0x55C990
        push ecx
        // 0x55C991
        mov al,byte ptr [esp+8]
        // 0x55C995
        test al,al
        // 0x55C997
        jne VehicleStats10_UpdateStatsWhenSwimming_55C9A9
        // 0x55C999
        mov al,byte ptr [esp+00Ch]
        // 0x55C99D
        test al,al
        // 0x55C99F
        jne VehicleStats10_UpdateStatsWhenSwimming_55C9A9
        // 0x55C9A1
        fld dword ptr ds:[000B78F80h]
        // 0x55C9A7
        jmp VehicleStats10_UpdateStatsWhenSwimming_55C9AF
VehicleStats10_UpdateStatsWhenSwimming_55C9A9:
        fld dword ptr ds:[000B78F84h]
VehicleStats10_UpdateStatsWhenSwimming_55C9AF:
        call VehicleStats10Ftol
        // 0x55C9B4
        push eax
        // 0x55C9B5
        call VehicleStats10FatMuscle
        // 0x55C9BA
        fild dword ptr ds:[000B794E4h]
        // 0x55C9C0
        mov eax,dword ptr ds:[000B794E4h]
        // 0x55C9C5
        add esp,4
        // 0x55C9C8
        test eax,eax
        // 0x55C9CA
        jge VehicleStats10_UpdateStatsWhenSwimming_55C9D2
        // 0x55C9CC
        fadd dword ptr ds:[000858C54h]
VehicleStats10_UpdateStatsWhenSwimming_55C9D2:
        fld dword ptr ds:[000B78F98h]
        // 0x55C9D8
        fmul dword ptr ds:[000858C4Ch]
        // 0x55C9DE
        fcompp
        // 0x55C9E0
        fnstsw ax
        // 0x55C9E2
        test ah,5
        // 0x55C9E5
        jp VehicleStats10_UpdateStatsWhenSwimming_55CA14
        // 0x55C9E7
        mov ecx,dword ptr ds:[000B78F14h]
        // 0x55C9ED
        push ecx
        // 0x55C9EE
        push 016h
        // 0x55C9F0
        mov dword ptr ds:[000B794E4h],0
        // 0x55C9FA
        call VehicleStats10Increment
        // 0x55C9FF
        mov edx,dword ptr ds:[000B78F14h]
        // 0x55CA05
        push edx
        // 0x55CA06
        push 016h
        // 0x55CA08
        push 1
        // 0x55CA0A
        call VehicleStats10Display
        // 0x55CA0F
        add esp,014h
        // 0x55CA12
        jmp VehicleStats10_UpdateStatsWhenSwimming_55CA31
VehicleStats10_UpdateStatsWhenSwimming_55CA14:
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55CA1A
        fmul dword ptr ds:[000858B38h]
        // 0x55CA20
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CA26
        call VehicleStats10Ftol
        // 0x55CA2B
        add dword ptr ds:[000B794E4h],eax
VehicleStats10_UpdateStatsWhenSwimming_55CA31:
        mov al,byte ptr [esp+8]
        // 0x55CA35
        test al,al
        // 0x55CA37
        je VehicleStats10_UpdateStatsWhenSwimming_55CAB3
        // 0x55CA39
        mov eax,dword ptr ds:[000B794E8h]
        // 0x55CA3E
        fild dword ptr ds:[000B794E8h]
        // 0x55CA44
        test eax,eax
        // 0x55CA46
        jge VehicleStats10_UpdateStatsWhenSwimming_55CA4E
        // 0x55CA48
        fadd dword ptr ds:[000858C54h]
VehicleStats10_UpdateStatsWhenSwimming_55CA4E:
        fld dword ptr ds:[000B78FC0h]
        // 0x55CA54
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CA5A
        fcompp
        // 0x55CA5C
        fnstsw ax
        // 0x55CA5E
        test ah,5
        // 0x55CA61
        jp VehicleStats10_UpdateStatsWhenSwimming_55CA96
        // 0x55CA63
        mov ecx,dword ptr ds:[000B78F38h]
        // 0x55CA69
        push ecx
        // 0x55CA6A
        push 00E1h
        // 0x55CA6F
        mov dword ptr ds:[000B794E8h],0
        // 0x55CA79
        call VehicleStats10Increment
        // 0x55CA7E
        mov edx,dword ptr ds:[000B78F38h]
        // 0x55CA84
        push edx
        // 0x55CA85
        push 00E1h
        // 0x55CA8A
        push 1
        // 0x55CA8C
        call VehicleStats10Display
        // 0x55CA91
        add esp,014h
        // 0x55CA94
        pop ecx
        // 0x55CA95
        ret
VehicleStats10_UpdateStatsWhenSwimming_55CA96:
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55CA9C
        fmul dword ptr ds:[000858B38h]
        // 0x55CAA2
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CAA8
        call VehicleStats10Ftol
        // 0x55CAAD
        add dword ptr ds:[000B794E8h],eax
VehicleStats10_UpdateStatsWhenSwimming_55CAB3:
        pop ecx
        // 0x55CAB4
        ret
    }
}

// 0x55CAC0

namespace {
__declspec(naked) void VehicleStats9Increment() { __asm { push 00055C180h } __asm { ret } }
__declspec(naked) void VehicleStats9Display() { __asm { push 00055B980h } __asm { ret } }
__declspec(naked) void VehicleStats9Ftol() { __asm { push 000821B40h } __asm { ret } }
}

__declspec(naked) void CStats::UpdateStatsWhenDriving(CVehicle* vehicle) {
    __asm {
        // 0x55CAC0
        push ecx
        // 0x55CAC1
        mov edx,dword ptr [esp+8]
        // 0x55CAC5
        mov eax,dword ptr [edx+000000590h]
        // 0x55CACB
        xor ecx,ecx
        // 0x55CACD
        test eax,eax
        // 0x55CACF
        jne VehicleStats9_UpdateStatsWhenDriving_55CAD3
        // 0x55CAD1
        mov ecx,edx
VehicleStats9_UpdateStatsWhenDriving_55CAD3:
        mov eax,dword ptr ds:[000B794ECh]
        // 0x55CAD8
        fild dword ptr ds:[000B794ECh]
        // 0x55CADE
        test eax,eax
        // 0x55CAE0
        jge VehicleStats9_UpdateStatsWhenDriving_55CAE8
        // 0x55CAE2
        fadd dword ptr ds:[000858C54h]
VehicleStats9_UpdateStatsWhenDriving_55CAE8:
        fstp dword ptr [esp+8]
        // 0x55CAEC
        fld dword ptr ds:[000B78FA4h]
        // 0x55CAF2
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CAF8
        fcomp dword ptr [esp+8]
        // 0x55CAFC
        fnstsw ax
        // 0x55CAFE
        test ah,5
        // 0x55CB01
        jp VehicleStats9_UpdateStatsWhenDriving_55CB36
        // 0x55CB03
        mov ecx,dword ptr ds:[000B78F20h]
        // 0x55CB09
        push ecx
        // 0x55CB0A
        push 00A0h
        // 0x55CB0F
        mov dword ptr ds:[000B794ECh],0
        // 0x55CB19
        call VehicleStats9Increment
        // 0x55CB1E
        mov edx,dword ptr ds:[000B78F20h]
        // 0x55CB24
        push edx
        // 0x55CB25
        push 00A0h
        // 0x55CB2A
        push 1
        // 0x55CB2C
        call VehicleStats9Display
        // 0x55CB31
        add esp,014h
        // 0x55CB34
        pop ecx
        // 0x55CB35
        ret
VehicleStats9_UpdateStatsWhenDriving_55CB36:
        fld dword ptr [edx+04Ch]
        // 0x55CB39
        fld dword ptr [edx+048h]
        // 0x55CB3C
        fld dword ptr [edx+044h]
        // 0x55CB3F
        fld st(0)
        // 0x55CB41
        fmul st,st(1)
        // 0x55CB43
        fld st(2)
        // 0x55CB45
        fmul st,st(3)
        // 0x55CB47
        faddp st(1),st
        // 0x55CB49
        fld st(3)
        // 0x55CB4B
        fmul st,st(4)
        // 0x55CB4D
        faddp st(1),st
        // 0x55CB4F
        fsqrt
        // 0x55CB51
        fstp st(3)
        // 0x55CB53
        fstp st(0)
        // 0x55CB55
        fstp st(0)
        // 0x55CB57
        fcom dword ptr ds:[000863E70h]
        // 0x55CB5D
        fnstsw ax
        // 0x55CB5F
        test ah,041h
        // 0x55CB62
        je VehicleStats9_UpdateStatsWhenDriving_55CBBE
        // 0x55CB64
        test ecx,ecx
        // 0x55CB66
        je VehicleStats9_UpdateStatsWhenDriving_55CB72
        // 0x55CB68
        mov al,byte ptr [ecx+000000960h]
        // 0x55CB6E
        test al,al
        // 0x55CB70
        je VehicleStats9_UpdateStatsWhenDriving_55CBBE
VehicleStats9_UpdateStatsWhenDriving_55CB72:
        fcomp dword ptr ds:[000863E68h]
        // 0x55CB78
        fnstsw ax
        // 0x55CB7A
        test ah,041h
        // 0x55CB7D
        jne VehicleStats9_UpdateStatsWhenDriving_55CBFD
        // 0x55CB7F
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55CB85
        fmul dword ptr ds:[000858B38h]
        // 0x55CB8B
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CB91
        call VehicleStats9Ftol
        // 0x55CB96
        test eax,eax
        // 0x55CB98
        mov dword ptr [esp],eax
        // 0x55CB9C
        fild dword ptr [esp]
        // 0x55CBA0
        jge VehicleStats9_UpdateStatsWhenDriving_55CBA8
        // 0x55CBA2
        fadd dword ptr ds:[000858C54h]
VehicleStats9_UpdateStatsWhenDriving_55CBA8:
        fmul dword ptr ds:[000863E6Ch]
        // 0x55CBAE
        fadd dword ptr [esp+8]
        // 0x55CBB2
        call VehicleStats9Ftol
        // 0x55CBB7
        mov dword ptr ds:[000B794ECh],eax
        // 0x55CBBC
        pop ecx
        // 0x55CBBD
        ret
VehicleStats9_UpdateStatsWhenDriving_55CBBE:
        fstp st(0)
        // 0x55CBC0
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55CBC6
        fmul dword ptr ds:[000858B38h]
        // 0x55CBCC
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CBD2
        call VehicleStats9Ftol
        // 0x55CBD7
        test eax,eax
        // 0x55CBD9
        mov dword ptr [esp],eax
        // 0x55CBDD
        fild dword ptr [esp]
        // 0x55CBE1
        jge VehicleStats9_UpdateStatsWhenDriving_55CBE9
        // 0x55CBE3
        fadd dword ptr ds:[000858C54h]
VehicleStats9_UpdateStatsWhenDriving_55CBE9:
        fmul dword ptr ds:[000863E74h]
        // 0x55CBEF
        fadd dword ptr [esp+8]
        // 0x55CBF3
        call VehicleStats9Ftol
        // 0x55CBF8
        mov dword ptr ds:[000B794ECh],eax
VehicleStats9_UpdateStatsWhenDriving_55CBFD:
        pop ecx
        // 0x55CBFE
        ret
    }
}

// 0x55CC00
__declspec(naked) void CStats::UpdateStatsWhenFlying(CVehicle* vehicle) {
    __asm {
        // 0x55CC00
        push ecx
        // 0x55CC01
        mov ecx,dword ptr [esp+8]
        // 0x55CC05
        mov eax,dword ptr [ecx+000000590h]
        // 0x55CC0B
        test eax,eax
        // 0x55CC0D
        jne VehicleStats9_UpdateStatsWhenFlying_55CD50
        // 0x55CC13
        mov eax,dword ptr ds:[000B794F0h]
        // 0x55CC18
        fild dword ptr ds:[000B794F0h]
        // 0x55CC1E
        test eax,eax
        // 0x55CC20
        jge VehicleStats9_UpdateStatsWhenFlying_55CC28
        // 0x55CC22
        fadd dword ptr ds:[000858C54h]
VehicleStats9_UpdateStatsWhenFlying_55CC28:
        fstp dword ptr [esp+8]
        // 0x55CC2C
        fld dword ptr ds:[000B78FA8h]
        // 0x55CC32
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CC38
        fcomp dword ptr [esp+8]
        // 0x55CC3C
        fnstsw ax
        // 0x55CC3E
        test ah,5
        // 0x55CC41
        jp VehicleStats9_UpdateStatsWhenFlying_55CC76
        // 0x55CC43
        mov ecx,dword ptr ds:[000B78F24h]
        // 0x55CC49
        push ecx
        // 0x55CC4A
        push 00DFh
        // 0x55CC4F
        mov dword ptr ds:[000B794F0h],0
        // 0x55CC59
        call VehicleStats9Increment
        // 0x55CC5E
        mov edx,dword ptr ds:[000B78F24h]
        // 0x55CC64
        push edx
        // 0x55CC65
        push 00DFh
        // 0x55CC6A
        push 1
        // 0x55CC6C
        call VehicleStats9Display
        // 0x55CC71
        add esp,014h
        // 0x55CC74
        pop ecx
        // 0x55CC75
        ret
VehicleStats9_UpdateStatsWhenFlying_55CC76:
        mov al,byte ptr [ecx+000000960h]
        // 0x55CC7C
        test al,al
        // 0x55CC7E
        jne VehicleStats9_UpdateStatsWhenFlying_55CD50
        // 0x55CC84
        fld dword ptr [ecx+04Ch]
        // 0x55CC87
        fld dword ptr [ecx+048h]
        // 0x55CC8A
        fld dword ptr [ecx+044h]
        // 0x55CC8D
        fld st(0)
        // 0x55CC8F
        fmul st,st(1)
        // 0x55CC91
        fld st(2)
        // 0x55CC93
        fmul st,st(3)
        // 0x55CC95
        faddp st(1),st
        // 0x55CC97
        fld st(3)
        // 0x55CC99
        fmul st,st(4)
        // 0x55CC9B
        faddp st(1),st
        // 0x55CC9D
        fsqrt
        // 0x55CC9F
        fstp st(3)
        // 0x55CCA1
        fstp st(0)
        // 0x55CCA3
        fstp st(0)
        // 0x55CCA5
        fcom dword ptr ds:[000863E80h]
        // 0x55CCAB
        fnstsw ax
        // 0x55CCAD
        test ah,041h
        // 0x55CCB0
        je VehicleStats9_UpdateStatsWhenFlying_55CD11
        // 0x55CCB2
        mov eax,dword ptr [ecx+014h]
        // 0x55CCB5
        fld dword ptr [eax+028h]
        // 0x55CCB8
        fcomp dword ptr ds:[000858B50h]
        // 0x55CCBE
        fnstsw ax
        // 0x55CCC0
        test ah,5
        // 0x55CCC3
        jnp VehicleStats9_UpdateStatsWhenFlying_55CD11
        // 0x55CCC5
        fcomp dword ptr ds:[000863E78h]
        // 0x55CCCB
        fnstsw ax
        // 0x55CCCD
        test ah,041h
        // 0x55CCD0
        jne VehicleStats9_UpdateStatsWhenFlying_55CD50
        // 0x55CCD2
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55CCD8
        fmul dword ptr ds:[000858B38h]
        // 0x55CCDE
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CCE4
        call VehicleStats9Ftol
        // 0x55CCE9
        test eax,eax
        // 0x55CCEB
        mov dword ptr [esp],eax
        // 0x55CCEF
        fild dword ptr [esp]
        // 0x55CCF3
        jge VehicleStats9_UpdateStatsWhenFlying_55CCFB
        // 0x55CCF5
        fadd dword ptr ds:[000858C54h]
VehicleStats9_UpdateStatsWhenFlying_55CCFB:
        fmul dword ptr ds:[000863E7Ch]
        // 0x55CD01
        fadd dword ptr [esp+8]
        // 0x55CD05
        call VehicleStats9Ftol
        // 0x55CD0A
        mov dword ptr ds:[000B794F0h],eax
        // 0x55CD0F
        pop ecx
        // 0x55CD10
        ret
VehicleStats9_UpdateStatsWhenFlying_55CD11:
        fstp st(0)
        // 0x55CD13
        fld dword ptr ds:[000B7CB5Ch]
        // 0x55CD19
        fmul dword ptr ds:[000858B38h]
        // 0x55CD1F
        fmul dword ptr ds:[000858C4Ch]
        // 0x55CD25
        call VehicleStats9Ftol
        // 0x55CD2A
        test eax,eax
        // 0x55CD2C
        mov dword ptr [esp],eax
        // 0x55CD30
        fild dword ptr [esp]
        // 0x55CD34
        jge VehicleStats9_UpdateStatsWhenFlying_55CD3C
        // 0x55CD36
        fadd dword ptr ds:[000858C54h]
VehicleStats9_UpdateStatsWhenFlying_55CD3C:
        fmul dword ptr ds:[000863E84h]
        // 0x55CD42
        fadd dword ptr [esp+8]
        // 0x55CD46
        call VehicleStats9Ftol
        // 0x55CD4B
        mov dword ptr ds:[000B794F0h],eax
VehicleStats9_UpdateStatsWhenFlying_55CD50:
        pop ecx
        // 0x55CD51
        ret
    }
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
__declspec(naked) void CStats::UpdateStatsWhenWeaponHit(eWeaponType weaponType) {
    __asm {
        // 0x55CEB0
        push ecx
        // 0x55CEB1
        mov eax,dword ptr [esp+8]
        // 0x55CEB5
        push esi
        // 0x55CEB6
        push edi
        // 0x55CEB7
        push eax
        // 0x55CEB8
        call VehicleStats10SkillIndex
        // 0x55CEBD
        mov esi,eax
        // 0x55CEBF
        add esp,4
        // 0x55CEC2
        cmp si,052h
        // 0x55CEC6
        lea edi,[esi-045h]
        // 0x55CEC9
        jae VehicleStats10_UpdateStatsWhenWeaponHit_55CED7
        // 0x55CECB
        movzx ecx,si
        // 0x55CECE
        fld dword ptr [ecx*4+000B79380h]
        // 0x55CED5
        jmp VehicleStats10_UpdateStatsWhenWeaponHit_55CEE1
VehicleStats10_UpdateStatsWhenWeaponHit_55CED7:
        movzx edx,si
        // 0x55CEDA
        fild dword ptr [edx*4+000B78E20h]
VehicleStats10_UpdateStatsWhenWeaponHit_55CEE1:
        fstp dword ptr [esp+8]
        // 0x55CEE5
        call VehicleStats10Coop
        // 0x55CEEA
        test al,al
        // 0x55CEEC
        jne VehicleStats10_UpdateStatsWhenWeaponHit_55CF90
        // 0x55CEF2
        fld dword ptr [esp+8]
        // 0x55CEF6
        fcomp dword ptr ds:[000858C4Ch]
        // 0x55CEFC
        fnstsw ax
        // 0x55CEFE
        test ah,5
        // 0x55CF01
        jp VehicleStats10_UpdateStatsWhenWeaponHit_55CF90
        // 0x55CF07
        mov eax,dword ptr [edi*4+000B78F40h]
        // 0x55CF0E
        push ebx
        // 0x55CF0F
        lea ebx,[edi*4+000B78F40h]
        // 0x55CF16
        push eax
        // 0x55CF17
        push esi
        // 0x55CF18
        call VehicleStats10Increment
        // 0x55CF1D
        mov eax,dword ptr ds:[000B79514h]
        // 0x55CF22
        add esp,8
        // 0x55CF25
        cmp eax,edi
        // 0x55CF27
        je VehicleStats10_UpdateStatsWhenWeaponHit_55CF3E
        // 0x55CF29
        pop ebx
        // 0x55CF2A
        mov dword ptr ds:[000B79514h],edi
        // 0x55CF30
        pop edi
        // 0x55CF31
        mov dword ptr ds:[000B79504h],0
        // 0x55CF3B
        pop esi
        // 0x55CF3C
        pop ecx
        // 0x55CF3D
        ret
VehicleStats10_UpdateStatsWhenWeaponHit_55CF3E:
        mov ecx,dword ptr ds:[000B79504h]
        // 0x55CF44
        fild dword ptr ds:[000B79504h]
        // 0x55CF4A
        test ecx,ecx
        // 0x55CF4C
        jge VehicleStats10_UpdateStatsWhenWeaponHit_55CF54
        // 0x55CF4E
        fadd dword ptr ds:[000858C54h]
VehicleStats10_UpdateStatsWhenWeaponHit_55CF54:
        fcom dword ptr [edi*4+000B78FC8h]
        // 0x55CF5B
        fnstsw ax
        // 0x55CF5D
        test ah,041h
        // 0x55CF60
        jne VehicleStats10_UpdateStatsWhenWeaponHit_55CF82
        // 0x55CF62
        fmul dword ptr [ebx]
        // 0x55CF64
        push ecx
        // 0x55CF65
        fstp dword ptr [esp]
        // 0x55CF68
        push esi
        // 0x55CF69
        push 1
        // 0x55CF6B
        call VehicleStats10Display
        // 0x55CF70
        add esp,00Ch
        // 0x55CF73
        pop ebx
        // 0x55CF74
        pop edi
        // 0x55CF75
        mov dword ptr ds:[000B79504h],0
        // 0x55CF7F
        pop esi
        // 0x55CF80
        pop ecx
        // 0x55CF81
        ret
VehicleStats10_UpdateStatsWhenWeaponHit_55CF82:
        mov eax,dword ptr ds:[000B79504h]
        // 0x55CF87
        fstp st(0)
        // 0x55CF89
        inc eax
        // 0x55CF8A
        mov dword ptr ds:[000B79504h],eax
        // 0x55CF8F
        pop ebx
VehicleStats10_UpdateStatsWhenWeaponHit_55CF90:
        pop edi
        // 0x55CF91
        pop esi
        // 0x55CF92
        pop ecx
        // 0x55CF93
        ret
    }
}

// 0x55CFA0
void CStats::UpdateStatsWhenFighting() {
    UpdateFatAndMuscleStats(static_cast<uint32>(StatsFtol(StatReactionValue[STAT_EXERCISE_RATE_FIGHT]))); // Como el original: ftol
}

// 0x55CFC0
__declspec(naked) void CStats::UpdateStatsOnRespawn() {
    __asm {
// 0x55CFC0
        push        ecx
// 0x55CFC1
        mov         eax,dword ptr ds:[00B79508h]
// 0x55CFC6
        fild        dword ptr ds:[00B79508h]
// 0x55CFCC
        test        eax,eax
// 0x55CFCE
        jge Ui8_UpdateStatsOnRespawn_55CFD6
// 0x55CFD0
        fadd        dword ptr ds:[00858C54h]
Ui8_UpdateStatsOnRespawn_55CFD6:
        fcomp       dword ptr ds:[00B78FF4h]
// 0x55CFDC
        fnstsw      ax
// 0x55CFDE
        test        ah,41h
// 0x55CFE1
        jne Ui8_UpdateStatsOnRespawn_55D023
// 0x55CFE3
        fld         dword ptr ds:[00B793E0h]
// 0x55CFE9
        fcomp       dword ptr ds:[0085A700h]
// 0x55CFEF
        fnstsw      ax
// 0x55CFF1
        test        ah,41h
// 0x55CFF4
        jne Ui8_UpdateStatsOnRespawn_55D017
// 0x55CFF6
        mov         ecx,dword ptr ds:[00B78F74h]
// 0x55CFFC
        push        ecx
// 0x55CFFD
        push        18h
// 0x55CFFF
        call Ui8_Bridge_0055C180
// 0x55D004
        mov         edx,dword ptr ds:[00B78F74h]
// 0x55D00A
        push        edx
// 0x55D00B
        push        18h
// 0x55D00D
        push        0
// 0x55D00F
        call Ui8_Bridge_0055B980
// 0x55D014
        add         esp,14h
Ui8_UpdateStatsOnRespawn_55D017:
        mov         dword ptr ds:[00B79508h],0
// 0x55D021
        pop         ecx
// 0x55D022
        ret
Ui8_UpdateStatsOnRespawn_55D023:
        inc         dword ptr ds:[00B79508h]
// 0x55D029
        pop         ecx
// 0x55D02A
        ret
    }
}

// 0x55D030
__declspec(naked) void CStats::UpdateStatsAddToHealth(uint32 addToHealth) {
    __asm {
// 0x55D030
        mov         eax,dword ptr ds:[00B79510h]
// 0x55D035
        add         eax,dword ptr [esp+4]
// 0x55D039
        test        eax,eax
// 0x55D03B
        mov         dword ptr [esp+4],eax
// 0x55D03F
        fild        dword ptr [esp+4]
// 0x55D043
        mov         dword ptr ds:[00B79510h],eax
// 0x55D048
        jge Ui8_UpdateStatsAddToHealth_55D050
// 0x55D04A
        fadd        dword ptr ds:[00858C54h]
Ui8_UpdateStatsAddToHealth_55D050:
        fcomp       dword ptr ds:[00B78FF8h]
// 0x55D056
        fnstsw      ax
// 0x55D058
        test        ah,41h
// 0x55D05B
        jne Ui8_UpdateStatsAddToHealth_55D087
// 0x55D05D
        mov         eax,dword ptr ds:[00B78F3Ch]
// 0x55D062
        push        eax
// 0x55D063
        push        18h
// 0x55D065
        call Ui8_Bridge_0055C180
// 0x55D06A
        mov         ecx,dword ptr ds:[00B78F3Ch]
// 0x55D070
        push        ecx
// 0x55D071
        push        18h
// 0x55D073
        push        1
// 0x55D075
        call Ui8_Bridge_0055B980
// 0x55D07A
        add         esp,14h
// 0x55D07D
        mov         dword ptr ds:[00B79510h],0
Ui8_UpdateStatsAddToHealth_55D087:
        ret
    }
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
