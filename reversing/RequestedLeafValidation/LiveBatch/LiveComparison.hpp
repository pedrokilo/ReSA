#pragma once
#include "DecisionMakers/Decision.h"
#include "TaskSimpleGangDriveBy.h"
#include "../../ProbeSupport.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include <fstream>

namespace RequestedLeafLive {
__declspec(noinline) inline eTaskType VirtualType(const CTask* task) { return task->GetTaskType(); }
inline void Update() {
    static bool done{};
    if (done) return;
    done = true;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "requested-leaf-runtime.start")) return;
    std::ofstream log(dir / "requested-leaf-runtime.jsonl");
    const auto write = [&](const json& row) { log << row.dump() << '\n'; log.flush(); };
    using namespace ReversibleHooks::ReversibleHook;
    Base* decision{}; Base* type{};
    std::vector<std::tuple<Base*,bool,bool>> flags;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat) {
        for (auto& h : cat.Items()) {
            flags.emplace_back(h.get(),h->Hooked(),h->Locked());
            if (cat.Name()=="CDecision" && h->Name()=="From") decision=h.get();
            if (cat.Name()=="CTaskSimpleGangDriveBy" && h->Name()=="GetTaskType") type=h.get();
        }
    });
    if (!decision || !type) { write({{"event","error"},{"reason","missing target"}}); return; }
    auto* dh=static_cast<Simple*>(decision); auto* th=static_cast<Virtual*>(type);
    auto** gtaTable=reinterpret_cast<void**>(0x86D944);
    CTaskSimpleGangDriveBy a{nullptr,nullptr,1.0f,1,eDrivebyStyle::AI_ALL_DIRN,false};
    CTaskSimpleGangDriveBy b{nullptr,nullptr,1.0f,1,eDrivebyStyle::AI_ALL_DIRN,false};
    auto** ourTable=*reinterpret_cast<void***>(&b);
    const auto dEntry=ReversingProbe::Bytes(reinterpret_cast<void*>(0x6006B0),52);
    const auto dCandidate=ReversingProbe::Bytes(dh->GetHookOurAddress(),52);
    const auto tEntry=ReversingProbe::Bytes(reinterpret_cast<void*>(0x6218B0),52);
    const auto tCandidate=ReversingProbe::Bytes(th->GetHookOurAddress(),52);
    const auto gTable=ReversingProbe::Bytes(gtaTable,9*sizeof(void*));
    const auto oTable=ReversingProbe::Bytes(ourTable,9*sizeof(void*));
    decision->LockState(false); type->LockState(false);
    bool passed=true; int decisionPairs=0,typePairs=0;
    for (int seed=0;seed<256;++seed) {
        CDecision input,left,right;
        for (size_t i=0;i<6;++i) {
            input.m_Tasks[i]=static_cast<eTaskType>((uint32(seed)*0x01010101u)^(uint32(i)*0x10203040u));
            for (size_t j=0;j<4;++j) input.m_Probs[i][j]=uint8(seed+i*31+j*67);
            for (size_t j=0;j<2;++j) input.m_Bools[i][j]=((seed>>(i+j)%8)&1)!=0;
        }
        for (int self=0;self<2;++self) {
            left=input; right=input;
            if (!self) {left.SetDefault();right.SetDefault();}
            const auto before=ReversingProbe::Bytes(&input,sizeof(input));
            using CopyFn=void(__thiscall*)(CDecision*,const CDecision&);
            decision->State(false);
            const bool originalBytes=memcmp(reinterpret_cast<void*>(0x6006B0),dh->m_OriginalFunctionContent,dh->m_iHookedBytes)==0;
            reinterpret_cast<CopyFn>(0x6006B0)(&left,self?left:input);
            decision->State(true);
            const bool candidateBytes=memcmp(reinterpret_cast<void*>(0x6006B0),&dh->m_HookContent,dh->m_iHookedBytes)==0;
            reinterpret_cast<CopyFn>(0x6006B0)(&right,self?right:input);
            const bool equal=originalBytes && candidateBytes && ReversingProbe::Bytes(&left,sizeof(left))==ReversingProbe::Bytes(&right,sizeof(right)) && before==ReversingProbe::Bytes(&input,sizeof(input));
            passed &= equal; ++decisionPairs;
            write({{"event","comparison"},{"function","CDecision::From"},{"seed",seed},{"self",self},{"equal",equal},{"original_bytes",originalBytes},{"candidate_bytes",candidateBytes}});
        }
        a.m_nFrequencyPercentage=b.m_nFrequencyPercentage=int8(seed);
        a.m_nAttackTimer=b.m_nAttackTimer=int16(seed*131);
        a.m_nLOSCheckTime=b.m_nLOSCheckTime=uint32(seed)*0x01010101u;
        *reinterpret_cast<void***>(&a)=gtaTable;
        for (int route=0;route<3;++route) {
            using Fn=eTaskType(__thiscall*)(const CTaskSimpleGangDriveBy*);
            const auto call=[&](const CTaskSimpleGangDriveBy* obj) {
                if (route==0) return reinterpret_cast<Fn>(0x6218B0)(obj);
                if (route==1) return reinterpret_cast<Fn>(gtaTable[4])(obj);
                return VirtualType(obj);
            };
            const auto ba=ReversingProbe::Bytes(&a,sizeof(a)),bb=ReversingProbe::Bytes(&b,sizeof(b));
            type->State(false);
            const std::array<uint8,6> expected{0xB8,0xFE,0x03,0,0,0xC3};
            const bool originalBytes=memcmp(reinterpret_cast<void*>(0x6218B0),expected.data(),6)==0;
            const bool originalTables=gtaTable[4]==reinterpret_cast<void*>(0x6218B0) && ourTable[4]==gtaTable[4];
            const auto original=call(&a);
            type->State(true);
            const bool candidateTables=gtaTable[4]==th->GetHookOurAddress() && ourTable[4]==gtaTable[4] && ReversingProbe::JumpsToDll(0x6218B0);
            const auto candidate=call(&b);
            const bool equal=originalBytes && originalTables && candidateTables && int32(original)==1022 && int32(candidate)==1022 && ba==ReversingProbe::Bytes(&a,sizeof(a)) && bb==ReversingProbe::Bytes(&b,sizeof(b));
            passed &= equal; ++typePairs;
            write({{"event","comparison"},{"function","CTaskSimpleGangDriveBy::GetTaskType"},{"seed",seed},{"route",route},{"original",int32(original)},{"candidate",int32(candidate)},{"equal",equal},{"original_bytes",originalBytes},{"original_tables",originalTables},{"candidate_tables",candidateTables}});
        }
        *reinterpret_cast<void***>(&a)=ourTable;
        ReversingProbe::PumpMessages();
    }
    for (const auto [h,enabled,locked]:flags) {h->LockState(false);h->State(enabled);h->LockState(locked);}
    bool restored=std::ranges::all_of(flags,[](auto& f){auto [h,e,l]=f;return h->Hooked()==e && h->Locked()==l;});
    const bool entries=dEntry==ReversingProbe::Bytes(reinterpret_cast<void*>(0x6006B0),52) && tEntry==ReversingProbe::Bytes(reinterpret_cast<void*>(0x6218B0),52);
    const bool candidates=dCandidate==ReversingProbe::Bytes(dh->GetHookOurAddress(),52) && tCandidate==ReversingProbe::Bytes(th->GetHookOurAddress(),52);
    const bool tables=gTable==ReversingProbe::Bytes(gtaTable,9*sizeof(void*)) && oTable==ReversingProbe::Bytes(ourTable,9*sizeof(void*));
    restored &= entries && candidates && tables;
    write({{"event",passed && restored?"complete":"error"},{"decision_pairs",decisionPairs},{"type_pairs",typePairs},{"pairs",decisionPairs+typePairs},{"state_restored",restored},{"entries_restored",entries},{"candidates_restored",candidates},{"tables_restored",tables}});
}
}
