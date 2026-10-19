#pragma once
#include <fstream>
#include <memory>
#include "Camera.h"
#include "World.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CameraSoundDist30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace CameraSoundDist30Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"camerasounddist30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"camerasounddist30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camerasounddist30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. Nombre del hook: "UpdateSoundDistances" (RH_ScopedInstall(UpdateSoundDistances, 0x515BD0)).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CCamera","UpdateSoundDistances",0x515BD0,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CCamera::UpdateSoundDistances"]=reg;
    const uintptr_t body=env.route->Body();bodies["CCamera::UpdateSoundDistances"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Cuerpo de la DLL del unico colaborador (la DLL lo llama directamente): se resuelve saltando los E9 de enlace incremental ANTES de crear parches.
    env.dllCollab=Resolve(reinterpret_cast<uintptr_t>(&CWorld::ProcessVerticalLine));
    Run(env,ready,registered,bodies);
}
}
