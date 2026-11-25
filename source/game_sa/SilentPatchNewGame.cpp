// SilentPatch (MIT, Silent): ver docs/THIRD_PARTY_NOTICES.md. Adaptado de SilentPatchSA.cpp (namespace VariableResets).
#include "StdInc.h"
#include "SilentPatchNewGame.h"

#include <string>
#include <string_view>
#include <vector>

#include "extensions/SilentPatchConfig.h"
#include "CarCtrl.h"
#include "Garages.h"
#include "GameLogic.h"
#include "Stats.h"
#include "FileLoader.h"
#include "Entity/Ped/PlayerPed.h"

namespace SilentPatchNewGame {
namespace {
std::vector<std::string> s_PickupDefs, s_CarGeneratorDefs, s_StuntJumpDefs;

bool ResetVariablesEnabled() {
    static const bool v = SilentPatchOption("ResetVariablesOnNewGame", true);
    return v;
}

bool ReinitIplObjectsEnabled() {
    static const bool v = SilentPatchOption("ReinitIplObjectsOnNewGame", true);
    return v;
}

std::string Trim(std::string_view line) {
    const size_t first = line.find_first_not_of(' ');
    if (first != std::string_view::npos) {
        const size_t last = line.find_last_not_of(' ');
        line = line.substr(first, last - first + 1);
    }
    return std::string{ line };
}
} // namespace

void BeforeReInit() {
    if (!ResetVariablesEnabled()) {
        return;
    }
    CPlayerPed::bHasDisplayedPlayerQuitEnterCarHelpText = false; // "To stop Carl..." message
    CGarages::RespraysAreFree = false;
    CGarages::BombsAreFree    = false;

    CCarCtrl::LastTimeFireTruckCreated = 0;
    CCarCtrl::LastTimeAmbulanceCreated = 0;
    CCarCtrl::TimeNextMadDriverChaseCreated = CGeneral::GetRandomNumberInRange(600.0f, 1200.0f);

    CStats::m_CycleSkillCounter       = 0;
    CStats::m_SwimUnderWaterCounter   = 0;
    CStats::m_WeaponCounter           = 0;
    CStats::m_LastWeaponTypeFired     = 0;
    CStats::m_DeathCounter            = 0;
    CStats::m_MaxHealthCounter        = 0;
    CStats::m_AddToHealthCounter      = 0;

    CGameLogic::bPenaltyForDeathApplies  = true; // keep weapons after wasted/busted
    CGameLogic::bPenaltyForArrestApplies = true;
}

void AfterReInit() {
    if (!ReinitIplObjectsEnabled()) {
        return;
    }
    for (const auto& l : s_PickupDefs) {
        CFileLoader::LoadPickup(l.c_str());
    }
    for (const auto& l : s_CarGeneratorDefs) {
        CFileLoader::LoadCarGenerator(l.c_str(), 0);
    }
    for (const auto& l : s_StuntJumpDefs) {
        CFileLoader::LoadStuntJump(l.c_str());
    }
}

void RecordPickupLine(const char* line) {
    if (ReinitIplObjectsEnabled()) {
        s_PickupDefs.emplace_back(Trim(line));
    }
}

void RecordCarGeneratorLine(const char* line) {
    if (ReinitIplObjectsEnabled()) {
        s_CarGeneratorDefs.emplace_back(Trim(line));
    }
}

void RecordStuntJumpLine(const char* line) {
    if (ReinitIplObjectsEnabled()) {
        s_StuntJumpDefs.emplace_back(Trim(line));
    }
}
} // namespace SilentPatchNewGame
