#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Stats.h"
#include "Localisation.h"
#include "Text/Text.h"
#include "GxtChar.h"
#include "AudioEngine.h"
#include "World.h"
#include "TagManager.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par StatsWrappers60 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp (compuertas, rutas, parches) y Cases.hpp / Fakes.hpp
// (estado de los casos y falsos de los colaboradores). Cuatro rutas: CStats::FindLeastFavoriteRadioStation (0x559010), BuildStatLine (0x559230), UpdateRespectStat (0x55BC50) y
// ConstructStatLine (0x55A780), cada una con su HookRoute (hook RH "Simple" registrado en Stats.cpp).
namespace StatsWrappers60Live {
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "statswrappers60-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "statswrappers60-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("statswrappers60"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    Env env;
    env.asi = ModuleOf(std::uintptr_t(&ModuleMarker));
    env.game = std::uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    json registered = json::object();
    json bodies = json::object();
    static constexpr const char* kFns[4] = { "FindLeastFavoriteRadioStation", "BuildStatLine", "UpdateRespectStat", "ConstructStatLine" };
    static constexpr std::uintptr_t kAddr[4] = { 0x559010, 0x559230, 0x55BC50, 0x55A780 };
    bool ready = true;
    for (int i = 0; i < 4; ++i) {
        env.route[i].reset(new HookRoute("CStats", kFns[i], kAddr[i], HookRoute::RegisteredTarget{}));
        registered[std::string("CStats::") + kFns[i]] = env.route[i]->Registered();
        bodies[std::string("CStats::") + kFns[i]] = env.route[i]->Body();
        ready &= env.route[i]->Registered() && env.route[i]->Body() && ModuleOf(env.route[i]->Body()) == env.asi;
    }
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B = [](auto fp) { return Body(std::bit_cast<std::uintptr_t>(fp)); };
    env.c.getStat = { 0x558E40, B(static_cast<float(*)(eStats)>(&CStats::GetStatValue)) };
    env.c.metric = { 0x56D220, B(&CLocalisation::Metric) };
    env.c.german = { 0x56D200, B(&CLocalisation::GermanGame) };
    env.c.populate = { 0x558EC0, B(&CStats::PopulateFavoriteRadioStationList) };
    env.c.findMost = { 0x558FA0, B(&CStats::FindMostFavoriteRadioStation) };
    env.c.findLeast = { 0x559010, B(&CStats::FindLeastFavoriteRadioStation) };
    env.c.progress = { 0x5591E0, B(&CStats::GetPercentageProgress) };
    env.c.mins = { 0x559540, B(&CStats::ConvertToMins) };
    env.c.secs = { 0x559560, B(&CStats::ConvertToSecs) };
    env.c.build = { 0x559230, B(&CStats::BuildStatLine) };
    env.c.get = { 0x6A0050, B(static_cast<const GxtChar*(CText::*)(const char*)>(&CText::Get)) };
    env.c.g2a = { 0x69F7E0, 0 };
    env.c.strcpy_ = { 0x718660, B(&GxtCharStrcpy) };
    env.c.strip = { 0x719240, 0 };
    env.c.a2g = { 0x718600, B(&AsciiToGxtChar) };
    env.c.sprintf_ = { 0x821BB5, 0 };
    env.c.stationKey = { 0x507010, B(&CAudioEngine::GetRadioStationNameKey) };
    env.c.findPed = { 0x56E210, B(&FindPlayerPed) };
    env.c.weapon = { 0x5E3B60, B(static_cast<eWeaponSkill(CPed::*)(eWeaponType)>(&CPed::GetWeaponSkill)) };
    env.c.tagged = { 0x49CDA0, B(&CTagManager::GetPercentageTagged) };
    env.c.threshold = { 0x5595F0, B(&CStats::CheckForThreshold) };
    env.c.display = { 0x55B980, B(&CStats::DisplayScriptStatUpdateMessage) };
    env.c.checkMsg = { 0x559760, B(&CStats::CheckForStatsMessage) };
    Run(env, ready, registered, bodies);
}
}
