#pragma once
#include <fstream>
#include <memory>
#include "FxInterpInfoFloat.h"
#include "FxInterpInfoU255.h"
#include "FxInterpInfo32.h"
#include "FxInterpInfo255.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxInterp61 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace FxInterp61Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fxinterp61-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxinterp61-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxinterp61");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. Nombres: clase (RH_ScopedClass) y funcion; la sobrecarga de Float es "GetVal-values".
    bool ready=true;json registered=json::object();json bodies=json::object();
    for(unsigned i=0;i<kFunctions;++i) {
        env.route[i].reset(new HookRoute(kClasses[i],kHookNames[i],kEntries[i],HookRoute::RegisteredTarget{}));
        const bool reg=env.route[i]->Registered();registered[kNames[i]]=reg;
        const uintptr_t body=env.route[i]->Body();bodies[kNames[i]]=body;
        ready&=reg&&body&&ModuleOf(body)==env.asi;
    }
    Run(env,ready,registered,bodies);
}
}
