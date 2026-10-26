#pragma once
#include <bit>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "DecisionMakers/DecisionMakerTypes.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Helpers67S dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CDecisionMakerTypes::Constructor (0x4650F0, hook S). CDecision::SetDefault (0x600530) conserva su hook encendido.
static_assert(sizeof(CDecisionMakerTypes) == H67Fix::kObjBytes, "el banco trabaja con sizeof(CDecisionMakerTypes) == 0xF1C0 a mano");
namespace H67Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers67s-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers67s-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers67s");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    env.route.reset(new HookRoute("CDecisionMakerTypes","Constructor",kEntry,HookRoute::RegisteredTarget{}));
    json registered={{kName,env.route->Registered()}};
    json bodies={{kName,env.route->Body()}};
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi&&ReversingProbe::JumpsToDll(kSetDefault);
    // Colaboradores del EXE con trampa en el lado candidato (no tienen cuerpo propio en la DLL que se sustituya: la DLL no los llama).
    auto fake=[](auto* f){return reinterpret_cast<void*>(f);};
    env.collabs={{{0x821D30,0},fake(&TrapVecCtor)},{{0x4650A0,0},fake(&TrapCtorThis)},{{0x6040C0,0},fake(&TrapCtorThis)}};
    Run(env,ready,registered,bodies);
}
}
