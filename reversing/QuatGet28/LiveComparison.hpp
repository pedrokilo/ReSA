#pragma once
#include <fstream>
#include <memory>
#include "Quaternion.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par QuatGet28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace QuatGet28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"quatget28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"quatget28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("quatget28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. Nombre del hook: "Get-matrix" (RH_ScopedOverloadedInstall(Get, "matrix", 0x59C080, ...)).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CQuaternion","Get-matrix",0x59C080,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CQuaternion::Get(RwMatrix*)"]=reg;
    const uintptr_t body=env.route->Body();bodies["CQuaternion::Get(RwMatrix*)"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    Run(env,ready,registered,bodies);
}
}
