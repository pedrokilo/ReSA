#pragma once
#include <algorithm>
#include <cfloat>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Entity.h"
#include "Core/Matrix.h"
#include "Timer.h"
#include "Cheat.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par AutoPreRender60 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace AutoPreRender60Live {
// Cuerpo de la DLL de un gancho virtual (la tabla virtual de la DLL apunta al cuerpo; se resuelven los E9 del enlace incremental)
inline uintptr_t Body(const char* cls,const char* name){
    ReversibleHooks::ReversibleHook::Base* h{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& x:c.Items())if(x->Name()==name)h=x.get();});
    return h&&h->Symbol()[0]=='V'?Resolve(uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress())):0;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"autoprerender60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"autoprerender60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("autoprerender60");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el gancho apagado. PreRender es virtual (RH_ScopedVMTInstall): el candidato es el cuerpo de la DLL de la tabla virtual.
    bool ready=true;json registered=json::object();json bodies=json::object();
    const uintptr_t preRenderBody=Body("CAutomobile","PreRender");
    env.route.reset(new HookRoute("CAutomobile","PreRender",0x6AAB50,reinterpret_cast<void*>(preRenderBody)));
    registered[kName]=env.route->Registered();bodies[kName]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // GetVehicleAppearance (0x6D1080): el gancho RH es la envoltura GetVehicleAppearance_Reversed; la fuente llama al miembro real CVehicle::GetVehicleAppearance
    env.extra.emplace_back(ResolveInAsi(std::bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),env.asi),size_t(10));
    Run(env,ready,registered,bodies);
}
}
