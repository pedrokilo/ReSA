#pragma once
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Entity.h"
#include "RwHelper.h"
#include "Timer.h"
#include "Cheat.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par AutoRender75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
namespace AutoRender75Live {
// Cuerpo de la DLL de un gancho virtual (la tabla virtual de la DLL apunta al cuerpo; se resuelven los E9 del enlace incremental)
inline uintptr_t Body(const char* cls,const char* name){
    ReversibleHooks::ReversibleHook::Base* h{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& x:c.Items())if(x->Name()==name)h=x.get();});
    return h&&h->Symbol()[0]=='V'?Resolve(uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress())):0;
}
// Direccion del cuerpo de la DLL de una funcion (miembro no virtual o libre), saltando los E9 de enlace ANTES de crear parches
// Se siguen los saltos (tabla de enlace incremental) sólo mientras caigan en el ASI: con el gancho apagado (SurfaceInfos_c::IsWater/IsSand) la entrada de la DLL salta
// al EXE y hay que parchear esa entrada, no la del EXE (que ya se parchea aparte).
inline uintptr_t ResolveInAsi(uintptr_t address,uintptr_t asi){for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i){const uintptr_t t=address+5+*reinterpret_cast<int32*>(address+1);if(ModuleOf(t)!=asi)break;address=t;}return address;}
template<class F> inline uintptr_t Addr(F f){return ResolveInAsi(std::bit_cast<uintptr_t>(f),ModuleOf(uintptr_t(&ModuleMarker)));}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"autorender75-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"autorender75-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("autorender75");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el gancho apagado. Render es virtual (RH_ScopedVMTInstall): el candidato es el cuerpo de la DLL de la tabla virtual.
    const uintptr_t renderBody=Body("CAutomobile","Render");
    env.route.reset(new HookRoute("CAutomobile","Render",0x6A2B10,reinterpret_cast<void*>(renderBody)));
    bool ready=true;json registered=json::object();json bodies=json::object();
    registered[kName]=env.route->Registered();bodies[kName]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores (la DLL los llama directamente)
    DllAddr& d=env.dll;
    d.getFirstObject=Addr(&GetFirstObject);
    d.vehicleRender=Body("CVehicle","Render");
    d.getColModel=Addr(&CEntity::GetColModel);
    d.appearance=Addr(&CVehicle::GetVehicleAppearance);
    d.beam=Addr(&CVehicle::DoHeadLightBeam);
    Run(env,ready,registered,bodies);
}
}
