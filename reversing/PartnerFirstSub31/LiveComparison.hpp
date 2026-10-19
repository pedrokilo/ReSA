#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Tasks/Task.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerFirstSub31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartner::CreateFirstSubTask (0x681F20). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del candidato se
// toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x681F20 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartnerChat) == PartnerFirstSub31Fix::kThisBytes, "CTaskComplexPartnerChat mide 0x78");
static_assert(sizeof(CTaskSimpleStandStill) == PartnerFirstSub31Fix::kObjBytes, "CTaskSimpleStandStill mide 0x20");
static_assert(TASK_COMPLEX_PARTNER_CHAT == PartnerFirstSub31Fix::kChatType, "TASK_COMPLEX_PARTNER_CHAT = 0x4B4");
static_assert(offsetof(CTaskComplexPartner, m_taskId) == PartnerFirstSub31Fix::kTaskIdAt, "m_taskId en +0x34");
static_assert(offsetof(CTaskComplexPartner, m_partner) == PartnerFirstSub31Fix::kPartnerAt, "m_partner en +0x38");
static_assert(offsetof(CTaskComplexPartner, m_leadSpeaker) == PartnerFirstSub31Fix::kLeadAt, "m_leadSpeaker en +0x58");
static_assert(offsetof(CTaskComplexPartnerChat, m_conversationEnabled) == PartnerFirstSub31Fix::kConvAt, "m_conversationEnabled en +0x74");
static_assert(offsetof(CTaskComplexPartnerChat, field_75) == PartnerFirstSub31Fix::kF75At, "field_75 en +0x75");
static_assert(offsetof(CTaskComplexPartnerChat, m_pedConversationLoaded) == PartnerFirstSub31Fix::kLoadedAt, "m_pedConversationLoaded en +0x76");
namespace PartnerFirstSub31Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnerfirstsub31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnerfirstsub31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnerfirstsub31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartner","CreateFirstSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartner","CreateFirstSubTask",0x681F20,body));
    registered["CTaskComplexPartner::CreateFirstSubTask"]=env.route->Registered();bodies["CTaskComplexPartner::CreateFirstSubTask"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas). Con sus hooks encendidos (estado normal del ASI)
    // la entrada del EXE salta a la DLL; Body() sigue el salto. El constructor 0x62F310 NO se sustituye (real en ambos lados).
    env.pReq={0x4E50E0,Body(std::bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::RequestPedConversation))};
    env.pHand={0x5E0480,Body(std::bit_cast<uintptr_t>(&CPed::StopPlayingHandSignal))};
    env.pNew={0x61A5A0,Body(std::bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CTask::operator new)))};
    Run(env,ready,registered,bodies);
}
}
