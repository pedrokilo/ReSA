#pragma once
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerShove.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/Task.h"
#include "Pools/TaskPool.h"
#include "References.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerShoveSeq28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartnerShove::GetPartnerSequence (0x683120). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x683120 con un salto a ese cuerpo: la llamada directa llega a la DLL).
namespace PartnerShoveSeq28Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnershoveseq28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnershoveseq28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnershoveseq28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartnerShove","GetPartnerSequence");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartnerShove","GetPartnerSequence",0x683120,body));
    registered["CTaskComplexPartnerShove::GetPartnerSequence"]=env.route->Registered();bodies["CTaskComplexPartnerShove::GetPartnerSequence"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    Run(env,ready,registered,bodies);
}
}
