/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssG5.h"
#include "DatosExe/DataBssG4.h"
#include "DatosExe/DataBssE5.h"
#include "DatosExe/DataBssE4.h"
#include "DatosExe/DataBssE3.h"
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataBssE1.h"
#include "DatosExe/DataInicializadaA.h"

#include "Stats.h"
#include "eHud.h"
#include <GxtChar.h>

class CSprite2d;
class CPed;
class CPlayerInfo;
class CPlayerPed;

class CHud {
public:
    static constexpr auto BIG_MESSAGE_SIZE = 128;

    static inline auto& bScriptDontDisplayAreaName = StaticRef<bool>(0xBAA3F8);
    static inline auto& bScriptDontDisplayVehicleName = StaticRef<bool>(0xBAA3F9);
    static inline auto& bScriptForceDisplayWithCounters = StaticRef<bool>(0xBAA3FA);
    static inline auto& bScriptDontDisplayRadar = StaticRef<bool>(0xBAA3FB);

    static inline auto& bDrawClock = StaticRef<bool>(0xBAA400);

    static inline auto& m_pVehicleNameToPrint = DatosExe::Bss::bBAA444;
    static inline auto& m_VehicleState = StaticRef<eNameState>(0xBAA448);
    static inline auto& m_VehicleFadeTimer = DatosExe::Bss::bBAA44C;
    static inline auto& m_VehicleNameTimer = DatosExe::Bss::bBAA450;
    static inline auto& m_pLastVehicleName = DatosExe::Bss::bBAA454;
    static inline auto& m_pVehicleName = DatosExe::Bss::bBAA458;

    static inline auto& m_bDraw3dMarkers = DatosExe::Bss::bBAA45C;
    static inline auto& m_Wants_To_Draw_Hud = DatosExe::Bss::bBAA45D;

    static inline auto& m_fHelpMessageTime = DatosExe::Bss::bBAA460; // in seconds
    static inline auto& m_fHelpMessageBoxWidth = StaticRef<float>(0x8D0934); // default 200.0
    static inline auto& m_bHelpMessagePermanent = DatosExe::Bss::bBAA464;
    static inline auto& m_fHelpMessageStatUpdateValue = DatosExe::Bss::bBAA468;
    static inline auto& m_nHelpMessageMaxStatValue = DatosExe::Bss::bBAA46C;
    static inline auto& m_nHelpMessageStatId = DatosExe::Bss::bBAA470;
    static inline auto& m_bHelpMessageQuick = DatosExe::Bss::bBAA472;
    static inline auto& m_nHelpMessageState = DatosExe::Bss::bBAA474;
    static inline auto& m_nHelpMessageFadeTimer = DatosExe::Bss::bBAA478;
    static inline auto& m_nHelpMessageTimer = DatosExe::Bss::bBAA47C;
    static inline auto& m_pHelpMessageToPrint = DatosExe::Bss::bBAA480;
    static inline auto& m_pLastHelpMessage = DatosExe::Bss::bBAA610;
    static inline auto& m_pHelpMessage = DatosExe::Bss::bBAA7A0;

    static inline auto& m_ZoneState = StaticRef<eNameState>(0xBAA930);
    static inline auto& m_ZoneFadeTimer = DatosExe::Bss::bBAA934;
    static inline auto& m_ZoneNameTimer = DatosExe::Bss::bBAA938;
    static inline auto& m_ZoneToPrint = DatosExe::Bss::bBAB1D0;
    static inline auto& m_pLastZoneName = DatosExe::Bss::bBAB1D4;
    static inline auto& m_pZoneName = DatosExe::Bss::bBAB1D8;

    static inline auto& m_ItemToFlash = StaticRef<eHudItem>(0xBAB1DC);
    static inline auto& bDrawingVitalStats = DatosExe::Bss::bBAB1DE;

    static inline auto& m_LastBreathTime = DatosExe::Bss::bBAA3FC;

    static inline auto& m_WeaponState = StaticRef<uint32>(0xBAA404);
    static inline auto& m_WeaponFadeTimer = DatosExe::Bss::bBAA408;
    static inline auto& m_WeaponTimer = DatosExe::Bss::bBAA40C;
    static inline auto& m_LastWeapon = DatosExe::Bss::bBAA410;

    static inline auto& m_WantedState = DatosExe::Bss::bBAA414;
    static inline auto& m_WantedFadeTimer = DatosExe::Bss::bBAA418;
    static inline auto& m_WantedTimer = DatosExe::Bss::bBAA41C;
    static inline auto& m_LastWanted = DatosExe::Bss::bBAA420;

    static inline auto& m_DisplayScoreState = StaticRef<uint32>(0xBAA424);
    static inline auto& m_DisplayScoreFadeTimer = DatosExe::Bss::bBAA428;
    static inline auto& m_DisplayScoreTimer = DatosExe::Bss::bBAA42C;
    static inline auto& m_LastDisplayScore = DatosExe::Bss::bBAA430;

    static inline auto& m_EnergyLostState = StaticRef<uint32>(0xBAA434);
    static inline auto& m_EnergyLostFadeTimer = DatosExe::Bss::bBAA438;
    static inline auto& m_EnergyLostTimer = DatosExe::Bss::bBAA43C;
    static inline auto& m_LastTimeEnergyLost = DatosExe::Bss::bBAA440;

    static inline auto& m_Message = DatosExe::Bss::bBAB040;
    static inline auto& m_BigMessage = StaticRef<GxtChar[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE]>(0xBAACC0);
    static inline auto& LastBigMessage = StaticRef<GxtChar[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE]>(0xBAA940);
    static inline auto& BigMessageAlpha = StaticRef<float[NUM_MESSAGE_STYLES]>(0xBAA3A4);
    static inline auto& BigMessageInUse = StaticRef<float[NUM_MESSAGE_STYLES]>(0xBAA3C0);
    static inline auto& BigMessageX = StaticRef<float[NUM_MESSAGE_STYLES]>(0xBAA3DC);

    static inline auto& Sprites = StaticRef<std::array<CSprite2d, 6>>(0xBAB1FC);

    static inline auto& TimerMainCounterHideState = DatosExe::Bss::bBAA388;
    static inline auto& TimerMainCounterWasDisplayed = StaticRef<bool>(0xBAA38A);
    static inline auto& TimerCounterHideState = StaticRef<std::array<int16, 4>>(0xBAA38C);
    static inline auto& TimerCounterWasDisplayed = StaticRef<std::array<int16, 4>>(0xBAA394);

    static inline auto& OddJob2OffTimer = StaticRef<float>(0xBAA398);
    static inline auto& OddJob2XOffset = DatosExe::Bss::bBAA39C;
    static inline auto& OddJob2Timer = DatosExe::Bss::bBAA3A0;
    static inline auto& OddJob2On = DatosExe::Bss::bBAB1E0;

    static inline auto& PagerXOffset = DatosExe::Data::v8D0938; // 150.0f
    static inline auto& HelpTripSkipShown = DatosExe::Bss::bBAB229;

public:
    static void InjectHooks();

    static void Initialise();
    static void ReInitialise();
    static void Shutdown();

    static void GetRidOfAllHudMessages(bool arg0);
    static float GetYPosBasedOnHealth(uint8 playerId, float pos, int8 offset);
    static bool HelpMessageDisplayed();

    static void SetMessage(const GxtChar* message);
    static void SetBigMessage(GxtChar* message, eMessageStyle style);
    static void SetHelpMessage(const GxtChar* text, bool quickMessage = false, bool permanent = false, bool addToBrief = false);
    static void SetHelpMessageStatUpdate(eStatUpdateState state, uint16 statId, float diff, float max);
    static void SetHelpMessageWithNumber(const GxtChar* text, int32 number, bool quickMessage, bool permanent);
    static void SetVehicleName(const GxtChar* name);
    static void SetZoneName(const GxtChar* name, bool displayImmediately);

    static void Draw();
    static void DrawAfterFade();
    static void DrawAreaName();
    static void DrawBustedWastedMessage();
    static void ResetWastedText();
    static void DrawCrossHairs();
    static float DrawFadeState(DRAW_FADE_STATE fadeState, int32 arg1);
    static void DrawHelpText();
    static void DrawMissionTimers();
    static void DrawMissionTitle();
    static void DrawOddJobMessage(bool displayImmediately);
    static void DrawRadar();
    static void DrawScriptText(bool displayImmediately);
    static void DrawSubtitles();
    static void DrawSuccessFailedMessage();
    static void DrawVehicleName();
    static void DrawVitalStats();
    static void DrawAmmo(CPed* ped, int32 x, int32 y, float alpha);
    static void DrawPlayerInfo();
    static inline void DrawClock();
    static inline void DrawMoney(const CPlayerInfo& playerInfo, uint8 alpha);
    static inline void DrawWeapon(CPlayerPed* ped0, CPlayerPed* ped1);

    static void DrawTripSkip();
    static void DrawWanted();
    static void DrawWeaponIcon(CPed* ped, int32 x, int32 y, float alpha);
    static void RenderArmorBar(int32 playerId, int32 x, int32 y);
    static void RenderBreathBar(int32 playerId, int32 x, int32 y);
    static void RenderHealthBar(int32 playerId, int32 x, int32 y);
};
