#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexGoPickUpEntity.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Physical.h"
#include "Core/Matrix.h"
#include "SimpleTransform.h"
#include "Collision/ColModel.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PickUpFirst68 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskComplexGoPickUpEntity::CreateFirstSubTask (0x693610). Es un hook VIRTUAL: HookRoute::RegisteredTarget solo conoce los hooks Simple, asi que el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress() (el hook Virtual parchea tambien la entrada 0x693610 con un salto a ese cuerpo).
static_assert(offsetof(CEntity, m_nModelIndex) == PickUp68Fix::kModelAt, "CEntity: m_nModelIndex +0x22");
static_assert(offsetof(CEntity, m_matrix) == PickUp68Fix::kMatrixAt && offsetof(CEntity, m_placement) == PickUp68Fix::kPlacementAt, "CPlaceable: m_placement +4, m_matrix +0x14");
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == PickUp68Fix::kMoveAt && offsetof(CPhysical, m_vecTurnSpeed) == PickUp68Fix::kTurnAt, "CPhysical: velocidades +0x44 / +0x50");
static_assert(offsetof(CTaskComplexGoPickUpEntity, m_pEntity) == PickUp68Fix::kEntityAt && offsetof(CTaskComplexGoPickUpEntity, m_vecPosition) == PickUp68Fix::kPosAt
           && offsetof(CTaskComplexGoPickUpEntity, m_vecPickupPosition) == PickUp68Fix::kPickupAt && offsetof(CTaskComplexGoPickUpEntity, m_nTimePassedSinceLastSubTaskCreatedInMs) == PickUp68Fix::kTimeFieldAt
           && sizeof(CTaskComplexGoPickUpEntity) == PickUp68Fix::kTaskBytes, "CTaskComplexGoPickUpEntity");
static_assert(sizeof(CTaskComplexGoToPointAndStandStill) == PickUp68Fix::kSizeGoTo, "tamano de la tarea creada (argumento de operator new)");
static_assert(sizeof(CColModel) >= PickUp68Fix::kColBytes && offsetof(CColModel, m_boundBox) == 0, "CColModel: caja en +0");
namespace PickUp68Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pickupfirst68-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pickupfirst68-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pickupfirst68");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    auto* hook=FindHook("CTaskComplexGoPickUpEntity","CreateFirstSubTask");
    uintptr_t body=0;
    if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
    env.route.reset(new HookRoute("CTaskComplexGoPickUpEntity","CreateFirstSubTask",0x693610,body));
    registered[kFn]=env.route->Registered();bodies[kFn]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    // Los cuatro primeros estan reconstruidos pero sin hook en HEAD (CMatrix::InjectHooks esta apagado en InjectHooksMain y CSimpleTransform no tiene hooks): la entrada del EXE es codigo original
    // y la DLL los llama por su propio cuerpo.
    env.stub[0]={0x59B020,Body(std::bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly)),false};
    env.stub[1]={0x59AD70,Body(std::bit_cast<uintptr_t>(&CMatrix::UpdateRwMatrix)),false};
    env.stub[2]={0x54EF40,Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateRwMatrix)),false};
    env.stub[3]={0x54F1B0,Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix)),false};
    env.stub[4]={0x54F560,Body(std::bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix))};
    env.stub[5]={0x59C890,Body(std::bit_cast<uintptr_t>(static_cast<CVector(*)(const CMatrix&,const CVector&)>(&operator*)))};
    env.stub[6]={0x61A5A0,Body(std::bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CTask::operator new)))};
    {   // m_pRwObject (privado) en +0x18 y el tipo en los 3 bits bajos del byte +0x36, comprobados sobre un bufer crudo
        alignas(16) u8 raw[0x80]{};
        Put32(raw+PickUp68Fix::kRwAt,0x12345678u);raw[PickUp68Fix::kFlagsAt]=0xFCu|4u;
        auto* e=reinterpret_cast<CEntity*>(raw);
        ready&=uintptr_t(e->GetRwObject())==0x12345678u&&e->GetIsTypeObject();
        raw[PickUp68Fix::kFlagsAt]=0xFCu|3u;
        ready&=!e->GetIsTypeObject();
    }
    Run(env,ready,registered,bodies);
}
}
