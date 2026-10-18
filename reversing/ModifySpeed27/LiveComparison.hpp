#pragma once
#include <fstream>
#include <memory>
#include "AutoPilot.h"
#include "Curves.h"
#include "PathFind.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par ModifySpeed27 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace ModifySpeed27Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"modifyspeed27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"modifyspeed27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("modifyspeed27");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CAutoPilot","ModifySpeed",0x41B980,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CAutoPilot::ModifySpeed"]=reg;
    const uintptr_t body=env.route->Body();bodies["CAutoPilot::ModifySpeed"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    // Cuerpo de la DLL del unico colaborador (la DLL lo llama directamente): se resuelve saltando los E9 de enlace incremental ANTES de crear parches.
    env.dllCalc=Resolve(reinterpret_cast<uintptr_t>(&CCurves::CalcSpeedScaleFactor));
    Run(env,ready,registered,bodies);
}
}
