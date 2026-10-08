#pragma once
#include "TaskComplexArrestPed.h"
#include "../ProbeRuntime.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
namespace RequestedArrestRemainingFour {
using namespace ReversibleHooks::ReversibleHook;
inline json trace;
alignas(16) inline std::array<uint8, sizeof(CTaskComplexArrestPed)> task;
alignas(16) inline std::array<uint8, sizeof(CPed)> ped, target;
alignas(16) inline std::array<uint8, sizeof(CAutomobile)> vehicle;
alignas(16) inline std::array<uint8, 64> child, eventArena, output;
inline std::array<void*, 11> childVT{};
inline bool melee{}, shotgun{}, pistol{}, upside{}, side{}, abortResult{}, returnNull{};
template<class T> inline void Put(void* p, size_t off, T v) { std::memcpy(static_cast<uint8*>(p)+off,&v,sizeof(v)); }
inline json Pointer(uintptr_t p) {
    if (!p) return nullptr;
#define ARENA(a) if(p>=uintptr_t(a.data())&&p<uintptr_t(a.data())+a.size())return json::array({#a,p-uintptr_t(a.data())});
    ARENA(task) ARENA(ped) ARENA(target) ARENA(vehicle) ARENA(child) ARENA(eventArena) ARENA(output)
#undef ARENA
    return p;
}
inline void __fastcall Ref(CEntity* entity, void*, CEntity** slot) { trace.push_back({"reference",Pointer(uintptr_t(entity)),Pointer(uintptr_t(slot))}); }
inline void __fastcall Clean(CEntity* entity, void*, CEntity** slot) { trace.push_back({"cleanup",Pointer(uintptr_t(entity)),Pointer(uintptr_t(slot))}); }
inline void* __fastcall Delete(void* object, void*, unsigned flags) { trace.push_back({"delete",Pointer(uintptr_t(object)),flags}); return object; }
inline bool __fastcall Abort(CTask* object, void*, CPed* p, eAbortPriority priority, const CEvent* event) {
    trace.push_back({"abort",Pointer(uintptr_t(object)),Pointer(uintptr_t(p)),int(priority),Pointer(uintptr_t(event)),abortResult});
    return abortResult;
}
inline CTask* __fastcall Sub(CTaskComplexArrestPed* object, void*, eTaskType type, CPed* p) {
    trace.push_back({"create_sub",Pointer(uintptr_t(object)),int(type),Pointer(uintptr_t(p)),returnNull});
    return returnNull?nullptr:reinterpret_cast<CTask*>(output.data());
}
inline bool __fastcall Upside(const CVehicle* v,void*) {trace.push_back({"upside",Pointer(uintptr_t(v)),upside});return upside;}
inline bool __fastcall Side(const CVehicle* v,void*) {trace.push_back({"side",Pointer(uintptr_t(v)),side});return side;}
inline bool __fastcall Melee(CWeapon* w,void*) {trace.push_back({"melee",Pointer(uintptr_t(w)),melee});return melee;}
inline bool __fastcall Has(CPed* p,void*,eWeaponType type) {const bool value=type==WEAPON_SHOTGUN?shotgun:pistol;trace.push_back({"has",Pointer(uintptr_t(p)),int(type),value});return value;}
inline void __fastcall Set(CPed* p,void*,eWeaponType type) {trace.push_back({"set",Pointer(uintptr_t(p)),int(type)});}
inline eWeaponSlot __fastcall Give(CPed* p,void*,eWeaponType type,uint32 ammo,bool equip) {trace.push_back({"give",Pointer(uintptr_t(p)),int(type),ammo,equip});return eWeaponSlot(0);}
inline Base* Find(const char* category,const char* name) {Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==category)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline std::pair<uintptr_t,uintptr_t> Entries(Base* h) {
    if(h->Type()==Base::HookType::Simple){auto* p=static_cast<Simple*>(h);return {uintptr_t(p->GetHookGTAAddress()),uintptr_t(p->GetHookOurAddress())};}
    auto* p=static_cast<Virtual*>(h);return {uintptr_t(p->GetHookGTAAddress()),uintptr_t(p->GetHookOurAddress())};
}
struct Gate {
    uintptr_t native,candidate;std::vector<uint8> beforeNative,beforeCandidate;std::unique_ptr<ReversingProbe::EntryPatch> a,b;
    template<class F> Gate(uintptr_t address,F replacement):native(address),candidate(0),beforeNative(ReversingProbe::Bytes(reinterpret_cast<void*>(address),52)) {
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items()){auto e=Entries(h.get());if(e.first==address)candidate=e.second;}});
        if(candidate)beforeCandidate=ReversingProbe::Bytes(reinterpret_cast<void*>(candidate),52);
        a=std::make_unique<ReversingProbe::EntryPatch>(native,replacement);
        if(candidate&&candidate!=native)b=std::make_unique<ReversingProbe::EntryPatch>(candidate,replacement);
    }
    bool Installed()const{return ReversingProbe::Bytes(reinterpret_cast<void*>(native),5)==std::vector<uint8>(a->jump.begin(),a->jump.end())&&(!b||ReversingProbe::Bytes(reinterpret_cast<void*>(candidate),5)==std::vector<uint8>(b->jump.begin(),b->jump.end()));}
    bool Restore(){b.reset();a.reset();return beforeNative==ReversingProbe::Bytes(reinterpret_cast<void*>(native),52)&&(!candidate||beforeCandidate==ReversingProbe::Bytes(reinterpret_cast<void*>(candidate),52));}
};
inline void Update() {
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
    std::ofstream log(dir/"arrest-remaining-four.jsonl");auto write=[&](const json& j){log<<j.dump()<<'\n';log.flush();};
    struct Function{const char* name;uintptr_t native;int count;};
    const Function functions[]{{"Constructor",0x68B990,128},{"Destructor",0x68BA00,256},{"MakeAbortable",0x68BA60,384},{"CreateFirstSubTask",0x6907A0,2048}};
    std::vector<std::unique_ptr<Gate>> gates;
    auto gate=[&](uintptr_t address,auto fn){gates.push_back(std::make_unique<Gate>(address,fn));};
    gate(0x571B70,Ref);gate(0x571A00,Clean);gate(0x68CF80,Sub);gate(0x6D1D90,Upside);gate(0x6D1DD0,Side);
    gate(0x73B1C0,Melee);gate(0x5DF300,Has);gate(0x5E6280,Set);gate(0x5E6080,Give);
    
    bool helpersKnown=true;for(const auto& g:gates)helpersKnown&=g->candidate!=0;
    if(!helpersKnown){bool restored=true;for(auto& g:gates)restored&=g->Restore();write({{"event","error"},{"reason","candidate helper body unresolved"}});write({{"event","complete"},{"comparisons",0},{"differences",0},{"all_equal",false},{"state_restored",restored},{"fp_restored",true}});return;}
    const auto savedFP=unsigned(_controlfp(0,0));const auto oldTrace=trace;const auto oldChildVT=childVT;const std::array<bool,7> oldControls{melee,shotgun,pistol,upside,side,abortResult,returnNull};const auto oldTask=task;const auto oldPed=ped;const auto oldTarget=target;const auto oldVehicle=vehicle;const auto oldChild=child;const auto oldEvent=eventArena;const auto oldOutput=output;
    childVT.fill(nullptr);childVT[0]=reinterpret_cast<void*>(Delete);childVT[6]=reinterpret_cast<void*>(Abort);
    const uintptr_t dllTaskVT=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CTask"));
    const uintptr_t dllArrestVT=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CTaskComplexArrestPed"));
    write({{"event","begin"},{"native_arrest_table",0x8709A8},{"candidate_arrest_table",dllArrestVT},{"native_task_table",0x86D48C},{"candidate_task_table",dllTaskVT},{"arrest_table_native",ReversingProbe::Bytes(reinterpret_cast<void*>(0x8709A8),44)},{"arrest_table_candidate",ReversingProbe::Bytes(reinterpret_cast<void*>(dllArrestVT),44)}});
    bool all=true;size_t total=0,differences=0;
    for(size_t functionIndex=0;functionIndex<std::size(functions);++functionIndex){const auto& f=functions[functionIndex];auto* hook=Find("CTaskComplexArrestPed",f.name);
        if(!hook){all=false;write({{"event","error"},{"reason","missing target"},{"function",f.name}});continue;}
        const auto [native,candidate]=Entries(hook);if(native!=f.native){all=false;write({{"event","error"},{"reason","wrong target address"},{"function",f.name}});continue;}
        const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);hook->State(false);
        for(int i=0;i<f.count;++i){json results[2];const uint8 poison=uint8(functionIndex<2?(i%64)*4:functionIndex==2?(i%16)*17:(i%4)*85);const bool ref=functionIndex<2?(i/64)%2:(i/4)%2,hasChild=functionIndex==2||((i/128)%2);
            const int scenario=(i/4)%8, controls=(i/32)%16;json fixture{{"index",i},{"poison",poison},{"reference",ref},{"child",hasChild},{"scenario",scenario},{"controls",controls}};
            for(int leg=0;leg<2;++leg){task.fill(poison);ped.fill(0);target.fill(0);vehicle.fill(0);child.fill(poison);eventArena.fill(poison);output.fill(poison);
                Put(child.data(),0,uintptr_t(childVT.data()));Put(task.data(),0,leg?dllArrestVT:uintptr_t(0x8709A8));Put(task.data(),4,uintptr_t(output.data()));Put(task.data(),8,hasChild?uintptr_t(child.data()):uintptr_t(0));Put(task.data(),0x10,ref?uintptr_t(target.data()):uintptr_t(0));Put(task.data(),0x20,uintptr_t(vehicle.data()));
                Put(ped.data(),0x718,int8((i/512)%4));Put(target.data(),0x58C,uintptr_t(vehicle.data()));Put(target.data(),0x46D,uint8(scenario>=2?1:0));
                Put(vehicle.data(),0x590,int32(scenario==2?9:0));Put(vehicle.data(),0x594,int32(scenario==3?2:scenario==4?5:0));
                if(functionIndex==3)Put(task.data(),0x10,scenario?uintptr_t(target.data()):uintptr_t(0));
                melee=controls&1;shotgun=controls&2;pistol=controls&4;returnNull=controls&8;upside=scenario==6;side=scenario==7;abortResult=(i/96)%2;
                const auto taskBefore=task;const auto pBefore=ped;const auto tBefore=target;const auto vBefore=vehicle;const auto cBefore=child;const auto eBefore=eventArena;const auto oBefore=output;
                trace=json::array();uintptr_t returned=0;_controlfp(savedFP,_MCW_PC|_MCW_RC);hook->State(leg!=0);
                if(functionIndex==0)returned=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(native)(task.data(),ref?reinterpret_cast<CPed*>(target.data()):nullptr));
                else if(functionIndex==1)reinterpret_cast<void(__thiscall*)(void*)>(native)(task.data());
                else if(functionIndex==2)returned=reinterpret_cast<bool(__thiscall*)(void*,CPed*,eAbortPriority,const CEvent*)>(native)(task.data(),((i/192)%2)?reinterpret_cast<CPed*>(ped.data()):nullptr,eAbortPriority((i/16)%3),((i/48)%2)?reinterpret_cast<CEvent*>(eventArena.data()):nullptr);
                else returned=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(native)(task.data(),reinterpret_cast<CPed*>(ped.data())));
                bool gatesOkay=true;for(const auto& g:gates)gatesOkay&=g->Installed();
                const uintptr_t expectedVT=functionIndex==1?(leg?dllTaskVT:uintptr_t(0x86D48C)):(leg?dllArrestVT:uintptr_t(0x8709A8));
                const bool role=*reinterpret_cast<const uintptr_t*>(task.data())==expectedVT;
                const bool targetGate=hook->Hooked()==bool(leg)&&ReversingProbe::JumpsToDll(native)==bool(leg);
                // Raw vptr is retained and checked against the exact native/DLL role; compare the remaining bytes separately.
                json payload{{"task",std::vector<uint8>(task.begin()+4,task.end())},{"ped",ped},{"target",target},{"vehicle",vehicle},{"child",child},{"event",eventArena},{"output",output},{"trace",trace},{"returned",functionIndex==1?json(nullptr):Pointer(returned)}};
                const bool privateImmutable=pBefore==ped&&tBefore==target&&vBefore==vehicle&&cBefore==child&&eBefore==eventArena&&oBefore==output;
                results[leg]={{"payload",payload},{"raw_vptr",*reinterpret_cast<const uintptr_t*>(task.data())},{"expected_vptr",expectedVT},{"vptr_role_exact",role},{"helper_gates",gatesOkay},{"target_gate",targetGate},{"entry_native",ReversingProbe::Bytes(reinterpret_cast<void*>(native),52)},{"entry_candidate",ReversingProbe::Bytes(reinterpret_cast<void*>(candidate),52)},{"private_immutable",privateImmutable},{"task_before",taskBefore},{"task_after",task}};
            }
            const bool equal=results[0]["payload"]==results[1]["payload"]&&results[0]["vptr_role_exact"]==true&&results[1]["vptr_role_exact"]==true&&results[0]["helper_gates"]==true&&results[1]["helper_gates"]==true&&results[0]["private_immutable"]==true&&results[1]["private_immutable"]==true&&results[0]["target_gate"]==true&&results[1]["target_gate"]==true;
            all&=equal;differences+=!equal;write({{"event","comparison"},{"function",std::string("CTaskComplexArrestPed::")+f.name},{"fixture",fixture},{"pair",total++},{"equal",equal},{"original",results[0]},{"candidate",results[1]}});
        }
        hook->State(enabled);hook->LockState(locked);
    }
    bool restored=true;for(auto& g:gates)restored&=g->Restore();_controlfp(savedFP,_MCW_PC|_MCW_RC);
    task=oldTask;ped=oldPed;target=oldTarget;vehicle=oldVehicle;child=oldChild;eventArena=oldEvent;output=oldOutput;childVT=oldChildVT;trace=oldTrace;melee=oldControls[0];shotgun=oldControls[1];pistol=oldControls[2];upside=oldControls[3];side=oldControls[4];abortResult=oldControls[5];returnNull=oldControls[6];
    restored&=task==oldTask&&ped==oldPed&&target==oldTarget&&vehicle==oldVehicle&&child==oldChild&&eventArena==oldEvent&&output==oldOutput&&childVT==oldChildVT&&trace==oldTrace;
    write({{"event","complete"},{"comparisons",total},{"differences",differences},{"all_equal",all},{"state_restored",restored},{"fp_restored",unsigned(_controlfp(0,0))==savedFP}});
}
}
