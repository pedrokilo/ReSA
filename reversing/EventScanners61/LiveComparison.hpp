#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "PedIntelligence.h"
#include "MentalHealth.h"
#include "Events/EventGroup.h"
#include "Events/EventScanner.h"
#include "Collision/CollisionEventScanner.h"
#include "PedStuckChecker.h"
#include "Tasks/TaskManager.h"
#include "Tasks/Task.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventPotentialWalkIntoVehicle.h"
#include "Collision/ColModel.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Events/EventHandlerHistory.h"
#include "Events/EventInAir.h"
#include "Events/EventOnFire.h"
#include "Events/EventAreaCodes.h"
#include "Events/EventEscalator.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventDamage.h"
#include "Weapon.h"
#include "Events/EventStuckInAir.h"
#include "Events/EventInWater.h"
#include "General.h"
#include "World.h"
#include "Entity/Physical.h"
#include "Fire.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par EventScanners61 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp y Case*.hpp.
// Una ruta por funcion (HookRoute::RegisteredTarget: todos los hooks son Simple).
static_assert(offsetof(CPed, m_fHealth) == 0x540, "CPed::m_fHealth");
static_assert(offsetof(CPed, m_pVehicle) == 0x58C, "CPed::m_pVehicle");
struct EventScanners61PedProbe : CPed { static constexpr size_t IntelOff() { return offsetof(EventScanners61PedProbe, m_pIntelligence); } };
static_assert(EventScanners61PedProbe::IntelOff() == 0x47C, "CPed::m_pIntelligence");
static_assert(offsetof(CVehicle, m_fHealth) == 0x4C0, "CVehicle::m_fHealth");
static_assert(offsetof(CPedIntelligence, m_eventGroup) == 0x68, "CPedIntelligence::m_eventGroup");
static_assert(offsetof(CPedIntelligence, m_mentalState) == 0x174, "CPedIntelligence::m_mentalState");
static_assert(sizeof(CMentalState) == 0x14, "CMentalState");
struct EventScanners61VehProbe : CVehicle { static constexpr size_t TypeOff() { return offsetof(EventScanners61VehProbe, m_nVehicleSubType); } };
static_assert(EventScanners61VehProbe::TypeOff() == 0x594, "CVehicle::m_nVehicleSubType");
static_assert(sizeof(CVehiclePotentialCollisionScanner) == 0xC, "CVehiclePotentialCollisionScanner");
static_assert(offsetof(CTaskSimpleGoTo, m_vecTargetPoint) == 0xC, "CTaskSimpleGoTo::m_vecTargetPoint");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_target) == 0x10, "CTaskComplexKillPedOnFoot::m_target");
static_assert(sizeof(CEventScanner) == 0xD4, "CEventScanner");
static_assert(PEDSTATE_DIE == 0x36 && PEDSTATE_DEAD == 0x37, "PEDSTATE_DIE/DEAD");
static_assert(sizeof(CPedStuckChecker) == 0x10 && offsetof(CPedStuckChecker, m_radius) == 0xC && offsetof(CPedStuckChecker, m_state) == 0xE, "CPedStuckChecker");
static_assert(EVENT_IN_WATER == 66 && EVENT_STUCK_IN_AIR == 93, "tipos de evento");
static_assert(offsetof(CEventScanner, m_pedAcquaintanceScanner) == 0xAC && offsetof(CEventScanner, m_sexyPedScanner) == 0xBC && offsetof(CEventScanner, m_nearbyFireScanner) == 0xC8, "CEventScanner miembros");
static_assert(offsetof(CEventScanner, m_attractorScanner) == 0x1C && offsetof(CEventScanner, m_objectPotentialCollisionScanner) == 0x10, "CEventScanner miembros 2");
struct EventScanners61PedProbe2 : CPed { static constexpr size_t Fire() { return offsetof(EventScanners61PedProbe2, m_pFire); } static constexpr size_t Contact() { return offsetof(EventScanners61PedProbe2, m_pContactEntity); } };
static_assert(EventScanners61PedProbe2::Fire() == 0x730 && EventScanners61PedProbe2::Contact() == 0x584, "CPed::m_pFire / m_pContactEntity");
static_assert(offsetof(CColModel, m_boundBox) == 0, "CColModel::m_boundBox");
namespace EventScanners61Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"eventscanners61-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"eventscanners61-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("eventscanners61");}
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
    route(kFnMental,0,"CMentalState","Process",0x6008A0);
    route(kFnVpc,1,"CVehiclePotentialCollisionScanner","ScanForVehiclePotentialCollisionEvents",0x603720);
    route(kFnScan,2,"CEventScanner","ScanForEvents",0x607E30);
    route(kFnStuck,3,"CPedStuckChecker","TestPedStuck",0x602C00);
    // Si la funcion colaboradora tiene su propia ruta en este banco, su hook queda apagado (el constructor de la ruta lo apaga): hooked = false.
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto dentro del ASI).
    env.stubs.resize(S_COUNT);
    const auto stub=[&](StubId id,unsigned bits,uintptr_t exe,uintptr_t dll,bool hooked,void* fake,const char* name){
        if(kEnabled&bits)env.stubs[id]={exe,dll?Body(dll,env.asi):0,hooked,fake,name};
    };
    stub(S_ADD,kFnMental|kFnVpc|kFnScan|kFnStuck,0x4AB420,std::bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),true,reinterpret_cast<void*>(&FakeAdd),"CEventGroup::Add");
    stub(S_SIMPLEST,kFnVpc|kFnScan,0x6819D0,std::bit_cast<uintptr_t>(&CTaskManager::GetSimplestActiveTask),true,reinterpret_cast<void*>(&FakeSimplest),"CTaskManager::GetSimplestActiveTask");
    stub(S_ISGOTO,kFnVpc,0x61A360,std::bit_cast<uintptr_t>(&CTask::IsGoToTask),false,reinterpret_cast<void*>(&FakeIsGoto),"CTask::IsGoToTask");
    stub(S_COLMODEL,kFnVpc,0x535300,std::bit_cast<uintptr_t>(&CEntity::GetColModel),true,reinterpret_cast<void*>(&FakeCol),"CEntity::GetColModel");
    stub(S_LOS,kFnVpc,0x5F5A30,std::bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,const CVector&,CEntity&,float&)>(&CPedGeometryAnalyser::GetIsLineOfSightClear)),true,reinterpret_cast<void*>(&FakeLos),"CPedGeometryAnalyser::GetIsLineOfSightClear");
    stub(S_REGREF,kFnVpc|kFnScan|kFnStuck,0x571B70,std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),true,reinterpret_cast<void*>(&FakeRegRef),"CEntity::RegisterReference");
    stub(S_CLEANREF,kFnVpc|kFnScan|kFnStuck,0x571A00,std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),true,reinterpret_cast<void*>(&FakeCleanRef),"CEntity::CleanUpOldReference");
    stub(S_VPCSCAN,kFnScan,0x603720,std::bit_cast<uintptr_t>(&CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents),!(kEnabled&kFnVpc),reinterpret_cast<void*>(&FakeVpcScan),"CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents");
    stub(S_X606580,kFnScan,0x606580,0,false,reinterpret_cast<void*>(&Fake606580),"sub_606580");
    stub(S_OBJSCAN,kFnScan,0x606890,0,false,reinterpret_cast<void*>(&FakeObjScan),"CObjectPotentialCollisionScanner::Scan");
    stub(S_ACQSCAN,kFnScan,0x607D80,0,false,reinterpret_cast<void*>(&FakeAcqScan),"CPedAcquaintanceScanner::Scan");
    stub(S_ATTRSCAN,kFnScan,0x6060A0,0,false,reinterpret_cast<void*>(&FakeAttrScan),"CAttractorScanner::Scan");
    stub(S_FIRESCAN,kFnScan,0x603E70,0,false,reinterpret_cast<void*>(&FakeFireScan),"CNearbyFireScanner::Scan");
    stub(S_SEXYSCAN,kFnScan,0x603BF0,0,false,reinterpret_cast<void*>(&FakeSexyScan),"CSexyPedScanner::Scan");
    stub(S_MENTAL,kFnScan,0x6008A0,std::bit_cast<uintptr_t>(&CMentalState::Process),!(kEnabled&kFnMental),reinterpret_cast<void*>(&FakeMental),"CMentalState::Process");
    stub(S_ISINAIR,kFnScan,0x5F1CB0,std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::IsInAir),true,reinterpret_cast<void*>(&FakeIsInAir),"CPedGeometryAnalyser::IsInAir");
    stub(S_GETCUR,kFnScan,0x4B8CA0,std::bit_cast<uintptr_t>(&CEventHandlerHistory::GetCurrentEvent),true,reinterpret_cast<void*>(&FakeGetCur),"CEventHandlerHistory::GetCurrentEvent");
    stub(S_ISPLAYER,kFnScan|kFnStuck,0x5DF8F0,std::bit_cast<uintptr_t>(&CPed::IsPlayer),true,reinterpret_cast<void*>(&FakeIsPlayer),"CPed::IsPlayer");
    stub(S_FINDTASK,kFnScan,0x6817D0,std::bit_cast<uintptr_t>(&CTaskManager::FindTaskByType),true,reinterpret_cast<void*>(&FakeFindTask),"CTaskManager::FindTaskByType");
    stub(S_GENDMG,kFnScan,0x73A530,std::bit_cast<uintptr_t>(&CWeapon::GenerateDamageEvent),true,reinterpret_cast<void*>(&FakeGenDmg),"CWeapon::GenerateDamageEvent");
    stub(S_FINDPLAYER,kFnScan,0x56E210,std::bit_cast<uintptr_t>(&FindPlayerPed),true,reinterpret_cast<void*>(&FakeFindPlayer),"FindPlayerPed");
    stub(S_ISALIVE,kFnScan,0x5E0170,std::bit_cast<uintptr_t>(&CPed::IsAlive),true,reinterpret_cast<void*>(&FakeIsAlive),"CPed::IsAlive");
    stub(S_HASEVENT,kFnScan,0x4AB5E0,std::bit_cast<uintptr_t>(&CEventGroup::HasEventOfType),true,reinterpret_cast<void*>(&FakeHasEvent),"CEventGroup::HasEventOfType");
    stub(S_SWIM,kFnStuck,0x601070,std::bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),true,reinterpret_cast<void*>(&FakeSwim),"CPedIntelligence::GetTaskSwim");
    stub(S_JET,kFnStuck,0x601110,std::bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),true,reinterpret_cast<void*>(&FakeJet),"CPedIntelligence::GetTaskJetPack");
    stub(S_CLIMB,kFnStuck,0x601180,std::bit_cast<uintptr_t>(&CPedIntelligence::GetTaskClimb),true,reinterpret_cast<void*>(&FakeClimb),"CPedIntelligence::GetTaskClimb");
    stub(S_NORM,kFnStuck,0x59C910,std::bit_cast<uintptr_t>(&CVector::Normalise),false,reinterpret_cast<void*>(&FakeNormalise),"CVector::Normalise");
    stub(S_PVL,kFnStuck,0x5674E0,std::bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),true,reinterpret_cast<void*>(&FakePvl),"CWorld::ProcessVerticalLine");
    stub(S_LOSV,kFnStuck,0x56A490,std::bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,const CVector&,bool,bool,bool,bool,bool,bool,bool)>(&CWorld::GetIsLineOfSightClear)),true,reinterpret_cast<void*>(&FakeLosV),"CWorld::GetIsLineOfSightClear");
    stub(S_SETPOSN,kFnStuck,0x4241C0,std::bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(const CVector&)>(&CPlaceable::SetPosn)),true,reinterpret_cast<void*>(&FakeSetPosn),"CPlaceable::SetPosn");
    stub(S_FORCE,kFnStuck,0x5429F0,std::bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),true,reinterpret_cast<void*>(&FakeForce),"CPhysical::ApplyMoveForce");
    stub(S_ORIENT,kFnStuck,0x439A80,std::bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),true,reinterpret_cast<void*>(&FakeOrient),"CPlaceable::SetOrientation");
    stub(S_ANGLE,kFnStuck,0x53CBE0,std::bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),true,reinterpret_cast<void*>(&FakeAngle),"CGeneral::GetRadianAngleBetweenPoints");
    stub(S_LIMIT,kFnStuck,0x53CB50,std::bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),true,reinterpret_cast<void*>(&FakeLimit),"CGeneral::LimitRadianAngle");
    stub(S_GETEV,kFnStuck,0x4AB650,std::bit_cast<uintptr_t>(&CEventGroup::GetEventOfType),true,reinterpret_cast<void*>(&FakeGetEv),"CEventGroup::GetEventOfType");
    stub(S_MOVE,kFnVpc,0x601D70,std::bit_cast<uintptr_t>(&CPedIntelligence::GetMoveStateFromGoToTask),true,reinterpret_cast<void*>(&FakeMove),"CPedIntelligence::GetMoveStateFromGoToTask");
    Run(env,ready,registered,bodies);
}
}
