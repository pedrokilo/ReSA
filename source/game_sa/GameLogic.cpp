/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGameLogic
 * Reconstrucción: COMPLETA — 27/27 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 27/27 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "MBlur.h"
#include "EntryExitManager.h"
#include "Garages.h"
#include "PedClothesDesc.h"
#include "PostEffects.h"
#include "X87Math.h"

void CGameLogic::InjectHooks() {
    RH_ScopedClass(CGameLogic);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CalcDistanceToForbiddenTrainCrossing, 0x4418E0);
    RH_ScopedInstall(ClearSkip, 0x441560);
    RH_ScopedInstall(DoWeaponStuffAtStartOf2PlayerGame, 0x4428B0);
    RH_ScopedInstall(FindCityClosestToPoint, 0x441B70);
    RH_ScopedInstall(ForceDeathRestart, 0x441240);
    RH_ScopedInstall(InitAtStartOfGame, 0x441210);
    RH_ScopedInstall(IsCoopGameGoingOn, 0x441390);
    RH_ScopedInstall(IsPlayerAllowedToGoInThisDirection, 0x441E10);
    RH_ScopedInstall(IsPlayerUse2PlayerControls, 0x442020);
    RH_ScopedInstall(IsPointWithinLineArea, 0x4416E0);
    RH_ScopedInstall(IsSkipWaitingForScriptToFadeIn, 0x4416C0);
    RH_ScopedInstall(LaRiotsActiveHere, 0x441C10);
    RH_ScopedInstall(Save, 0x5D33C0);
    RH_ScopedInstall(Load, 0x5D3440);
    RH_ScopedInstall(PassTime, 0x4414C0);
    RH_ScopedInstall(Remove2ndPlayerIfPresent, 0x4413C0);
    RH_ScopedInstall(ResetStuffUponResurrection, 0x442980);
    RH_ScopedInstall(StorePedsWeapons, 0x441D00);
    RH_ScopedInstall(RestorePedsWeapons, 0x441D30);
    RH_ScopedInstall(RestorePlayerStuffDuringResurrection, 0x442060);
    RH_ScopedInstall(SetPlayerWantedLevelForForbiddenTerritories, 0x441770);
    RH_ScopedInstall(SetUpSkip, 0x4423C0);
    RH_ScopedInstall(SkipCanBeActivated, 0x4415C0);
    RH_ScopedInstall(SortOutStreamingAndMemory, 0x441440);
    RH_ScopedInstall(StopPlayerMovingFromDirection, 0x441290);
    RH_ScopedInstall(Update, 0x442AD0);
    RH_ScopedInstall(UpdateSkip, 0x442480);
}

// Como el original: constantes del EXE (0x8595EC grados->radianes, 0x859878 radianes->grados, 0x858FE4 pi/2)
static constexpr float GAMELOGIC_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
static constexpr float GAMELOGIC_RAD_TO_DEG = std::bit_cast<float>(0x42652EE0u);
static constexpr float GAMELOGIC_HALF_PI    = std::bit_cast<float>(0x3FC90FDBu);

// Como el original: GetHeading (0x441DB0) deja atan2(-forward.x, forward.y) en ST0 sin redondear (fpatan)
// y el llamador le resta pi/2 en x87 antes de guardarlo como float.
static float GetHeadingMinusHalfPi(const CPlaceable& placeable) {
    if (const auto* const matrix = placeable.m_matrix) {
        const float forwardX = matrix->GetForward().x;
        const float forwardY = matrix->GetForward().y;
        return HeadingMinusHalfPiCpp(forwardX, forwardY, GAMELOGIC_HALF_PI);
    }
    return placeable.m_placement.m_fHeading - GAMELOGIC_HALF_PI;
}

// 0x4418E0
float CGameLogic::CalcDistanceToForbiddenTrainCrossing(CVector point, CVector moveSpeed, bool ignoreMoveSpeed, CVector& outDistance) {
    auto closest = 100'000.0f;

    // Como el original: el cruce tiene z = 0, así que la dirección es (cx - x, cy - y, -z); el producto escalar
    // con la velocidad es 3D en el orden z, y, x y la distancia es 2D.
    const auto Calculate = [&](float crossingX, float crossingY) {
        const float dirX = crossingX - point.x;
        const float dirY = crossingY - point.y;
        const float dirZ = -point.z;
        const float dot  = dirZ * moveSpeed.z + dirY * moveSpeed.y + dirX * moveSpeed.x;
        if (!(dot > 0.0f) && !ignoreMoveSpeed) {
            return;
        }
        const float dx   = point.x - crossingX;
        const float dy   = point.y - crossingY;
        const float dist = std::sqrt(dy * dy + dx * dx);
        if (dist < closest) {
            outDistance = CVector{ dirX, dirY, dirZ };
            closest     = dist;
        }
    };

    // Como el original: tres comprobaciones independientes (== 0, == 1 y <= 1), cada una con su GetStatValue
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) == 0.0f) {
        Calculate(82.0f, -1021.0f); // LS-SF; Flint Country
    }
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) == 1.0f) {
        Calculate(-1568.0f, 537.0f); // SF-LV; Downtown
    }
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) <= 1.0f) {
        Calculate(2770.0f, 277.0f); // Como el original: (2770, 277), Palomino Creek
    }

    return closest;
}

// 0x441560
void CGameLogic::ClearSkip(bool afterMission) {
    if (afterMission && SkipState == SKIP_AFTER_MISSION)
        return;

    if (SkipState == SKIP_IN_PROGRESS) {
        TheCamera.SetFadeColour(0, 0, 0);
        TheCamera.Fade(0.5f, eFadeFlag::FADE_OUT);
    }
    SkipState = SKIP_NONE;
    CPad::GetPad(PED_TYPE_PLAYER1)->bPlayerSkipsToDestination = false; // Como el original: bit 0 del byte 0x10F
}

// 0x4428B0
void CGameLogic::DoWeaponStuffAtStartOf2PlayerGame(bool shareWeapons) {
    // Como el original: guarda las armas del jugador 1 (StorePedsWeapons en línea), no las restaura
    StorePedsWeapons(FindPlayerPed(PED_TYPE_PLAYER1));

    if (shareWeapons) {
        for (auto& weapon : FindPlayerPed(PED_TYPE_PLAYER1)->m_aWeapons) {
            if (weapon.m_Type == WEAPON_UNARMED)
                continue;

            FindPlayerPed(PED_TYPE_PLAYER2)->GiveWeapon(weapon.m_Type, weapon.m_TotalAmmo, true);
        }
        FindPlayerPed(PED_TYPE_PLAYER1)->PickWeaponAllowedFor2Player();
        FindPlayerPed(PED_TYPE_PLAYER2)->GetPlayerData()->m_nChosenWeapon = FindPlayerPed(PED_TYPE_PLAYER1)->GetPlayerData()->m_nChosenWeapon;
    }
}

// 0x441B70
// 1 - Los Santos, 2 - San Fierro, 3 - Las Venturas
int32 CGameLogic::FindCityClosestToPoint(CVector2D point) {
    const auto Distance = [&](float x, float y) {
        const float dx = point.x - x;
        const float dy = point.y - y;
        return std::sqrt(dy * dy + dx * dx);
    };

    // Como el original: orden LS, LV, SF con comparación estricta (empates y NaN se quedan con el anterior)
    float      closest = Distance(1670.0f, -1137.0f);
    eLevelName level   = LEVEL_NAME_LOS_SANTOS;
    if (const auto d = Distance(2161.0f, 2140.0f); d < closest) {
        closest = d;
        level   = LEVEL_NAME_LAS_VENTURAS;
    }
    if (Distance(-1810.0f, 884.0f) < closest) {
        return LEVEL_NAME_SAN_FIERRO;
    }
    return level;
}

// 0x441240
void CGameLogic::ForceDeathRestart() {
    CWorld::Players[CWorld::PlayerInFocus].m_nPlayerState = PLAYERSTATE_HAS_DIED;
    GameState = GAMELOGIC_STATE_WASTED;
    TimeOfLastEvent = CTimer::GetTimeInMS() - 3001;
    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(4.0f, eFadeFlag::FADE_IN);
}

// 0x441210
void CGameLogic::InitAtStartOfGame() {
    ZoneScoped;

    ActivePlayers            = 1;
    SkipState                = SKIP_NONE;
    NumAfterDeathStartPoints = 0;
    GameState                = GAMELOGIC_STATE_PLAYING;
    TimeOfLastEvent          = 0;
    nPrintFocusHelpTimer     = 0;
    nPrintFocusHelpCounter   = 0;
    bScriptCoopGameGoingOn   = false;
}

// 2 players are playing
// 0x441390
bool CGameLogic::IsCoopGameGoingOn() {
    // Como el original: lee CWorld::Players directamente
    return CWorld::Players[PED_TYPE_PLAYER1].m_pPed && CWorld::Players[PED_TYPE_PLAYER2].m_pPed;
}

// 0x441E10
bool CGameLogic::IsPlayerAllowedToGoInThisDirection(CPed* ped, CVector moveDirection, float distanceLimit) {
    const auto player1 = CWorld::Players[PED_TYPE_PLAYER1].m_pPed;
    const auto player2 = CWorld::Players[PED_TYPE_PLAYER2].m_pPed;

    if (!player1 || !player2 || !bLimitPlayerDistance)
        return true;

    // Como el original: comparación sin signo (< 2) y el jugador en foco se lee de CWorld::Players
    if (static_cast<uint32>(n2PlayerPedInFocus) < 2) {
        if (ped == CWorld::Players[static_cast<uint32>(n2PlayerPedInFocus)].m_pPed) {
            return true;
        }

        CVector headPos{0.0f, 0.0f, 0.2f};
        ped->GetTransformedBonePosition(headPos, BONE_HEAD, false);

        if (!ped->GetIsOnScreen()) {
            return false;
        }

        if (!CWorld::GetIsLineOfSightClear(*TheCamera.GetGameCamPosition(), headPos, true, false, false, false, false, true, false)) {
            return false;
        }
    }

    const auto& pos1 = player1->GetPosition();
    const auto& pos2 = player2->GetPosition();
    const float dx = pos1.x - pos2.x;
    const float dy = pos1.y - pos2.y;
    if (MaxPlayerDistance + distanceLimit > std::sqrt(dy * dy + dx * dx)) {
        return true;
    }

    // Como el original: producto escalar en el orden z, y, x; el jugador 1 solo puede acercarse (< 0)
    // y el resto solo alejarse en sentido contrario (> 0)
    const float dirDist = (pos1.z - pos2.z) * moveDirection.z + dy * moveDirection.y + dx * moveDirection.x;
    if (ped == player1) {
        return dirDist < 0.0f;
    }
    return dirDist > 0.0f;
}

// 0x442020
bool CGameLogic::IsPlayerUse2PlayerControls(CPed* ped) {
    if (!CWorld::Players[PED_TYPE_PLAYER1].m_pPed || !CWorld::Players[PED_TYPE_PLAYER2].m_pPed)
        return false;

    // Como el original: sin jugador en foco se usan los controles de 2 jugadores
    if (n2PlayerPedInFocus == eFocusedPlayer::NONE)
        return true;

    return ped != CWorld::Players[static_cast<int32>(n2PlayerPedInFocus)].m_pPed;
}

// 0x4416E0
bool CGameLogic::IsPointWithinLineArea(const CVector* points, uint32 numPoints, float x, float y) {
    // Como el original: cuenta los cruces del rayo (x, y) -> (x + 1e6, y) con cada lado y devuelve la paridad
    uint32 crossings = 0;
    for (int32 i = 0; i < static_cast<int32>(numPoints); i++) {
        const auto& point     = points[i];
        const auto& nextPoint = points[(i + 1) % static_cast<int32>(numPoints)];
        if (CCollision::Test2DLineAgainst2DLine(x, y, 1'000'000.0f, 0.0f, point.x, point.y, nextPoint.x - point.x, nextPoint.y - point.y)) {
            crossings++;
        }
    }

    return crossings & 1;
}

// 0x4416C0
bool CGameLogic::IsSkipWaitingForScriptToFadeIn() {
    if (CGameLogic::SkipState != SKIP_WAITING_SCRIPT) {
        return false;
    }
    CGameLogic::SkipState = SKIP_NONE;
    return true;
}

// 0x441C10
bool CGameLogic::LaRiotsActiveHere() {
    if (FindPlayerCoors().z > 950.0f) {
        return false;
    }

    if (CCheat::IsActive(CHEAT_RIOT_MODE)) {
        return true;
    }

    if (!gbLARiots) {
        return false;
    }

    // Como el original: límites estrictos en los dos ejes
    const auto coors = FindPlayerCoors();
    if (coors.x > 1620.0f && coors.x < 2824.0f && coors.y > -2178.0f && coors.y < -1067.0f) {
        return true;
    }
    return coors.x > 157.0f && coors.x < 1630.0f && coors.y > -1950.0f && coors.y < -1192.0f;
}

// 0x5D33C0
bool CGameLogic::Save() {
    // Como el original: se guardan las globales directamente (GameState como byte) y devuelve true
    CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::NumAfterDeathStartPoints);
    CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::bPenaltyForDeathApplies);
    CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::bPenaltyForArrestApplies);
    CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::GameState);
    CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::TimeOfLastEvent);

    for (int32 i = 0; i < NumAfterDeathStartPoints; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::AfterDeathStartPoints.data()[i]);
        CGenericGameStorage::SaveDataToWorkBuffer(CGameLogic::AfterDeathStartPointOrientations.data()[i]);
    }
    return true;
}

// 0x5D3440
bool CGameLogic::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::NumAfterDeathStartPoints);
    CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::bPenaltyForDeathApplies);
    CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::bPenaltyForArrestApplies);
    CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::GameState);
    CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::TimeOfLastEvent);
    for (int32 i = 0; i < NumAfterDeathStartPoints; ++i) {
        CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::AfterDeathStartPoints.data()[i]);
        CGenericGameStorage::LoadDataFromWorkBuffer(CGameLogic::AfterDeathStartPointOrientations.data()[i]);
    }
    return true;
}

// 0x4414C0
void CGameLogic::PassTime(uint32 minutes) {
    int32 weekDay = CClock::GetGameWeekDay();
    int32 hours   = CClock::GetGameClockHours();
    int32 mins    = static_cast<int32>(minutes) + CClock::GetGameClockMinutes();

    // Como el original: enteros de 32 bits (comparaciones con signo, divisiones sin signo)
    if (mins >= 60) {
        hours += static_cast<int32>(static_cast<uint32>(mins) / 60);
        mins   = static_cast<int32>(static_cast<uint32>(mins) % 60);
    }

    if (hours >= 24) {
        auto days = static_cast<uint32>(hours) / 24;
        hours -= static_cast<int32>(days) * 24;

        // Como el original: un día cada vez (día de la semana con vuelta a 1 y +1 a la estadística)
        do {
            if (++weekDay > 7) {
                weekDay = 1;
            }
            CStats::IncrementStat(STAT_DAYS_PASSED_IN_GAME, 1.0f);
        } while (--days);
    }

    CClock::SetGameClock(static_cast<uint8>(hours), static_cast<uint8>(mins), static_cast<uint8>(weekDay));
    CPickups::PassTime(minutes * 1000);
}

// 0x4413C0
void CGameLogic::Remove2ndPlayerIfPresent() {
    if (auto ped = CWorld::Players[PED_TYPE_PLAYER2].m_pPed) {
        CWorld::Remove(ped);
        delete CWorld::Players[PED_TYPE_PLAYER2].m_pPed;
        CWorld::Players[PED_TYPE_PLAYER2].m_pPed = nullptr;

        auto player1 = CWorld::Players[PED_TYPE_PLAYER1].m_pPed;
        CClothes::RebuildPlayer(player1, false);
        player1->CantBeKnockedOffBike = CANT_BE_KNOCKED_OFF_DEFAULT;
    }
    n2PlayerPedInFocus = eFocusedPlayer::NONE;
}

// 0x442980
void CGameLogic::ResetStuffUponResurrection() {
    auto& player = CWorld::Players[CWorld::PlayerInFocus];

    CMessages::ClearMessages(false);
    CCarCtrl::ClearInterestingVehicleList();
    CWorld::ClearExcitingStuffFromArea(player.GetPos(), 4000.0f, true);
    PassTime(12 * 60);
    // Como el original: rotación por 57.29578 (0x859878) y el peatón se vuelve a leer de CWorld::Players
    RestorePlayerStuffDuringResurrection(player.m_pPed, player.m_pPed->GetPosition(), player.m_pPed->m_fCurrentRotation * GAMELOGIC_RAD_TO_DEG);
    const float heading = player.m_pPed->GetHeading();
    SortOutStreamingAndMemory(player.m_pPed->GetPosition(), heading);
    TheCamera.m_fCamShakeForce = 0.0f;
    TheCamera.SetMotionBlur(0, 0, 0, 0, eMotionBlurType::NONE);
    CPad::GetPad(PED_TYPE_PLAYER1)->StopShaking(0);
    CReferences::RemoveReferencesToPlayer();
    CCarCtrl::CountDownToCarsAtStart = 10;
    CPopulation::m_CountDownToPedsAtStart = 10;
    CPad::GetPad(CWorld::PlayerInFocus)->DisablePlayerControls = 0;

    player.m_pPed->ClearWeapons();
    GameState = GAMELOGIC_STATE_PLAYING;
    TimeOfLastEvent = 0;
}

// used in CGameLogic::DoWeaponStuffAtStartOf2PlayerGame
// 0x441D00
void CGameLogic::StorePedsWeapons(CPed* ped) {
    // Como el original: copia byte a byte (rep movs)
    std::memcpy(SavedWeapons.data(), ped->m_aWeapons.data(), sizeof(CWeapon) * NUM_WEAPON_SLOTS);
}

// 0x441D30
void CGameLogic::RestorePedsWeapons(CPed* ped) {
    ped->ClearWeapons();
    for (auto& weapon : SavedWeapons) {
        const auto modelId1 = CWeaponInfo::GetWeaponInfo(weapon.m_Type, eWeaponSkill::STD)->m_nModelId1;
        const auto modelId2 = CWeaponInfo::GetWeaponInfo(weapon.m_Type, eWeaponSkill::STD)->m_nModelId2;
        const auto IsLoaded = [](int32 modelId) { return CStreaming::ms_aInfoForModel.data()[modelId].m_LoadState == LOADSTATE_LOADED; };

        // Como el original: el segundo modelo se comprueba con el estado de carga del primero
        if ((modelId1 == MODEL_INVALID || IsLoaded(modelId1)) && (modelId2 == MODEL_INVALID || IsLoaded(modelId1))) {
            ped->GiveWeapon(weapon.m_Type, weapon.m_TotalAmmo, true);
        }
    }
}

// 0x442060
void CGameLogic::RestorePlayerStuffDuringResurrection(CPlayerPed* player, CVector posn, float playerStartHeading) {
    ClearSkip(false);

    if (player->m_fHealth <= 0.0f) {
        CStats::UpdateStatsOnRespawn();
    }
    auto playerInfo = player->GetPlayerInfoForThisPlayerPed();
    auto playerData = player->GetPlayerData();

    player->physicalFlags.bRenderScorched = false;
    player->m_fArmour = 0.0f;
    player->m_fHealth = static_cast<float>(playerInfo->m_nMaxHealth);
    player->SetIsVisible(true);
    player->m_nDeathTimeMS = 0;
    player->bDoBloodyFootprints = false;
    playerData->m_nDrunkenness = 0;
    playerData->m_nFadeDrunkenness = 0;
    CMBlur::ClearDrunkBlur();
    playerData->m_nDrugLevel = 0;
    player->ClearAdrenaline();
    player->ResetSprintEnergy();
    if (player->m_pFire) {
        // Como el original: el puntero se limpia después de Extinguish
        player->m_pFire->SetIsScript(false);
        player->m_pFire->Extinguish();
        player->m_pFire = nullptr;
    }
    player->GetAE().TurnOffJetPack();
    player->bInVehicle = false;
    if (player->m_pVehicle) {
        // Como el original: se limpia la referencia guardada en el propio peatón (&m_pVehicle)
        player->m_pVehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&player->m_pVehicle));
    }
    player->m_pVehicle = nullptr;
    player->GetWanted()->Reset();
    player->RestartNonPartialAnims();
    playerInfo->MakePlayerSafe(false, 10'000.0f);
    player->m_bRemoveFromWorld = false;
    player->ClearWeaponTarget();
    player->SetInitialState(false);
    player->GetAcquaintance() = CPedType::GetPedTypeAcquaintances(player->m_nPedType);
    CCarCtrl::ClearInterestingVehicleList();

    player->Teleport(CVector{ posn.x, posn.y, posn.z + 1.0f }, false);
    // Como el original: también anula la velocidad
    player->m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f };
    player->m_fCurrentRotation = player->m_fAimingRotation = playerStartHeading * GAMELOGIC_DEG_TO_RAD;
    player->SetHeading(player->m_fCurrentRotation);

    CTheScripts::ClearSpaceForMissionEntity(posn, player);
    CWorld::ClearExcitingStuffFromArea(posn, 4000.0f, true);
    player->RestoreHeadingRate();
    player->SetAreaCode(AREA_CODE_NORMAL_WORLD);
    player->m_pEnex = 0;
    CEntryExitManager::ms_entryExitStackPosn = 0;
    CGame::currArea = AREA_CODE_NORMAL_WORLD;
    CPopulation::bInPoliceStation = 0;
    CStreaming::RemoveBuildingsNotInArea(AREA_CODE_NORMAL_WORLD);
    TheCamera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
    TheCamera.RestoreWithJumpCut();
    CReferences::RemoveReferencesToPlayer();
    CGarages::PlayerArrestedOrDied();
    CStats::SetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT, 0.0f);
    CShopping::RemoveLoadedShop();
    CWorld::Remove(player);
    CWorld::Add(player);
    CHud::ResetWastedText();
    CStreaming::StreamZoneModels(posn);
    CPostEffects::m_waterEnable = false;
    CTimeCycle::StopExtraColour(0);
    CPostEffects::ScriptResetForEffects();

    if (auto clothesDesc = playerData->m_pPedClothesDesc; clothesDesc->m_anModelKeys[9]) {
        clothesDesc->SetTextureAndModel(static_cast<uint32>(0), static_cast<uint32>(0), CLOTHES_TEXTURE_SPECIAL); // Como el original: versión con hashes (0x5A8050)
        CClothes::RebuildPlayer(player, false);
    } else {
        CClothes::RebuildPlayerIfNeeded(player);
    }
    CAudioZones::Update(true, posn);
    AudioEngine.StopPoliceScanner(true);
    CWaterLevel::FindNearestWaterAndItsFlow();
    TheCamera.m_fDistanceToWater = std::bit_cast<float>(0x47C34FF3u); // Como el original: 99999.9f, no 100000.0f
}

// @brief Set wanted level to 4 if player is in a forbidden territory.
// @param immediately   Do position check without waiting for the frame counter.
// @addr 0x441770
void CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool immediately) {
    if (!immediately && (CTimer::GetFrameCounter() % 32) != 18)
        return;

    // Como el original: posición de FindPlayerCoors (la del vehículo si va en uno), leída en cada comprobación
    if (FindPlayerCoors().z > 950.0f)
        return;

    if (FindPlayerPed()->GetIntelligence()->GetTaskSwim())
        return;

    // Como el original: nivel de búsqueda comparado con signo
    if (static_cast<int32>(FindPlayerPed()->GetWanted()->GetWantedLevel()) >= static_cast<int32>(eWantedLevel::WANTED_LEVEL_4))
        return;

    const auto SetWantedIfInArea = [&](const CVector* vertices, size_t size) {
        const auto coords = FindPlayerCoors();
        if (IsPointWithinLineArea(vertices, static_cast<uint32>(size), coords.x, coords.y)) {
            FindPlayerPed()->SetWantedLevel(eWantedLevel::WANTED_LEVEL_4);
            if (immediately) {
                FindPlayerWanted()->m_LastTimeWantedLevelChanged = 0;
            }
        }
    };

    // 0x8A5EC8; Las Venturas
    constexpr CVector town1ShapeVertices[] = {
        {3000.0f, 535.0f, 0.0f},
        {1759.0f, 576.0f, 0.0f},
        {989.0f, 693.0f, 0.0f},
        {-128.0f, 490.0f, 0.0f},
        {-845.0f, 707.0f, 0.0f},
        {-1477.0f, 1677.0f, 0.0f},
        {-2154.0f, 2497.0f, 0.0f},
        {-2971.0f, 2180.0f, 0.0f},
        {-3000.0f, 3000.0f, 0.0f},
        {3000.0f, 3000.0f, 0.0f}
    };

    // 0x8A5E58; San Fierro
    constexpr CVector town2ShapeVertices[] = {
        {28.0f, -3000.0f, 0.0f},
        {-30.0f, -1280.0f, 0.0f},
        {-148.0f, -911.0f, 0.0f},
        {-487.0f, -372.0f, 0.0f},
        {-1028.0f, -424.0f, 0.0f},
        {-1145.0f, 479.0f, 0.0f},
        {-1461.0f, 1488.0f, 0.0f},
        {-3000.0f, 1668.0f, 0.0f},
        {-3000.0f, -3000.0f, 0.0f}
    };

    // LV
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) <= 1.0f) {
        SetWantedIfInArea(town1ShapeVertices, std::size(town1ShapeVertices));
    }

    // SF
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) <= 0.0f) {
        SetWantedIfInArea(town2ShapeVertices, std::size(town2ShapeVertices));
    }
}

// 0x4423C0
void CGameLogic::SetUpSkip(CVector coors, float angle, bool afterMission, CEntity* vehicle, bool finishedByScript) {
    if (SkipState == SKIP_IN_PROGRESS) {
        TheCamera.SetFadeColour(0, 0, 0);
        TheCamera.Fade(0.5f, eFadeFlag::FADE_OUT);
    }
    SkipState = SKIP_NONE;
    CPad::GetPad(PED_TYPE_PLAYER1)->bPlayerSkipsToDestination = false;
    SkipPosition = coors;
    SkipHeading = angle; // Como el original: también guarda el ángulo
    SkipState = afterMission ? SKIP_AFTER_MISSION : SKIP_AVAILABLE;
    SkipTimer = CTimer::GetTimeInMS();
    // Como el original: SkipVehicle siempre se asigna y la referencia se registra sobre &SkipVehicle
    SkipVehicle = static_cast<CVehicle*>(vehicle);
    if (vehicle) {
        vehicle->RegisterReference(reinterpret_cast<CEntity**>(&SkipVehicle));
    }
    SkipToBeFinishedByScript = finishedByScript;
}

// 0x4415C0
bool CGameLogic::SkipCanBeActivated() {
    if (!CGame::CanSeeOutSideFromCurrArea() || TheCamera.m_bFading || (SkipState != SKIP_AVAILABLE && SkipState != SKIP_AFTER_MISSION))
        return false;

    if (auto vehicle = FindPlayerVehicle()) {
        switch (vehicle->m_nVehicleSubType) {
        case VEHICLE_TYPE_BIKE:
        case VEHICLE_TYPE_AUTOMOBILE:
        case VEHICLE_TYPE_MTRUCK:
        case VEHICLE_TYPE_QUAD:
        case VEHICLE_TYPE_BMX:
            if (!SkipVehicle || SkipVehicle == vehicle) {
                return true;
            }
            break;
        }
    }

    return SkipState == SKIP_AFTER_MISSION && FindPlayerPed()->GetIntelligence()->GetTaskSwim();
}

// 0x441440
void CGameLogic::SortOutStreamingAndMemory(const CVector& translation, float angle) {
    CTimer::Stop();
    RwCameraSetFarClipPlane(TheCamera.m_pRwCamera, CTimeCycle::FindFarClipForCoors(translation));
    CStreaming::FlushRequestList();
    CStreaming::DeleteRwObjectsAfterDeath(translation);
    CStreaming::RemoveUnusedModelsInLoadedList();
    CGame::DrasticTidyUpMemory(true);
    CStreaming::LoadSceneCollision(translation);
    CRenderer::RequestObjectsInDirection(translation, angle, STREAMING_LOADING_SCENE);
    CStreaming::LoadScene(translation);
    CTimer::Update();
}

// 0x441290
void CGameLogic::StopPlayerMovingFromDirection(int32 playerId, CVector direction) {
    // Como el original: peatón leído de CWorld::Players; vehículo si bInVehicle y m_pVehicle, si no el peatón
    // si está de pie o con jetpack
    auto* const ped = CWorld::Players[playerId].m_pPed;
    CPhysical* obj;
    if (ped->bInVehicle && ped->m_pVehicle) {
        obj = ped->m_pVehicle;
    } else if (ped->bIsStanding || ped->GetIntelligence()->GetTaskJetPack()) {
        obj = ped;
    } else {
        return;
    }

    if (direction.z < 0.0f) {
        direction.z = 0.0f;
    }

    if (const auto compSum = (direction * obj->m_vecMoveSpeed).ComponentwiseSum(); compSum < 0.0f) {
        obj->m_vecMoveSpeed -= direction * compSum;
    }
}

// Como el original (en línea en Update): el jugador 1 deja el vehículo antes de resucitar
static void LeaveVehicleForResurrection() {
    auto* const ped = CWorld::Players[PED_TYPE_PLAYER1].m_pPed;
    if (!ped->bInVehicle || !ped->m_pVehicle) {
        return;
    }

    auto* const vehicle = ped->m_pVehicle;
    if (vehicle->m_pDriver == ped) {
        if (vehicle->m_pDriver) {
            vehicle->m_pDriver->CleanUpOldReference(reinterpret_cast<CEntity**>(&vehicle->m_pDriver));
        }
        CWorld::Players[PED_TYPE_PLAYER1].m_pPed->m_pVehicle->m_pDriver = nullptr;
        auto* const driven = CWorld::Players[PED_TYPE_PLAYER1].m_pPed->m_pVehicle;
        if (driven->GetStatus() != STATUS_WRECKED) {
            driven->SetStatus(STATUS_ABANDONED);
        }
    } else {
        vehicle->RemovePassenger(ped);
    }
}

// Como el original (en línea en Update): posición del peatón del jugador 1, corregida si está en un interior
static CVector GetRestartSearchPosition() {
    auto* const ped = CWorld::Players[PED_TYPE_PLAYER1].m_pPed;
    CVector posn = ped->GetPosition();
    if (auto* const enEx = ped->m_pEnex) {
        enEx->GetPositionRelativeToOutsideWorld(posn);
    }
    return posn;
}

// Como el original (en línea en Update): partida a dos jugadores -> punto y rumbo de inicio (por 57.29578, 0x859878)
static void UseTwoPlayerRestartPoint(CVector& restartPoint, float& restartAngle) {
    if (CWorld::Players[PED_TYPE_PLAYER1].m_pPed && CWorld::Players[PED_TYPE_PLAYER2].m_pPed) {
        restartAngle = CGameLogic::f2PlayerStartHeading * GAMELOGIC_RAD_TO_DEG;
        restartPoint = CGameLogic::vec2PlayerStartLocation;
        CGameLogic::RestorePedsWeapons(FindPlayerPed(PED_TYPE_PLAYER1));
    }
}

// Como el original (en línea en Update): final común de la resurrección del jugador 1
static void FinishResurrection() {
    auto* const ped = CWorld::Players[PED_TYPE_PLAYER1].m_pPed;
    const float heading = ped->GetHeading();
    CGameLogic::SortOutStreamingAndMemory(ped->GetPosition(), heading);
    TheCamera.m_fCamShakeForce = 0.0f;
    TheCamera.SetMotionBlur(0, 0, 0, 0, eMotionBlurType::NONE);
    CPad::GetPad(PED_TYPE_PLAYER1)->StopShaking(0);
    CReferences::RemoveReferencesToPlayer();
    CCarCtrl::CountDownToCarsAtStart = 10;
    CPopulation::m_CountDownToPedsAtStart = 10;
    CPad::GetPad(CWorld::PlayerInFocus)->DisablePlayerControls = 0;
}

// 0x442AD0
void CGameLogic::Update() {
    ZoneScoped;

    CStats::UpdateRespectStat(0);
    CStats::UpdateSexAppealStat();
    SetPlayerWantedLevelForForbiddenTerritories(false);

    if (CCutsceneMgr::ms_cutsceneProcessing)
        return;

    auto& player1 = CWorld::Players[PED_TYPE_PLAYER1];

    // Como el original: la partida a dos jugadores se decide con CWorld::Players y las distancias con la posición de los peatones
    if (!CWorld::Players[PED_TYPE_PLAYER1].m_pPed || !CWorld::Players[PED_TYPE_PLAYER2].m_pPed) {
        UpdateSkip();
    } else {
        static float& lastPlayerDistance = StaticRef<float>(0x96AB24);
        const auto& pos1 = CWorld::Players[PED_TYPE_PLAYER1].m_pPed->GetPosition();
        const auto& pos2 = CWorld::Players[PED_TYPE_PLAYER2].m_pPed->GetPosition();
        const float dx   = pos1.x - pos2.x;
        const float dy   = pos1.y - pos2.y;
        const float dist = std::sqrt(dy * dy + dx * dx);

        if (dist > 100.0f) {
            if (GameState != GAMELOGIC_STATE_MISSION_FAILED) {
                SetMissionFailed();
            }
        } else if (dist > 85.0f && lastPlayerDistance <= 85.0f) {
            CHud::SetHelpMessage(TheText.Get("WRN_2P"), false, false, false); // any further and 2P game will end.
        } else if (dist > MaxPlayerDistance && static_cast<uint32>(nPrintFocusHelpCounter) < 5) {
            // Como el original: el contador y el temporizador solo cambian si se muestra el mensaje
            if (lastPlayerDistance <= MaxPlayerDistance) {
                if (CTimer::GetTimeInMS() > static_cast<uint32>(nPrintFocusHelpTimer)) {
                    CHud::SetHelpMessage(TheText.Get("WRN1_2P"), false, false, false); // distance limit reached
                    nPrintFocusHelpTimer = CTimer::GetTimeInMS() + 15'000;
                    nPrintFocusHelpCounter++;
                }
            } else if (CTimer::GetTimeInMS() > static_cast<uint32>(nPrintFocusHelpTimer)) {
                CHud::SetHelpMessage(TheText.Get("WRN2_2P"), false, false, false); // press SELECT to toggle focus to individual player
                nPrintFocusHelpTimer = CTimer::GetTimeInMS() + 60'000;
                nPrintFocusHelpCounter++;
            }
        }

        if (n2PlayerPedInFocus != eFocusedPlayer::NONE && nPrintFocusHelpTimer != 0) {
            nPrintFocusHelpTimer = CTimer::GetTimeInMS() + 30'000;
        }
        lastPlayerDistance = dist;
    }

    for (auto i = 0; i < MAX_PLAYERS; i++) {
        auto& player = CWorld::Players[i];
        if (!player.m_pPed || player.m_nPlayerState != PLAYERSTATE_PLAYING)
            continue;

        player.m_pPed->GetTaskManager().GetSimplestActiveTask();
        if (player.m_pPed->m_nPedState == PEDSTATE_DEAD || player.m_pPed->m_nPedState == PEDSTATE_DIE && player.m_pPed->bIsDyingStuck) {
            player.m_pPed->ClearAdrenaline();
            player.KillPlayer();
            GameState = GAMELOGIC_STATE_WASTED;
            TimeOfLastEvent = CTimer::GetTimeInMS();
        }
        // Como el original: comprobación independiente (no else)
        if (player.m_pPed->m_nPedState == PEDSTATE_ARRESTED) {
            player.m_pPed->ClearAdrenaline();
            player.ArrestPlayer();
            player.m_pPed->Say(CTX_GLOBAL_ARRESTED, 2300, 1.0f, true, true, true);
            GameState = GAMELOGIC_STATE_BUSTED;
            TimeOfLastEvent = CTimer::GetTimeInMS();

            if (i == PED_TYPE_PLAYER2 && CHud::m_BigMessage[2][0] == '\0') {
                CMessages::AddBigMessage(TheText.Get("BUSTED"), 5000, STYLE_WHITE_MIDDLE);
            }
        }
    }

    const auto FadeInIfNewEvent = [] {
        if (CTimer::GetPreviousTimeInMS() - TimeOfLastEvent <= 3000) {
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(2.0f, eFadeFlag::FADE_IN);
        }
    };
    const auto FadeOut = [] {
        TheCamera.SetFadeColour(0, 0, 0);
        TheCamera.Fade(3.0f, eFadeFlag::FADE_OUT);
    };

    switch (GameState) {
    case GAMELOGIC_STATE_WASTED: {
        CWeaponEffects::ClearCrossHairs();
        if (CTimer::GetTimeInMS() - TimeOfLastEvent <= 3000)
            break;

        FadeInIfNewEvent();

        if (TheCamera.m_bFading)
            break;

        if (!IsCoopGameGoingOn() && bPenaltyForDeathApplies) {
            if (player1.m_bFreeHealthCare) {
                player1.m_bFreeHealthCare = false;
            } else {
                // Como el original: el dinero solo se toca si es positivo
                if (player1.m_nMoney > 0) {
                    player1.m_nMoney = std::max(player1.m_nMoney - 100, 0);
                }
                player1.m_pPed->ClearWeapons();
            }
        }

        player1.m_nPlayerState = PLAYERSTATE_PLAYING;
        GameState = GAMELOGIC_STATE_PLAYING;
        LeaveVehicleForResurrection();
        CMessages::ClearMessages(false);
        CHud::SetHelpMessage(nullptr, true, false, false);
        CCarCtrl::ClearInterestingVehicleList();
        CGarages::CloseHideOutGaragesBeforeSave();
        CWorld::ClearExcitingStuffFromArea(player1.GetPos(), 4000.0f, true);

        CVector restartPoint;
        float restartAngle;
        CRestart::FindClosestHospitalRestartPoint(GetRestartSearchPosition(), restartPoint, restartAngle);
        UseTwoPlayerRestartPoint(restartPoint, restartAngle);
        Remove2ndPlayerIfPresent();
        PassTime(12 * 60);
        CWorld::Players[CWorld::PlayerInFocus].m_nNumHoursDidntEat = 0;
        RestorePlayerStuffDuringResurrection(player1.m_pPed, restartPoint, restartAngle);
        FinishResurrection();

        // Como el original: la primera vez solo se activa la bandera global
        if (!CRestart::bFadeInAfterNextDeath) {
            CRestart::bFadeInAfterNextDeath = true;
            break;
        }
        FadeOut();
        break;
    }
    case GAMELOGIC_STATE_BUSTED: {
        CWeaponEffects::ClearCrossHairs();
        const uint32 delta = CTimer::GetTimeInMS() - TimeOfLastEvent;
        if (delta > 3000) {
            FadeInIfNewEvent();
        }

        if (!CTheScripts::IsPlayerOnAMission() && !player1.m_nBustedAudioStatus) {
            if (CGeneral::GetRandomIntInRange(0, 4)) {
                player1.m_nBustedAudioStatus = 2;
            } else {
                player1.m_nBustedAudioStatus = 1;
                player1.m_nLastBustMessageNumber = player1.m_nLastBustMessageNumber % 28 + 1;
            }
        }

        if (delta < 4000 || TheCamera.m_bFading)
            break;

        player1.m_nPlayerState = PLAYERSTATE_PLAYING;
        GameState = GAMELOGIC_STATE_PLAYING;

        if (!IsCoopGameGoingOn()) {
            // Como el original: la multa sale del CWanted de CWorld::Players[0] (por defecto 100)
            int32 fee;
            switch (static_cast<uint32>(player1.m_PlayerData.m_pWanted->GetWantedLevel())) {
            case 2:  fee = 200;  break;
            case 3:  fee = 400;  break;
            case 4:  fee = 600;  break;
            case 5:  fee = 900;  break;
            case 6:  fee = 1500; break;
            default: fee = 100;  break;
            }
            if (bPenaltyForArrestApplies) {
                if (player1.m_bGetOutOfJailFree) {
                    player1.m_bGetOutOfJailFree = false;
                } else {
                    if (player1.m_nMoney > 0) {
                        player1.m_nMoney = std::max(player1.m_nMoney - fee, 0);
                    }
                    player1.m_pPed->ClearWeapons();
                }
            }
        }

        LeaveVehicleForResurrection();
        CMessages::ClearMessages(false);
        CHud::SetHelpMessage(nullptr, true, false, false);
        CCarCtrl::ClearInterestingVehicleList();
        CGarages::CloseHideOutGaragesBeforeSave();
        CWorld::ClearExcitingStuffFromArea(player1.GetPos(), 4000.0f, true);

        CVector restartPoint;
        float restartAngle;
        CRestart::FindClosestPoliceRestartPoint(GetRestartSearchPosition(), restartPoint, restartAngle);
        UseTwoPlayerRestartPoint(restartPoint, restartAngle);
        Remove2ndPlayerIfPresent();
        PassTime(12 * 60);
        RestorePlayerStuffDuringResurrection(player1.m_pPed, restartPoint, restartAngle);
        FinishResurrection();

        if (!CRestart::bFadeInAfterNextArrest) {
            CRestart::bFadeInAfterNextArrest = true;
            break;
        }
        FadeOut();
        break;
    }
    case GAMELOGIC_STATE_MISSION_FAILED:
    case GAMELOGIC_STATE_MISSION_PASSED: {
        if (CTimer::GetTimeInMS() - TimeOfLastEvent <= 3000)
            break;

        FadeInIfNewEvent();

        if (TheCamera.m_bFading)
            break;

        player1.m_nPlayerState = PLAYERSTATE_PLAYING;
        GameState = GAMELOGIC_STATE_PLAYING;
        LeaveVehicleForResurrection();
        // Como el original: sin mensaje de ayuda, sin PassTime y siempre con fundido
        CMessages::ClearMessages(false);
        CCarCtrl::ClearInterestingVehicleList();
        CGarages::CloseHideOutGaragesBeforeSave();
        CWorld::ClearExcitingStuffFromArea(player1.GetPos(), 4000.0f, true);

        CVector restartPoint;
        float restartAngle;
        CRestart::FindClosestPoliceRestartPoint(GetRestartSearchPosition(), restartPoint, restartAngle);
        UseTwoPlayerRestartPoint(restartPoint, restartAngle);
        Remove2ndPlayerIfPresent();
        RestorePlayerStuffDuringResurrection(player1.m_pPed, restartPoint, restartAngle);
        FinishResurrection();
        FadeOut();
        break;
    }
    }

    if (bLimitPlayerDistance && CWorld::Players[PED_TYPE_PLAYER1].m_pPed && CWorld::Players[PED_TYPE_PLAYER2].m_pPed) {
        const auto& pos1 = CWorld::Players[PED_TYPE_PLAYER1].m_pPed->GetPosition();
        const auto& pos2 = CWorld::Players[PED_TYPE_PLAYER2].m_pPed->GetPosition();
        const float dx = pos1.x - pos2.x;
        const float dy = pos1.y - pos2.y;
        if (std::sqrt(dy * dy + dx * dx) > MaxPlayerDistance) {
            // Como el original: CVector::Normalise (0x59C910) sobre la diferencia de los peatones
            CVector dir{ pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z };
            dir.Normalise();
            StopPlayerMovingFromDirection(PED_TYPE_PLAYER1, dir);
            StopPlayerMovingFromDirection(PED_TYPE_PLAYER2, CVector{ -dir.x, -dir.y, -dir.z });
        }
    }
}

// 0x442480
void CGameLogic::UpdateSkip() {
    const auto IsPlayerNearSkipPosition = [] {
        const auto coors = FindPlayerCoors();
        const float dx = coors.x - SkipPosition.x;
        const float dy = coors.y - SkipPosition.y;
        return std::sqrt(dy * dy + dx * dx) < 25.0f;
    };
    if (SkipState == SKIP_AVAILABLE && IsPlayerNearSkipPosition() ||
        CanPlayerTripSkip() && CTimer::GetTimeInMS() > SkipTimer + 35'000) {
        SkipState = SKIP_NONE;
        return;
    }

    switch (SkipState) {
    case SKIP_AVAILABLE:
    case SKIP_AFTER_MISSION: {
        if (!SkipCanBeActivated()) {
            return;
        }

        // TRIP SKIP
        // Como el original: primero la cruceta y después la tarea de salir del coche
        if (auto pad = CPad::GetPad(PED_TYPE_PLAYER1); pad->IsDPadRightPressed()) {
            if (FindPlayerPed()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
                return;
            }
            SkipState = SKIP_IN_PROGRESS;
            CPad::GetPad(PED_TYPE_PLAYER1)->bPlayerSkipsToDestination = true;

            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(2.5f, eFadeFlag::FADE_IN);
            SkipTimer = CTimer::GetTimeInMS() + 3000;
            CMessages::AddBigMessage(TheText.Get("SKIP"), 4500, STYLE_BOTTOM_RIGHT);
        }
        break;
    }
    case SKIP_IN_PROGRESS: {
        // Como el original: continúa solo cuando el tiempo supera estrictamente al temporizador
        if (CTimer::GetTimeInMS() <= SkipTimer)
            return;

        CTimer::Suspend();
        CStreaming::LoadSceneCollision(SkipPosition);
        CStreaming::LoadScene(SkipPosition);
        const auto coors = FindPlayerCoors();
        const float dx = SkipPosition.x - coors.x;
        const float dy = SkipPosition.y - coors.y;
        const float playerDistToSkipPos = std::sqrt(dy * dy + dx * dx);

        if (auto vehicle = FindPlayerVehicle()) {
            CTheScripts::ClearSpaceForMissionEntity(SkipPosition, vehicle);

            vehicle->Teleport(SkipPosition, false);
            vehicle->SetHeading(SkipHeading * GAMELOGIC_DEG_TO_RAD);

            if (vehicle->IsBike()) {
                vehicle->AsBike()->PlaceOnRoadProperly();
            } else if (vehicle->IsAutomobile()) {
                vehicle->AsAutomobile()->PlaceOnRoadProperly();
            }

            // Como el original: forward * 0.4f (operator* 0x40FEC0) con la matriz directa
            vehicle->m_vecMoveSpeed = vehicle->m_matrix->GetForward() * 0.4f; // Make the vehicle appear as it's driving to the point.
            CCamera::GetActiveCamera().m_fHorizontalAngle = GetHeadingMinusHalfPi(*vehicle);
        } else {
            auto player = FindPlayerPed();
            CTheScripts::ClearSpaceForMissionEntity(SkipPosition, player);
            player->Teleport(SkipPosition, false);
            player->GetPosition().z = CWorld::FindGroundZForCoord(SkipPosition.x, SkipPosition.y) + 1.0f;
        }
        SkipTimer = CTimer::GetTimeInMS() + 1500;
        CHud::SetMessage(nullptr);
        CTimer::Resume();
        // Como el original: dist * 0.05f + 23.0f truncado con _ftol2
        PassTime(static_cast<uint32>(static_cast<int64>(playerDistToSkipPos * 0.05f + 23.0f)));

        if (SkipToBeFinishedByScript) {
            SkipState = SKIP_WAITING_SCRIPT;
            CPad::GetPad(PED_TYPE_PLAYER1)->bPlayerSkipsToDestination = false;
        } else {
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(1.5f, eFadeFlag::FADE_OUT);
            SkipState = SKIP_FADING_OUT;
        }
        break;
    }
    case SKIP_FADING_OUT:
        if (CTimer::GetTimeInMS() > SkipTimer) {
            SkipState = SKIP_NONE;
            CPad::GetPad(PED_TYPE_PLAYER1)->bPlayerSkipsToDestination = false;
        }
        break;

    case SKIP_WAITING_SCRIPT:
    default:
        return;
    }
}

// notsa
bool CGameLogic::IsAPlayerInFocusOn2PlayerGame() {
    return n2PlayerPedInFocus == eFocusedPlayer::PLAYER1 || n2PlayerPedInFocus == eFocusedPlayer::PLAYER2;
}

// notsa
CPlayerPed* CGameLogic::GetFocusedPlayerPed() {
    if (!IsAPlayerInFocusOn2PlayerGame()) {
        return nullptr;
    } else {
        return FindPlayerPed((int32)n2PlayerPedInFocus);
    }
}

// notsa
bool CGameLogic::CanPlayerTripSkip() {
    return SkipState == SKIP_AVAILABLE || SkipState == SKIP_AFTER_MISSION;
}

// notsa
void CGameLogic::SetMissionFailed(){
    GameState = GAMELOGIC_STATE_MISSION_FAILED;
    TimeOfLastEvent = CTimer::GetTimeInMS();
}
