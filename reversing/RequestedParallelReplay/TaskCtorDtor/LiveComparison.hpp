#pragma once
#include "TaskComplexKillPedFromBoat.h"
#include "TaskComplexHitPedWithCar.h"
#include "TaskComplexSunbathe.h"
#include "AnimManager.h"
#include "../../ProbeSupport.hpp"
#include "../../RequestedParallelSmall/HookSnapshots.hpp"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include <fstream>
#include <set>

namespace RequestedTaskCtorDtorLive {
using namespace ReversibleHooks::ReversibleHook;
inline json trace;inline uintptr_t taskAddress,entityAddress,subAddress;
inline int objectKind;
alignas(16) inline std::array<uint8,0x800> entity;
alignas(16) inline std::array<uint8,0x100> sub;
inline std::array<void*,7> subTable;
inline std::array<CAnimBlock,2> blocks;
inline void __fastcall Register(CEntity* self,void*,CEntity** field) {
    trace.push_back({"register",reinterpret_cast<uintptr_t>(self)==entityAddress,
        reinterpret_cast<uintptr_t>(field)-taskAddress,*field==self});
}
inline void __fastcall Cleanup(CEntity* self,void*,CEntity** field) {
    trace.push_back({"cleanup",reinterpret_cast<uintptr_t>(self)==entityAddress,
        reinterpret_cast<uintptr_t>(field)-taskAddress,*field==self,
        objectKind==2?int(entity[0x13C]):-1});
}
inline CAnimBlock* GetBlock(const char* name) {
    trace.push_back({"block",name});return &blocks[strcmp(name,"beach")!=0];
}
inline int32 GetIndex(const char* name) {trace.push_back({"index",name});return strcmp(name,"beach")?5:3;}
inline void RemoveRef(int32 index) {trace.push_back({"remove_ref",index});}
inline void* __fastcall DeleteSub(void* self,void*,uint32 flags) {
    trace.push_back({"delete_sub",reinterpret_cast<uintptr_t>(self)==subAddress,flags});return self;
}
struct Spec {const char* cls;uintptr_t ctor,dtor,vt;size_t size;};
inline constexpr Spec specs[]{
    {"CTaskComplexKillPedFromBoat",0x6227C0,0x622830,0x86DA98,sizeof(CTaskComplexKillPedFromBoat)},
    {"CTaskComplexHitPedWithCar",0x6539A0,0x653A30,0x86F294,sizeof(CTaskComplexHitPedWithCar)},
    {"CTaskComplexSunbathe",0x631F80,0x632050,0x86E0AC,sizeof(CTaskComplexSunbathe)}};
inline Simple* Find(const char* cls,const char* name) {
    Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==cls)for(auto& h:cat.Items())if(h->Name()==name)result=h.get();});
    return result&&result->Type()==Base::HookType::Simple?static_cast<Simple*>(result):nullptr;
}
inline uintptr_t Body(uintptr_t at) {for(int i=0;i<8&&*reinterpret_cast<uint8*>(at)==0xE9;++i)at=at+5+*reinterpret_cast<int32*>(at+1);return at;}
struct Range {const void* ptr;std::vector<uint8> data;};
inline std::string VptrName(uintptr_t vt,const Spec& fn) {
    if(vt==fn.vt||vt==reinterpret_cast<uintptr_t>(ReversibleHooks::detail::GetVTableAddress(fn.cls)))return "derived";
    if(vt==0x86D48C||vt==reinterpret_cast<uintptr_t>(ReversibleHooks::detail::GetVTableAddress("CTask")))return "CTask";
    if(vt==0x86D4CC||vt==reinterpret_cast<uintptr_t>(ReversibleHooks::detail::GetVTableAddress("CTaskComplex")))return "CTaskComplex";
    return "unknown:"+std::to_string(vt);
}
inline void Update() {
    static bool done{};if(done)return;done=true;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"requested-task-ctor-dtor.start"))return;
    std::ofstream log(dir/"requested-task-ctor-dtor.jsonl");const auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    const auto flags=SmallPendingSupport::SnapshotHooks();std::vector<Range> ranges;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items()){
        const void* a{};const void* b{};
        if(h->Type()==Base::HookType::Simple){auto* s=static_cast<Simple*>(h.get());a=s->GetHookGTAAddress();b=s->GetHookOurAddress();}
        else if(h->Type()==Base::HookType::Virtual){auto* v=static_cast<Virtual*>(h.get());a=v->GetHookGTAAddress();b=v->GetHookOurAddress();}
        if(a)ranges.push_back({a,ReversingProbe::Bytes(a,52)});if(b)ranges.push_back({b,ReversingProbe::Bytes(b,52)});
    }});
    for(const auto& fn:specs){auto* c=Find(fn.cls,"Constructor");auto* d=Find(fn.cls,"Destructor");if(!c||!d){write({{"event","error"},{"reason","missing target"},{"class",fn.cls}});return;}
        ranges.push_back({reinterpret_cast<void*>(fn.vt),ReversingProbe::Bytes(reinterpret_cast<void*>(fn.vt),11*sizeof(void*))});
        auto* table=ReversibleHooks::detail::GetVTableAddress(fn.cls);ranges.push_back({table,ReversingProbe::Bytes(table,11*sizeof(void*))});
    }
    using RefFn=void(CEntity::*)(CEntity**);
    using BlockFn=CAnimBlock*(*)(const char*);using IndexFn=int32(*)(const char*);
    std::vector<std::pair<uintptr_t,void*>> deps{
        {0x571B70,reinterpret_cast<void*>(&Register)},
        {bit_cast<uintptr_t>(static_cast<RefFn>(&CEntity::RegisterReference)),reinterpret_cast<void*>(&Register)},
        {0x571A00,reinterpret_cast<void*>(&Cleanup)},
        {bit_cast<uintptr_t>(static_cast<RefFn>(&CEntity::CleanUpOldReference)),reinterpret_cast<void*>(&Cleanup)},
        {0x4D3940,reinterpret_cast<void*>(&GetBlock)},
        {bit_cast<uintptr_t>(static_cast<BlockFn>(&CAnimManager::GetAnimationBlock)),reinterpret_cast<void*>(&GetBlock)},
        {0x4D3990,reinterpret_cast<void*>(&GetIndex)},
        {bit_cast<uintptr_t>(static_cast<IndexFn>(&CAnimManager::GetAnimationBlockIndex)),reinterpret_cast<void*>(&GetIndex)},
        {0x4D3FD0,reinterpret_cast<void*>(&RemoveRef)},
        {bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),reinterpret_cast<void*>(&RemoveRef)}};
    const auto originalDeps=deps;for(const auto [at,to]:originalDeps)deps.emplace_back(Body(at),to);
    std::set<uintptr_t> seen;std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
    for(const auto [at,to]:deps)if(seen.insert(at).second){ranges.push_back({reinterpret_cast<void*>(at),ReversingProbe::Bytes(reinterpret_cast<void*>(at),5)});patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,to));}
    bool passed=true;int pairs{};subTable.fill(nullptr);subTable[0]=reinterpret_cast<void*>(&DeleteSub);
    for(int k=0;k<3;++k)for(int destructor=0;destructor<2;++destructor){
        const auto& fn=specs[k];auto* target=Find(fn.cls,destructor?"Destructor":"Constructor");target->LockState(false);
        int failures{};
        for(int seed=0;seed<256;++seed){
            const auto run=[&](bool candidate){
                alignas(16) std::array<uint8,0x80> storage;storage.fill(uint8(seed));
                entity.fill(uint8(seed));sub.fill(0);*reinterpret_cast<void***>(sub.data())=subTable.data();
                taskAddress=reinterpret_cast<uintptr_t>(storage.data());entityAddress=reinterpret_cast<uintptr_t>(entity.data());subAddress=reinterpret_cast<uintptr_t>(sub.data());objectKind=k;trace=json::array();
                auto* task=storage.data();auto* ent=(seed&1)?entity.data():nullptr;
                if(destructor){
                    if(k==2)entity[0x13C]=5; // A valid towel owned by a live Sunbathe task.
                    *reinterpret_cast<uintptr_t*>(task)=candidate?reinterpret_cast<uintptr_t>(ReversibleHooks::detail::GetVTableAddress(fn.cls)):fn.vt;
                    *reinterpret_cast<uintptr_t*>(task+4)=0;*reinterpret_cast<void**>(task+8)=(seed&2)?sub.data():nullptr;
                    *reinterpret_cast<void**>(task+(k==2?0x34:0xC))=ent;
                    if(k==2){task[0xE]=uint8((seed>>2)&1);task[0xF]=uint8((seed>>3)&1);*reinterpret_cast<int32*>(task+0x2C)=3;*reinterpret_cast<int32*>(task+0x30)=5;}
                }
                target->State(candidate);const bool switched=SmallPendingSupport::ExactSwitch(*target,candidate);
                bool returnOk=true;
                if(destructor)reinterpret_cast<void(__thiscall*)(void*)>(target->GetHookGTAAddress())(task);
                else {
                    void* result{};
                    if(k==0)result=reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target->GetHookGTAAddress())(task,reinterpret_cast<CPed*>(ent));
                    if(k==1)result=reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,float)>(target->GetHookGTAAddress())(task,reinterpret_cast<CVehicle*>(ent),float(seed-128)*0.125f);
                    if(k==2)result=reinterpret_cast<void*(__thiscall*)(void*,CObject*,bool)>(target->GetHookGTAAddress())(task,reinterpret_cast<CObject*>(ent),(seed&2)!=0);
                    returnOk=result==task;
                }
                const auto vt=VptrName(*reinterpret_cast<uintptr_t*>(task),fn);
                std::vector<uint8> payload(task+4,task+fn.size);
                return json{{"switched",switched},{"return_ok",returnOk},{"vptr",vt},{"payload",payload},{"entity",entity},{"trace",trace}};
            };
            const auto original=run(false),candidate=run(true);
            const bool equal=original==candidate&&original["switched"]==true&&original["return_ok"]==true && !original["vptr"].get<std::string>().starts_with("unknown:");
            passed&=equal;failures+=!equal;
            write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+(destructor?"Destructor":"Constructor")},{"pair",pairs++},{"seed",seed},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            ReversingProbe::PumpMessages();
        }
        write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+(destructor?"Destructor":"Constructor")},{"cases",256},{"failures",failures}});
    }
    patches.clear(); // Restore collaborators before target hooks and terminal evidence.
    for(const auto& f:flags){f.hook->LockState(false);f.hook->State(f.enabled);f.hook->LockState(f.locked);}
    bool bytes=true;json failed=json::array();for(const auto& r:ranges)if(r.data!=ReversingProbe::Bytes(r.ptr,r.data.size())){bytes=false;failed.push_back(reinterpret_cast<uintptr_t>(r.ptr));}
    const bool restored=bytes&&SmallPendingSupport::HooksUnchanged(flags);
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored},{"bytes_restored",bytes},{"failed_ranges",failed}});
}
}
