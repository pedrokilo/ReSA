#pragma once
#include <bit>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Object/Projectile.h"
#include "Entity/Object/Object.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Helpers65C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco rutas (una por funcion), cada una con su HookRoute (hook S): Fx_c::{TriggerWaterHydrant,TriggerTankFire,AddTyreBurst}, CProjectile::Constructor y
// CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB. Colaboradores sustituidos: ver Core.hpp y README.md.
static_assert(sizeof(CProjectile) == Helpers65CFix::kObjBytes, "el banco trabaja con sizeof(CProjectile) == 0x17C a mano");
namespace Helpers65CLive {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers65c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers65c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers65c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    const struct {const char* cls;const char* fn;} hooks[kFunc]{{"Fx_c","TriggerWaterHydrant"},{"Fx_c","TriggerTankFire"},{"Fx_c","AddTyreBurst"},{"CProjectile","Constructor"},{"CTaskSimpleCarDrive","FinishAnimCloseDoorRollingCB"}};
    json registered=json::object();json bodies=json::object();
    bool ready=true;
    for(unsigned f=0;f<kFunc;++f){
        env.route[f].reset(new HookRoute(hooks[f].cls,hooks[f].fn,kEntries[f],HookRoute::RegisteredTarget{}));
        registered[kNames[f]]=env.route[f]->Registered();bodies[kNames[f]]=env.route[f]->Body();
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas). Hooks S/V por (clase, nombre); CObject::CObject() no tiene hook:
    // es la primera llamada del constructor de la DLL de CProjectile (que a su vez es la primera llamada de la envoltura `Constructor`); CaptureDllVptr lo confirma con la traza.
    env.create={0x4A9BE0,DllBody("FxManager_c","CreateFxSystem-4")};
    env.play={0x4AA3D0,DllBody("FxSystem_c","PlayAndKill")};
    env.matCreate={0x4A9440,DllBody("FxManager_c","FxRwMatrixCreate")};
    env.matDestroy={0x4A9460,DllBody("FxManager_c","FxRwMatrixDestroy")};
    env.createMat={0x49E950,DllBody("Fx_c","CreateMatFromVec")};
    env.copyParent={0x4AA890,DllBody("FxSystem_c","CopyParentMatrix")};
    env.addParticle={0x4AA440,DllBody("FxSystem_c","AddParticle-v3d")};
    env.dllProjCtor=ready?FirstDllCall(env.asi,env.route[kProjCtor]->Body()):0;
    env.objCtor={0x5A1D10,env.dllProjCtor?FirstDllCall(env.asi,env.dllProjCtor):0};
    env.setStatic={0x5A0760,DllBody("CObject","SetIsStatic")};
    env.setModel={0x532AE0,DllBody("CEntity","SetModelIndex")};
    env.clear={0x6D3060,DllBody("CVehicle","ClearGettingOutFlags")};
    env.group={0x6E3B00,DllBody("CVehicleAnimGroup","GetGroup")};
    const Pair* all[]{&env.create,&env.play,&env.matCreate,&env.matDestroy,&env.createMat,&env.copyParent,&env.addParticle,&env.objCtor,&env.setStatic,&env.setModel,&env.clear,&env.group};
    const auto rel=[&](uintptr_t a){return a?json(a-env.asi):json(0);};
    json resolved={{"proj_ctor",rel(env.dllProjCtor)}};
    const char* labels[]{"create_fx","play_and_kill","mat_create","mat_destroy","create_mat_from_vec","copy_parent_matrix","add_particle","object_ctor","set_is_static","set_model_index","clear_getting_out","get_group"};
    for(unsigned i=0;i<std::size(all);++i){
        resolved[labels[i]]=rel(all[i]->dll);
        ready&=all[i]->dll&&ModuleOf(all[i]->dll)==env.asi&&all[i]->dll!=all[i]->exe;
    }
    // Los cuerpos de la DLL deben ser distintos entre si (si dos coincidieran, un mismo parche serviria de dos falsos).
    for(unsigned i=0;i<std::size(all);++i)for(unsigned j=i+1;j<std::size(all);++j)ready&=all[i]->dll!=all[j]->dll&&all[i]->exe!=all[j]->exe;
    Run(env,ready,registered,bodies,resolved);
}
}
