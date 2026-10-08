#pragma once
#include "TaskComplexWanderStandard.h"
#include "../../ProbeSupport.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include <fstream>

namespace RequestedWanderTypeLive {
using namespace ReversibleHooks::ReversibleHook;
__declspec(noinline) inline eWanderType Dispatch(CTaskComplexWander* obj) { return obj->GetWanderType(); }
inline bool JumpEquals(const void* from,const void* to) {
    std::array<uint8,5> expected{0xE9};
    const auto delta=uint32(reinterpret_cast<uintptr_t>(to)-reinterpret_cast<uintptr_t>(from)-5);
    memcpy(expected.data()+1,&delta,4);
    return memcmp(from,expected.data(),expected.size())==0;
}
struct Range { const void* address; std::vector<uint8> bytes; };
inline void Update() {
    static bool done{}; if(done) return; done=true;
    char path[MAX_PATH]{}; GetModuleFileNameA(nullptr,path,MAX_PATH);
    const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"requested-wander-type-runtime.start")) return;
    std::ofstream log(dir/"requested-wander-type-runtime.jsonl");
    const auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    Base* base{}; std::vector<std::tuple<Base*,bool,bool>> flags;
    std::vector<Range> entries;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){
        for(auto& h:cat.Items()) {
            flags.emplace_back(h.get(),h->Hooked(),h->Locked());
            if(cat.Name()=="CTaskComplexWanderStandard" && h->Name()=="GetWanderType") base=h.get();
            const void* a{}; const void* b{};
            if(h->Type()==Base::HookType::Simple) {auto* s=static_cast<Simple*>(h.get());a=s->GetHookGTAAddress();b=s->GetHookOurAddress();}
            else if(h->Type()==Base::HookType::Virtual) {auto* v=static_cast<Virtual*>(h.get());a=v->GetHookGTAAddress();b=v->GetHookOurAddress();}
            if(a) entries.push_back({a,ReversingProbe::Bytes(a,52)});
            if(b) entries.push_back({b,ReversingProbe::Bytes(b,52)});
        }
    });
    if(!base || base->Type()!=Base::HookType::Virtual) {write({{"event","error"},{"reason","missing virtual target"}});return;}
    auto* hook=static_cast<Virtual*>(base);
    if(hook->GetHookGTAAddress()!=reinterpret_cast<void*>(0x48E5D0)) {write({{"event","error"},{"reason","target identity mismatch"}});return;}
    // Private valid objects. Only the getter is under test; construction is fixture setup.
    CTaskComplexWanderStandard a{static_cast<eMoveState>(0),0,false};
    CTaskComplexWanderStandard b{static_cast<eMoveState>(0),0,false};
    auto** gtaTable=reinterpret_cast<void**>(0x85A200);
    auto** ourTable=*reinterpret_cast<void***>(&b);
    const auto gtaBefore=ReversingProbe::Bytes(gtaTable,15*sizeof(void*));
    const auto ourBefore=ReversingProbe::Bytes(ourTable,15*sizeof(void*));
    const bool initialHooked=hook->Hooked(),initialLocked=hook->Locked();
    hook->LockState(false);hook->State(true);
    const auto enabledCandidate=ReversingProbe::Bytes(hook->GetHookOurAddress(),5);
    const auto enabledGTA=ReversingProbe::Bytes(hook->GetHookGTAAddress(),5);
    bool passed=JumpEquals(hook->GetHookGTAAddress(),hook->GetHookOurAddress());
    int pairs{};
    for(int seed=0;seed<256;++seed) {
        a.m_nDir=b.m_nDir=uint8(seed);a.m_nFlags=b.m_nFlags=uint8(seed);
        a.m_nMoveState=b.m_nMoveState=static_cast<eMoveState>(seed%4);
        a.m_nMinNextScanTime=b.m_nMinNextScanTime=uint32(seed)*0x01010101u;
        a.m_TaskTimer.m_nStartTime=b.m_TaskTimer.m_nStartTime=uint32(seed)*7919u;
        a.m_TaskTimer.m_nInterval=b.m_TaskTimer.m_nInterval=uint32(seed)*31u;
        *reinterpret_cast<void***>(&a)=gtaTable;
        *reinterpret_cast<void***>(&b)=ourTable;
        const auto ab=ReversingProbe::Bytes(&a,sizeof(a)),bb=ReversingProbe::Bytes(&b,sizeof(b));
        for(int route=0;route<5;++route) {
            using Fn=eWanderType(__thiscall*)(CTaskComplexWanderStandard*);
            const auto call=[&](CTaskComplexWanderStandard* obj){
                if(route==0)return reinterpret_cast<Fn>(hook->GetHookGTAAddress())(obj);
                if(route==1)return reinterpret_cast<Fn>(hook->GetHookOurAddress())(obj);
                if(route==2)return reinterpret_cast<Fn>(gtaTable[11])(obj);
                if(route==3)return reinterpret_cast<Fn>(ourTable[11])(obj);
                return Dispatch(obj);
            };
            hook->State(false);
            const std::array<uint8,3> expected{0x33,0xC0,0xC3};
            const bool originalBytes=memcmp(hook->GetHookGTAAddress(),expected.data(),3)==0;
            const bool disabledCandidate=JumpEquals(hook->GetHookOurAddress(),hook->GetHookGTAAddress());
            const bool originalTables=gtaTable[11]==hook->GetHookGTAAddress() && ourTable[11]==gtaTable[11];
            const auto original=call(&a);
            hook->State(true);
            const bool candidateBytes=memcmp(hook->GetHookOurAddress(),enabledCandidate.data(),5)==0
                && memcmp(hook->GetHookGTAAddress(),enabledGTA.data(),5)==0
                && JumpEquals(hook->GetHookGTAAddress(),hook->GetHookOurAddress());
            const bool candidateTables=gtaTable[11]==hook->GetHookOurAddress() && ourTable[11]==gtaTable[11];
            const auto candidate=call(&b);
            const bool objects=ab==ReversingProbe::Bytes(&a,sizeof(a)) && bb==ReversingProbe::Bytes(&b,sizeof(b));
            const bool equal=originalBytes && disabledCandidate && originalTables && candidateBytes && candidateTables
                && objects && int32(original)==0 && int32(candidate)==0;
            passed &= equal;++pairs;
            write({{"event","comparison"},{"function","CTaskComplexWanderStandard::GetWanderType"},{"pair",pairs-1},{"seed",seed},{"route",route},
                {"original",int32(original)},{"candidate",int32(candidate)},{"equal",equal},{"accepted",equal},{"original_bytes",originalBytes},
                {"disabled_candidate_bytes",disabledCandidate},{"candidate_bytes",candidateBytes},{"original_tables",originalTables},{"candidate_tables",candidateTables},{"objects_unchanged",objects}});
        }
        ReversingProbe::PumpMessages();
    }
    hook->State(initialHooked);hook->LockState(initialLocked);
    *reinterpret_cast<void***>(&a)=ourTable;
    bool restoredFlags=std::ranges::all_of(flags,[](const auto& f){auto [h,e,l]=f;return h->Hooked()==e && h->Locked()==l;});
    bool restoredEntries=true;json failedRanges=json::array();
    for(const auto& r:entries) if(r.bytes!=ReversingProbe::Bytes(r.address,r.bytes.size())) {restoredEntries=false;failedRanges.push_back(reinterpret_cast<uintptr_t>(r.address));}
    const bool restoredTables=gtaBefore==ReversingProbe::Bytes(gtaTable,15*sizeof(void*)) && ourBefore==ReversingProbe::Bytes(ourTable,15*sizeof(void*));
    const bool restored=restoredFlags && restoredEntries && restoredTables;
    write({{"event","function_summary"},{"function","CTaskComplexWanderStandard::GetWanderType"},{"cases",pairs},{"failures",passed?0:1}});
    write({{"event",passed && restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored},{"flags_restored",restoredFlags},
        {"entries_restored",restoredEntries},{"tables_restored",restoredTables},{"snapshot_ranges",entries.size()},{"failed_ranges",failedRanges}});
}
}
