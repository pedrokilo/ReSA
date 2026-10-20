#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"
#include "Tasks/TaskTypes/TaskSimpleLeaveGroup.h"
#include "Tasks/TaskTypes/TaskSimpleCarDriveTimed.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexDragPedFromCar.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Events/EventAreaCodes.h"
#include "Events/EventGroup.h"
#include "CarEnterExit.h"
#include "CullZones.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "CopPed.h"
#include "PlayerPedData.h"
#include "Wanted.h"
#include "General.h"
#include "Timer.h"
#include "Weapon.h"
#include "Tasks/Task.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par KillPedOnFoot62 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco rutas (CTaskComplexKillPedOnFoot): ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, MakeAbortable 0x625E40 (hooks VIRTUALES: el cuerpo del
// candidato se toma de ReversibleHook::Virtual::GetHookOurAddress()) y CreateSubTask 0x625E70 (hook Simple, no virtual).
struct KillPedProbeSub : CTaskComplex { static constexpr size_t Off() { return offsetof(KillPedProbeSub, m_pSubTask); } };
static_assert(KillPedProbeSub::Off() == KillPedOnFootFix::kSubAt, "m_pSubTask en +0x8");
static_assert(sizeof(CTaskComplexKillPedOnFoot) == 0x38, "CTaskComplexKillPedOnFoot mide 0x38");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_target) == KillPedOnFootFix::kTargetAt, "m_target en +0x10");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_pedFlags) == KillPedOnFootFix::kPedFlagsAt, "m_pedFlags en +0x14");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_actionDelay) == KillPedOnFootFix::kDelayAt, "m_actionDelay en +0x18");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_actionChance) == KillPedOnFootFix::kChanceAt, "m_actionChance en +0x1C");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_nCompetence) == KillPedOnFootFix::kCompAt, "m_nCompetence en +0x20");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_startTime) == KillPedOnFootFix::kStartAt, "m_startTime en +0x24");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_time) == KillPedOnFootFix::kTimeAt, "m_time en +0x28");
static_assert(offsetof(CTaskComplexKillPedOnFoot, m_timer) == KillPedOnFootFix::kTmStartAt, "m_timer en +0x2C");
static_assert(offsetof(CTaskTimer, m_nStartTime) == 0 && offsetof(CTaskTimer, m_nInterval) == 4 && offsetof(CTaskTimer, m_bStarted) == 8, "CTaskTimer: inicio +0, intervalo +4, iniciado +8");
static_assert(offsetof(CTaskComplexKillPedOnFootMelee, m_bShotFiredByPlayerFlag) == 0x30, "Melee: bandera de disparo en +0x30");
static_assert(offsetof(CTaskComplexKillPedOnFootArmed, m_bShotFiredByPlayer) == 0x33 && offsetof(CTaskComplexKillPedOnFootArmed, m_aimImmediate) == 0x35, "Armed: banderas en +0x33 y +0x35");
static_assert(sizeof(CTaskSimplePause) == 0x18 && sizeof(CTaskSimpleLeaveGroup) == 8 && sizeof(CTaskSimpleStandStill) == 0x20 && sizeof(CTaskComplexDragPedFromCar) == 0x54 && sizeof(CTaskComplexLeaveCar) == 0x34
           && sizeof(CTaskSimpleCarDriveTimed) == 0x70 && sizeof(CTaskComplexKillPedOnFootMelee) == 0x38 && sizeof(CTaskComplexKillPedOnFootArmed) == 0x5C && sizeof(CTaskComplexDestroyCar) == 0x20
           && sizeof(CTaskComplexSignalAtPed) == 0x18 && sizeof(CEventAreaCodes) == 0x10, "tamanos de las tareas creadas (argumento de operator new)");
static_assert(sizeof(CPed) == 0x79C && sizeof(CVehicle) == 0x5A0 && sizeof(CMatrix) == 0x48 && sizeof(CPlayerPedData) <= 0x40 + 0x70, "tamanos de CPed / CVehicle / CMatrix");
struct KillPedProbePed : CPed { static constexpr size_t OffData() { return offsetof(KillPedProbePed, m_pPlayerData); } static constexpr size_t OffIntel() { return offsetof(KillPedProbePed, m_pIntelligence); } };
static_assert(KillPedProbePed::OffData() == KillPedOnFootFix::kPDataAt && KillPedProbePed::OffIntel() == KillPedOnFootFix::kIntelAt && offsetof(CPed, m_fHealth) == KillPedOnFootFix::kHealthAt
           && offsetof(CPed, m_standingOnEntity) == KillPedOnFootFix::kStandAt && offsetof(CPed, m_pContactEntity) == KillPedOnFootFix::kContactAt && offsetof(CPed, m_pVehicle) == KillPedOnFootFix::kVehAt
           && offsetof(CPed, m_nPedType) == KillPedOnFootFix::kTypeAt && offsetof(CPed, m_nActiveWeaponSlot) == KillPedOnFootFix::kSlotAt && offsetof(CPed, m_aWeapons) == KillPedOnFootFix::kWeaponsAt
           && sizeof(CWeapon) == KillPedOnFootFix::kWeaponSize && offsetof(CWeapon, m_TotalAmmo) == KillPedOnFootFix::kAmmoInWeapon, "desplazamientos de CPed / CWeapon");
struct KillPedProbeEnt : CEntity { static constexpr size_t OffArea() { return offsetof(KillPedProbeEnt, m_AreaCode); } };
static_assert(offsetof(CEntity, m_pReferences) == KillPedOnFootFix::kRefsAt && offsetof(CEntity, m_matrix) == KillPedOnFootFix::kMatrixAt && KillPedProbeEnt::OffArea() == KillPedOnFootFix::kAreaAt, "desplazamientos de CEntity");
static_assert(offsetof(CVehicle, m_vecMoveSpeed) == KillPedOnFootFix::kVelAt && offsetof(CVehicle, m_nVehicleType) == KillPedOnFootFix::kVTypeAt && offsetof(CVehicle, m_nVehicleSubType) == KillPedOnFootFix::kVSubAt, "desplazamientos de CVehicle");
static_assert(offsetof(CWanted, m_ChanceOnRoadBlock) == KillPedOnFootFix::kWChanceAt && offsetof(CPlayerPedData, m_pWanted) == KillPedOnFootFix::kPdWantedAt, "desplazamientos de CWanted / CPlayerPedData");
namespace KillPedOnFoot62Live {
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
template<class P> inline uintptr_t B(P p) { return Body(std::bit_cast<uintptr_t>(p)); }
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"killpedonfoot62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"killpedonfoot62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("killpedonfoot62");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();
    // Cuerpos de la DLL de los colaboradores sustituidos: se resuelven ANTES de construir las rutas y de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto).
    env.stub[S_WANTED]={0x56E230,B(&FindPlayerWanted),true};
    env.stub[S_PLAYERPED]={0x56E210,B(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),true};
    env.stub[S_NOPOLICE]={0x72DD50,B(&CCullZones::NoPolice),true};
    env.stub[S_MELEE]={0x73B1C0,B(&CWeapon::IsTypeMelee),true};
    env.stub[S_SAY]={0x5EFFE0,B(&CPed::Say),true};
    env.stub[S_GIVE]={0x5E6080,B(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),true};
    env.stub[S_SETTYPE]={0x5E6280,B(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),true};
    env.stub[S_SETSLOT]={0x5E61F0,B(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon)),true};
    env.stub[S_MOVESTATE]={0x5DEC00,B(&CPed::SetMoveState),true};
    env.stub[S_DROP]={0x5E0360,B(&CPed::DropEntityThatThisPedIsHolding),true};
    env.stub[S_OPEN]={0x6D1E20,B(&CVehicle::CanPedOpenLocks),true};
    env.stub[S_DRIVER]={0x6D1C40,B(static_cast<bool(CVehicle::*)(const CPed*)const>(&CVehicle::IsDriver)),true};
    env.stub[S_PASSENGER]={0x6D1BD0,B(static_cast<bool(CVehicle::*)(CPed*)const>(&CVehicle::IsPassenger)),true};
    env.stub[S_DOOR]={0x64F110,B(&CCarEnterExit::ComputeTargetDoorToExit),true};
    env.stub[S_ROOM]={0x6504C0,B(&CCarEnterExit::IsRoomForPedToLeaveCar),true};
    env.stub[S_HEALTHY]={0x64EEC0,B(&CCarEnterExit::IsVehicleHealthy),true};
    env.stub[S_GROUP]={0x5F7E80,B(&CPedGroups::GetPedsGroup),true};
    env.stub[S_LEADER]={0x5F69A0,B(&CPedGroupMembership::GetLeader),true};
    env.stub[S_NEW]={0x61A5A0,B(static_cast<void*(*)(size_t)>(&CTask::operator new)),true};
    {   // CTaskSimpleStandStill::ProcessPed es un hook VIRTUAL: el cuerpo sale de GetHookOurAddress()
        auto* h=FindHook("CTaskSimpleStandStill","ProcessPed");
        const uintptr_t body=(h&&!strcmp(h->Symbol(),"V"))?uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()):0;
        env.stub[S_STILL]={0x62F370,Body(body),body!=0};
    }
    env.stub[S_ADD]={0x4AB420,B(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),true};
    env.stub[S_C65]={0x65E9A0,0,false};               // sin reconstruir: la DLL lo llama por su direccion del EXE
    env.stub[S_RANDEXE]={0x821B1E,0,false};           // rand del CRT del EXE (lo usa el 0x407180 original)
    env.stub[S_RANDDLL]={0,B(&CGeneral::GetRandomNumber),false};   // rand de la DLL (CGeneral::GetRandomNumber, sin hook): lo usa GetRandomIntInRange en linea
    // Rutas RH: se construyen antes que nada; el constructor deja cada hook apagado.
    for(unsigned r=0;r<kRoutes;++r) {
        auto* hook=FindHook("CTaskComplexKillPedOnFoot",kInfo[r].fn);
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        if(hook&&!strcmp(hook->Symbol(),"V")) env.route[r].reset(new HookRoute("CTaskComplexKillPedOnFoot",kInfo[r].fn,kInfo[r].entry,body));
        else env.route[r].reset(new HookRoute("CTaskComplexKillPedOnFoot",kInfo[r].fn,kInfo[r].entry,HookRoute::RegisteredTarget{}));
        registered[kInfo[r].name]=env.route[r]->Registered();bodies[kInfo[r].name]=env.route[r]->Body();
        ready&=env.route[r]->Registered()&&env.route[r]->Body()&&ModuleOf(env.route[r]->Body())==env.asi;
    }
    env.randRoute.reset(new HookRoute("CGeneral","GetRandomNumberInRange_int32",0x407180,HookRoute::RegisteredTarget{}));
    Run(env,ready,registered,bodies);
}
}
