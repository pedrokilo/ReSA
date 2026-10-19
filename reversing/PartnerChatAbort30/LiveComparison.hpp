#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Tasks/Task.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerChatAbort30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartnerChat::MakeAbortable (0x682C60). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del candidato se
// toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x682C60 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartnerChat) == PartnerChatAbort30Fix::kThisBytes, "CTaskComplexPartnerChat mide 0x78");
struct PartnerChatAbort30OffProbe : CTaskComplexPartnerChat { static constexpr size_t Off() { return offsetof(PartnerChatAbort30OffProbe, m_pSubTask); } };
static_assert(PartnerChatAbort30OffProbe::Off() == PartnerChatAbort30Fix::kSubAt, "m_pSubTask en +8");
static_assert(offsetof(CTaskComplexPartnerChat, m_conversationEnabled) == PartnerChatAbort30Fix::kConvAt, "m_conversationEnabled en +0x74");
static_assert(offsetof(CTaskComplexPartnerChat, m_pedConversationLoaded) == PartnerChatAbort30Fix::kLoadedAt, "m_pedConversationLoaded en +0x76");
namespace PartnerChatAbort30Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnerchatabort30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnerchatabort30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnerchatabort30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartnerChat","MakeAbortable");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartnerChat","MakeAbortable",0x682C60,body));
    registered["CTaskComplexPartnerChat::MakeAbortable"]=env.route->Registered();bodies["CTaskComplexPartnerChat::MakeAbortable"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpo de la DLL del colaborador: se resuelve ANTES de escribir ningun parche (PROCESO.md, trampas). Con el hook de ReleasePedConversation encendido
    // (estado normal del ASI) la entrada 0x4E52A0 salta a la DLL; Body() sigue el salto.
    env.pRelease={0x4E52A0,Body(std::bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::ReleasePedConversation))};
    Run(env,ready,registered,bodies);
}
}
