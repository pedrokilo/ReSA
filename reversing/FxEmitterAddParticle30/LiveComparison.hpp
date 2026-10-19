#pragma once
#include <fstream>
#include <memory>
#include "FxEmitter.h"
#include "FxManager.h"
#include "FxPrimBP.h"
#include "FxInfoManager.h"
#include "FxEmitterPrt.h"
#include "EmissionInfo.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxEmitterAddParticle30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace FxAdd30Live {
inline uintptr_t Entry(auto member){return Resolve(std::bit_cast<uintptr_t>(member));}
// AddParticle es virtual: el hook RH es de tipo Virtual ("V"; RH_ScopedVMTOverloadedInstall(AddParticle, "vector", 0x4A3EA0, ...), nombre "AddParticle-vector"), que cambia la entrada de la
// vtable del EXE (+0x14) y, ademas, parchea la entrada 0x4A3EA0 para las llamadas directas. Un puntero a miembro virtual seria el trampolin de la vcall, no el cuerpo: el cuerpo de la DLL
// (la entrada de SU vtable) se lee del propio hook.
inline uintptr_t VirtualBody(const char* cls,const char* fn){
    uintptr_t body{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){
        if(category.Name()==cls)for(auto& hook:category.Items())
            if(hook->Name()==fn&&hook->Symbol()[0]=='V')body=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook.get())->GetHookOurAddress());
    });
    return body;
}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fxemitteraddparticle30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxemitteraddparticle30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxemitteraddparticle30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado. El "candidato" es la direccion del cuerpo de la DLL (ver VirtualBody), resuelta saltando los E9 del enlace incremental.
    bool ready=true;json registered=json::object();json bodies=json::object();
    const uintptr_t body=Resolve(VirtualBody("FxEmitter_c","AddParticle-vector"));
    env.route.reset(new HookRoute("FxEmitter_c","AddParticle-vector",0x4A3EA0,body));
    const bool reg=env.route->Registered();registered["FxEmitter_c::AddParticle(vector)"]=reg;
    bodies["FxEmitter_c::AddParticle(vector)"]=env.route->Body();
    ready&=reg&&body&&ModuleOf(env.route->Body())==env.asi;
    // Entradas de la DLL de los colaboradores (se resuelven saltando los E9 de enlace incremental).
    env.dllAt[0]=Entry(&FxInfoManager_c::ProcessEmissionInfo);
    env.dllAt[1]=Entry(&FxManager_c::FxRwMatrixCreate);
    env.dllAt[2]=Entry(&FxManager_c::FxRwMatrixDestroy);
    env.dllAt[3]=Entry(&FxPrimBP_c::GetRWMatrix);
    env.dllAt[4]=Resolve(uintptr_t(&RwMatrixUpdate));
    env.dllAt[5]=Resolve(uintptr_t(&RwMatrixMultiply));
    env.dllAt[6]=Entry(&FxEmitter_c::CreateParticle);
    Run(env,ready,registered,bodies);
}
}
