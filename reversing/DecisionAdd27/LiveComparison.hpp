#pragma once
#include <fstream>
#include <memory>
#include "DecisionMakers/Decision.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par DecisionAdd27 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos rutas: CDecision::Add (0x600600) y CDecisionMakerTypes::AddEventResponse (0x6044C0), cada una con su HookRoute.
static_assert(sizeof(CDecision) == 0x3C, "el banco trabaja con sizeof(CDecision) == 0x3C a mano");
static_assert(4u + 20u * sizeof(CDecisionMaker) == DecisionAdd27Fix::kEventTable, "m_EventIndices debe estar en +0xC034");
namespace DecisionAdd27Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"decisionadd27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"decisionadd27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("decisionadd27");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.routeAdd.reset(new HookRoute("CDecision","Add",0x600600,HookRoute::RegisteredTarget{}));
    env.routeAer.reset(new HookRoute("CDecisionMakerTypes","AddEventResponse",0x6044C0,HookRoute::RegisteredTarget{}));
    registered["CDecision::Add"]=env.routeAdd->Registered();registered["CDecisionMakerTypes::AddEventResponse"]=env.routeAer->Registered();
    bodies["CDecision::Add"]=env.routeAdd->Body();bodies["CDecisionMakerTypes::AddEventResponse"]=env.routeAer->Body();
    ready&=env.routeAdd->Registered()&&env.routeAer->Registered()&&env.routeAdd->Body()&&env.routeAer->Body()
        &&ModuleOf(env.routeAdd->Body())==env.asi&&ModuleOf(env.routeAer->Body())==env.asi;
    Run(env,ready,registered,bodies);
}
}
