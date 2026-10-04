/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPopCycle
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 15/15 entradas.
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

#include "PopCycle.h"
#include <CustomBuildingDNPipeline.h>

void CPopCycle::InjectHooks() {
    RH_ScopedClass(CPopCycle);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(Initialise, 0x5BC090);
    RH_ScopedGlobalInstall(PickGangToCreateMembersOf, 0x60F8D0);
    RH_ScopedGlobalInstall(FindNewPedType, 0x60FBD0);
    RH_ScopedGlobalInstall(PickPedMIToStreamInForCurrentZone, 0x60FFD0);
    RH_ScopedGlobalInstall(IsPedAppropriateForCurrentZone, 0x610150);
    RH_ScopedGlobalInstall(IsPedInGroup, 0x610210);
    RH_ScopedGlobalInstall(PickARandomGroupOfOtherPeds, 0x610420);
    RH_ScopedGlobalInstall(PlayerKilledADealer, 0x610490);
    RH_ScopedGlobalInstall(UpdateDealerStrengths, 0x6104B0);
    RH_ScopedGlobalInstall(UpdateAreaDodgyness, 0x610560);
    RH_ScopedGlobalInstall(UpdateIsGangArea, 0x6106D0);
    RH_ScopedGlobalInstall(PedIsAcceptableInCurrentZone, 0x610720);
    RH_ScopedGlobalInstall(UpdatePercentages, 0x610770);
    RH_ScopedGlobalInstall(Update, 0x610BF0);
    RH_ScopedGlobalInstall(GetCurrentPercOther_Peds, 0x610310);
}

// 0x5BC090
void CPopCycle::Initialise() {
    CFileMgr::SetDir("DATA");
    const auto file = CFileMgr::OpenFile("POPCYCLE.DAT", "rb"); // Como el original: modo "rb"
    CFileMgr::SetDir("");

    const notsa::ScopeGuard autoCloser{ [&] { CFileMgr::CloseFile(file); } };

    auto nline{ 1u };
    for (auto zone = 0u; zone < +eZonePopulationType::COUNT; zone++) {
        for (auto wktime = 0; wktime < 2; wktime++) { // weekday (0) / weekend(1)
            for (auto daytime = 0; daytime < 24 / PERC_DATA_TIME_RESOLUTION_HR; daytime++) {
                // Find next suitable data line
                const char* l{};
                while (true) {
                    l = CFileLoader::LoadLine(file);
                    if (!l) {
                        NOTSA_UNREACHABLE("Expected more data, got EOF!");
                    }
                    nline++;
                    if (l[0] != '/' && l[0]) {
                        break;
                    }
                }

                // Ideally we could/would use a file pointer here (instead of `LoadLine`)
                // and read each number one-by-one.
                // But until then we're stuck with this hardcoded version.

                auto& percs = m_nPercTypeGroup[daytime][wktime][zone];
                const auto nread = sscanf_s(
                    l,
                    "%hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",

                    &m_nMaxNumPeds[daytime][wktime][zone],
                    &m_nMaxNumCars[daytime][wktime][zone],

                    &m_nPercDealers[daytime][wktime][zone],
                    &m_nPercGang[daytime][wktime][zone],
                    &m_nPercCops[daytime][wktime][zone],
                    &m_nPercOther[daytime][wktime][zone],

                    &percs[0], &percs[1], &percs[2], &percs[3], &percs[4], &percs[5],
                    &percs[6], &percs[7], &percs[8], &percs[9], &percs[10], &percs[11],
                    &percs[12], &percs[13], &percs[14], &percs[15], &percs[16], &percs[17]
                );

                if (nread != 6 + 18) {
                    NOTSA_UNREACHABLE("Failed reading all data!");
                }

                // Como el original: siempre se reescala con 100.0f / suma en la FPU (truncando con _ftol2) y la
                // diferencia hasta 100 se suma (en bytes) al ultimo porcentaje maximo.
                {
                    int32 percsSum = 0;
                    for (const auto p : percs) {
                        percsSum += p;
                    }
                    const float scale = 100.0f / (float)percsSum;
                    for (auto& p : percs) {
                        p = (uint8)(int64)((float)p * scale);
                    }
                    int32 newSum = 0, maxPerc = 0;
                    size_t maxIdx = 0;
                    for (size_t i = 0; i < std::size(percs); i++) {
                        newSum += percs[i];
                        if ((int32)percs[i] >= maxPerc) {
                            maxPerc = percs[i];
                            maxIdx  = i;
                        }
                    }
                    percs[maxIdx] = (uint8)(percs[maxIdx] + (uint8)(100 - newSum));
                }

            }
        }
    }

    // Como el original: se reinicia el estado actual antes de cerrar el archivo
    m_nCurrentTimeIndex  = 0;
    m_nCurrentTimeOfWeek = 0;
    m_pCurrZoneInfo      = nullptr;
    m_nCurrentZoneType   = -1;

    NOTSA_LOG_DEBUG("POPCYCLE.DAT has been loaded successfully!");
}

// 0x60FBD0
bool CPopCycle::FindNewPedType(ePedType& outPedType, eModelID& outPedMI, bool noGangs, bool noCops) {
    // Como el original: sin zona actual se devuelve false sin tocar las salidas.
    if (!m_pCurrZoneInfo) {
        return false;
    }

    // Como el original: (int)(rand * (1/32768) * 100) < 70
    if (CPopulation::bInPoliceStation && CGeneral::GetRandomIntInRange(0, 100) < 70) {
        outPedType = PED_TYPE_COP;
        outPedMI = CPopulation::ChoosePolicePedOccupation();
        return true;
    }

    auto dealersChance = m_NumDealers_Peds - (float)(int32)CPopulation::ms_nNumDealers;

    auto gangChance = m_NumGangs_Peds - (float)(int32)CPopulation::CalculateTotalNumGangPeds();
    if (CPopulation::m_bOnlyCreateRandomGangMembers) {
        gangChance = 50.f;
    }
    if (CPopulation::m_bDontCreateRandomGangMembers || noGangs) {
        gangChance = -10.f;
    }

    auto copChance = m_NumCops_Peds - (float)(int32)CPopulation::ms_nNumCop;
    if (CGangWars::GangWarFightingGoingOn() || CPopulation::m_bDontCreateRandomCops || noCops || m_pCurrZoneInfo->IsNoCops) {
        copChance = -10.f;
    }

    auto civPedsChance = CPopCycle::m_NumOther_Peds - (float)(int32)(CPopulation::ms_nNumCivMale + CPopulation::ms_nNumCivFemale);

    for (auto chance : { &civPedsChance, &copChance, &dealersChance, &gangChance }) {
        if (*chance < 2.f) {
            *chance *= CGeneral::GetRandomNumberInRange(0.f, 1.f);
        }
    }

    if (!CGame::CanSeeOutSideFromCurrArea()) {
        dealersChance = -10.f;
    }

    while (true) {
        // Como el original: el maximo se elige por parejas (dealers/bandas, luego policias, luego civiles).
        auto highestChance = dealersChance > gangChance ? dealersChance : gangChance;
        highestChance = highestChance > copChance ? highestChance : copChance;
        highestChance = highestChance > civPedsChance ? highestChance : civPedsChance;

        if (highestChance <= 0.f) {
            return false;
        }

        if (highestChance == dealersChance) {
            // Como el original (0x60FEA4): el tipo y el modelo invalido se escriben antes de buscar; se queda
            // con el ultimo modelo cargado del grupo de camellos.
            outPedType = PED_TYPE_DEALER;
            outPedMI = MODEL_INVALID;
            const auto pedGroup = CPopulation::GetPedGroupId(POPCYCLE_GROUP_DEALERS);
            for (auto i = 0; i < (int16)CPopulation::m_nNumPedsInGroup[pedGroup]; i++) {
                const auto modelId = (eModelID)(uint16)CPopulation::m_PedGroups[pedGroup][i];
                if (CStreaming::IsModelLoaded(modelId)) {
                    outPedMI = modelId;
                }
            }
            if (outPedMI >= 0) {
                return true;
            }
            dealersChance = 0.f;
            continue;
        }

        if (highestChance == gangChance) { // 0x60FF13
            // Como el original: PickGangToCreateMembersOf puede devolver 0 (sin densidad de bandas); entonces
            // el modelo queda invalido sin llamar a ChooseGangOccupation.
            outPedType = PickGangToCreateMembersOf();
            if (outPedType != (ePedType)0) {
                outPedMI = CPopulation::ChooseGangOccupation((eGangID)(outPedType - ePedType::PED_TYPE_GANG1));
                if (outPedMI >= 0) {
                    return true;
                }
            } else {
                outPedMI = MODEL_INVALID;
            }
            if (CPopulation::m_bOnlyCreateRandomGangMembers) {
                return false;
            }
            gangChance = 0.f;
            continue;
        }

        if (highestChance == copChance) { // 0x60FF62
            outPedType = PED_TYPE_COP;
            outPedMI = CPopulation::ChoosePolicePedOccupation();
            return true;
        }

        if (highestChance == civPedsChance) { // 0x60FF86
            outPedMI = CPopulation::ChooseCivilianOccupation(false, false, ANIM_GROUP_NONE, MODEL_INVALID, ePedStats::NONE, false, true, false, nullptr);
            if (outPedMI < 0 || outPedMI == MODEL_MALE01) {
                return false;
            }
            outPedType = CModelInfo::GetPedModelInfo(outPedMI)->m_nPedType;
            return true;
        }

        // Como el original: solo con NaN; devuelve true sin escribir nada.
        return true;
    }
}

// 0x610310
int32 CPopCycle::GetCurrentPercOther_Peds() {
    const auto percOther = m_nPercOther[CPopCycle::m_nCurrentTimeIndex][CPopCycle::m_nCurrentTimeOfWeek][CPopCycle::m_nCurrentZoneType];

    // Como el original: primero el factor de lluvia; IsPlayerOnAMission (y FindPlayerPed) se consultan siempre,
    // FrenzyOnGoing despues, y el resultado se trunca a entero.
    auto mult = 1.f - std::sqrt(CWeather::Rain) * 0.8f;

    if (CTheScripts::IsPlayerOnAMission()) {
        if (const auto plyr = FindPlayerPed()) {
            if (plyr->IsInVehicle()) {
                switch (plyr->m_pVehicle->m_nModelIndex) {
                case MODEL_TAXI:
                case MODEL_CABBIE:
                    mult = 1.f;
                    break;
                }
            }
        }
    }

    if (CDarkel::FrenzyOnGoing()) {
        mult = 1.f;
    }

    return (int32)((float)percOther * mult);
}

// 0x610150
bool CPopCycle::IsPedAppropriateForCurrentZone(int32 modelIndex) {
    if (!IsRaceAllowedInCurrentZone((eModelID)modelIndex)) {
        return false;
    }

    // Como el original: solo los 18 grupos con porcentaje y se comprueba el porcentaje de cada grupo
    const auto& percs = m_nPercTypeGroup[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_pCurrZoneInfo->PopType];
    for (auto grpId = 0; grpId < POPCYCLE_TOTAL_GROUP_PERCS; grpId++) {
        // Check if this group is active now
        if (!percs[grpId]) {
            continue;
        }

        // Check if group contains this model
        const auto pedGroup = CPopulation::GetPedGroupId((ePopcycleGroup)(grpId), CPopulation::CurrentWorldZone);
        for (auto i = 0; i < (int16)CPopulation::m_nNumPedsInGroup[pedGroup]; i++) {
            if (modelIndex == (int32)(uint16)CPopulation::m_PedGroups[pedGroup][i]) {
                return true;
            }
        }
    }

    // No group currently active contains the given model
    return false;
}

// 0x610210
bool CPopCycle::IsPedInGroup(int32 modelIndex, ePopcycleGroup group) {
    for (auto pedGroup : CPopulation::GetPedGroupsOfGroup(group)) {
        for (auto mdlId : CPopulation::GetModelsInPedGroup(pedGroup)) {
            if (mdlId == modelIndex) {
                return true;
            }
        }
    }
    return false;
}

// 0x610720
bool CPopCycle::PedIsAcceptableInCurrentZone(int32 modelIndex) {
    if (!m_pCurrZoneInfo) {
        return false;
    }

    if (CCheat::IsZoneStreamingAllowed()) {
        return true;
    }

    if (IsRaceAllowedInCurrentZone((eModelID)modelIndex)) {
        return true;
    }

    return false;
}

// 0x610420
ePopcycleGroup CPopCycle::PickARandomGroupOfOtherPeds() {
    const auto& percs = m_nPercTypeGroup[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_pCurrZoneInfo->PopType];
    // Como el original: (int)(rand * (1/32768) * 100), sin limite de grupos al restar
    auto rndPerc = CGeneral::GetRandomIntInRange(0, 100);
    int32 grpIdx = 0;
    while ((int32)percs[grpIdx] < rndPerc) {
        rndPerc -= (int32)percs[grpIdx];
        grpIdx++;
    }
    return (ePopcycleGroup)grpIdx;
}

// 0x60FFD0
eModelID CPopCycle::PickPedMIToStreamInForCurrentZone() {
    for (auto tr = 0; tr < 10; tr++) { // 10 tries
        // This inlined selector uses strict boundaries and rand / 32768;
        // PickARandomGroupOfOtherPeds has different boundary semantics.
        auto percentage = int32(CGeneral::GetRandomNumber()) * 100 / 32768;
        const auto& percentages = m_nPercTypeGroup[m_nCurrentTimeIndex][m_nCurrentTimeOfWeek][m_pCurrZoneInfo->PopType];
        int32 group = 0;
        while (percentage >= percentages[group]) {
            percentage -= percentages[group++];
        }
        const auto grpId = ePopcycleGroup(group);
        const auto pedGrpId     = CPopulation::GetPedGroupId(grpId, CPopulation::CurrentWorldZone);
        const auto npeds        = (int32)(int16)CPopulation::m_nNumPedsInGroup[pedGrpId]; // Como el original: movsx
        auto& nextPedToLoadSlot = CStreaming::ms_NextPedToLoadFromGroup.data()[grpId];
        for (auto p = 0; p < npeds; p++) {
            nextPedToLoadSlot  = (nextPedToLoadSlot + 1) % npeds;
            const auto modelId = (eModelID)(uint16)CPopulation::m_PedGroups[pedGrpId][nextPedToLoadSlot];
            if (!notsa::contains(CStreaming::ms_pedsLoaded, modelId) && IsRaceAllowedInCurrentZone(modelId)) {
                return modelId;
            }
        }
    }
    return MODEL_INVALID;
}

// 0x610490
void CPopCycle::PlayerKilledADealer() {
    if (m_pCurrZoneInfo && m_pCurrZoneInfo->DealerStrength) {
        m_pCurrZoneInfo->DealerStrength--;
    }
}

// 0x610BF0
void CPopCycle::Update() {
    ZoneScoped;

    // Como el original: un dia fuera de 0..7 deja el valor sin tocar
    switch (CClock::GetGameWeekDay()) {
    case 0: // Not sure (Maybe Sunday)
    case 7: // Sunday
        m_nCurrentTimeOfWeek = 1;
        break;
    case 1:  // Monday
        m_nCurrentTimeOfWeek = CClock::GetGameClockHours() >= 20 ? 0 : 1;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        m_nCurrentTimeOfWeek = 0;
        break;
    case 6: // Saturday
        m_nCurrentTimeOfWeek = CClock::GetGameClockHours() >= 20 ? 1 : 0;
        break;
    }

    m_nCurrentTimeIndex = CClock::GetGameClockHours() / 2;

    // Como el original: se pasa el jugador con el foco y se copia la posicion
    if (const CVector pos = FindPlayerCentreOfWorld(CWorld::PlayerInFocus); pos.z < 950.f || !m_pCurrZoneInfo) {
        m_pCurrZoneInfo = CTheZones::GetZoneInfo(pos, &m_pCurrZone);
        m_nCurrentZoneType = m_pCurrZoneInfo->PopType;
    }

    UpdatePercentages();
    UpdateDealerStrengths();
    UpdateAreaDodgyness();
    UpdateIsGangArea();
}

// 0x610560
void CPopCycle::UpdateAreaDodgyness() {
    // Como el original: se acumula banda a banda (x0.01 cada una) sobre DealerStrength * 0.07
    auto dodgyness = (float)m_pCurrZoneInfo->DealerStrength * 0.07f;
    for (const auto strength : m_pCurrZoneInfo->GangStrength) {
        dodgyness = (float)strength * 0.01f + dodgyness;
    }
    m_fCurrentZoneDodgyness = dodgyness < 1.0f ? dodgyness : 1.0f;
}

// 0x6104B0
void CPopCycle::UpdateDealerStrengths() {
    if (!CGangWars::bGangWarsActive) {
        return;
    }

    // Como el original: solo actua cuando cambia el minuto de juego
    if (CTimer::m_snTimeInMilliseconds / 60000 == CTimer::m_snPreviousTimeInMilliseconds / 60000) {
        return;
    }

    // Como el original: recorre las TotalNumberOfZoneInfos zonas de informacion (contador sin signo)
    for (auto i = 0; i < (int32)(uint16)CTheZones::TotalNumberOfZoneInfos; i++) {
        auto& zone = CTheZones::ZoneInfoArray.data()[i];
        const auto Chk = [&](eGangID gangId) { return zone.GangStrength[gangId] > 10u; };
        if (!Chk(GANG_BALLAS) && !Chk(GANG_GROVE) && !Chk(GANG_VAGOS)) {
            continue;
        }

        // Como el original: el numero aleatorio se pide siempre (indice limitado a 14) y se compara rand / 32767
        constexpr float chances[]{ 0.05f, 0.2f, 0.3f, 0.35f, 0.4f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f }; // 0x8D24F0
        const auto chance = chances[std::min<uint8>(zone.DealerStrength, 14u)];
        if ((float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL < chance && zone.DealerStrength < 15u) {
            zone.DealerStrength++;
        }
    }
}

// 0x610770
void CPopCycle::UpdatePercentages() {
    // Como el original: minimo (no maximo) entre 0.1 y DealerStrength * 0.01
    const auto dealers = (float)m_pCurrZoneInfo->DealerStrength * 0.01f;
    m_fPercDealers = 0.1f < dealers ? 0.1f : dealers;

    const auto gangs = (float)(int32)m_pCurrZoneInfo->GetSumOfGangDensity() * 0.01f;
    m_fPercGangs = 0.5f < gangs ? 0.5f : gangs;
    if (!(m_fPercGangs < 0.15f)) {
        const auto cops = 0.3f - m_fPercGangs;
        m_fPercCops = 0.03f > cops ? 0.03f : cops;
    } else {
        m_fPercCops = 0.02f > m_fPercGangs ? 0.02f : m_fPercGangs;
    }

    // 0x610881
    switch (m_pCurrZoneInfo->PopType) {
    case POPCYCLE_PEDGROUP_BUSINESS_SF:
    case POPCYCLE_PEDGROUP_CASUAL_RICH_LA:
    case POPCYCLE_PEDGROUP_CASUAL_RICH_VG:
        m_fPercCops = m_fPercCops > 0.1f ? m_fPercCops : 0.1f;
        break;
    case POPCYCLE_PEDGROUP_BUSINESS_VG:
        m_fPercCops = m_fPercCops > 0.05f ? m_fPercCops : 0.05f;
        break;
    case POPCYCLE_PEDGROUP_CLUBBERS_VG:
    case POPCYCLE_PEDGROUP_CASUAL_AVERAGE_LA:
        m_fPercCops = 0.f;
        break;
    }

    // 0x610905: Como el original: suma policias + bandas + camellos; si pasa de 1 se multiplica por el reciproco
    if (const auto sum = m_fPercCops + m_fPercGangs + m_fPercDealers; sum > 1.f) {
        const auto recip = 1.f / sum;
        m_fPercOther   = 0.f;
        m_fPercDealers = m_fPercDealers * recip;
        m_fPercGangs   = m_fPercGangs * recip;
        m_fPercCops    = recip * m_fPercCops;
    } else {
        m_fPercOther = 1.f - m_fPercDealers - m_fPercGangs - m_fPercCops;
    }

    // 0x61097E: Como el original: (porcentaje de la tabla * fraccion) * 0.01; los peatones "otros" usan el entero de
    // GetCurrentPercOther_Peds (sin volver a multiplicar por la tabla)
    const auto t = m_nCurrentTimeIndex, w = m_nCurrentTimeOfWeek, z = m_nCurrentZoneType;
    const auto kDealers   = (float)m_nPercDealers[t][w][z] * m_fPercDealers * 0.01f;
    const auto kGangs     = (float)m_nPercGang[t][w][z] * m_fPercGangs * 0.01f;
    const auto kCops      = (float)m_nPercCops[t][w][z] * m_fPercCops * 0.01f;
    const auto kOtherCars = (float)m_nPercOther[t][w][z] * m_fPercOther * 0.01f;
    const auto kOtherPeds = (float)GetCurrentPercOther_Peds() * m_fPercOther * 0.01f;

    // 0x610A41: Como el original: durante los disturbios el maximo de peatones sube al menos a 20
    int32 maxNumPeds = GetMaxPedsCurrently();
    if (CGameLogic::LaRiotsActiveHere() && maxNumPeds <= 20) {
        maxNumPeds = 20;
    }

    // 0x610A6D
    const auto dnBalance = 1.f - CCustomBuildingDNPipeline::m_fDNBalanceParam;
    if (CGameLogic::LaRiotsActiveHere() && dnBalance > 0.5f) {
        gfLaRiotsLightMult = gfLaRiotsLightMult - 0.01f; // Decrease
        if (gfLaRiotsLightMult < 0.6f) {
            gfLaRiotsLightMult = 0.6f;
        }
    } else {
        gfLaRiotsLightMult = gfLaRiotsLightMult + 0.01f; // Increase
        if (gfLaRiotsLightMult > 1.f) {
            gfLaRiotsLightMult = 1.f;
        }
    }

    // 0x610AEC
    const auto maxPeds = (float)maxNumPeds;
    m_NumDealers_Peds = kDealers * maxPeds;
    m_NumGangs_Peds   = kGangs * maxPeds;
    m_NumCops_Peds    = kCops * maxPeds;
    m_NumOther_Peds   = maxPeds * kOtherPeds;

    const auto maxCars = (float)GetMaxCarsCurrently();
    m_NumDealers_Cars = kDealers * maxCars;
    m_NumGangs_Cars   = kGangs * maxCars;
    m_NumCops_Cars    = kCops * maxCars;
    m_NumOther_Cars   = maxCars * kOtherCars;

    if (CGameLogic::LaRiotsActiveHere()) {
        m_NumDealers_Cars *= 0.75f;
        m_NumGangs_Cars   *= 0.75f;
        m_NumCops_Cars    *= 0.75f;
        m_NumOther_Cars   *= 0.75f;
    }
}

// 0x60F8D0
ePedType CPopCycle::PickGangToCreateMembersOf() {
    if (CCheat::IsActive(CHEAT_GANGS_CONTROLS_THE_STREETS)) {
        // Como el original: 7 - (int)(rand * (1/32768) * -8.0), es decir PED_TYPE_GANG1..PED_TYPE_GANG8
        return (ePedType)(7 - (int32)((float)CGeneral::GetRandomNumber() * (1.f / 32768.f) * -8.f));
    }

    // Como el original: sin densidad de bandas devuelve 0; si no, fuerza relativa con reciprocos y el primer
    // maximo estrictamente mayor que 0 (si ninguno lo supera, la banda 0).
    const auto sumGangDensity = (float)(int32)m_pCurrZoneInfo->GetSumOfGangDensity();
    if (sumGangDensity <= 0.f) {
        return (ePedType)0;
    }
    const auto recipDensity = 1.f / sumGangDensity;
    const auto recipNumGangs = 1.f / m_NumGangs_Peds;
    auto best = 0.f;
    auto dominatingGangId = 0;
    for (auto gangId = 0; gangId < TOTAL_GANGS; gangId++) {
        const auto value = (float)m_pCurrZoneInfo->GangStrength[gangId] * recipDensity - (float)(int32)CPopulation::ms_nNumGang[gangId] * recipNumGangs;
        if (value > best) {
            best = value;
            dominatingGangId = gangId;
        }
    }
    return (ePedType)((int32)PED_TYPE_GANG1 + dominatingGangId);
}

// notsa
bool CPopCycle::IsRaceAllowedInCurrentZone(ePedRace race) {
    return race == RACE_DEFAULT || m_pCurrZoneInfo->PopRaces & (1 << (race - 1));
}

// notsa
bool CPopCycle::IsRaceAllowedInCurrentZone(eModelID pedModelId) {
    return IsRaceAllowedInCurrentZone(CModelInfo::GetPedModelInfo((int32)pedModelId)->GetRace());
}

void CPopCycle::UpdateIsGangArea() {
    m_bCurrentZoneIsGangArea = m_pCurrZoneInfo->GetSumOfGangDensity() > 20;
}
