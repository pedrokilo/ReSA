#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedList.h"
#include "Pools/PedPool.h"
#include "TaskComplexKillPedOnFoot.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace PedListLive {
inline bool done{}; inline std::ofstream log;
inline int seed{}; inline json trace;
inline std::array<CTask*,64> tasks{};
inline bool __fastcall InCar(CPedIntelligence* p,void*) { auto id=uintptr_t(p)-1; trace.push_back({"car",id}); return (id+seed)%3==0; }
inline CTask* __fastcall Task(CPedIntelligence* p,void*,eTaskType type) { auto id=uintptr_t(p)-1; trace.push_back({"task",id,int(type)}); return tasks[id]; }
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline int Report(int type,char* msg,int*) { if(type!=_CRT_ASSERT)return FALSE; Write({{"event","assertion"},{"message",msg ? msg:""}}); ExitProcess(91); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pedlist-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pedlist-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; auto report=_CrtSetReportHook(Report);
    const std::array names{"BuildListFromGroup_NotInCar_NoLeader","BuildListOfPedsOfPedType","RemovePedsAttackingPedType","RemovePedsThatDontListenToPlayer"};
    const std::array<uintptr_t,4> addresses{0x69A340,0x69A3B0,0x69A450,0x69A420};
    std::array<ReversibleHooks::ReversibleHook::Base*,4> hooks{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPedList")for(auto& h:c.Items())for(int n=0;n<4;++n)if(h->Name()==names[n])hooks[n]=h.get();});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"}});_CrtSetReportHook(report);return;}
    auto& poolGlobal=*reinterpret_cast<CPedPool**>(0xB74490); auto* savedPool=poolGlobal;
    const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    bool passed=true; int pair=0;
    {
        ReversingProbe::EntryPatch carDll(std::bit_cast<uintptr_t>(&CPedIntelligence::IsInACarOrEnteringOne),InCar), carExe(0x601CC0,InCar);
        ReversingProbe::EntryPatch taskDll(std::bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),Task), taskExe(0x600EE0,Task);
        alignas(CCopPed) std::array<uint8,sizeof(CCopPed)*64> storage{};
        std::array<uint8,64> states{};
        alignas(CTaskComplexKillPedOnFoot) std::array<std::array<uint8,sizeof(CTaskComplexKillPedOnFoot)>,64> taskStorage{};
        alignas(CPedGroupMembership) std::array<uint8,sizeof(CPedGroupMembership)> groupStorage{};
        auto* group=reinterpret_cast<CPedGroupMembership*>(groupStorage.data());
        auto* members=reinterpret_cast<CPed**>(groupStorage.data()+4);
        for(int fn=0;fn<4&&passed;++fn) {
            auto* hook=hooks[fn];const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
            for(int i=0;i<1024&&passed;++i) {
                seed=i; CPedPool pool((i%65),storage.data(),states.data());poolGlobal=&pool;
                std::memset(storage.data(),0,storage.size());
                for(int n=0;n<64;++n) {
                    auto* p=reinterpret_cast<CPed*>(storage.data()+n*sizeof(CCopPed));
                    p->m_nPedType=ePedType((i&32)?i%23:(n+i)%23);
                    *reinterpret_cast<CPedIntelligence**>(reinterpret_cast<uint8*>(p)+0x47C)=reinterpret_cast<CPedIntelligence*>(uintptr_t(n+1));
                    p->bDoesntListenToPlayerGroupCommands=((i>>((n+i)%10))&1)!=0;
                    states[n]=((i&64)||((n+i)%4))?uint8(n):uint8(0x80|n);
                    auto* task=reinterpret_cast<CTaskComplexKillPedOnFoot*>(taskStorage[n].data());
                    task->m_target=(n+i)%4==1?nullptr:p;tasks[n]=(n+i)%4==0?nullptr:task;
                    if(n<8)members[n]=(i&(1<<n))?p:nullptr;
                }
                CPedList before{};before.m_count=i%31;
                for(int n=0;n<30;++n)before.m_peds[n]=(fn<2||n<int(before.m_count))?reinterpret_cast<CPed*>(storage.data()+n*sizeof(CCopPed)):nullptr;
                const auto inputs=ReversingProbe::Bytes(storage.data(),storage.size());
                const auto groupBefore=groupStorage;
                const auto taskBefore=taskStorage;
                const auto statesBefore=states;
                auto run=[&]() {
                    CPedList list=before; trace=json::array();
                    if(fn==0)reinterpret_cast<void(__thiscall*)(CPedList*,CPedGroupMembership*)>(addresses[fn])(&list,group);
                    else if(fn==3)reinterpret_cast<void(__thiscall*)(CPedList*)>(addresses[fn])(&list);
                    else reinterpret_cast<void(__thiscall*)(CPedList*,int32)>(addresses[fn])(&list,i%23);
                    return json{{"list",ReversingProbe::Bytes(&list,sizeof(list))},{"trace",trace}};
                };
                hook->State(false);auto start=PedListProbe::calls;auto original=run();auto middle=PedListProbe::calls;
                hook->State(true);auto candidate=run();auto end=PedListProbe::calls;
                bool intact=inputs==ReversingProbe::Bytes(storage.data(),storage.size())&&groupBefore==groupStorage&&taskBefore==taskStorage&&statesBefore==states;
                passed=original==candidate&&intact&&middle==start&&end[fn]==middle[fn]+1;
                Write({{"event","comparison"},{"pair",pair++},{"function",names[fn]},{"original",original},{"candidate",candidate},{"equal",passed},{"inputs_restored",intact},{"original_candidate_calls",middle[fn]-start[fn]},{"candidate_calls",end[fn]-middle[fn]}});
                poolGlobal=savedPool;
            }
            hook->State(enabled);hook->LockState(locked);
        }
    }
    const bool restored=poolGlobal==savedPool&&ReversingProbe::Bytes(player,sizeof(CPlayerPed))==savedPlayer;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
