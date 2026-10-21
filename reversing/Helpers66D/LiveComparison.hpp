#pragma once
#include <bit>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Heli.h"
#include "Entity/Object/Object.h"
#include "Entity/Object/CutsceneObject.h"
#include "Garage.h"
#include "Cam.h"
#include "World.h"
#include "DamageManager.h"
#include "RwHelper.h"
#include "RenderWare/rw/rpworld.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Helpers66D dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro rutas (una por funcion), cada una con su HookRoute (hook S): RwHelper::RpClumpGetBoundingSphere, CGarage::FindDoorsWithGarage, CCam::GetArrestCamCopSourceSide y CHeli::Constructor.
static_assert(sizeof(CHeli) == H66Fix::kHeliBytes, "el banco trabaja con sizeof(CHeli) == 0xA18 a mano");
static_assert(sizeof(CCutsceneObject) == H66Fix::kObjBytes, "el pool de objetos tiene elementos de 0x19C bytes (el mayor, CCutsceneObject)");
static_assert(sizeof(CGarage) == 0xD8, "el indice del garaje divide entre sizeof(CGarage) = 0xD8 (imul 0x4BDA12F7, sar 6)");
namespace H66Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers66d-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers66d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers66d");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    const struct {const char* cls;const char* fn;} hooks[kFunc]{{"RwHelper","RpClumpGetBoundingSphere"},{"CGarage","FindDoorsWithGarage"},{"CCam","GetArrestCamCopSourceSide"},{"CHeli","Constructor"}};
    json registered=json::object();json bodies=json::object();
    bool ready=true;
    for(unsigned f=0;f<kFunc;++f){
        env.route[f].reset(new HookRoute(hooks[f].cls,hooks[f].fn,kEntries[f],HookRoute::RegisteredTarget{}));
        registered[kNames[f]]=env.route[f]->Registered();bodies[kNames[f]]=env.route[f]->Body();
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto fake=[](auto* f){return reinterpret_cast<void*>(f);};
    env.collabs[kClump]={{{0x7498E0,Body(reinterpret_cast<uintptr_t>(&RpClumpGetNumAtomics))},fake(&FkGetNum),true},
                         {{0x749B70,0},fake(&FkForAll),false},{{0x7F0990,0},fake(&FkGetLtm),false},{{0x7F2070,0},fake(&FkInvert),false},{{0x7EDD90,0},fake(&FkTransform),false}};
    env.collabs[kGarage]={};
    void (CVector::*normalise)()=&CVector::Normalise;
    CVector* (*crossOut)(CVector*,CVector*,CVector*)=&CrossProduct;
    env.collabs[kCop]={{{0x59C910,Body(std::bit_cast<uintptr_t>(normalise))},fake(&FkNormalise),true},
                       {{0x59C730,Body(reinterpret_cast<uintptr_t>(crossOut))},fake(&FkCross),true},
                       {{0x5696C0,Body(reinterpret_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord))},fake(&FkGround),true}};
    env.dllHeliCtor=ready?FirstDllCall(env.asi,env.route[kHeli]->Body()):0;
    void (CDamageManager::*setDoor)(eDoors,eDoorStatus)=&CDamageManager::SetDoorStatus;
    env.collabs[kHeli]={{{0x6B0A90,env.dllHeliCtor?FirstDllCall(env.asi,env.dllHeliCtor):0},fake(&FkAutoCtor),true},
                        {{0x6C21C0,Body(std::bit_cast<uintptr_t>(setDoor))},fake(&FkSetDoor),true},
                        {{0x821B1E,0},fake(&FkRand),false}};
    json resolved=json::object();
    const auto rel=[&](uintptr_t a){return a?json(a-env.asi):json(0);};
    resolved["heli_ctor"]=rel(env.dllHeliCtor);
    const char* const labels[kFunc][5]{{"get_num_atomics","for_all_atomics","get_ltm","matrix_invert","transform_points"},{},{"normalise","cross_product","find_ground_z",nullptr,nullptr},{"automobile_ctor","set_door_status","rand",nullptr,nullptr}};
    for(unsigned f=0;f<kFunc;++f){
        unsigned i=0;
        for(const auto& c:env.collabs[f]){
            resolved[std::string(kNames[f])+"/"+labels[f][i++]]=rel(c.pair.dll);
            ready&=c.hooked?(c.pair.dll&&ModuleOf(c.pair.dll)==env.asi&&c.pair.dll!=c.pair.exe):c.pair.dll==0;
        }
    }
    // Los cuerpos de la DLL deben ser distintos entre si (si dos coincidieran, un mismo parche serviria de dos falsos).
    for(unsigned f=0;f<kFunc;++f)for(size_t i=0;i<env.collabs[f].size();++i)for(size_t j=i+1;j<env.collabs[f].size();++j){
        const auto& a=env.collabs[f][i].pair;const auto& b=env.collabs[f][j].pair;
        ready&=a.exe!=b.exe&&(a.dll==0||a.dll!=b.dll);
    }
    Run(env,ready,registered,bodies,resolved);
}
}
