#pragma once
#include <fstream>
#include <memory>
#include "Quaternion.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par QuatMultiply29 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace QuatMultiply29Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"quatmultiply29-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"quatmultiply29-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("quatmultiply29");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. Nombre del hook: "Multiply" (RH_ScopedInstall(Multiply, 0x59C270)).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CQuaternion","Multiply",0x59C270,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CQuaternion::Multiply"]=reg;
    const uintptr_t body=env.route->Body();bodies["CQuaternion::Multiply"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    Run(env,ready,registered,bodies);
}
}
