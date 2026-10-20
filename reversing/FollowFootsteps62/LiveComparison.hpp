#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexFollowPedFootsteps.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleHitHead.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h"
#include "PedGeometryAnalyser.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "Timer.h"
#include "World.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FollowFootsteps62 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco rutas (hooks VIRTUALES: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del candidato se toma de ReversibleHook::Virtual::GetHookOurAddress()).
struct FollowFootsteps62SubProbe : CTaskComplex { static constexpr size_t Off() { return offsetof(FollowFootsteps62SubProbe, m_pSubTask); } };
static_assert(sizeof(CTaskComplexFollowPedFootsteps) == FollowFootsteps62Fix::kThisBytes, "CTaskComplexFollowPedFootsteps mide 0x20");
static_assert(FollowFootsteps62SubProbe::Off() == FollowFootsteps62Fix::kSubAtThis, "m_pSubTask en +0x8");
static_assert(offsetof(CTaskComplexFollowPedFootsteps, m_targetPed) == FollowFootsteps62Fix::kTargetAt, "m_targetPed en +0xC");
static_assert(offsetof(CTaskComplexFollowPedFootsteps, m_subTaskCreateCheckTimer) == FollowFootsteps62Fix::kCreateTimerAt, "m_subTaskCreateCheckTimer en +0x10");
static_assert(offsetof(CTaskComplexFollowPedFootsteps, m_lineOfSightCheckTimer) == FollowFootsteps62Fix::kLosTimerAt, "m_lineOfSightCheckTimer en +0x14");
static_assert(offsetof(CTaskComplexFollowPedFootsteps, m_pointRoute) == FollowFootsteps62Fix::kRouteAt, "m_pointRoute en +0x18");
static_assert(offsetof(CTaskComplexFollowPedFootsteps, m_moveState) == FollowFootsteps62Fix::kMoveAt, "m_moveState en +0x1C");
static_assert(sizeof(CPointRoute) >= FollowFootsteps62Fix::kRouteBytes && offsetof(CPointRoute, m_NumEntries) == 0 && offsetof(CPointRoute, m_Entries) == 4, "CPointRoute: contador +0, puntos +4");
static_assert(sizeof(CTaskSimpleGoToPoint) == 0x30 && sizeof(CTaskSimpleStandStill) == 0x20 && sizeof(CTaskSimpleHitHead) == 0x10 && sizeof(CTaskComplexSeekEntityStandard) == 0x4C, "tamanos de las tareas creadas (argumento de operator new)");
static_assert(offsetof(CTaskSimpleGoTo, m_moveState) == FollowFootsteps62Fix::kSubMoveAt, "CTaskSimpleGoToPoint::m_moveState en +0x8");
static_assert(offsetof(CPlaceable, m_matrix) == FollowFootsteps62Fix::kMatAt && offsetof(CPlaceable, m_placement) == FollowFootsteps62Fix::kPlaceAt, "CPlaceable: placement +4, matriz +0x14");
struct FollowFootsteps62PedProbe : CPed { static constexpr size_t Off() { return offsetof(FollowFootsteps62PedProbe, m_pIntelligence); } };
static_assert(FollowFootsteps62PedProbe::Off() == FollowFootsteps62Fix::kIntelAt && offsetof(CPed, field_468) + 4 == FollowFootsteps62Fix::kPedFlagsAt, "CPed: inteligencia +0x47C, banderas +0x46C");
static_assert(offsetof(CPedIntelligence, m_AnotherStaticCounter) == FollowFootsteps62Fix::kCounterAt, "m_AnotherStaticCounter en +0x274");
static_assert(sizeof(CMatrix) == FollowFootsteps62Fix::kMatBytes, "CMatrix mide 0x48");
namespace FollowFootsteps62Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"followfootsteps62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"followfootsteps62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("followfootsteps62");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    for(unsigned i=0;i<kFns;++i) {
        const auto& f=Fns()[i];
        auto* hook=FindHook("CTaskComplexFollowPedFootsteps",f.id);
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        env.route[i].reset(new HookRoute("CTaskComplexFollowPedFootsteps",f.id,f.entry,body));
        registered[f.name]=env.route[i]->Registered();bodies[f.name]=env.route[i]->Body();
        ready&=env.route[i]->Registered()&&env.route[i]->Body()&&ModuleOf(env.route[i]->Body())==env.asi;
    }
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.stub[StAlive]={0x5E0170,Body(std::bit_cast<uintptr_t>(&CPed::IsAlive))};
    env.stub[StLos]={0x56A490,Body(std::bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear))};
    env.stub[StSimplest]={0x6819D0,Body(std::bit_cast<uintptr_t>(&CTaskManager::GetSimplestActiveTask))};
    env.stub[StIsGoTo]={0x61A360,Body(std::bit_cast<uintptr_t>(&CTask::IsGoToTask)),false};   // sin hook: la entrada del EXE sigue siendo codigo original
    env.stub[StUpdate]={0x645700,Body(std::bit_cast<uintptr_t>(&CTaskSimpleGoToPoint::UpdatePoint))};
    env.stub[StNew]={0x61A5A0,Body(std::bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CTask::operator new)))};
    Run(env,ready,registered,bodies);
}
}
