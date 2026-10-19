#pragma once
#include <fstream>
#include <memory>
#include "Quaternion.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par QuatEulerGet29 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace QuatEulerGet29Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"quateulerget29-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"quateulerget29-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("quateulerget29");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. Hooks: "Get-euler" (RH_ScopedOverloadedInstall(Get, "euler", 0x59C160, ...)) y, como colaborador apagado, "Get-matrix" (0x59C080).
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CQuaternion","Get-euler",0x59C160,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CQuaternion::Get(float*, float*, float*)"]=reg;
    const uintptr_t body=env.route->Body();bodies["CQuaternion::Get(float*, float*, float*)"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Colaborador: el hook de 0x59C080 debe existir y quedar apagado (el lado original ejecuta el EXE). Su cuerpo en el ASI se anota (no es el candidato).
    env.matrixRoute.reset(new HookRoute("CQuaternion","Get-matrix",0x59C080,HookRoute::RegisteredTarget{}));
    const bool regMatrix=env.matrixRoute->Registered();registered["CQuaternion::Get(RwMatrix*) [colaborador, hook apagado]"]=regMatrix;
    const uintptr_t matrixBody=env.matrixRoute->Body();bodies["CQuaternion::Get(RwMatrix*) [colaborador, hook apagado]"]=matrixBody;
    ready&=regMatrix&&matrixBody&&ModuleOf(matrixBody)==env.asi;
    Run(env,ready,registered,bodies);
}
}
