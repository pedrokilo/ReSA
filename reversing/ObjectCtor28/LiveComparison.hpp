#pragma once
#include <bit>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Object/Object.h"
#include "Entity/Entity.h"
#include "Entity/Physical.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par ObjectCtor28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CObject::CObject(int32, bool) (0x5A1D70) con su HookRoute (hook S `Constructor`, categoria/clase CObject). Colaboradores sustituidos: ver Core.hpp y README.md.
static_assert(sizeof(CObject) == ObjectCtor28Fix::kObjBytes, "el banco trabaja con sizeof(CObject) == 0x17C a mano");
static_assert(offsetof(CObject, m_pDummyObject) == ObjectCtor28Fix::kDummyAt, "m_pDummyObject debe estar en +0x170");
namespace ObjectCtor28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"objectctor28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"objectctor28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("objectctor28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CObject","Constructor",0x5A1D70,HookRoute::RegisteredTarget{}));
    registered["CObject::Constructor"]=env.route->Registered();bodies["CObject::Constructor"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL (constructor y colaboradores): se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    // El constructor de la DLL es la primera llamada con prologo de la envoltura `Constructor`; el constructor base CPhysical, la primera de ese constructor y debe coincidir con la que
    // sale de la envoltura CPhysical::Constructor (mismo cruce que el banco CPedD). Los virtuales de CEntity se leen de su hook V (la direccion de un puntero a virtual es un thunk).
    env.dllCtor=ready?FirstDllCall(env.asi,env.route->Body()):0;
    const uintptr_t physFromCtor=env.dllCtor?FirstDllCall(env.asi,env.dllCtor):0;
    const uintptr_t physFromWrapper=FirstDllCall(env.asi,DllBody("CPhysical","Constructor"));
    env.pPhys={0x542260,physFromCtor};
    env.pSet={0x532AE0,DllBody("CEntity","SetModelIndex")};
    env.pNoCreate={0x533700,DllBody("CEntity","SetModelIndexNoCreate")};
    env.pInit={0x59F840,DllBody("CObject","Init")};
    const auto rel=[&](uintptr_t a){return a?json(a-env.asi):json(0);};
    json resolved={{"dll_ctor",rel(env.dllCtor)},{"phys_from_ctor",rel(physFromCtor)},{"phys_from_wrapper",rel(physFromWrapper)},{"set_model_index",rel(env.pSet.dll)},
                   {"set_model_index_no_create",rel(env.pNoCreate.dll)},{"init",rel(env.pInit.dll)}};
    ready&=env.dllCtor&&physFromCtor&&physFromCtor==physFromWrapper;
    for(const Pair* p:{&env.pPhys,&env.pSet,&env.pNoCreate,&env.pInit})ready&=p->dll&&ModuleOf(p->dll)==env.asi&&p->dll!=p->exe;
    // Los cuatro cuerpos de la DLL deben ser distintos entre si (si dos coincidieran, un mismo parche serviria de dos falsos).
    ready&=env.pPhys.dll!=env.pSet.dll&&env.pPhys.dll!=env.pNoCreate.dll&&env.pPhys.dll!=env.pInit.dll&&env.pSet.dll!=env.pNoCreate.dll&&env.pSet.dll!=env.pInit.dll&&env.pNoCreate.dll!=env.pInit.dll;
    Run(env,ready,registered,bodies,resolved);
}
}
