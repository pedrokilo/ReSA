#pragma once
#include <fstream>
#include <memory>
#include "Radar.h"
#include "World.h"
#include "Camera.h"
#include "Pickups.h"
#include "Pickup.h"
#include "Scripts/TheScripts.h"
#include "Sprite2d.h"
#include "EntryExit.h"
#include "Entity/Placeable.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par RadarBlip28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace RadarBlip28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"radarblip28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"radarblip28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radarblip28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CRadar","DrawEntityBlip",0x587000,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CRadar::DrawEntityBlip"]=reg;
    const uintptr_t body=env.route->Body();bodies["CRadar::DrawEntityBlip"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    Run(env,ready,registered,bodies);
}
}
