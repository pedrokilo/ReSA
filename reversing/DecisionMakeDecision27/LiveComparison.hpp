#pragma once
#include <fstream>
#include <memory>
#include "DecisionMakers/Decision.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par DecisionMakeDecision27 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace DecisionMakeDecision27Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"decisionmakedecision27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"decisionmakedecision27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("decisionmakedecision27");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CDecision","MakeDecision",0x6040D0,HookRoute::RegisteredTarget{}));
    const bool reg=env.route->Registered();registered["CDecision::MakeDecision"]=reg;
    const uintptr_t body=env.route->Body();bodies["CDecision::MakeDecision"]=body;
    ready&=reg&&body&&ModuleOf(body)==env.asi;
    Run(env,ready,registered,bodies);
}
}
