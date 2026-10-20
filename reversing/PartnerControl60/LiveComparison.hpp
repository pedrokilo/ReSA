#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskManager.h"
#include "Ragdoll/IKChainManager.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerControl60 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartner::ControlSubTask (0x6840D0). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x6840D0 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartnerDeal) == PartnerControl60Fix::kThisBytes, "CTaskComplexPartnerDeal mide 0x74 (el word +0x70 es de las derivadas)");
struct PartnerControl60SubProbe : CTaskComplex { static constexpr size_t Off() { return offsetof(PartnerControl60SubProbe, m_pSubTask); } };
static_assert(PartnerControl60SubProbe::Off() == PartnerControl60Fix::kSubAtThis, "m_pSubTask en +0x8");
static_assert(offsetof(CTaskComplexPartner, m_taskId) == PartnerControl60Fix::kTaskIdAt, "m_taskId en +0x34");
static_assert(offsetof(CTaskComplexPartner, m_partner) == PartnerControl60Fix::kPartnerAt, "m_partner en +0x38");
static_assert(offsetof(CTaskComplexPartner, m_partnerState) == PartnerControl60Fix::kStateAt, "m_partnerState en +0x59");
static_assert(offsetof(CTaskComplexPartner, m_taskCompleted) == PartnerControl60Fix::kCompletedAt, "m_taskCompleted en +0x5C");
namespace PartnerControl60Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnercontrol60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnercontrol60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnercontrol60");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartner","ControlSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartner","ControlSubTask",0x6840D0,body));
    registered["CTaskComplexPartner::ControlSubTask"]=env.route->Registered();bodies["CTaskComplexPartner::ControlSubTask"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los cuatro colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.stub[0]={0x681740,Body(std::bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType))};
    env.stub[1]={0x5E0360,Body(std::bit_cast<uintptr_t>(&CPed::DropEntityThatThisPedIsHolding))};
    env.stub[2]={0x6181A0,Body(std::bit_cast<uintptr_t>(&IKChainManager_c::IsLooking))};
    env.stub[3]={0x618280,Body(std::bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt))};
    Run(env,ready,registered,bodies);
}
}
