#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartner.h"
#include "Animation/AnimManager.h"
#include "Streaming.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "Pools/TaskPool.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerStream60 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartner::StreamRequiredAnims (0x682310). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x682310 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartner) == PartnerStream60Fix::kThisBytes, "CTaskComplexPartner mide 0x70");
static_assert(offsetof(CTaskComplexPartner, m_animBlockName) == PartnerStream60Fix::kNameAt, "m_animBlockName en +0x5E");
static_assert(offsetof(CTaskComplexPartner, m_requiredAnimsStreamedIn) == PartnerStream60Fix::kFlagAt, "m_requiredAnimsStreamedIn en +0x6E");
static_assert(sizeof(CAnimBlock) == PartnerStream60Fix::kBlockSize && offsetof(CAnimBlock, IsLoaded) == PartnerStream60Fix::kLoadedAt, "CAnimBlock mide 0x20 con IsLoaded en +0x10");
static_assert(RESOURCE_ID_IFP == PartnerStream60Fix::kModelBase, "IFPToModelId(i) = i + 0x63E7");
static_assert(STREAMING_KEEP_IN_MEMORY == PartnerStream60Fix::kKeepInMemory, "STREAMING_KEEP_IN_MEMORY = 8");
namespace PartnerStream60Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnerstream60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnerstream60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnerstream60");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartner","StreamRequiredAnims");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartner","StreamRequiredAnims",0x682310,body));
    registered["CTaskComplexPartner::StreamRequiredAnims"]=env.route->Registered();bodies["CTaskComplexPartner::StreamRequiredAnims"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los tres colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.stub[0]={0x4D3990,Body(std::bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)))};
    env.stub[1]={0x4087E0,Body(std::bit_cast<uintptr_t>(&CStreaming::RequestModel))};
    env.stub[2]={0x4D3FB0,Body(std::bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef))};
    Run(env,ready,registered,bodies);
}
}
