#pragma once
#include <fstream>
#include <memory>
#include "Camera.h"
#include "HandShaker.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CameraShake30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace CameraShake30Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"camerashake30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"camerashake30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camerashake30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. Nombre del hook: "ProcessShake-float" (RH_ScopedOverloadedInstall(ProcessShake, "float", 0x516560, ...)).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CCamera","ProcessShake-float",0x516560,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CCamera::ProcessShake"]=reg;
    const uintptr_t body=env.route->Body();bodies["CCamera::ProcessShake"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Las rutinas CVector del EXE que llama el original (Normalise 0x59C910, CrossProduct 0x59C730) deben ser las del EXE: se apagan sus hooks durante el banco (si existen).
    env.exeHelpers.emplace_back(new HookOff("CVector","Normalise"));
    env.exeHelpers.emplace_back(new HookOff("CVector","CrossProduct-out"));
    // Cuerpo de la DLL del unico colaborador (la DLL lo llama directamente): se resuelve saltando los E9 de enlace incremental ANTES de crear parches.
    env.dllCollab=Resolve(std::bit_cast<uintptr_t>(&CHandShaker::Process));
    Run(env,ready,registered,bodies);
}
}
