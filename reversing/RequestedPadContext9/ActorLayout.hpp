#pragma once
#include <Ped.h>
#include <Vehicle.h>
#include <PedIntelligence.h>
#include <TaskSimple.h>
#include <TaskSimpleUseGun.h>
namespace RequestedPadContext9 {
// This is an ABI field fixture, not a constructed engine CPed/CVehicle/Intelligence.
// The compared selectors only read these bounded fields; no actor virtual call/destructor is permitted.
template<size_t N> struct alignas(16) Blob {
    std::array<uint8_t,N> bytes{};
    template<class T>void Put(size_t offset,T value){assert(offset+sizeof(T)<=N);std::memcpy(bytes.data()+offset,&value,sizeof(value));}
};
struct Task final:CTaskSimple {
    eTaskType type=TASK_SIMPLE_GANG_DRIVEBY;
    // The caller only tests the returned pointer, but reserve at least the full tagged gun-task extent.
    std::array<uint8_t,sizeof(CTaskSimpleUseGun)> abiGuard{};
    CTask* Clone()const override{return nullptr;}
    eTaskType GetTaskType()const override{return type;}
    bool MakeAbortable(CPed*,eAbortPriority,const CEvent*)override{return false;}
    bool ProcessPed(CPed*)override{return false;}
};
static_assert(sizeof(Task)>=sizeof(CTaskSimpleUseGun));
// Access-only derived declaration; never instantiated, no fields or virtual overrides.
struct PedFieldAccess : CPed {using CPed::m_pIntelligence;};
static_assert(sizeof(PedFieldAccess)==sizeof(CPed));
static_assert(offsetof(PedFieldAccess,m_pIntelligence)==0x47C);
static_assert(offsetof(CPed,m_pAttachedTo)==0xFC && offsetof(CPed,m_pVehicle)==0x58C);
static_assert(offsetof(CVehicle,m_pDriver)==0x460);
static_assert(offsetof(CPedIntelligence,m_TaskMgr)==4);
static_assert(offsetof(CTaskManager,m_aPrimaryTasks)==0 && offsetof(CTaskManager,m_aSecondaryTasks)==0x14 && offsetof(CTaskManager,m_pPed)==0x2C);
enum class ActorState {Null,Detached,Attached,VehicleNullGuard,Driver,Passenger,NoDriver};
enum class GunState {NoTasks,SecondaryUseGun,PrimaryGangDriveBy,PrimaryOther,SecondaryOther};
struct ActorLayout {
    Blob<sizeof(CPed)> ped;
    Blob<sizeof(CPed)> otherPed;
    Blob<sizeof(CVehicle)> vehicle;
    Blob<sizeof(CPedIntelligence)> intelligence;
    Task primary,secondary;
    CPed* actor{};
    ActorLayout(ActorState state,GunState gun){
        static_assert(sizeof(void*)==4 && sizeof(CPed)==0x79C && sizeof(CVehicle)==0x5A0 && sizeof(CPedIntelligence)==0x294);
        actor=state==ActorState::Null?nullptr:reinterpret_cast<CPed*>(ped.bytes.data());
        ped.Put(0x47C,reinterpret_cast<CPedIntelligence*>(intelligence.bytes.data()));
        intelligence.Put(0,actor);
        // Task manager starts at intelligence+4: five primary slots, six secondary slots, owner.
        intelligence.Put(4+sizeof(void*)*(TASK_PRIMARY_MAX+TASK_SECONDARY_MAX),actor);
        if(state==ActorState::Attached)ped.Put(0xFC,reinterpret_cast<CEntity*>(vehicle.bytes.data()));
        if(state==ActorState::VehicleNullGuard||state==ActorState::Driver||state==ActorState::Passenger||state==ActorState::NoDriver){
            ped.Put<uint32_t>(0x46C,0x100);
            if(state!=ActorState::VehicleNullGuard)ped.Put(0x58C,reinterpret_cast<CVehicle*>(vehicle.bytes.data()));
            vehicle.Put(0x460,state==ActorState::Driver?actor:state==ActorState::Passenger?reinterpret_cast<CPed*>(otherPed.bytes.data()):nullptr);
        }
        if(gun==GunState::SecondaryUseGun){secondary.type=TASK_SIMPLE_USE_GUN;intelligence.Put(4+TASK_PRIMARY_MAX*sizeof(void*),static_cast<CTask*>(&secondary));}
        if(gun==GunState::PrimaryGangDriveBy||gun==GunState::PrimaryOther){primary.type=gun==GunState::PrimaryGangDriveBy?TASK_SIMPLE_GANG_DRIVEBY:TASK_NONE;intelligence.Put(4,static_cast<CTask*>(&primary));}
        if(gun==GunState::SecondaryOther){secondary.type=TASK_NONE;intelligence.Put(4+TASK_PRIMARY_MAX*sizeof(void*),static_cast<CTask*>(&secondary));}
    }
    ActorLayout(const ActorLayout&)=delete;
    ActorLayout& operator=(const ActorLayout&)=delete;
};
}
