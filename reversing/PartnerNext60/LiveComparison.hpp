#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskTypes/TaskComplexWanderStandard.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskManager.h"
#include "General.h"
#include "Timer.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PartnerNext60 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexPartner::CreateNextSubTask (0x683AD0). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x683AD0 con un salto a ese cuerpo).
static_assert(sizeof(CTaskComplexPartnerDeal) == PartnerNext60Fix::kThisBytes, "CTaskComplexPartnerDeal mide 0x74 (el word +0x70 es de las derivadas)");
struct PartnerNext60SubProbe : CTaskComplex { static constexpr size_t Off() { return offsetof(PartnerNext60SubProbe, m_pSubTask); } };
static_assert(PartnerNext60SubProbe::Off() == PartnerNext60Fix::kSubAtThis, "m_pSubTask en +0x8");
static_assert(offsetof(CTaskComplexPartner, m_taskId) == PartnerNext60Fix::kTaskIdAt, "m_taskId en +0x34");
static_assert(offsetof(CTaskComplexPartner, m_partner) == PartnerNext60Fix::kPartnerAt, "m_partner en +0x38");
static_assert(offsetof(CTaskComplexPartner, m_point) == PartnerNext60Fix::kPointAt, "m_point en +0x40");
static_assert(offsetof(CTaskComplexPartner, m_targetPoint) == PartnerNext60Fix::kTargetAt, "m_targetPoint en +0x4C");
static_assert(offsetof(CTaskComplexPartner, m_leadSpeaker) == PartnerNext60Fix::kLeadAt, "m_leadSpeaker en +0x58");
static_assert(offsetof(CTaskComplexPartner, m_partnerState) == PartnerNext60Fix::kStateAt, "m_partnerState en +0x59");
static_assert(offsetof(CTaskComplexPartner, m_firstToTargetFlag) == PartnerNext60Fix::kFirstAt, "m_firstToTargetFlag en +0x5A");
static_assert(offsetof(CTaskComplexPartner, m_updateDirectionCount) == PartnerNext60Fix::kUpdAt, "m_updateDirectionCount en +0x5B");
static_assert(offsetof(CTaskComplexPartner, m_makePedAlwaysFacePartner) == PartnerNext60Fix::kFaceAt, "m_makePedAlwaysFacePartner en +0x5D");
static_assert(offsetof(CTaskComplexPartner, m_requiredAnimsStreamedIn) == PartnerNext60Fix::kStreamedAt, "m_requiredAnimsStreamedIn en +0x6E");
static_assert(offsetof(CPed, m_vecAnimMovingShiftLocal) == PartnerNext60Fix::kShiftAt, "m_vecAnimMovingShiftLocal en +0x4D8");
static_assert(sizeof(CTaskSimpleStandStill) == PartnerNext60Fix::kSizeStand && sizeof(CTaskComplexWanderStandard) == PartnerNext60Fix::kSizeWander
           && sizeof(CTaskComplexGoToPointAndStandStill) == PartnerNext60Fix::kSizeGoTo && sizeof(CTaskComplexTurnToFaceEntityOrCoord) == PartnerNext60Fix::kSizeTurn, "tamanos de las tareas creadas (argumento de operator new)");
static_assert(sizeof(CMatrix) == PartnerNext60Fix::kMatBytes, "CMatrix mide 0x48");
static_assert(sizeof(CTaskManager) >= 0x14 && offsetof(CTaskManager, m_aPrimaryTasks) == 0, "m_aPrimaryTasks al principio del administrador (intel+4)");
namespace PartnerNext60Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"partnernext60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"partnernext60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("partnernext60");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexPartner","CreateNextSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexPartner","CreateNextSubTask",0x683AD0,body));
    registered["CTaskComplexPartner::CreateNextSubTask"]=env.route->Registered();bodies["CTaskComplexPartner::CreateNextSubTask"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    // 0x681FE0 no esta reconstruido: la DLL lo llama por su direccion del EXE, que se parchea como los demas (sin cuerpo propio en la DLL).
    env.stub[0]={0x681740,Body(std::bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType))};
    env.stub[1]={0x53CDC0,Body(std::bit_cast<uintptr_t>(&CGeneral::GetNodeHeadingFromVector))};
    env.stub[2]={0x61A5A0,Body(std::bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CTask::operator new)))};
    env.stub[3]={0x681AF0,Body(std::bit_cast<uintptr_t>(&CTaskManager::SetTask))};
    env.stub[4]={0x681FE0,0,false};
    Run(env,ready,registered,bodies);
}
}
