#pragma once
#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexGoToAttractor.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "../ProbeSupport.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include <fstream>

// Prepared instrumentation, not validated evidence. Invoke once on the game thread.
namespace RequestedTaskLifetime {
using namespace ReversibleHooks::ReversibleHook;
struct Snapshot {
    Base* hook; bool enabled, locked;
    void* gta{}; void* ours{};
    std::vector<uint8> gtaBytes, ourBytes;
};
inline eTaskType __declspec(noinline) TaskType(const CTask* t) { return t->GetTaskType(); }
inline bool __declspec(noinline) TaskSimple(const CTask* t) { return t->IsSimple(); }
__declspec(noinline) inline CTask* ReadOwnedSubtask(const CTask* t) { return t->GetSubTask(); }
inline void Update() {
    static bool done{}; if (done) return; done=true;
    char path[MAX_PATH]{}; GetModuleFileNameA(nullptr,path,MAX_PATH);
    const auto dir=std::filesystem::path(path).parent_path();
    if (!std::filesystem::exists(dir/"requested-task-lifetime.start")) return;
    std::ofstream log(dir/"requested-task-lifetime.jsonl");
    const auto write=[&](const json& j){log<<j.dump()<<'\n';log.flush();};
    std::vector<Snapshot> snapshots;
    std::map<std::string,Simple*> targets;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){
        for(auto& holder:cat.Items()) {
            auto* h=holder.get(); Snapshot s{h,h->Hooked(),h->Locked()};
            if(h->Type()==Base::HookType::Simple) {
                auto* p=static_cast<Simple*>(h); s.gta=p->GetHookGTAAddress();s.ours=p->GetHookOurAddress();
                const auto key=std::string(cat.Name())+"::"+h->Name();
                targets[key]=p;
            } else if(h->Type()==Base::HookType::Virtual) {
                auto* p=static_cast<Virtual*>(h);s.gta=p->GetHookGTAAddress();s.ours=p->GetHookOurAddress();
            }
            if(s.gta){s.gtaBytes=ReversingProbe::Bytes(s.gta,52);s.ourBytes=ReversingProbe::Bytes(s.ours,52);}
            snapshots.push_back(std::move(s));
        }
    });
    const std::array<const char*,7> required{
        "CTaskComplexGangJoinRespond::Constructor","CTaskComplexGangJoinRespond::Destructor",
        "CTaskComplexGoToAttractor::Constructor","CTaskComplexGoToAttractor::Destructor",
        "CTaskComplexSmartFleePoint::Constructor","CTaskComplexSmartFleePoint::Destructor",
        "CTaskComplexFollowLeaderInFormation::Destructor"};
    for(const auto* key:required) if(!targets.contains(key)) {write({{"event","error"},{"missing",key}});return;}
    // Snapshot both vtables before changing any hook, using ordinary valid objects.
    CTaskComplexGangJoinRespond gang{false};
    CTaskComplexGoToAttractor attr{nullptr,CVector{1,2,3},0,0,0,PEDMOVE_WALK};
    CTaskComplexSmartFleePoint smart{CVector{1,2,3},false,10,-1};
    CTaskComplexFollowLeaderInFormation follow{nullptr,nullptr,CVector{1,2,3},10};
    std::vector<std::pair<void*,std::vector<uint8>>> tables;
    for(auto* t:std::array<CTask*,4>{&gang,&attr,&smart,&follow}) {
        auto* v=*reinterpret_cast<void**>(t);tables.emplace_back(v,ReversingProbe::Bytes(v,11*sizeof(void*)));
    }
    for(uintptr_t v:std::array<uintptr_t,4>{0x86FB5C,0x86FF3C,0x86F744,0x870C3C})
        tables.emplace_back(reinterpret_cast<void*>(v),ReversingProbe::Bytes(reinterpret_cast<void*>(v),11*sizeof(void*)));
    for(auto& s:snapshots){s.hook->LockState(false);s.hook->State(false);}
    bool passed=true; int pairs=0;
    const auto mode=[&](Simple* hook,bool candidate){
        hook->State(candidate);
        return hook->Hooked()==candidate && memcmp(hook->GetHookGTAAddress(),candidate?static_cast<void*>(&hook->m_HookContent):static_cast<void*>(hook->m_OriginalFunctionContent),hook->m_iHookedBytes)==0;
    };
    const auto ctor=[&]<class T>(const char* key,uintptr_t expectedTable,auto call,int seed){
        alignas(T) std::array<uint8,sizeof(T)> a,b;
        a.fill(uint8(seed*37+0x5A));b=a;
        auto* h=targets.at(key);const bool originalBytes=mode(h,false);
        auto* left=call(reinterpret_cast<T*>(a.data()),h->GetHookGTAAddress());
        const auto originalVptr=*reinterpret_cast<uintptr_t*>(a.data());
        const auto originalType=TaskType(left);const bool originalSimple=TaskSimple(left);const auto originalSub=ReadOwnedSubtask(left);
        const bool candidateBytes=mode(h,true);
        auto* right=call(reinterpret_cast<T*>(b.data()),h->GetHookGTAAddress());
        const auto candidateVptr=*reinterpret_cast<uintptr_t*>(b.data());
        const auto candidateType=TaskType(right);const bool candidateSimple=TaskSimple(right);const auto candidateSub=ReadOwnedSubtask(right);
        std::vector<size_t> differingSlots;
        const auto** av=reinterpret_cast<const void**>(originalVptr);const auto** bv=reinterpret_cast<const void**>(candidateVptr);
        for(size_t slot=0;slot<11;++slot)if(av[slot]!=bv[slot])differingSlots.push_back(slot);
        const bool payload=std::equal(a.begin()+4,a.end(),b.begin()+4);
        const size_t tableIndex=std::string_view(key).starts_with("CTaskComplexGangJoinRespond")?0:std::string_view(key).starts_with("CTaskComplexGoToAttractor")?1:2;
        const bool candidateTable=candidateVptr==reinterpret_cast<uintptr_t>(tables[tableIndex].first);
        const bool equal=originalBytes&&candidateBytes&&payload&&left==reinterpret_cast<T*>(a.data())&&right==reinterpret_cast<T*>(b.data())&&originalVptr==expectedTable&&candidateTable&&originalType==T::Type&&candidateType==T::Type&&!originalSimple&&!candidateSimple&&!originalSub&&!candidateSub;
        passed&=equal;++pairs;
        write({{"event","comparison"},{"function",key},{"seed",seed},{"equal",equal},{"raw_equal",a==b},{"payload_equal",payload},{"vptr_original",originalVptr},{"vptr_candidate",candidateVptr},{"different_vtable_slots",differingSlots},{"type_original",int(originalType)},{"type_candidate",int(candidateType)},{"original_bytes",originalBytes},{"candidate_bytes",candidateBytes},{"scope","constructor state and GetTaskType/IsSimple/GetSubTask; remaining vtable methods not certified"}});
        h->State(false);
        // These classes own no resources in the constructor fixtures. Raw storage is discarded.
    };
    const auto dtor=[&]<class T>(const char* key,auto prepare,int seed){
        alignas(T) std::array<uint8,sizeof(T)> a,b;a.fill(uint8(seed*37+0x5A));b=a;
        prepare(reinterpret_cast<T*>(a.data()));prepare(reinterpret_cast<T*>(b.data()));
        auto* h=targets.at(key);using Fn=void(__thiscall*)(T*);
        const bool originalBytes=mode(h,false);reinterpret_cast<Fn>(h->GetHookGTAAddress())(reinterpret_cast<T*>(a.data()));
        const bool candidateBytes=mode(h,true);reinterpret_cast<Fn>(h->GetHookGTAAddress())(reinterpret_cast<T*>(b.data()));
        const bool payload=std::equal(a.begin()+4,a.end(),b.begin()+4);
        const bool equal=originalBytes&&candidateBytes&&payload;
        passed&=equal;++pairs;
        write({{"event","comparison"},{"function",key},{"seed",seed},{"equal",equal},{"raw_equal",a==b},{"payload_equal",payload},{"vptr_original",*reinterpret_cast<uintptr_t*>(a.data())},{"vptr_candidate",*reinterpret_cast<uintptr_t*>(b.data())},{"original_bytes",originalBytes},{"candidate_bytes",candidateBytes},{"scope","null subtask; gang anim refs false; formation leader null; terminal vptr recorded but not certified"}});
        h->State(false);
    };
    for(int seed=0;seed<128;++seed){
        const CVector p{float(seed-64),float(seed)*0.125f,float(seed%11)};
        ctor.operator()<CTaskComplexGangJoinRespond>(required[0],0x86FB5C,[&](auto* t,void* address){using F=CTaskComplexGangJoinRespond*(__thiscall*)(CTaskComplexGangJoinRespond*,uint8);return reinterpret_cast<F>(address)(t,uint8(seed&1));},seed);
        ctor.operator()<CTaskComplexGoToAttractor>(required[2],0x86FF3C,[&](auto* t,void* address){using F=CTaskComplexGoToAttractor*(__thiscall*)(CTaskComplexGoToAttractor*,CPedAttractor*,const CVector&,float,float,int32,eMoveState);return reinterpret_cast<F>(address)(t,nullptr,p,float(seed)*0.1f,float(seed)*2.f,seed-64,seed&1?PEDMOVE_RUN:PEDMOVE_WALK);},seed);
        ctor.operator()<CTaskComplexSmartFleePoint>(required[4],0x86F744,[&](auto* t,void* address){using F=CTaskComplexSmartFleePoint*(__thiscall*)(CTaskComplexSmartFleePoint*,const CVector&,bool,float,int32);return reinterpret_cast<F>(address)(t,p,bool(seed&1),float(seed),seed%4==0?-1:seed*500);},seed);
        dtor.operator()<CTaskComplexGangJoinRespond>(required[1],[&](auto* t){::new(t) CTaskComplexGangJoinRespond{false};},seed);
        dtor.operator()<CTaskComplexGoToAttractor>(required[3],[&](auto* t){::new(t) CTaskComplexGoToAttractor{nullptr,p,0,0,0,PEDMOVE_WALK};},seed);
        dtor.operator()<CTaskComplexSmartFleePoint>(required[5],[&](auto* t){::new(t) CTaskComplexSmartFleePoint{p,false,10,-1};},seed);
        dtor.operator()<CTaskComplexFollowLeaderInFormation>(required[6],[&](auto* t){::new(t) CTaskComplexFollowLeaderInFormation{nullptr,nullptr,p,10};},seed);
        ReversingProbe::PumpMessages();
    }
    for(auto& s:snapshots){s.hook->LockState(false);s.hook->State(s.enabled);s.hook->LockState(s.locked);}
    bool flags=true,entries=true,candidates=true,vtables=true;
    for(const auto& s:snapshots){flags&=s.hook->Hooked()==s.enabled&&s.hook->Locked()==s.locked;if(s.gta){entries&=s.gtaBytes==ReversingProbe::Bytes(s.gta,52);candidates&=s.ourBytes==ReversingProbe::Bytes(s.ours,52);}}
    for(const auto& [address,bytes]:tables)vtables&=bytes==ReversingProbe::Bytes(address,bytes.size());
    const bool restored=flags&&entries&&candidates&&vtables;
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored},{"flags_restored",flags},{"entries_restored",entries},{"candidates_restored",candidates},{"tables_restored",vtables},{"scope","limited fixtures; no full-function validation claim"}});
}
}
