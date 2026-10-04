/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWanted
 * Reconstrucción: COMPLETA — 33/33 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 33/33 entradas.
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

#include "Wanted.h"

void CWanted::InjectHooks() {
    RH_ScopedClass(CWanted);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x562390);
    RH_ScopedInstall(Reset, 0x562400);
    RH_ScopedInstall(InitialiseStaticVariables, 0x561C70);
    RH_ScopedInstall(UpdateWantedLevel, 0x561C90);
    RH_ScopedInstall(SetMaximumWantedLevel, 0x561E70);
    RH_ScopedInstall(AreMiamiViceRequired, 0x561F30);
    RH_ScopedInstall(AreSwatRequired, 0x561F40);
    RH_ScopedInstall(AreFbiRequired, 0x561F60);
    RH_ScopedInstall(AreArmyRequired, 0x561F80);
    RH_ScopedInstall(NumOfHelisRequired, 0x561FA0);
    RH_ScopedInstall(ResetPolicePursuit, 0x561FD0);
    RH_ScopedInstall(UpdateCrimesQ, 0x562760);
    RH_ScopedInstall(ClearQdCrimes, 0x561FE0);
    RH_ScopedInstall(AddCrimeToQ, 0x562000);
    RH_ScopedInstall(ReportCrimeNow, 0x562120);
    RH_ScopedInstall(IsInPursuit, 0x562330);
    RH_ScopedInstall(UpdateEachFrame, 0x562360);
    RH_ScopedInstall(RegisterCrime, 0x562410);
    RH_ScopedInstall(RegisterCrime_Immediately, 0x562430);
    RH_ScopedInstall(SetWantedLevel, 0x562470);
    RH_ScopedInstall(CheatWantedLevel, 0x562540);
    RH_ScopedInstall(SetWantedLevelNoDrop, 0x562570);
    RH_ScopedInstall(ClearWantedLevelAndGoOnParole, 0x5625A0);
    RH_ScopedInstall(WorkOutPolicePresence, 0x5625F0);
    RH_ScopedInstall(IsClosestCop, 0x5627D0);
    RH_ScopedInstall(ComputePursuitCopToDisplace, 0x562B00);
    RH_ScopedOverloadedInstall(RemovePursuitCop, "func", 0x562300, void (*)(CCopPed*, CCopPed**, uint8&));
    RH_ScopedOverloadedInstall(RemovePursuitCop, "method", 0x562C10, void(CWanted::*)(CCopPed*));
    RH_ScopedInstall(RemoveExcessPursuitCops, 0x562C40);
    RH_ScopedInstall(Update, 0x562C90);
    RH_ScopedOverloadedInstall(CanCopJoinPursuit, "func", 0x562F60, bool (*)(CCopPed*, uint8, CCopPed**, uint8&));
    RH_ScopedOverloadedInstall(CanCopJoinPursuit, "method", 0x562FB0, bool(CWanted::*)(CCopPed*));
    RH_ScopedInstall(SetPursuitCop, 0x563060);
}

// Como el original: los niveles de búsqueda y el caos se comparan con signo (jl/jg/jle).
static int32 WantedAsInt(eWantedLevel level) {
    return static_cast<int32>(level);
}

// Como el original: distancia al cuadrado de jugador menos punto, sumando (dz² + dy²) + dx².
static float WantedDistSqZYX(const CVector& from, const CVector& to) {
    const float dx = from.x - to.x;
    const float dy = from.y - to.y;
    const float dz = from.z - to.z;
    float sum = dz * dz + dy * dy;
    sum = sum + dx * dx;
    return sum;
}

// 0x562390
void CWanted::Initialise() {
    m_bPoliceBackOff = false;
    m_bPoliceBackOffGarage = false;
    m_bEverybodyBackOff = false;
    SetSwatRequired(false);
    SetFbiRequired(false);
    SetArmyRequired(false);

    m_ChaosLevel = 0;
    m_ChaosLevelBeforeParole = 0;
    m_LastTimeWantedDecreased = 0;
    m_LastTimeWantedLevelChanged = 0;
    m_TimeOfParole = 0;
    m_NumCopsInPursuit = 0;
    m_MaxCopsInPursuit = 0;
    m_MaxCopCarsInPursuit = 0;
    m_ChanceOnRoadBlock = 0;
    m_Multiplier = 1.0f;
    m_TimeCounting = false;
    m_StoredPoliceBackOff = false;
    m_WantedLevel = eWantedLevel::WANTED_CLEAN;
    m_WantedLevelBeforeParole = eWantedLevel::WANTED_CLEAN;
    m_CopsBeatingSuspect = 0;

    rng::fill(m_CopsInPursuit, nullptr);
    ClearQdCrimes();
}

// 0x562400
void CWanted::Reset() {
    ResetPolicePursuit();
    Initialise();
}

// Initialize Static Variables
// 0x561C70
void CWanted::InitialiseStaticVariables() {
    MaximumWantedLevel = eWantedLevel::WANTED_LEVEL_6;
    MaximumChaosLevel = 9200;
    UseNewsHeliInAdditionToPolice = false;
}

// 0x561C90
void CWanted::UpdateWantedLevel() {
    // Como el original: tope y umbrales con signo; el valor de la estadística es un entero con signo (fild)
    if ((int32)m_ChaosLevel > (int32)MaximumChaosLevel) {
        m_ChaosLevel = MaximumChaosLevel;
    }
    const auto oldWanted = m_WantedLevel;
    const auto chaos     = (int32)m_ChaosLevel;
    const auto StarsGained = [oldWanted](int32 level) { return (float)(level - WantedAsInt(oldWanted)); };

    if (chaos >= 4600) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, StarsGained(6));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_6;
        m_MaxCopCarsInPursuit = 3;
        m_MaxCopsInPursuit    = 10;
        m_ChanceOnRoadBlock   = 30;
    } else if (chaos >= 2400) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, StarsGained(5));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_5;
        m_MaxCopCarsInPursuit = 3;
        m_MaxCopsInPursuit    = 8;
        m_ChanceOnRoadBlock   = 24;
    } else if (chaos >= 1200) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, StarsGained(4));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_4;
        m_MaxCopCarsInPursuit = 2;
        m_MaxCopsInPursuit    = 6;
        m_ChanceOnRoadBlock   = 18;
    } else if (chaos >= 550) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, StarsGained(3));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_3;
        m_MaxCopCarsInPursuit = 2;
        m_MaxCopsInPursuit    = 4;
        m_ChanceOnRoadBlock   = 12;
    } else if (chaos >= 180) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, StarsGained(2));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_2;
        m_MaxCopCarsInPursuit = 2;
        m_MaxCopsInPursuit    = 3;
        m_ChanceOnRoadBlock   = 0;
    } else if (chaos >= 50) {
        CStats::IncrementStat(eStats::STAT_TOTAL_NUMBER_OF_WANTED_STARS_ATTAINED, StarsGained(1));
        m_WantedLevel         = eWantedLevel::WANTED_LEVEL_1;
        m_MaxCopCarsInPursuit = 1;
        m_MaxCopsInPursuit    = 1;
        m_ChanceOnRoadBlock   = 0;
    } else {
        if (oldWanted == eWantedLevel::WANTED_LEVEL_1) {
            CStats::IncrementStat(STAT_TOTAL_NUMBER_OF_WANTED_STARS_EVADED);
        }
        m_WantedLevel         = eWantedLevel::WANTED_CLEAN;
        m_MaxCopCarsInPursuit = 0;
        m_MaxCopsInPursuit    = 0;
        m_ChanceOnRoadBlock   = 0;
    }

    if (oldWanted != m_WantedLevel) {
        m_LastTimeWantedLevelChanged = CTimer::GetTimeInMS();
    }

    if (PoliceBackOff()) {
        m_MaxCopCarsInPursuit = 0;
        m_MaxCopsInPursuit    = 0;
        m_ChanceOnRoadBlock   = 0;
    }
}

// Set Maximum Wanted Level
// 0x561E70
void CWanted::SetMaximumWantedLevel(eWantedLevel level) {
    // Como el original: un nivel fuera de 0..6 (comparación sin signo) no cambia nada
    switch (level) {
    case eWantedLevel::WANTED_CLEAN:   MaximumWantedLevel = level; MaximumChaosLevel = 0; break;
    case eWantedLevel::WANTED_LEVEL_1: MaximumWantedLevel = level; MaximumChaosLevel = 115; break;
    case eWantedLevel::WANTED_LEVEL_2: MaximumWantedLevel = level; MaximumChaosLevel = 365; break;
    case eWantedLevel::WANTED_LEVEL_3: MaximumWantedLevel = level; MaximumChaosLevel = 875; break;
    case eWantedLevel::WANTED_LEVEL_4: MaximumWantedLevel = level; MaximumChaosLevel = 1800; break;
    case eWantedLevel::WANTED_LEVEL_5: MaximumWantedLevel = level; MaximumChaosLevel = 3500; break;
    case eWantedLevel::WANTED_LEVEL_6: MaximumWantedLevel = level; MaximumChaosLevel = 6900; break;
    default:                           break;
    }
}

// 0x561F30, Leftover from VC
bool CWanted::AreMiamiViceRequired() const {
    return WantedAsInt(m_WantedLevel) >= 3; // Como el original: setge (con signo)
}

// Checks if SWAT is needed after four wanted level stars
// 0x561F40
bool CWanted::AreSwatRequired() const {
    return m_WantedLevel == eWantedLevel::WANTED_LEVEL_4 || m_bSwatRequired;
}

// Checks if FBI is needed after five wanted level stars
// 0x561F60
bool CWanted::AreFbiRequired() const {
    return m_WantedLevel == eWantedLevel::WANTED_LEVEL_5 || m_bFbiRequired;
}

// Checks if Army is needed after six wanted level stars
// 0x561F80
bool CWanted::AreArmyRequired() const {
    return m_WantedLevel == eWantedLevel::WANTED_LEVEL_6 || m_bArmyRequired;
}

// 0x561FA0
int32 CWanted::NumOfHelisRequired() const {
    if (PoliceBackOff()) {
        return 0;
    }

    // Como el original: 3 -> 1, 4..6 -> 2 y cualquier otro valor (con signo) -> 0
    const auto level = WantedAsInt(m_WantedLevel);
    if (level == 3) {
        return 1;
    }
    if (level <= 3 || level > 6) {
        return 0;
    }
    return 2;
}

// In III
// 0x561FD0
void CWanted::ResetPolicePursuit() {
    // NOP
}

// 0x562760
void CWanted::UpdateCrimesQ() {
    for (auto& crime : m_CrimesBeingQd) {
        if (crime.m_nCrimeType == CRIME_NONE) {
            continue;
        }

        // Como el original: tiempo > inicio + 500 (sin signo, la suma puede desbordar)
        if (CTimer::GetTimeInMS() > crime.m_nStartTime + 500 && !crime.m_bAlreadyReported) {
            ReportCrimeNow(crime.m_nCrimeType, crime.m_vecCoors, crime.m_bPoliceDontReallyCare);

            crime.m_bAlreadyReported = true;
        }

        if (CTimer::GetTimeInMS() > crime.m_nStartTime + 10'000) {
            crime.m_nCrimeType = CRIME_NONE;
        }
    }
}

// 0x561FE0
void CWanted::ClearQdCrimes() {
    for (auto& crime : m_CrimesBeingQd) {
        crime.m_nCrimeType = eCrimeType::CRIME_NONE;
    }
}

// 0x562000
bool CWanted::AddCrimeToQ(eCrimeType crimeType, uint32 crimeId, const CVector& posn, bool alreadyReported, bool policeDontReallyCare) {
    // Como el original: sin assert; un crimen ya en cola devuelve si ya estaba denunciado
    for (auto& c : m_CrimesBeingQd) {
        if (c.m_nCrimeType == crimeType && c.m_nCrimeId == crimeId) {
            if (c.m_bAlreadyReported) {
                return true;
            }
            if (alreadyReported) {
                c.m_bAlreadyReported = alreadyReported;
            }
            return false;
        }
    }

    for (auto& c : m_CrimesBeingQd) {
        if (c.m_nCrimeType != eCrimeType::CRIME_NONE) {
            continue;
        }
        c.m_nCrimeId              = crimeId;
        c.m_nCrimeType            = crimeType;
        c.m_vecCoors              = posn;
        c.m_nStartTime            = CTimer::GetTimeInMS();
        c.m_bAlreadyReported      = alreadyReported;
        c.m_bPoliceDontReallyCare = policeDontReallyCare;
        break;
    }
    return false; // Como el original: un crimen nuevo siempre devuelve falso
}

// 0x562120
void CWanted::ReportCrimeNow(eCrimeType crimeType, const CVector& posn, bool bPoliceDontReallyCare) {
    if (CCheat::IsActive(CHEAT_I_DO_AS_I_PLEASE)) {
        return;
    }

    const auto wantedLevel = m_WantedLevel;
    float crimeMult = m_Multiplier;

    if (CDarkel::ReadStatus() == eDarkelStatus::FRENZY_ON_GOING) {
        crimeMult *= 0.3f;
    }

    if (crimeMult < 0.0f) {
        crimeMult = 0.0f;
    }

    if (CGangWars::GangWarFightingGoingOn()) {
        crimeMult = 0.0f;
    }

    if (bPoliceDontReallyCare) {
        crimeMult *= 0.333f; // Como el original: multiplica por 0.333f (0x864E30), no divide entre 3
    }

    if (CGangWars::GangWarFightingGoingOn()) {
        crimeMult = 0.0f;
    }

    bool addChaos = true;
    switch (crimeType) {
    case CRIME_DAMAGED_PED:
    case CRIME_VEHICLE_DAMAGE:
    case CRIME_SPEEDING:
        crimeMult *= 5.0f;
        break;
    case CRIME_DAMAGED_COP:
        crimeMult *= 45.0f;
        break;
    case CRIME_DAMAGE_CAR:
        crimeMult *= 30.0f;
        break;
    case CRIME_DAMAGE_COP_CAR:
    case CRIME_KILL_COP_PED_WITH_CAR:
    case CRIME_SET_COP_PED_ON_FIRE:
        crimeMult *= 80.0f;
        break;
    case CRIME_CAR_STEAL:
        crimeMult *= 15.0f;
        break;
    case CRIME_RUN_REDLIGHT:
        crimeMult *= 10.0f;
        break;
    case CRIME_KILL_PED_WITH_CAR:
        crimeMult *= 18.0f;
        break;
    case CRIME_DESTROY_HELI:
    case CRIME_DESTROY_PLANE:
        crimeMult *= 400.0f;
        break;
    case CRIME_SET_PED_ON_FIRE:
    case CRIME_SET_CAR_ON_FIRE:
        crimeMult *= 20.0f;
        break;
    case CRIME_EXPLOSION:
        crimeMult *= 25.0f;
        break;
    case CRIME_STAB_PED:
        crimeMult *= 35.0f;
        break;
    case CRIME_STAB_COP:
        crimeMult *= 100.0f;
        break;
    case CRIME_DESTROY_VEHICLE:
        crimeMult *= 70.0f;
        break;
    case CRIME_HIT_CAR:
    case CRIME_AIM_GUN:
        crimeMult += crimeMult;
        break;
    default:
        addChaos = false; // Como el original: solo los tipos 2..22 suman caos
        break;
    }

    if (addChaos) {
        // Como el original: fiadd del caos (entero con signo) al float y truncado con _ftol2 (64 bits)
        int32 chaosInt = (int32)m_ChaosLevel;
        float chaosSum;
        __asm {
            fld   crimeMult
            fiadd chaosInt
            fstp  chaosSum
        }
        m_ChaosLevel = (uint32)(int32)(int64)chaosSum;
    }

    // Como el original: máximo con signo frente al caos previo a la condicional
    if ((int32)m_ChaosLevel <= (int32)m_ChaosLevelBeforeParole) {
        m_ChaosLevel = m_ChaosLevelBeforeParole;
    }
    UpdateWantedLevel();
    if (WantedAsInt(m_WantedLevel) > WantedAsInt(wantedLevel)) {
        m_PoliceScannerAudioEntity.AddAudioEvent(AE_CRIME_COMMITTED, crimeType, posn);
    }
}

// 0x562330
bool CWanted::IsInPursuit(CCopPed* cop) {
    for (auto& copInPursuit : m_CopsInPursuit) {
        if (copInPursuit == cop) {
            return true;
        }
    }

    return false;
}

// 0x562360
void CWanted::UpdateEachFrame() {
    ZoneScoped;

    auto playerWanted = FindPlayerWanted();
    const auto wantedLevel = WantedAsInt(playerWanted->GetWantedLevel());

    // Como el original: comparaciones con signo
    if (playerWanted->PoliceBackOff() || (wantedLevel != 3) && (wantedLevel <= 3 || wantedLevel > 6)) {
        UseNewsHeliInAdditionToPolice = true;
    }
}

// 0x562410
void CWanted::RegisterCrime(eCrimeType crimeType, const CVector& posn, uint32 IDKey, bool bPoliceDontReallyCare) {
    AddCrimeToQ(crimeType, IDKey, posn, false, bPoliceDontReallyCare);
}

// 0x562430
void CWanted::RegisterCrime_Immediately(eCrimeType crimeType, const CVector& posn, uint32 IDKey, bool bPoliceDontReallyCare) {
    if (!AddCrimeToQ(crimeType, IDKey, posn, true, bPoliceDontReallyCare)) {
        ReportCrimeNow(crimeType, posn, bPoliceDontReallyCare);
    }
}

// 0x562470
void CWanted::SetWantedLevel(eWantedLevel level) {
    if (CCheat::IsActive(CHEAT_I_DO_AS_I_PLEASE)) {
        return;
    }

    // Como el original: mínimo con signo; un nivel fuera de 0..6 (sin signo) no toca el caos
    eWantedLevel newLevel = WantedAsInt(level) > WantedAsInt(MaximumWantedLevel) ? MaximumWantedLevel : level;
    ClearQdCrimes();

    switch (newLevel) {
    case eWantedLevel::WANTED_CLEAN:   m_ChaosLevel = 0; break;
    case eWantedLevel::WANTED_LEVEL_1: m_ChaosLevel = 70; break;
    case eWantedLevel::WANTED_LEVEL_2: m_ChaosLevel = 200; break;
    case eWantedLevel::WANTED_LEVEL_3: m_ChaosLevel = 570; break;
    case eWantedLevel::WANTED_LEVEL_4: m_ChaosLevel = 1220; break;
    case eWantedLevel::WANTED_LEVEL_5: m_ChaosLevel = 2420; break;
    case eWantedLevel::WANTED_LEVEL_6: m_ChaosLevel = 4620; break;
    default:                           break;
    }
    UpdateWantedLevel();
}

// 0x562540
void CWanted::CheatWantedLevel(eWantedLevel level) {
    if (WantedAsInt(level) > WantedAsInt(MaximumWantedLevel)) { // Como el original: con signo
        SetMaximumWantedLevel(level);
    }

    SetWantedLevel(level);
    UpdateWantedLevel();
}

// 0x562570
void CWanted::SetWantedLevelNoDrop(eWantedLevel level) {
    // Como el original: comparaciones con signo
    if (WantedAsInt(m_WantedLevel) < WantedAsInt(m_WantedLevelBeforeParole)) {
        SetWantedLevel(m_WantedLevelBeforeParole);
    }

    if (WantedAsInt(level) > WantedAsInt(m_WantedLevel)) {
        SetWantedLevel(level);
    }
}

// 0x5625A0
void CWanted::ClearWantedLevelAndGoOnParole() {
    CStats::IncrementStat(STAT_TOTAL_NUMBER_OF_WANTED_STARS_EVADED, static_cast<float>(WantedAsInt(m_WantedLevel)));

    // Como el original: FindPlayerWanted se llama una vez por campo
    m_ChaosLevelBeforeParole = FindPlayerWanted()->m_ChaosLevel;
    m_WantedLevelBeforeParole = FindPlayerWanted()->m_WantedLevel;
    m_TimeOfParole = CTimer::GetTimeInMS();
    m_ChaosLevel = 0;
    m_WantedLevel = eWantedLevel::WANTED_CLEAN;
}

// 0x5625F0
int32 CWanted::WorkOutPolicePresence(CVector posn, float radius) {
    auto numCops = 0;

    // Como el original: distancia = sqrt((dz² + dy²) + dx²) con d = punto - entidad; cuenta si dist < radio
    const auto InRange = [&](const CVector& entityPos) {
        return std::sqrt(WantedDistSqZYX(posn, entityPos)) < radius;
    };

    auto pedPool = GetPedPool();
    for (auto i = pedPool->GetSize(); i; i--) {
        if (auto ped = pedPool->GetAt(i - 1)) {
            if (ped->m_nPedType != PED_TYPE_COP || !ped->IsAlive())
                continue;

            if (InRange(ped->GetPosition()))
                numCops++;
        }
    }

    auto vehPool = GetVehiclePool();
    for (auto i = vehPool->GetSize(); i; i--) {
        if (auto veh = vehPool->GetAt(i - 1)) {
            bool isCopVehicle = veh->vehicleFlags.bIsLawEnforcer || veh->m_nModelIndex == MODEL_POLMAV;

            if (!isCopVehicle || veh == FindPlayerVehicle()) {
                continue;
            }

            if (veh->GetStatus() == STATUS_ABANDONED || veh->GetStatus() == STATUS_WRECKED) {
                continue;
            }

            if (InRange(veh->GetPosition())) {
                numCops++;
            }
        }
    }

    return numCops;
}

// 0x5627D0
// Returns true if the specified ped is one of the closest to the player.
bool CWanted::IsClosestCop(CPed* ped, const int32 numCopsToCheck) const {
    // Como el original: compacta los policías, elige numCopsToCheck veces el más cercano (estrictamente menor,
    // primer índice en empate) y compara el peatón con los elegidos (huecos sin elegir quedan a nulo).
    CCopPed* cops[MAX_COPS_IN_PURSUIT];
    int32 numCops = 0;
    for (auto* cop : m_CopsInPursuit) {
        if (cop) {
            cops[numCops++] = cop;
        }
    }

    float dists[MAX_COPS_IN_PURSUIT];
    CCopPed* closest[MAX_COPS_IN_PURSUIT];
    for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
        dists[i] = FLT_MAX;
        closest[i] = nullptr;
    }

    const auto& playerPos = FindPlayerPed()->GetPosition();
    for (auto i = 0; i < numCops; i++) {
        // Como el original: d = jugador - policía, (dz² + dx²) + dy²
        const auto& copPos = cops[i]->GetPosition();
        const float dx = playerPos.x - copPos.x;
        const float dy = playerPos.y - copPos.y;
        const float dz = playerPos.z - copPos.z;
        float sum = dz * dz + dx * dx;
        dists[i] = sum + dy * dy;
    }

    for (auto k = 0; k < numCopsToCheck; k++) {
        float best = FLT_MAX;
        int32 bestIdx = -1;
        for (auto i = 0; i < numCops; i++) {
            if (best > dists[i]) {
                bestIdx = i;
                best = dists[i];
            }
        }
        if (bestIdx != -1) {
            closest[k] = cops[bestIdx];
            cops[bestIdx] = nullptr;
            dists[bestIdx] = FLT_MAX;
        }
    }

    for (auto k = 0; k < numCopsToCheck; k++) {
        if (static_cast<void*>(closest[k]) == static_cast<void*>(ped)) {
            return true;
        }
    }
    return false;
}

// 0x562B00
CCopPed* CWanted::ComputePursuitCopToDisplace(CCopPed* cop, CCopPed** copsArray) {
    CCopPed* displacedCop = nullptr;
    auto distTargetCop = 1.0f;

    // Como el original: FindPlayerPed se llama para el policía objetivo y para cada policía vivo
    if (cop) {
        const auto& copPos = cop->GetPosition();
        distTargetCop = WantedDistSqZYX(FindPlayerPed()->GetPosition(), copPos);
        if (distTargetCop < 1.0f) {
            distTargetCop = 1.0f;
        }
    }

    for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
        const auto& copInPursuit = copsArray[i];

        if (!copInPursuit) {
            continue;
        }

        if (!copInPursuit->IsAlive()) {
            return copInPursuit;
        }

        const auto& copPos = copInPursuit->GetPosition();
        const auto distPursuitCop = WantedDistSqZYX(FindPlayerPed()->GetPosition(), copPos);

        if (distPursuitCop > distTargetCop) {
            displacedCop  = copInPursuit;
            distTargetCop = distPursuitCop;
        }
    }

    return displacedCop;
}

// 0x562300
void CWanted::RemovePursuitCop(CCopPed* cop, CCopPed** copsArray, uint8& copsCounter) {
    for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
        if (copsArray[i] != cop) {
            continue;
        }

        // R*: delete?
        copsArray[i] = nullptr;
        copsCounter--;
        break;
    }
}

// 0x562C10
void CWanted::RemovePursuitCop(CCopPed* cop) {
    for (auto& copInPursuit : m_CopsInPursuit) {
        if (copInPursuit != cop) {
            continue;
        }

        copInPursuit = nullptr;
        m_NumCopsInPursuit--;
        break;
    }
}

// 0x562C40
void CWanted::RemoveExcessPursuitCops() {
    while (m_NumCopsInPursuit > m_MaxCopsInPursuit) {
        RemovePursuitCop(ComputePursuitCopToDisplace(nullptr, m_CopsInPursuit));
    }
}

// 0x562C90
void CWanted::Update() {
    if (WantedAsInt(m_WantedLevel) < 5) { // Como el original: con signo
        if (m_TimeCounting) {
            // Como el original: récord en la estadística 51 y valor de la última persecución en la 52
            CStats::SetNewRecordStat(STAT_LONGEST_CHASE_TIME_WITH_5_OR_MORE_STARS, static_cast<float>(m_CurrentChaseTime));
            CStats::SetStatValue(STAT_LAST_CHASE_TIME_WITH_5_OR_MORE_STARS, static_cast<float>(m_CurrentChaseTime));
            m_TimeCounting = false;
        }
    } else {
        if (!m_TimeCounting) {
            m_CurrentChaseTime = 0;
            m_CurrentChaseTimeCounter = CTimer::GetTimeInMS();
            m_TimeCounting = true;
        }
        if (m_TimeCounting && CTimer::GetTimeInMS() - m_CurrentChaseTimeCounter > 1000) {
            m_CurrentChaseTime++;
            m_CurrentChaseTimeCounter = CTimer::GetTimeInMS();
        }
    }

    // Como el original: tiempo > condicional + 20000 (sin signo, la suma puede desbordar)
    if (CTimer::GetTimeInMS() > m_TimeOfParole + 20000) {
        m_ChaosLevelBeforeParole = 0;
        m_WantedLevelBeforeParole = eWantedLevel::WANTED_CLEAN;
    }

    if (CTimer::GetTimeInMS() - m_LastTimeWantedDecreased > 1000) {
        // Como el original: solo la región del tiempo (campo o desierto) cuenta como zona rural
        const bool inCountryside = CWeather::WeatherRegion == WEATHER_REGION_DEFAULT
                                || CWeather::WeatherRegion == WEATHER_REGION_DESERT;

        // Como el original: con más de una estrella no baja en ciudad ni en interiores, y nunca en un vehículo
        // policial, avión o helicóptero (FindPlayerVehicle se llama en cada comprobación)
        bool keepWanted;
        if (WantedAsInt(m_WantedLevel) > 1 && (!inCountryside || CGame::currArea != AREA_CODE_NORMAL_WORLD)) {
            keepWanted = true;
        } else if (!FindPlayerVehicle()) {
            keepWanted = false;
        } else {
            keepWanted = FindPlayerVehicle()->IsLawEnforcementVehicle()
                      || FindPlayerVehicle()->m_nVehicleSubType == VEHICLE_TYPE_PLANE
                      || FindPlayerVehicle()->m_nVehicleSubType == VEHICLE_TYPE_HELI;
        }

        if (keepWanted) {
            m_LastTimeWantedDecreased = CTimer::GetTimeInMS();
        } else {
            const auto playerCoors = FindPlayerCoors();

            if (!WorkOutPolicePresence(playerCoors, 18.0f)) {
                m_LastTimeWantedDecreased = CTimer::GetTimeInMS();

                int32 chaosLevel = (int32)m_ChaosLevel - (inCountryside ? 2 : 1);
                m_ChaosLevel = (uint32)std::max(chaosLevel, 0);

                UpdateWantedLevel();
                CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(true);
            }
        }

        UpdateCrimesQ();

        uint8 cops = 0;
        bool nilEncountered = false;
        bool listMessedUp = false;

        for (auto& copInPursuit : m_CopsInPursuit) {
            if (copInPursuit != nullptr) {
                ++cops;

                if (nilEncountered) {
                    listMessedUp = true;
                }
            }
            else {
                nilEncountered = true;
            }
        }

        if (cops != m_NumCopsInPursuit) {
            NOTSA_LOG_DEBUG("CopPursuit total messed up: re-setting!"); // leftover debug shit
            m_NumCopsInPursuit = cops;
        }
        if (listMessedUp) {
            NOTSA_LOG_DEBUG("CopPursuit pointer list messed up: re-sorting!");
            bool notFixed = true;

            for (auto i = 0u; i < MAX_COPS_IN_PURSUIT; i++) {
                auto& cop = m_CopsInPursuit[i];

                if (!cop) {
                    for (auto j = i; j < MAX_COPS_IN_PURSUIT; j++) {
                        if (m_CopsInPursuit[j]) {
                            cop = m_CopsInPursuit[j];
                            m_CopsInPursuit[j] = nullptr;
                            notFixed = false;

                            break;
                        }
                    }

                    if (notFixed) {
                        break;
                    }
                }
            }
        }
    }

    if (m_StoredPoliceBackOff != PoliceBackOff()) {
        UpdateWantedLevel();
        m_StoredPoliceBackOff = PoliceBackOff();
    }
}

// 0x562F60
bool CWanted::CanCopJoinPursuit(CCopPed* target, uint8 maxCopsCount, CCopPed** copsArray, uint8& copsCounter) {
    if (!maxCopsCount) {
        return false;
    }

    while (true) {
        if (copsCounter < maxCopsCount) {
            return true;
        }

        auto cop = ComputePursuitCopToDisplace(target, copsArray);
        if (!cop) {
            break;
        }

        RemovePursuitCop(cop, copsArray, copsCounter);
    }
    return false;
}

// 0x562FB0
bool CWanted::CanCopJoinPursuit(CCopPed* cop) {
    if (PoliceBackOff()) {
        return false;
    }
    std::array<CCopPed*, MAX_COPS_IN_PURSUIT> cops{};
    rng::copy(m_CopsInPursuit, cops.begin());
    auto copsCount = m_NumCopsInPursuit;
    return CanCopJoinPursuit(cop, m_MaxCopsInPursuit, cops.data(), copsCount);
}

// 0x563060
bool CWanted::SetPursuitCop(CCopPed* cop) {
    if (!CanCopJoinPursuit(cop)) {
        return false;
    }

    while (m_NumCopsInPursuit >= m_MaxCopsInPursuit) {
        RemovePursuitCop(ComputePursuitCopToDisplace(cop, m_CopsInPursuit));
    }

    for (auto& copInPursuit : m_CopsInPursuit) {
        if (copInPursuit != nullptr) {
            continue;
        }

        copInPursuit = cop;
        m_NumCopsInPursuit++;
        break;
    }

    return true;
}
