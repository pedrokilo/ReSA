#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "EntryExitManager.h"
#include "Core/QuadTreeNode.h"
#include "Probe.hpp"
#include "TraceOperations.hpp"
#include "../ProbeSupport.hpp"
namespace EELive {
inline bool done{};
inline std::ofstream log;
inline void Write(json row){log<<row.dump()<<'\n';}
inline auto* Hook(const char* name){ReversibleHooks::ReversibleHook::Base* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CEntryExitManager")for(auto& h:c.Items())if(h->Name()==name)found=h.get();});return found;}
inline json Entry(CEntryExit* e) {
    return {{"rect",{e->m_recEntrance.left,e->m_recEntrance.top,e->m_recEntrance.right,e->m_recEntrance.bottom}},
        {"z",e->m_fEntranceZ},{"angle",e->m_fEntranceAngleRad},{"exit",{e->m_vecExitPos.x,e->m_vecExitPos.y,e->m_vecExitPos.z}},
        {"exit_angle",e->m_fExitAngle},{"flags",e->m_nFlags},{"area",int(e->m_nArea)},{"sky",e->m_nSkyColor},
        {"time",{e->m_nTimeOn,e->m_nTimeOff}},{"peds",e->m_nNumberOfPeds},{"link_null",e->m_pLink==nullptr},{"name",std::string(e->m_szName)}};
}
inline void Update(){
    if(done)return;done=true;
    if(!std::filesystem::exists("entryexit-pool-runtime.start"))return;
    log.open("entryexit-pool-runtime.jsonl");
    auto* add=Hook("AddOne");auto* del=Hook("DeleteOne");
    if(!add||!del){Write({{"event","error"},{"reason","hooks"}});log.flush();return;}
    auto& globalPool=*reinterpret_cast<CEntryExitsPool**>(0x96A7D8);
    auto& globalTree=*reinterpret_cast<CQuadTreeNode<CEntryExit*>**>(0x96A7D0);
    auto* savedPool=globalPool;auto* savedTree=globalTree;
    const auto liveStates=ReversingProbe::Bytes(*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(savedPool)+4),savedPool->GetSize());
    const auto liveStorage=ReversingProbe::Bytes(*reinterpret_cast<void**>(savedPool),savedPool->GetSize()*sizeof(CEntryExit));
    const bool addState=add->Hooked(),delState=del->Hooked();
    const auto burglary=CEntryExitManager::ms_bBurglaryHousesEnabled;
    CEntryExitManager::ms_bBurglaryHousesEnabled=false;
    int pairs{};bool passed=true;
    // The 400-entry control repeats the captured allocation demand against the
    // native code. The 512-entry run differs deliberately in capacity only.
    for(const int capacity: {400,512}){
        std::array<std::vector<json>,2> results;
        std::array<unsigned,2> calls{};
        for(int mode=0;mode<2;++mode){
            CEntryExitsPool pool(capacity,"entryexit isolated");pool.SetDealWithNoMemory(true);
            CQuadTreeNode<CEntryExit*> tree(WORLD_BOUNDS,0);
            globalPool=&pool;globalTree=&tree;add->State(mode!=0);del->State(mode!=0);
            const auto beforeCalls=EEProbe::adds+EEProbe::deletes;
            std::array<int,444> handles;handles.fill(-1);
            for(int op:EntryExitOperations){
                const auto before=pool.GetNoOfUsedSpaces();json r;
                if(op>=0){
                    // Integer, axis-aligned fixtures isolate allocation and avoid
                    // claiming coverage of random burglary flags or rotated trees.
                    const float x=float(op%20)*5.f,y=float(op/20)*5.f;
                    const auto flags=static_cast<CEntryExit::eFlags>(op%2?0x1000:0);
                    const int slot=reinterpret_cast<decltype(&CEntryExitManager::AddOne)>(0x43FA00)(x,y,10.f,0.f,2.f,4.f,0.f,x+1.f,y+2.f,11.f,90.f,AREA_CODE_NORMAL_WORLD,flags,3,8,20,2,"POOL");
                    const bool allocated=pool.GetNoOfUsedSpaces()==before+1;
                    r={{"operation","add"},{"allocated",allocated},{"used",pool.GetNoOfUsedSpaces()}};
                    if(allocated){handles[op]=slot;r["entry"]=Entry(pool.GetAt(slot));r["valid_slot"]=slot>=0&&slot<capacity;r["reference"]=pool.GetIdAt(slot);}
                    else r["failure_return"]=slot;
                }else{
                    const int slot=handles[-op-1];
                    if(slot<0){Write({{"event","error"},{"reason","invalid fixture delete"}});std::abort();}
                    reinterpret_cast<decltype(&CEntryExitManager::DeleteOne)>(0x43FD50)(slot);
                    r={{"operation","delete"},{"used",pool.GetNoOfUsedSpaces()},{"freed",pool.IsFreeSlotAtIndex(slot)}};
                    handles[-op-1]=-1;
                }
                results[mode].push_back(r);
            }
            // Clean up all objects, then fill every slot including the added
            // range. Native/candidate slot order after frees is intentionally
            // normalized: generic CPool has a pre-existing different cursor policy.
            std::vector<int> occupied;for(auto [index,e]:pool.GetAllValidWithIndex())occupied.push_back(index);
            for(int index:occupied)reinterpret_cast<decltype(&CEntryExitManager::DeleteOne)>(0x43FD50)(index);
            results[mode].push_back({{"operation","drain"},{"used",pool.GetNoOfUsedSpaces()}});
            for(int i=0;i<capacity;++i){
                const auto used=pool.GetNoOfUsedSpaces();
                const int slot=reinterpret_cast<decltype(&CEntryExitManager::AddOne)>(0x43FA00)(float(i),0.f,0.f,0.f,1.f,1.f,0.f,float(i),0.f,0.f,0.f,AREA_CODE_NORMAL_WORLD,static_cast<CEntryExit::eFlags>(0),0,0,24,0,"POOL");
                results[mode].push_back({{"operation","refill"},{"allocated",pool.GetNoOfUsedSpaces()==used+1},{"valid_slot",slot>=0&&slot<capacity},{"used",pool.GetNoOfUsedSpaces()},{"entry",Entry(pool.GetAt(slot))}});
            }
            const auto fullUsed=pool.GetNoOfUsedSpaces();
            const int full=reinterpret_cast<decltype(&CEntryExitManager::AddOne)>(0x43FA00)(0,0,0,0,1,1,0,0,0,0,0,AREA_CODE_NORMAL_WORLD,static_cast<CEntryExit::eFlags>(0),0,0,24,0,"FULL");
            results[mode].push_back({{"operation","full"},{"return",full},{"used",pool.GetNoOfUsedSpaces()},{"unchanged",fullUsed==pool.GetNoOfUsedSpaces()}});
            calls[mode]=EEProbe::adds+EEProbe::deletes-beforeCalls;
            globalPool=savedPool;globalTree=savedTree;
        }
        int originalFailures{},candidateFailures{};
        for(size_t i=0;i<results[0].size();++i){
            // Reference generation depends on the pre-existing allocation order.
            results[0][i].erase("reference");results[1][i].erase("reference");
            const bool equal=results[0][i]==results[1][i];
            auto normalizedOriginal=results[0][i],normalizedCandidate=results[1][i];
            const char* returnKey=normalizedOriginal["operation"]=="full"?"return":"failure_return";
            const bool failureReturn=normalizedOriginal.contains(returnKey)&&normalizedCandidate.contains(returnKey)&&normalizedOriginal[returnKey]==0&&normalizedCandidate[returnKey]==-1;
            normalizedOriginal.erase(returnKey);normalizedCandidate.erase(returnKey);
            const bool expectedDifference=!equal&&failureReturn&&normalizedOriginal==normalizedCandidate;
            passed&=equal||expectedDifference;
            Write({{"event","comparison"},{"capacity",capacity},{"case",i},{"original",results[0][i]},{"candidate",results[1][i]},{"equal",equal},{"expected_FIX_BUGS_difference",expectedDifference}});++pairs;
            if(results[0][i]["operation"]=="add"&&!results[0][i]["allocated"].get<bool>())++originalFailures;
            if(results[1][i]["operation"]=="add"&&!results[1][i]["allocated"].get<bool>())++candidateFailures;
        }
        passed&=calls[0]==0&&calls[1]>0;
        Write({{"event","capacity_result"},{"capacity",capacity},{"original_failures",originalFailures},{"candidate_failures",candidateFailures},{"original_candidate_calls",calls[0]},{"candidate_calls",calls[1]}});
        passed&=originalFailures==(capacity==400?19:0)&&candidateFailures==originalFailures;
    }
    add->State(addState);del->State(delState);CEntryExitManager::ms_bBurglaryHousesEnabled=burglary;
    const bool restored=globalPool==savedPool&&globalTree==savedTree&&liveStates==ReversingProbe::Bytes(*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(savedPool)+4),savedPool->GetSize())&&liveStorage==ReversingProbe::Bytes(*reinterpret_cast<void**>(savedPool),savedPool->GetSize()*sizeof(CEntryExit));
    Write({{"event","complete"},{"passed",passed&&restored},{"pairs",pairs},{"state_restored",restored},{"live_capacity",savedPool->GetSize()}});log.flush();
}
}
