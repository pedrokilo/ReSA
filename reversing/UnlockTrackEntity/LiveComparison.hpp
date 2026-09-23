#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexTrackEntity.h"
#include "TaskSimpleGoToPointFine.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace TELive {
inline bool done{};inline std::ofstream log;
inline constexpr std::array names{"SetOffsetPos","CalcTargetPos","CalcMoveRatio","ControlSubTask","SetTargetPos"};
inline constexpr std::array counts{512,2048,4096,2048,512};
inline void Write(const json& row){log<<row.dump()<<'\n';log.flush();}
template<class T>struct Raw{alignas(T)std::array<uint8,sizeof(T)> bytes{};T* get(){return reinterpret_cast<T*>(bytes.data());}};
inline json Bytes(const void* p,size_t n){return ReversingProbe::Bytes(p,n);}
struct Child:CTaskSimpleGoToPointFine {
    Child():CTaskSimpleGoToPointFine(0.0f,{},0.5f,nullptr){}
    eTaskType type=TASK_SIMPLE_GO_TO_POINT_FINE;int aborts{},typeCalls{};bool abortResult{};
    eTaskType GetTaskType()const override{++const_cast<Child*>(this)->typeCalls;return type;}
    bool MakeAbortable(CPed*,eAbortPriority p,const CEvent* e)override{aborts+=1+int(p)+(e?100:0);return abortResult;}
};
inline int nextCalls{};inline CTask* nextResult{};
inline CTask* __fastcall Next(CTaskComplexTrackEntity*,void*,CPed*){++nextCalls;return nextResult;}
inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto game=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(game/"unlock-track-entity-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(game/"unlock-track-entity-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,5> hooks{};std::array<bool,5> enabled{},locked{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())for(int i=0;i<5;++i)if(c.Name()==(i==4?"CTaskSimpleGoToPointFine":"CTaskComplexTrackEntity")&&h->Name()==names[i])hooks[i]=h.get();});
    for(auto* h:hooks)if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}
    const auto playerBefore=Bytes(player,sizeof(CPlayerPed));const auto time=CTimer::GetTimeInMS();const auto ratioStatics=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC18CF4),24);
    for(int i=0;i<5;++i){enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);Write({{"event","hook_state"},{"function",i},{"enabled",enabled[i]},{"locked",locked[i]}});}
    // Helpers may now be called directly from native ControlSubTask without recursion.
    hooks[1]->State(true);hooks[2]->State(true);hooks[4]->State(true);
    Raw<CTaskComplexTrackEntity> task;Raw<CPhysical> target;Raw<CPed> ped;Raw<CMatrixLink> matrix;Child child;Child sentinel;
    std::array<void*,11> vtable;std::memcpy(vtable.data(),reinterpret_cast<void*>(0x86F998),sizeof(vtable));vtable[8]=reinterpret_cast<void*>(&Next);nextResult=&sentinel;
    bool passed=true;int pairs=0;
    for(int fn=0;fn<5&&passed;++fn)for(int i=0;i<counts[fn]&&passed;++i){
        auto run=[&]{
            task.bytes.fill(0);target.bytes.fill(0);ped.bytes.fill(0);matrix.bytes.fill(0);nextCalls=0;child.aborts=child.typeCalls=0;child.abortResult=(i&1)!=0;
            child.type=std::array{TASK_COMPLEX_FOLLOW_POINT_ROUTE,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_FOLLOW_NODE_ROUTE}[i%4];
            child.m_vecTargetPoint=CVector{float(i%7),float(i%11),float(i%13)};child.m_fRadius=i&1?0.5f:0.25f;child.m_GoToFlags=uint8(i);child.m_fMoveRatio=-7.0f;
            auto* t=task.get();*reinterpret_cast<void***>(t)=vtable.data();*reinterpret_cast<CTask**>(task.bytes.data()+8)=&child;
            t->m_toTrack=target.get();t->m_offsetPosn=CVector{float(i%17)*.125f,float(i%13)*-.25f,float(i%19)};t->a=uint8(i%3);t->f=uint8(i&1);
            t->m_rangeMin=2.0f;t->m_rangeMax=12.0f;t->m_fMoveRatio=float(i%37)*.125f-1.0f;
            target.bytes[0x36]=uint8(1+i%5);target.get()->SetPosn({float(i%31)*.25f,float(i%23)*-.5f,float(i%29)*.125f});
            target.get()->m_vecMoveSpeed=CVector{float(i%11)*.03125f,float(i%7)*-.0625f,float(i%5)*.125f};
            matrix.get()->SetRotateZ(float(i%23)*.125f);matrix.get()->GetPosition()=target.get()->GetPosition();
            if(t->f || (i&2))*reinterpret_cast<CMatrixLink**>(target.bytes.data()+0x14)=matrix.get();
            ped.get()->SetPosn({float(i%41)*.125f,float(i%17)*.25f,float(i%3)});
            const auto targetBefore=Bytes(target.get(),sizeof(CPhysical));const auto pedBefore=Bytes(ped.get(),sizeof(CPed));
            if(fn==0){using F=void(__thiscall*)(CTaskComplexTrackEntity*,CVector);reinterpret_cast<F>(0x65F760)(t,{float(i)*.25f,float(i)*-.5f,float(i%13)});}
            if(fn==1){using F=void(__thiscall*)(CTaskComplexTrackEntity*,CPed*);reinterpret_cast<F>(0x65F780)(t,ped.get());}
            if(fn==2){
                constexpr std::array edge{0.0f,0.04f,0.2f*0.2f,1.0f,25.0f,100.0f};
                const auto base=edge[i%edge.size()];t->m_distToTargetSq=i<128?std::nextafter(base,i&1?INFINITY:-INFINITY):float(i-128)*.0125f;
                using F=void(__thiscall*)(CTaskComplexTrackEntity*,CPed*);reinterpret_cast<F>(0x65F930)(t,ped.get());
            }
            json r;
            if(fn==3){
                CTimer::m_snTimeInMilliseconds=1000;
                if(i%11==0)t->m_toTrack=nullptr;
                t->gap2=i&4;t->gap3=i&8;t->m_someStartTimeMs=uint32(std::array{0u,500u,1000u,0xFFFFFFF0u}[i/16%4]);t->m_expirationInterval=uint32(std::array{0,1,500,1500}[i/64%4]);
                using F=CTask*(__thiscall*)(CTaskComplexTrackEntity*,CPed*);auto* result=reinterpret_cast<F>(0x663640)(t,ped.get());
                r["returned"]=result==nullptr?0:result==&child?1:result==&sentinel?2:3;
                r["aborts"]=child.aborts;r["type_calls"]=child.typeCalls;r["next_calls"]=nextCalls;
            }
            if(fn==4){using F=void(__thiscall*)(CTaskSimpleGoToPointFine*,CVector);reinterpret_cast<F>(0x65F330)(&child,i%3?CVector{float(i%7)*.5f,-float(i%11),float(i%13)}:child.m_vecTargetPoint);}
            r["task"]=Bytes(task.bytes.data()+0x10,sizeof(CTaskComplexTrackEntity)-0x10);
            r["child"]=Bytes(reinterpret_cast<uint8*>(&child)+8,sizeof(CTaskSimpleGoToPointFine)-8);
            r["untouched"]=targetBefore==Bytes(target.get(),sizeof(CPhysical))&&pedBefore==Bytes(ped.get(),sizeof(CPed));return r;
        };
        hooks[fn]->State(false);const auto a=TEProbe::calls[fn];const auto original=run();const auto b=TEProbe::calls[fn];
        hooks[fn]->State(true);const auto candidate=run();const auto c=TEProbe::calls[fn];
        passed=original==candidate&&b==a&&c==b+1;
        Write({{"event","comparison"},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});++pairs;
    }
    for(int i=0;i<5;++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);}
    CTimer::m_snTimeInMilliseconds=time;std::memcpy(reinterpret_cast<void*>(0xC18CF4),ratioStatics.data(),24);
    const bool restored=playerBefore==Bytes(player,sizeof(CPlayerPed));Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
