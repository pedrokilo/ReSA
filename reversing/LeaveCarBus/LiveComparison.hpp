#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskComplexWaitForBus.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexWander.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskSimpleNone.h"
#include "References.h"
#include "Reference.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace LBLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int Report(int t,char* s,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",s?s:""}});log.flush();ExitProcess(91);}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
struct FakeTask:CTaskSimple {
    eTaskType type=TASK_NONE;
    CTask* Clone()const override{return nullptr;} eTaskType GetTaskType()const override{return type;}
    bool MakeAbortable(CPed*,eAbortPriority,const CEvent*)override{return true;} bool ProcessPed(CPed*)override{return true;}
};
inline CPed* expectedPed{};inline CPlayerPed* playerResult{};inline CTask* wanderResult{};inline CTaskManager* expectedManager{};
inline json trace;
inline CPlayerPed* Player(int32 n){trace.push_back({"player",n});return playerResult;}
inline CTaskComplexWander* Wander(CPed* p){trace.push_back({"wander",p==expectedPed});return reinterpret_cast<CTaskComplexWander*>(wanderResult);}
inline void __fastcall Set(CTaskManager* self,void*,CTask* t,ePrimaryTasks slot,bool unused){trace.push_back({"set",self==expectedManager,t==wanderResult,int(slot),unused});self->m_aPrimaryTasks[slot]=t;}
inline int RefCount(CEntity* e){int n=0;for(auto* r=e->m_pReferences;r;r=r->m_pNext)++n;return n;}
inline json TaskState(CTask* t,CVehicle* car){
    if(!t)return nullptr;
    json r={{"type",int(t->GetTaskType())},{"parent_null",!t->GetParent()},{"subtask_null",!t->GetSubTask()}};
    switch(t->GetTaskType()){
    case TASK_COMPLEX_LEAVE_CAR_AND_WANDER:{auto* v=static_cast<CTaskComplexLeaveCarAndWander*>(t);r["fields"]={v->m_veh==car,int(v->m_door),v->m_leaveDelayMs,v->m_bSensibleLeaveCar};break;}
    case TASK_COMPLEX_WAIT_FOR_BUS:r["bus_null"]=static_cast<CTaskComplexWaitForBus*>(t)->m_bus==nullptr;break;
    case TASK_SIMPLE_WAIT_FOR_BUS:r["bus_null"]=static_cast<CTaskSimpleWaitForBus*>(t)->m_bus==nullptr;break;
    case TASK_COMPLEX_LEAVE_CAR:{auto* v=static_cast<CTaskComplexLeaveCar*>(t);r["fields"]={v->m_pTargetVehicle==car,v->m_nTargetDoor,v->m_nDelayTime,v->m_bSensibleLeaveCar,v->m_bForceGetOut,v->m_bDie,v->m_pTaskUtilityLineUpPedWithCar==nullptr,v->m_nDoorFlagsSet,v->m_nNumGettingInSet,v->m_nDieAnimID,v->m_fDieAnimBlendDelta,v->m_fDieAnimSpeed,v->m_bIsInAir};break;}
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:{auto* p=reinterpret_cast<uint8*>(t);r["car"]=static_cast<CTaskComplexEnterCarAsPassenger*>(t)->GetTargetCar()==car;r["flags"]=p[0x10]&15;r["seat"]=*reinterpret_cast<int32*>(p+0x1C);r["move"]=*reinterpret_cast<int32*>(p+0x24);break;}
    default:break;
    }return r;
}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"leave-car-bus-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leave-car-bus-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* realPlayer=FindPlayerPed();if(!realPlayer||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    using Hook=ReversibleHooks::ReversibleHook::Base;
    const std::array names{"CreateSubTask","Clone","GetTaskType","CreateNextSubTask","CreateFirstSubTask","ControlSubTask","Clone","GetTaskType","CreateNextSubTask","CreateFirstSubTask","ControlSubTask"};
    const std::array<uintptr_t,11> addresses{0x63BEC0,0x63DAD0,0x63BE40,0x6422C0,0x642340,0x63BEB0,0x636A60,0x632920,0x632940,0x638940,0x632A40};
    std::array<Hook*,11> hooks{};std::vector<std::tuple<Hook*,bool,bool>> dependencies;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items()){
        for(int n=0;n<11;++n)if(c.Name()==(n<6?"CTaskComplexLeaveCarAndWander":"CTaskComplexWaitForBus")&&h->Name()==names[n])hooks[n]=h.get();
        if(h->Name()=="Constructor"&&(c.Name()=="CTaskComplexLeaveCarAndWander"||c.Name()=="CTaskComplexWaitForBus"||c.Name()=="CTaskSimpleWaitForBus"||c.Name()=="CTaskComplexEnterCar"))dependencies.emplace_back(h.get(),h->Hooked(),h->Locked());
    }});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;}
    auto playerBefore=ReversingProbe::Bytes(realPlayer,sizeof(CPlayerPed));auto refsBefore=ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs));auto* savedEmpty=CReferences::pEmptyList;
    auto poolBefore=GetTaskPool()->GetNoOfUsedSpaces();std::array<CReference,32> refs{};
    for(auto [h,e,l]:dependencies){h->LockState(false);h->State(false);}
    const auto startup=LBProbe::calls;
    bool passed=true;int pairs=0;
    {
    ReversingProbe::EntryPatch pn(0x56E210,Player),pl(reinterpret_cast<uintptr_t>(&FindPlayerPed),Player);
    ReversingProbe::EntryPatch wn(0x673D00,Wander),wl(reinterpret_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType),Wander);
    ReversingProbe::EntryPatch sn(0x681AF0,Set),sl(std::bit_cast<uintptr_t>(&CTaskManager::SetTask),Set);
    Raw<CVehicle> car;Raw<CPed> ped;Raw<CPlayerPed> player;Raw<CPlayerPedData> data;Raw<CPedIntelligence> intel;
    Raw<CTaskComplexLeaveCarAndWander> leave;Raw<CTaskComplexWaitForBus> bus;Raw<CTaskSimpleWaitForBus> wait;
    FakeTask sub,primary,wander;
    for(int fn=0;fn<11&&passed;++fn){
        auto* h=hooks[fn];bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);
        Write({{"event","hook_state"},{"function",fn},{"enabled",enabled},{"locked",locked},{"startup_calls",startup[fn]}});
        for(int i=0;i<512&&passed;++i){
            auto run=[&]{
                // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0; }
                trace=json::array();for(int j=0;j<32;++j)refs[j]={j==31?nullptr:&refs[j+1],nullptr};CReferences::pEmptyList=refs.data();
                car.b.fill(0);car.get()->SetTypeVehicle();ped.b.fill(0);player.b.fill(0);data.b.fill(0);intel.b.fill(0);leave.b.fill(0);bus.b.fill(0);wait.b.fill(0);
                auto* vehicle=i%7?car.get():nullptr;
                expectedPed=ped.get();expectedManager=&intel.get()->m_TaskMgr;wanderResult=&wander;playerResult=i%6?player.get():nullptr;
                *reinterpret_cast<CPedIntelligence**>(ped.b.data()+0x47C)=intel.get();ped.get()->bInVehicle=(i/2)%2;
                *reinterpret_cast<CPlayerPedData**>(player.b.data()+0x480)=data.get();data.get()->m_nBusFaresCollected=i*7;
                if(i%6==1)car.get()->m_pDriver=player.get();
                if(i%6>=2&&i%6<=4)car.get()->m_apPassengers[(i/6)%8]=player.get();
                sub.type=std::array{TASK_NONE,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_WAIT_FOR_BUS,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_SIMPLE_STAND_STILL,TASK_FINISHED}[i%6];
                primary.type=i%2?TASK_COMPLEX_WANDER:TASK_NONE;
                expectedManager->m_aPrimaryTasks[TASK_PRIMARY_DEFAULT]=(i/6)%3?&primary:nullptr;
                auto* l=leave.get();*reinterpret_cast<uintptr_t*>(l)=0x86E880;l->m_veh=vehicle;l->m_door=eTargetDoor(i%16);l->m_leaveDelayMs=i*53-2000;l->m_bSensibleLeaveCar=(i/16)%2;
                CEntity::SafeRegisterRef(l->m_veh);
                auto* b=bus.get();*reinterpret_cast<uintptr_t*>(b)=0x86E18C;b->m_bus=car.get();
                *reinterpret_cast<uintptr_t*>(wait.get())=0x86E1B8;wait.get()->m_bus=vehicle;
                auto* child=(i%6==2)?static_cast<CTask*>(wait.get()):static_cast<CTask*>(&sub);
                *reinterpret_cast<CTask**>(leave.b.data()+8)=child;*reinterpret_cast<CTask**>(bus.b.data()+8)=child;
                CTask* result{};int type=-1;
                void* self=fn<6?static_cast<void*>(l):static_cast<void*>(b);
                if(fn==0)result=reinterpret_cast<CTask*(__thiscall*)(void*,eTaskType)>(addresses[fn])(self,std::array{TASK_NONE,TASK_COMPLEX_LEAVE_CAR,TASK_FINISHED,TASK_SIMPLE_STAND_STILL}[i%4]);
                else if(fn==1||fn==6)result=reinterpret_cast<CTask*(__thiscall*)(void*)>(addresses[fn])(self);
                else if(fn==2||fn==7)type=int(reinterpret_cast<eTaskType(__thiscall*)(void*)>(addresses[fn])(self));
                else result=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(addresses[fn])(self,ped.get());
                const bool control=fn==5||fn==10;
                json out={{"type",type},{"result",control?json(result==child):TaskState(result,vehicle)},{"trace",trace},{"bus",b->m_bus==vehicle},{"refs",RefCount(car.get())},{"removed",bool(ped.get()->m_bRemoveFromWorld)},{"dont_drag",bool(ped.get()->bDontDragMeOutCar)},{"fares",data.get()->m_nBusFaresCollected},{"wander_assigned",expectedManager->m_aPrimaryTasks[TASK_PRIMARY_DEFAULT]==&wander},{"pool_delta",GetTaskPool()->GetNoOfUsedSpaces()-poolBefore}};
                if(result&&!control)delete result;
                CEntity::SafeCleanUpRef(l->m_veh);
                out["refs_clean"]=car.get()->m_pReferences==nullptr;out["pool_restored"]=GetTaskPool()->GetNoOfUsedSpaces()==poolBefore;
                return out;
            };
            h->State(false);auto a=LBProbe::calls[fn];auto original=run();auto b=LBProbe::calls[fn];
            h->State(true);auto candidate=run();auto c=LBProbe::calls[fn];
            passed=original==candidate&&a==b&&c==b+1&&original["refs_clean"]==true&&original["pool_restored"]==true;
            Write({{"event","comparison"},{"pair",pairs++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        h->State(enabled);h->LockState(locked);
    }
    }
    CReferences::pEmptyList=savedEmpty;
    for(auto [h,e,l]:dependencies){h->State(e);h->LockState(l);}
    bool restored=playerBefore==ReversingProbe::Bytes(realPlayer,sizeof(CPlayerPed))&&refsBefore==ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs))&&GetTaskPool()->GetNoOfUsedSpaces()==poolBefore;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();_CrtSetReportHook(report);
}
}
