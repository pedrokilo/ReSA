#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/Task.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "Pools/TaskPool.h"
#include "References.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerChatSeq30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartnerChat::GetPartnerSequence (0x684380). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x684380 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartnerChat) == PartnerChatSeq30Fix::kThisBytes, "CTaskComplexPartnerChat mide 0x78");
static_assert(offsetof(CTaskComplexPartnerChat, m_partner) == PartnerChatSeq30Fix::kPartnerAt, "m_partner en +0x38");
static_assert(offsetof(CTaskComplexPartnerChat, m_point) == PartnerChatSeq30Fix::kPointAt, "m_point en +0x40");
static_assert(offsetof(CTaskComplexPartnerChat, m_leadSpeaker) == PartnerChatSeq30Fix::kLeadAt, "m_leadSpeaker en +0x58");
static_assert(offsetof(CTaskComplexPartnerChat, m_updateDirectionCount) == PartnerChatSeq30Fix::kNAt, "m_updateDirectionCount en +0x5B");
static_assert(offsetof(CTaskComplexPartnerChat, m_conversationEnabled) == PartnerChatSeq30Fix::kConvAt, "m_conversationEnabled en +0x74");
struct PartnerChatSeq30PedOffProbe : CPed { static constexpr size_t Off() { return offsetof(PartnerChatSeq30PedOffProbe, m_pIntelligence); } };
static_assert(PartnerChatSeq30PedOffProbe::Off() == PartnerChatSeq30Fix::kIntAt, "CPed::m_pIntelligence en +0x47C");
static_assert(sizeof(CVector) == 12, "m_point son tres floats");
namespace PartnerChatSeq30Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnerchatseq30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnerchatseq30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnerchatseq30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartnerChat","GetPartnerSequence");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartnerChat","GetPartnerSequence",0x684380,body));
    registered["CTaskComplexPartnerChat::GetPartnerSequence"]=env.route->Registered();bodies["CTaskComplexPartnerChat::GetPartnerSequence"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpo de la DLL del colaborador sustituido: se resuelve ANTES de escribir ningun parche (con su hook encendido, la entrada 0x600EE0 salta a la DLL; Body() sigue el salto).
    env.pFind={0x600EE0,Body(std::bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType))};
    Run(env,ready,registered,bodies);
}
}
