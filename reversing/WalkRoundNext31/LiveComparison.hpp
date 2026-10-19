#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/Task.h"
#include "PedGeometryAnalyser.h"
#include "Entity/Ped/Ped.h"
#include "Pools/TaskPool.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par WalkRoundNext31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexWalkRoundObject::CreateNextSubTask (0x657220). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x657220 con un salto a ese cuerpo).
struct WalkRoundNext31TaskOffProbe : CTaskComplexWalkRoundObject {
    static constexpr size_t SubOff() { return offsetof(WalkRoundNext31TaskOffProbe, m_pSubTask); }
};
static_assert(sizeof(CTaskComplexWalkRoundObject) == WalkRoundNext31Fix::kThisBytes, "CTaskComplexWalkRoundObject mide 0x54");
static_assert(WalkRoundNext31TaskOffProbe::SubOff() == WalkRoundNext31Fix::kSubTaskAt, "m_pSubTask en +8");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_moveState) == WalkRoundNext31Fix::kMoveAt, "m_moveState en +0xC");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_targetPoint) == WalkRoundNext31Fix::kTargetAt, "m_targetPoint en +0x10");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_object) == WalkRoundNext31Fix::kObjectAt, "m_object en +0x1C");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_pointRoute) == WalkRoundNext31Fix::kRouteAt, "m_pointRoute en +0x20");
static_assert(sizeof(CPointRoute) == WalkRoundNext31Fix::kRouteBytes, "CPointRoute: contador (+0) + 8 vectores (+4), 0x64 bytes");
static_assert(sizeof(CVector) == 12, "los vectores son tres floats");
namespace WalkRoundNext31Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"walkroundnext31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"walkroundnext31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("walkroundnext31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexWalkRoundObject","CreateNextSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexWalkRoundObject","CreateNextSubTask",0x657220,body));
    registered["CTaskComplexWalkRoundObject::CreateNextSubTask"]=env.route->Registered();bodies["CTaskComplexWalkRoundObject::CreateNextSubTask"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.pClosestPed={0x5F3B70,Body(std::bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,CEntity&,CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint)))};
    env.pClosestPosn={0x5F36F0,Body(std::bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CEntity&,CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint)))};
    env.pHitSide={0x5F3AC0,Body(std::bit_cast<uintptr_t>(static_cast<int32(*)(const CVector&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide)))};
    env.pCreateRoute={0x655140,0};   // CreateRouteTask no esta enganchada: la DLL la llama por plugin a la direccion del EXE, que se parchea
    Run(env,ready,registered,bodies);
}
}
