#pragma once
#include <bit>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "WaterLevel.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn88B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una sola ruta (hook S, RH_ScopedGlobalInstall): CWaterLevel::RenderSeaBedSegmentNear 0x6E6A10. Sin colaboradores.
namespace H88Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fn88b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fn88b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fn88b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // La ruta RH se construye antes que nada; el constructor deja el hook apagado.
    env.route.reset(new HookRoute("CWaterLevel","RenderSeaBedSegmentNear",kEntry,HookRoute::RegisteredTarget{}));
    json registered=json::object();json bodies=json::object();
    registered[kName]=env.route->Registered();bodies[kName]=env.route->Body();
    const bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    Run(env,ready,registered,bodies);
}
}
