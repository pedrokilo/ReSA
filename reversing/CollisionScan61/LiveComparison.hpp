#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Object/Object.h"
#include "PedIntelligence.h"
#include "Events/EventGroup.h"
#include "Events/EventDamage.h"
#include "Events/EventVehicleCollision.h"
#include "Events/EventPedCollisionWithPed.h"
#include "Events/EventPedCollisionWithPlayer.h"
#include "Events/EventPlayerCollisionWithPed.h"
#include "Events/EventObjectCollision.h"
#include "Events/EventBuildingCollision.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventAcquaintancePedHate.h"
#include "Collision/CollisionEventScanner.h"
#include "Collision/ColModel.h"
#include "Tasks/TaskManager.h"
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "PedDamageResponseCalculator.h"
#include "PedGeometryAnalyser.h"
#include "PedClothesDesc.h"
#include "PlayerPedData.h"
#include "PedType.h"
#include "Acquaintance.h"
#include "Weapon.h"
#include "General.h"
#include "Timer.h"
#include "RwHelper.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CollisionScan61 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp y CaseColl.hpp.
// Una ruta (HookRoute::RegisteredTarget: el hook es Simple).
struct CollisionScan61PedProbe : CPed {
    static constexpr size_t IntelOff() { return offsetof(CollisionScan61PedProbe, m_pIntelligence); }
    static constexpr size_t PlayerOff() { return offsetof(CollisionScan61PedProbe, m_pPlayerData); }
};
static_assert(CollisionScan61PedProbe::IntelOff() == 0x47C && CollisionScan61PedProbe::PlayerOff() == 0x480, "CPed::m_pIntelligence / m_pPlayerData");
struct CollisionScan61VehProbe : CVehicle {
    static constexpr size_t SubtypeOff() { return offsetof(CollisionScan61VehProbe, m_nVehicleSubType); }
    static constexpr size_t TowingOff() { return offsetof(CollisionScan61VehProbe, m_pTowingVehicle); }
};
static_assert(CollisionScan61VehProbe::SubtypeOff() == 0x594 && CollisionScan61VehProbe::TowingOff() == 0x4C4, "CVehicle::m_nVehicleSubType / m_pTowingVehicle");
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44 && offsetof(CPhysical, m_vecTurnSpeed) == 0x50 && offsetof(CPhysical, m_fMass) == 0x8C, "CPhysical 1");
static_assert(offsetof(CPhysical, m_fDamageIntensity) == 0xD8 && offsetof(CPhysical, m_pDamageEntity) == 0xDC && offsetof(CPhysical, m_vecLastCollisionImpactVelocity) == 0xE0
              && offsetof(CPhysical, m_vecLastCollisionPosn) == 0xEC && offsetof(CPhysical, m_nPieceType) == 0xF8 && offsetof(CPhysical, m_pAttachedTo) == 0xFC && offsetof(CPhysical, m_pEntityIgnoredCollision) == 0x128, "CPhysical 2");
static_assert(offsetof(CPed, m_vecAnimMovingShift) == 0x550 && offsetof(CPed, m_standingOnEntity) == 0x568 && offsetof(CPed, m_pContactEntity) == 0x584 && offsetof(CPed, m_nPedType) == 0x598
              && offsetof(CPed, m_acquaintance) == 0x4E0, "CPed");
static_assert(offsetof(CPlayerPedData, m_pPedClothesDesc) == 4, "CPlayerPedData::m_pPedClothesDesc");
static_assert(offsetof(CPedIntelligence, m_eventGroup) == 0x68 && offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPedIntelligence");
static_assert(offsetof(CTaskSimpleGoTo, m_moveState) == 8, "CTaskSimpleGoTo::m_moveState");
static_assert(offsetof(CColModel, m_boundBox) == 0, "CColModel::m_boundBox");
static_assert(sizeof(CPedDamageResponseCalculator) == 0x14 && sizeof(CEventDamage) == 0x44, "tamanos");
namespace CollisionScan61Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){if(category.Name()==cls)for(auto& hook:category.Items())if(hook->Name()==fn)found=hook.get();});
    return found;
}
// Cuerpo de la DLL de un hook Simple o Virtual registrado (0 si no existe)
inline uintptr_t HookBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn);if(!h)return 0;
    if(h->Symbol()[0]=='V')return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    if(h->Symbol()[0]=='S')return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return 0;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"collisionscan61-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"collisionscan61-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("collisionscan61");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    const auto route=[&](unsigned bit,unsigned slot,const char* cls,const char* fn,uintptr_t entry){
        if(!(kEnabled&bit))return;
        env.route[slot].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        const std::string name=std::string(cls)+"::"+fn;
        registered[name]=env.route[slot]->Registered();bodies[name]=env.route[slot]->Body();
        ready&=env.route[slot]->Registered()&&env.route[slot]->Body()&&ModuleOf(env.route[slot]->Body())==env.asi;
    };
    route(kFnColl,0,"CCollisionEventScanner","ScanForCollisionEvents",0x604500);
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto dentro del ASI).
    env.stubs.resize(S_COUNT);
    const auto stub=[&](StubId id,uintptr_t exe,uintptr_t dll,bool hooked,void* fake,const char* name){
        env.stubs[id]={exe,dll?Body(dll,env.asi):0,hooked,fake,name};
    };
    const auto B=[](auto f){return std::bit_cast<uintptr_t>(f);};
    stub(S_ADD,0x4AB420,B(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),true,reinterpret_cast<void*>(&FakeAdd),"CEventGroup::Add");
    stub(S_SIMPLEST,0x6819D0,B(&CTaskManager::GetSimplestActiveTask),true,reinterpret_cast<void*>(&FakeSimplest),"CTaskManager::GetSimplestActiveTask");
    stub(S_ISGOTO,0x61A360,B(&CTask::IsGoToTask),false,reinterpret_cast<void*>(&FakeIsGoto),"CTask::IsGoToTask");
    stub(S_COLMODEL,0x535300,B(&CEntity::GetColModel),true,reinterpret_cast<void*>(&FakeCol),"CEntity::GetColModel");
    stub(S_MOVE,0x601D70,B(&CPedIntelligence::GetMoveStateFromGoToTask),true,reinterpret_cast<void*>(&FakeMove),"CPedIntelligence::GetMoveStateFromGoToTask");
    stub(S_REGREF,0x571B70,B(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),true,reinterpret_cast<void*>(&FakeRegRef),"CEntity::RegisterReference");
    stub(S_CLEANREF,0x571A00,B(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),true,reinterpret_cast<void*>(&FakeCleanRef),"CEntity::CleanUpOldReference");
    stub(S_ISPLAYER,0x5DF8F0,B(&CPed::IsPlayer),true,reinterpret_cast<void*>(&FakeIsPlayer),"CPed::IsPlayer");
    stub(S_GENDMG,0x73A530,B(&CWeapon::GenerateDamageEvent),true,reinterpret_cast<void*>(&FakeGenDmg),"CWeapon::GenerateDamageEvent");
    stub(S_NORM,0x59C910,B(&CVector::Normalise),false,reinterpret_cast<void*>(&FakeNormalise),"CVector::Normalise");
    stub(S_LIMIT,0x53CB50,B(&CGeneral::LimitRadianAngle),true,reinterpret_cast<void*>(&FakeLimit),"CGeneral::LimitRadianAngle");
    stub(S_KILL,0x5F0360,B(&CPed::KillPedWithCar),true,reinterpret_cast<void*>(&FakeKill),"CPed::KillPedWithCar");
    stub(S_HEADING,0x441DB0,B(&CPlaceable::GetHeading),true,reinterpret_cast<void*>(&FakeHeading),"CPlaceable::GetHeading");
    stub(S_AFFECTS,0x4B35A0,HookBody("CEventDamage","AffectsPed"),true,reinterpret_cast<void*>(&FakeAffects),"CEventDamage::AffectsPed");
    stub(S_COMPUTE,0x4B5AC0,B(&CPedDamageResponseCalculator::ComputeDamageResponse),true,reinterpret_cast<void*>(&FakeCompute),"CPedDamageResponseCalculator::ComputeDamageResponse");
    stub(S_HITSIDE,0x5F3640,B(static_cast<int32(*)(const CPed&,const CPhysical&)>(&CPedGeometryAnalyser::ComputePedHitSide)),true,reinterpret_cast<void*>(&FakeHitSide),"CPedGeometryAnalyser::ComputePedHitSide");
    stub(S_SAY,0x5EFFE0,B(&CPed::Say),true,reinterpret_cast<void*>(&FakeSay),"CPed::Say");
    stub(S_ANGER,0x421050,B(&IncrementAngerAtPlayerRegs),true,reinterpret_cast<void*>(&FakeAnger),"CPedIntelligence::IncrementAngerAtPlayer");
    stub(S_ACQ,0x608970,B(&CAcquaintance::GetAcquaintances),true,reinterpret_cast<void*>(&FakeAcq),"CAcquaintance::GetAcquaintances");
    stub(S_PEDFLAG,0x608830,B(&CPedType::GetPedFlag),true,reinterpret_cast<void*>(&FakePedFlag),"CPedType::GetPedFlag");
    stub(S_BALA,0x5A7950,B(&CPedClothesDesc::GetIsWearingBalaclava),true,reinterpret_cast<void*>(&FakeBala),"CPedClothesDesc::GetIsWearingBalaclava");
    stub(S_LOCALDIR,0x5DEF60,B(&CPed::GetLocalDirection),true,reinterpret_cast<void*>(&FakeLocalDir),"CPed::GetLocalDirection");
    Run(env,ready,registered,bodies);
}
}
