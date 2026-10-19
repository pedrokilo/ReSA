#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "Pools/TaskPool.h"
#include "References.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerDealSeq31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartnerDeal::GetPartnerSequence (0x682440). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x682440 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartnerDeal) == PartnerDealSeq31Fix::kThisBytes, "CTaskComplexPartnerDeal mide 0x74");
static_assert(offsetof(CTaskComplexPartnerDeal, m_partner) == PartnerDealSeq31Fix::kPartnerAt, "m_partner en +0x38");
static_assert(offsetof(CTaskComplexPartnerDeal, m_leadSpeaker) == PartnerDealSeq31Fix::kLeadAt, "m_leadSpeaker en +0x58");
static_assert(sizeof(CEntity) <= PartnerDealSeq31Fix::kEntBytes, "el partner falso cabe en su arena");
namespace PartnerDealSeq31Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnerdealseq31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnerdealseq31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnerdealseq31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartnerDeal","GetPartnerSequence");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartnerDeal","GetPartnerSequence",0x682440,body));
    registered["CTaskComplexPartnerDeal::GetPartnerSequence"]=env.route->Registered();bodies["CTaskComplexPartnerDeal::GetPartnerSequence"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    Run(env,ready,registered,bodies);
}
}
