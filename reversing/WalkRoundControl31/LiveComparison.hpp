#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/Task.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "SimpleTransform.h"
#include "Pools/TaskPool.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par WalkRoundControl31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexWalkRoundObject::ControlSubTask (0x6575F0). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x6575F0 con un salto a ese cuerpo).
struct WalkRoundControl31TaskOffProbe : CTaskComplexWalkRoundObject {
    static constexpr size_t SubOff() { return offsetof(WalkRoundControl31TaskOffProbe, m_pSubTask); }
};
struct WalkRoundControl31PedOffProbe : CPed { static constexpr size_t Off() { return offsetof(WalkRoundControl31PedOffProbe, m_pIntelligence); } };
static_assert(sizeof(CTaskComplexWalkRoundObject) == WalkRoundControl31Fix::kThisBytes, "CTaskComplexWalkRoundObject mide 0x54");
static_assert(WalkRoundControl31TaskOffProbe::SubOff() == WalkRoundControl31Fix::kSubTaskAt, "m_pSubTask en +8");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_object) == WalkRoundControl31Fix::kObjectAt, "m_object en +0x1C");
static_assert(offsetof(CTaskComplexWalkRoundObject, field_24) == WalkRoundControl31Fix::kF24At, "field_24 en +0x24");
static_assert(offsetof(CTaskComplexWalkRoundObject, field_28) == WalkRoundControl31Fix::kF28At, "field_28 en +0x28");
static_assert(offsetof(CTaskComplexWalkRoundObject, field_2C) == WalkRoundControl31Fix::kF2CAt, "field_2C en +0x2C");
static_assert(offsetof(CTaskComplexWalkRoundObject, field_2D) == WalkRoundControl31Fix::kF2DAt, "field_2D en +0x2D");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_objectStartPos) == WalkRoundControl31Fix::kStartPosAt, "m_objectStartPos en +0x30");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_objectStartForward) == WalkRoundControl31Fix::kStartFwdAt, "m_objectStartForward en +0x3C");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_objectStartRight) == WalkRoundControl31Fix::kStartRightAt, "m_objectStartRight en +0x48");
static_assert(WalkRoundControl31PedOffProbe::Off() == WalkRoundControl31Fix::kIntPtrAt, "CPed::m_pIntelligence en +0x47C");
static_assert(offsetof(CPedIntelligence, m_AnotherStaticCounter) == WalkRoundControl31Fix::kCounterAt, "m_AnotherStaticCounter en +0x274");
static_assert(offsetof(CEntity, m_matrix) == WalkRoundControl31Fix::kMatrixAt, "CEntity::m_matrix en +0x14");
static_assert(offsetof(CEntity, m_placement) == WalkRoundControl31Fix::kPlaceAt, "CEntity::m_placement en +4");
static_assert(sizeof(CVector) == 12, "los vectores son tres floats");
namespace WalkRoundControl31Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"walkroundcontrol31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"walkroundcontrol31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("walkroundcontrol31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexWalkRoundObject","ControlSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexWalkRoundObject","ControlSubTask",0x6575F0,body));
    registered["CTaskComplexWalkRoundObject::ControlSubTask"]=env.route->Registered();bodies["CTaskComplexWalkRoundObject::ControlSubTask"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.pAlloc={0x54F560,Body(std::bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix))};
    env.pUpdate={0x54F1B0,Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix))};
    Run(env,ready,registered,bodies);
}
}
