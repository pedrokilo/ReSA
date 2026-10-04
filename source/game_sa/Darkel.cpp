/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDarkel
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 19/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Darkel.h"
#include "Hud.h"

// Rampages are against to some specific 'model', using these below
// as enemies mean all peds/vehicles/bikes can be destroyed.
constexpr eModelID MODELTK_ANY_PED = eModelID(-1);
constexpr eModelID MODELTK_ANY_VEHICLE = eModelID(-2);
constexpr eModelID MODELTK_ANY_BIKE = eModelID(-3);

void CDarkel::InjectHooks() {
    RH_ScopedClass(CDarkel);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FrenzyOnGoing, 0x43D1F0);
    RH_ScopedInstall(Init, 0x43CEB0);
    RH_ScopedInstall(DrawMessages, 0x43CEC0);
    RH_ScopedInstall(ReadStatus, 0x43D1E0);
    RH_ScopedInstall(RegisterKillNotByPlayer, 0x43D210);
    RH_ScopedInstall(ThisPedShouldBeKilledForFrenzy, 0x43D2F0);
    RH_ScopedInstall(ThisVehicleShouldBeKilledForFrenzy, 0x43D350);
    RH_ScopedInstall(StartFrenzy, 0x43D3B0);
    RH_ScopedInstall(ResetModelsKilledByPlayer, 0x43D6A0);
    RH_ScopedInstall(QueryModelsKilledByPlayer, 0x43D6C0);
    RH_ScopedInstall(FindTotalPedsKilledByPlayer, 0x43D6E0);
    RH_ScopedInstall(DealWithWeaponChangeAtEndOfFrenzy, 0x43D7A0);
    RH_ScopedInstall(CheckDamagedWeaponType, 0x43D9E0);
    RH_ScopedInstall(Update, 0x43DAC0);
    RH_ScopedInstall(ResetOnPlayerDeath, 0x43DC10);
    RH_ScopedInstall(FailKillFrenzy, 0x43DC60);
    RH_ScopedInstall(RegisterKillByPlayer, 0x43DCD0);
    RH_ScopedInstall(RegisterCarBlownUpByPlayer, 0x43DF20);

    // unused
    RH_ScopedInstall(CalcFade, 0x43D740);
}

// 0x43CEB0
void CDarkel::Init() {
    Status = eDarkelStatus::INITIAL;
}

// 0x43D1F0
bool CDarkel::FrenzyOnGoing() {
    return Status == eDarkelStatus::FRENZY_ON_GOING || Status == eDarkelStatus::FRENZY_ON_GOING_2P;
}

// Como el original: CGameLogic::SetMissionFailed va en línea y GameState se escribe como byte
static void SetGameLogicStateNow(eGameLogicState state) {
    reinterpret_cast<uint8&>(CGameLogic::GameState) = static_cast<uint8>(state);
    CGameLogic::TimeOfLastEvent = CTimer::GetTimeInMS();
}

// 0x43CEC0
void CDarkel::DrawMessages() {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    switch (Status) {
    case eDarkelStatus::FRENZY_ON_GOING:
    case eDarkelStatus::FRENZY_ON_GOING_2P: {
        const uint32 elapsed = CTimer::GetTimeInMS() - TimeOfFrenzyStart;
        if (bStandardSoundAndMessages ? (elapsed >= 3000 && elapsed < 11'000) : elapsed < 8000) {
            if (pStartMessage) {
                CMessages::AddBigMessage(pStartMessage, 3000, STYLE_MIDDLE);
            }
        }

        // Como el original: escalas con las constantes del EXE (ancho * 1/640 y alto * 1/448), sin corrección de aspecto
        constexpr float kW = 1.0f / 640.0f; // 0x859520
        constexpr float kH = 1.0f / 448.0f; // 0x859524
        const float W  = (float)RsGlobal.maximumWidth;
        const float H  = (float)RsGlobal.maximumHeight;
        const float sy = H * kH;

        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        CFont::SetScale(W * kW * 0.5f, sy);
        CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        CFont::SetRightJustifyWrap(0.0f);
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetWrapx(W * kW * 640.0f);
        CFont::SetEdge(2);
        CFont::SetDropColor({0, 0, 0, 255});
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));

        const float y1 = CHud::GetYPosBasedOnHealth(CWorld::PlayerInFocus, sy * 148.0f, 12);
        float remainingTimerY = CHud::GetYPosBasedOnHealth(PED_TYPE_PLAYER2, y1, 12);

        const float y2 = CHud::GetYPosBasedOnHealth(CWorld::PlayerInFocus, sy * 20.0f + sy * 148.0f, 12);
        float remainingKillsY = CHud::GetYPosBasedOnHealth(PED_TYPE_PLAYER2, y2, 12);

        // Como el original: FindPlayerPed(1) va en línea (lee CWorld::Players[1].m_pPed)
        if (CWorld::Players[PED_TYPE_PLAYER2].m_pPed) {
            const float offset = sy * 72.0f;
            remainingTimerY = remainingTimerY + offset;
            remainingKillsY = offset + remainingKillsY;
        }

        const float x = W - kW * W * 32.0f;
        if (TimeLimit >= 0) {
            const uint32 remaining = TimeLimit - CTimer::GetTimeInMS() + TimeOfFrenzyStart;
            sprintf(gString, "%d:%02d", remaining / 60'000, remaining % 60'000 / 1000); // Como el original: sprintf (sprintf_s rellena con 0xFE en debug)
            AsciiToGxtChar(gString, gGxtString);

            // blink in last 4 second
            if (remaining > 4000 || (CTimer::GetFrameCounter() & 1)) {
                CFont::PrintString(x, remainingTimerY, gGxtString);
            }
        }
        sprintf(gString, "%d", KillsNeeded < 0 ? 0 : KillsNeeded);
        AsciiToGxtChar(gString, gGxtString);
        CFont::PrintString(x, remainingKillsY, gGxtString);
        break;
    }
    case eDarkelStatus::FRENZY_PASSED:
        if (bStandardSoundAndMessages && CTimer::GetTimeInMS() - TimeOfFrenzyStart < 5'000) {
            CMessages::AddBigMessage(TheText.Get("KILLPA"), 3000, STYLE_MIDDLE);
        }
        break;
    default:
        break;
    }
}

// 0x43D1E0
eDarkelStatus CDarkel::ReadStatus() {
    return Status;
}

// 0x43D210
void CDarkel::RegisterKillNotByPlayer(const CPed* killedPed) {
    // Como el original: FindPlayerPed se llama dos veces
    if (killedPed != FindPlayerPed()) {
        if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(killedPed)) {
            CStats::DecrementStat(STAT_RESPECT, 2.0f);
            CStats::IncrementStat(STAT_RECRUITED_GANG_MEMBERS_KILLED);
            CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_GANG_STRENGTH, 1.0f);
        }
    }
    CStats::IncrementStat(STAT_PEOPLE_WASTED_BY_OTHERS);

    if (const auto friendly = killedPed->m_nPedType == PED_TYPE_GANG2; killedPed->IsGangster()) {
        CStats::IncrementStat(friendly ? STAT_FRIENDLY_GANG_MEMBERS_KILLED : STAT_ENEMY_GANG_MEMBERS_KILLED);
    }
}

// 0x43D2F0
bool CDarkel::ThisPedShouldBeKilledForFrenzy(const CPed& ped) {
    if (!FrenzyOnGoing())
        return false;

    // Como el original: el modelo se extiende con signo (movsx)
    const int32 model = (int16)ped.GetModelIndex();
    if (ModelToKill[3] != (int32)MODELTK_ANY_PED && model != ModelToKill[3] && model != ModelToKill[2] && model != ModelToKill[1] && model != ModelToKill[0]) {
        return false;
    }
    return !ped.IsPlayer();
}

// 0x43D350
bool CDarkel::ThisVehicleShouldBeKilledForFrenzy(const CVehicle& vehicle) {
    if (!FrenzyOnGoing())
        return false;

    if (ModelToKill[3] == (int32)MODELTK_ANY_VEHICLE)
        return true;

    // Como el original: el modelo se extiende con signo (movsx)
    const int32 model = (int16)vehicle.GetModelIndex();
    if (model == ModelToKill[3] || model == ModelToKill[2] || model == ModelToKill[1] || model == ModelToKill[0])
        return true;

    return ModelToKill[3] == (int32)MODELTK_ANY_BIKE && vehicle.IsSubBike();
}

// 0x43D3B0
void CDarkel::StartFrenzy(eWeaponType weaponType, int32 timeLimit, uint16 killsNeeded, int32 modelToKill, const GxtChar* startMessage, int32 modelToKill2, int32 modelToKill3, int32 modelToKill4, bool standardSoundAndMessages, bool needHeadShot) {
    CGameLogic::ClearSkip(false);
    // Como el original: se mira el arma del parámetro (no la global WeaponType)
    const eWeaponType weapon = (weaponType != WEAPON_UZI_DRIVEBY) ? weaponType : WEAPON_MICRO_UZI;

    if (CGameLogic::IsCoopGameGoingOn()) {
        Status = eDarkelStatus::FRENZY_ON_GOING_2P;
        CGameLogic::DoWeaponStuffAtStartOf2PlayerGame(false);
    } else {
        Status = eDarkelStatus::FRENZY_ON_GOING;
    }

    ModelToKill[3] = modelToKill;
    ModelToKill[2] = modelToKill2;
    ModelToKill[1] = modelToKill3;
    ModelToKill[0] = modelToKill4;

    KillsNeeded = killsNeeded;
    WeaponType = weaponType;
    pStartMessage = startMessage;
    if (bProperKillFrenzy = (startMessage == TheText.Get("PAGE_00"))) {
        pStartMessage = nullptr;
    }
    bStandardSoundAndMessages = standardSoundAndMessages;
    bHeadShotRequired = needHeadShot;
    TimeOfFrenzyStart = CTimer::GetTimeInMS();
    TimeLimit = timeLimit;
    PreviousTime = timeLimit / 1000;

    // Como el original: comparación con signo y FindPlayerPed() en cada acceso
    if ((int32)weapon >= (int32)WEAPON_LAST_WEAPON) {
        return;
    }
    InterruptedWeaponTypeSelected = FindPlayerPed()->GetActiveWeapon().m_Type;
    FindPlayerPed()->RemoveWeaponAnims(InterruptedWeaponTypeSelected, -1000.0f);

    const auto frenzyWeaponSlot = CWeaponInfo::GetWeaponInfo(weapon)->m_nSlot;
    InterruptedWeaponType = FindPlayerPed()->GetWeaponInSlot((size_t)frenzyWeaponSlot).m_Type;
    AmmoInterruptedWeapon = FindPlayerPed()->GetWeaponInSlot((size_t)frenzyWeaponSlot).m_TotalAmmo;

    if (InterruptedWeaponType != WEAPON_UNARMED) {
        CModelInfo::GetModelInfo(CWeaponInfo::GetWeaponInfo(InterruptedWeaponType)->m_nModelId1)->AddRef();
    }

    for (auto i = 0; i < 2; i++) {
        if (FindPlayerPed(i)) {
            FindPlayerPed(i)->GiveWeapon(weapon, 30'000, true);
            FindPlayerPed(i)->SetCurrentWeapon(weapon);
            FindPlayerPed(i)->MakeChangesForNewWeapon(weapon);
        }
    }

    if (!FindPlayerVehicle()) {
        return;
    }

    const int32 chosenWeapon = (int8)FindPlayerPed()->GetPlayerData()->m_nChosenWeapon;
    FindPlayerPed()->SetCurrentWeapon(chosenWeapon);

    auto* const player = FindPlayerPed();
    const int32 ammoClip = (int16)CWeaponInfo::GetWeaponInfo(FindPlayerPed()->GetActiveWeapon().m_Type)->m_nAmmoClip;
    int32 ammoInClip;
    if ((int32)player->GetActiveWeapon().m_TotalAmmo < ammoClip) {
        ammoInClip = (int32)FindPlayerPed()->GetActiveWeapon().m_TotalAmmo;
    } else {
        ammoInClip = (int16)CWeaponInfo::GetWeaponInfo(FindPlayerPed()->GetActiveWeapon().m_Type)->m_nAmmoClip;
    }
    FindPlayerPed()->GetActiveWeapon().m_AmmoInClip = (uint32)ammoInClip;
    FindPlayerPed()->ClearWeaponTarget();
}

// 0x43D6A0
void CDarkel::ResetModelsKilledByPlayer(int32 playerId) {
    for (auto& model : RegisteredKills) {
        model[playerId] = 0;
    }
}

// 0x43D6C0
int16 CDarkel::QueryModelsKilledByPlayer(eModelID modelId, int32 playerId) {
    return RegisteredKills.data()[modelId][playerId];
}

// 0x43D6E0
int32 CDarkel::FindTotalPedsKilledByPlayer(int32 playerId) {
    // Como el original: solo suma los 400 primeros modelos (peatones)
    int32 sum = 0;
    for (auto i = 0; i < 400; i++) {
        sum += RegisteredKills[i][playerId];
    }
    return sum;
}

// 0x43D7A0
void CDarkel::DealWithWeaponChangeAtEndOfFrenzy() {
    // Como el original: el límite se comprueba sobre el arma ya convertida (UZI_DRIVEBY -> MICRO_UZI)
    // y FindPlayerPed() se llama en cada acceso
    const eWeaponType weapon = (WeaponType != WEAPON_UZI_DRIVEBY) ? WeaponType : WEAPON_MICRO_UZI;
    if ((int32)weapon < (int32)WEAPON_LAST_WEAPON) {
        if (InterruptedWeaponType != WEAPON_UNARMED) {
            CModelInfo::GetModelInfo(CWeaponInfo::GetWeaponInfo(InterruptedWeaponType)->m_nModelId1)->RemoveRef();
        }

        const auto slot = (size_t)(uint8)CWeaponInfo::GetWeaponInfo(weapon)->m_nSlot;
        const auto modelInSlot = CWeaponInfo::GetWeaponInfo(FindPlayerPed()->GetWeaponInSlot(slot).m_Type)->m_nModelId1;
        FindPlayerPed()->RemoveWeaponModel(modelInSlot);

        FindPlayerPed()->GetWeaponInSlot(slot).m_Type = WEAPON_UNARMED;
        FindPlayerPed()->GetWeaponInSlot(slot).m_TotalAmmo = 0;
        FindPlayerPed()->GetWeaponInSlot(slot).m_AmmoInClip = 0;
        FindPlayerPed()->GetWeaponInSlot(slot).m_State = WEAPONSTATE_READY;
        FindPlayerPed()->RemoveWeaponAnims(weapon, -1000.0f);

        if ((int16)CModelInfo::GetModelInfo(CWeaponInfo::GetWeaponInfo(weapon)->m_nModelId1)->m_nRefCount > 0) {
            CModelInfo::GetModelInfo(CWeaponInfo::GetWeaponInfo(weapon)->m_nModelId1)->RemoveRef();
        }

        // Como el original: se restaura el arma elegida con el slot del arma activa antes del frenesí
        auto* const playerData = FindPlayerPed()->GetPlayerData();
        playerData->m_nChosenWeapon = (uint8)CWeaponInfo::GetWeaponInfo(InterruptedWeaponTypeSelected)->m_nSlot;
        FindPlayerPed()->GiveWeapon(InterruptedWeaponType, AmmoInterruptedWeapon, true);
    }

    if (FindPlayerVehicle()) {
        const auto activeModel = CWeaponInfo::GetWeaponInfo(FindPlayerPed()->GetActiveWeapon().m_Type)->m_nModelId1;
        FindPlayerPed()->RemoveWeaponModel(activeModel);

        if (FindPlayerPed()->GetWeaponInSlot(eWeaponSlot::SMG).m_Type != WEAPON_UNARMED) {
            FindPlayerPed()->GetPlayerData()->m_nChosenWeapon = (uint8)eWeaponSlot::SMG;
        } else {
            FindPlayerPed()->GetPlayerData()->m_nChosenWeapon = (uint8)eWeaponSlot::UNARMED;
        }

        const int32 chosenWeaponSlot = (int8)FindPlayerPed()->GetPlayerData()->m_nChosenWeapon;
        FindPlayerPed()->SetCurrentWeapon(chosenWeaponSlot);

        // Como el original: MakeChangesForNewWeapon recibe el tipo del arma activa (0x60B460), no el slot
        const eWeaponType activeType = FindPlayerPed()->GetActiveWeapon().m_Type;
        FindPlayerPed()->MakeChangesForNewWeapon(activeType);
    }
}

// 0x43D9E0
bool CDarkel::CheckDamagedWeaponType(eWeaponType damageWeaponId, eWeaponType expectedDamageWeaponId) {
    if (damageWeaponId == -1) {
        return false;
    }

    if (expectedDamageWeaponId == WEAPON_ANYMELEE) {
        switch (damageWeaponId) {
            case WEAPON_UNARMED:
            case WEAPON_BRASSKNUCKLE:
            case WEAPON_GOLFCLUB:
            case WEAPON_NIGHTSTICK:
            case WEAPON_KNIFE:
            case WEAPON_BASEBALLBAT:
            case WEAPON_SHOVEL:
            case WEAPON_POOL_CUE:
            case WEAPON_KATANA:
            case WEAPON_CHAINSAW:
            case WEAPON_DILDO1:
            case WEAPON_DILDO2:
            case WEAPON_VIBE1:
            case WEAPON_VIBE2:
            case WEAPON_FLOWERS:
            case WEAPON_CANE:
                return true;
            default:
                return false;
        }
    }
    if (expectedDamageWeaponId != WEAPON_ANYWEAPON) {
        return false;
    }

    switch (damageWeaponId) {
        case WEAPON_UNARMED:
        case WEAPON_BRASSKNUCKLE:
        case WEAPON_GOLFCLUB:
        case WEAPON_NIGHTSTICK:
        case WEAPON_KNIFE:
        case WEAPON_BASEBALLBAT:
        case WEAPON_SHOVEL:
        case WEAPON_POOL_CUE:
        case WEAPON_KATANA:
        case WEAPON_CHAINSAW:
        case WEAPON_DILDO1:
        case WEAPON_DILDO2:
        case WEAPON_VIBE1:
        case WEAPON_VIBE2:
        case WEAPON_FLOWERS:
        case WEAPON_CANE:
        case WEAPON_GRENADE:
        case WEAPON_TEARGAS:
        case WEAPON_MOLOTOV:
        case WEAPON_ROCKET:
        case WEAPON_ROCKET_HS:
        case WEAPON_FREEFALL_BOMB:
        case WEAPON_PISTOL:
        case WEAPON_PISTOL_SILENCED:
        case WEAPON_DESERT_EAGLE:
        case WEAPON_SHOTGUN:
        case WEAPON_SAWNOFF_SHOTGUN:
        case WEAPON_SPAS12_SHOTGUN:
        case WEAPON_MICRO_UZI:
        case WEAPON_MP5:
        case WEAPON_AK47:
        case WEAPON_M4:
        case WEAPON_TEC9:
        case WEAPON_COUNTRYRIFLE:
        case WEAPON_SNIPERRIFLE:
        case WEAPON_RLAUNCHER:
        case WEAPON_RLAUNCHER_HS:
        case WEAPON_FLAMETHROWER:
        case WEAPON_MINIGUN:
        case WEAPON_REMOTE_SATCHEL_CHARGE:
        case WEAPON_DETONATOR:
        case WEAPON_SPRAYCAN:
        case WEAPON_EXTINGUISHER:
        case WEAPON_CAMERA:
        case WEAPON_NIGHTVISION:
        case WEAPON_INFRARED:
        case WEAPON_EXPLOSION:
        case WEAPON_UZI_DRIVEBY:
            return true;
        default:
            return false;
    }
}

// 0x43DAC0
void CDarkel::Update() {
    ZoneScoped;

    if (!FrenzyOnGoing())
        return;

    const auto FailFrenzy = [] {
        if (Status == eDarkelStatus::FRENZY_ON_GOING_2P) {
            SetGameLogicStateNow(GAMELOGIC_STATE_MISSION_FAILED);
        }
        Status = eDarkelStatus::FRENZY_FAILED;
        CPopulation::m_AllRandomPedsThisType = -1;
        TimeOfFrenzyStart = CTimer::GetTimeInMS();
        DealWithWeaponChangeAtEndOfFrenzy();
    };

    const int32 remaining = TimeOfFrenzyStart + TimeLimit - CTimer::GetTimeInMS();
    if (remaining <= 0 && TimeLimit >= 0) {
        FailFrenzy();
    } else if (Status == eDarkelStatus::FRENZY_ON_GOING_2P && !CWorld::Players[PED_TYPE_PLAYER2].m_pPed) { // Como el original: FindPlayerPed(1) en línea
        FailFrenzy();
    } else if (remaining / 1000 != PreviousTime) {
        if (PreviousTime < 12) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_TIMER_COUNT);
        }
        PreviousTime = remaining / 1000;
    }

    if (KillsNeeded <= 0) {
        if (Status == eDarkelStatus::FRENZY_ON_GOING_2P) {
            SetGameLogicStateNow(GAMELOGIC_STATE_MISSION_PASSED);
        }

        Status = eDarkelStatus::FRENZY_PASSED;
        CPopulation::m_AllRandomPedsThisType = -1;

        if (bProperKillFrenzy) {
            CStats::IncrementStat(STAT_RAMPAGES_PASSED);
        }
        TimeOfFrenzyStart = CTimer::GetTimeInMS();
        FindPlayerPed()->SetWantedLevel(eWantedLevel::WANTED_CLEAN);

        DealWithWeaponChangeAtEndOfFrenzy();
    }
}

// 0x43DC10
void CDarkel::ResetOnPlayerDeath() {
    CHud::SetHelpMessage(nullptr, true, false, false);
    if (FrenzyOnGoing()) {
        Status = eDarkelStatus::FRENZY_FAILED;
        CPopulation::m_AllRandomPedsThisType = -1;
        TimeOfFrenzyStart = CTimer::GetTimeInMS();
        DealWithWeaponChangeAtEndOfFrenzy();
    }
}

// 0x43DC60
void CDarkel::FailKillFrenzy() {
    if (Status == eDarkelStatus::FRENZY_ON_GOING_2P) {
        SetGameLogicStateNow(GAMELOGIC_STATE_MISSION_FAILED);
    }
    ResetOnPlayerDeath();
}

// 0x43DCD0
void CDarkel::RegisterKillByPlayer(const CPed& killedPed, eWeaponType damageWeaponId, bool headShotted, int32 playerId) {
    // Como el original: orden de las estadísticas por tipo y sin comparar con el jugador antes de IsMember
    switch (killedPed.m_nPedType) {
    case PED_TYPE_COP:
    case PED_TYPE_DEALER:
        CStats::IncrementStat(STAT_RESPECT, 1.5f);
        break;
    case PED_TYPE_GANG2:
        CStats::DecrementStat(STAT_RESPECT, 5.0f);
        CStats::IncrementStat(STAT_FRIENDLY_GANG_MEMBERS_KILLED);
        if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(&killedPed)) {
            CStats::IncrementStat(STAT_RECRUITED_GANG_MEMBERS_KILLED);
            CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_GANG_STRENGTH, 1.0f);
        }
        break;
    case PED_TYPE_GANG1:
    case PED_TYPE_GANG3:
    case PED_TYPE_GANG4:
    case PED_TYPE_GANG5:
    case PED_TYPE_GANG6:
    case PED_TYPE_GANG7:
    case PED_TYPE_GANG8:
    case PED_TYPE_GANG9:
    case PED_TYPE_GANG10:
        CStats::IncrementStat(STAT_RESPECT, 0.75f);
        CStats::IncrementStat(STAT_ENEMY_GANG_MEMBERS_KILLED);
        break;
    default:
        break;
    }

    if (FrenzyOnGoing()) {
        // Como el original: una sola comprobación encadenada, la muerte cuenta como mucho una vez
        const bool counts = ((WeaponType == WEAPON_ANYMELEE || WeaponType == WEAPON_ANYWEAPON) && CheckDamagedWeaponType(damageWeaponId, WeaponType))
            || damageWeaponId == WeaponType
            || damageWeaponId == WEAPON_EXPLOSION
            || (damageWeaponId == WEAPON_UZI_DRIVEBY && WeaponType == WEAPON_MICRO_UZI)
            || (damageWeaponId == WEAPON_RAMMEDBYCAR && WeaponType == WEAPON_RUNOVERBYCAR)
            || (damageWeaponId == WEAPON_RUNOVERBYCAR && WeaponType == WEAPON_RAMMEDBYCAR)
            || (damageWeaponId == WEAPON_FLAMETHROWER && WeaponType == WEAPON_MOLOTOV);
        if (counts) {
            if (ThisPedShouldBeKilledForFrenzy(killedPed) && (!bHeadShotRequired || headShotted)) {
                KillsNeeded--;
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
            }
            CStats::IncrementStat(killedPed.m_nPedType == PED_TYPE_COP ? STAT_HIGHEST_POLICE_PEDS_KILLED_ON_RAMPAGE : STAT_HIGHEST_CIVILIAN_PEDS_KILLED_ON_RAMPAGE);
        }
    }

    RegisteredKills.data()[(int16)killedPed.GetModelIndex()][playerId]++;
    CStats::IncrementStat(STAT_PEOPLE_YOUVE_WASTED);
    CStats::PedsKilledOfThisType.data()[killedPed.bChrisCriminal ? PED_TYPE_CRIMINAL : killedPed.m_nPedType]++;

    if (headShotted) {
        CStats::IncrementStat(STAT_NUMBER_OF_HEADSHOTS);
    }
    CStats::IncrementStat(STAT_KILLS_SINCE_LAST_CHECKPOINT);

    // Como el original: FindPlayerPed(0) en línea (lee CWorld::Players[0].m_pPed)
    if (playerId == PED_TYPE_PLAYER1 && !CWorld::Players[PED_TYPE_PLAYER1].m_pPed->bInVehicle) {
        CGangWars::AddKillToProvocation(killedPed.m_nPedType);
    }

    if ((int32)killedPed.m_nPedType >= (int32)PED_TYPE_DEALER) { // BUG?
        CPopCycle::PlayerKilledADealer();
    }
}

// 0x43DF20
void CDarkel::RegisterCarBlownUpByPlayer(CVehicle& vehicle, int32 playerId) {
    if (ThisVehicleShouldBeKilledForFrenzy(vehicle)) {
        KillsNeeded--;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
    }
    RegisteredKills.data()[(int16)vehicle.GetModelIndex()][playerId]++;

    switch (vehicle.GetVehicleAppearance()) {
    case VEHICLE_APPEARANCE_AUTOMOBILE:
    case VEHICLE_APPEARANCE_BIKE:
        CStats::IncrementStat(STAT_ROAD_VEHICLES_DESTROYED);
        break;
    case VEHICLE_APPEARANCE_HELI:
    case VEHICLE_APPEARANCE_PLANE:
        CStats::IncrementStat(STAT_PLANES_HELICOPTERS_DESTROYED);
        break;
    case VEHICLE_APPEARANCE_BOAT:
        CStats::IncrementStat(STAT_BOATS_DESTROYED);
        break;
    default:
        break;
    }

    if (FrenzyOnGoing()) {
        if (vehicle.GetModelIndex() == MODEL_RHINO) {
            CStats::IncrementStat(STAT_HIGHEST_NUMBER_OF_TANKS_DESTROYED_ON_RAMPAGE);
        } else if (vehicle.IsLawEnforcementVehicle()) {
            CStats::IncrementStat(STAT_HIGHEST_POLICE_VEHICLES_DESTROYED_ON_RAMPAGE);
        } else {
            CStats::IncrementStat(STAT_HIGHEST_CIVILIAN_VEHICLES_DESTROYED_ON_RAMPAGE);
        }
    }
}

// unused
// 0x43D740
uint8 CDarkel::CalcFade(uint32 t, uint32 begin, uint32 end) {
    if (t < begin || t > end) {
        return 0;
    }

    if (t >= begin + 500) {
        if (t <= end - 500) {
            return 255;
        }

        return 255 * (end - t) / 500;
    }

    return 255 * (t - begin) / 500;
}
