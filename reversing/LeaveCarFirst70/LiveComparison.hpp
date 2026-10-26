#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexLeaveBoat.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"
#include "Tasks/TaskTypes/TaskSimpleCarDriveTimed.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Events/Event.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventLeaderExitedCarAsDriver.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCarFirst70 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexLeaveCar::CreateFirstSubTask (0x641FC0). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x641FC0 con un salto a ese cuerpo).
static_assert(offsetof(CTaskComplexLeaveCar, m_pTargetVehicle) == Leave70Fix::kTaskVehAt && offsetof(CTaskComplexLeaveCar, m_nDelayTime) == Leave70Fix::kTaskDelayAt && sizeof(CTaskComplexLeaveCar) == Leave70Fix::kTaskBytes, "CTaskComplexLeaveCar");
static_assert(offsetof(CPed, m_nPedState) == Leave70Fix::kPedStateAt && offsetof(CPed, m_pVehicle) == Leave70Fix::kPedVehAt && Leave70Fix::kPedVehAt + 4 <= Leave70Fix::kPedBytes, "CPed: m_nPedState +0x530, m_pVehicle +0x58C");
static_assert(offsetof(CVehicle, m_pDriver) == Leave70Fix::kVehDriverAt && offsetof(CVehicle, m_nVehicleType) == Leave70Fix::kVehTypeAt && offsetof(CVehicle, m_vehicleAudio) == Leave70Fix::kVehAudioAt && Leave70Fix::kVehTypeAt + 4 <= Leave70Fix::kVehBytes && Leave70Fix::kVehAudioAt < Leave70Fix::kVehBytes, "CVehicle: m_pDriver +0x460, m_nVehicleType +0x590, m_vehicleAudio +0x138");
static_assert(sizeof(CTaskSimplePause) == 0x18 && sizeof(CTaskComplexLeaveBoat) == 0x14 && sizeof(CTaskSimpleCarDriveTimed) == 0x70, "tamanos de las tareas creadas (argumento de CTask::operator new)");
static_assert(sizeof(CEventLeaderExitedCarAsDriver) == 0x14 && sizeof(CEventGroupEvent) == 0x14 && offsetof(CEventGroupEvent, m_ped) == 0xC && offsetof(CEventGroupEvent, m_event) == 0x10
           && offsetof(CEvent, m_nTimeActive) == 4 && offsetof(CEvent, m_bValid) == 8, "eventos");
static_assert(offsetof(CPedGroup, m_groupMembership) == Leave70Fix::kGrpMemberAt && offsetof(CPedGroup, m_groupIntelligence) == Leave70Fix::kGrpIntelAt, "CPedGroup: membresia +8, inteligencia +0x30");
static_assert(PEDSTATE_ARRESTED == Leave70Fix::kStateArrested && VEHICLE_TYPE_BOAT == Leave70Fix::kTypeBoat, "constantes");
namespace Leave70Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"leavecarfirst70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecarfirst70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecarfirst70");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexLeaveCar","CreateFirstSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexLeaveCar","CreateFirstSubTask",0x641FC0,body));
    registered[kFn]=env.route->Registered();bodies[kFn]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.stub[0]={0x5DF8F0,Body(std::bit_cast<uintptr_t>(&CPed::IsPlayer))};
    env.stub[1]={0x61A5A0,Body(std::bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CTask::operator new)))};
    env.stub[2]={0x821982,0};   // printf del CRT del EXE (la DLL lo reemplaza con un salto en esa direccion): la DLL lo llama por esa direccion, no hay cuerpo aparte
    env.stub[3]={0x5F7E80,Body(std::bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup))};
    env.stub[4]={0x5F69C0,Body(std::bit_cast<uintptr_t>(&CPedGroupMembership::IsLeader))};
    env.stub[5]={0x4B5620,Body(std::bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CEvent::operator new))),false};
    env.stub[6]={0x571B70,Body(std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)))};
    env.stub[7]={0x5F7470,Body(std::bit_cast<uintptr_t>(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent)))};
    env.stub[8]={0x571A00,Body(std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)))};
    env.stub[9]={0x4B5630,Body(std::bit_cast<uintptr_t>(static_cast<void(*)(void*)>(&CEvent::operator delete))),false};
    env.stub[10]={0x5DFD90,Body(std::bit_cast<uintptr_t>(&CPed::SetRadioStation))};
    env.stub[11]={0x4F5BA0,Body(std::bit_cast<uintptr_t>(&CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver)),false};
    {   // banderas del ped (anonimas) sobre un bufer crudo: bInVehicle = bit 8 de +0x46C, bIsBeingArrested = bit 6 del byte +0x474
        alignas(16) static u8 raw[sizeof(CPed)]{};
        auto* p=reinterpret_cast<CPed*>(raw);
        raw[Leave70Fix::kPedFlagsAt+1]=0x01;ready&=p->bInVehicle&&!p->bIsBeingArrested;
        raw[Leave70Fix::kPedFlagsAt+1]=0xFE;raw[Leave70Fix::kPedFlags2At]=0x40;ready&=!p->bInVehicle&&p->bIsBeingArrested;
        raw[Leave70Fix::kPedFlags2At]=0xBF;ready&=!p->bIsBeingArrested;
    }
    Run(env,ready,registered,bodies);
}
}
