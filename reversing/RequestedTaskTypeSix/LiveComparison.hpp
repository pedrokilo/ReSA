#pragma once
#include <fstream>
#include <new>
#include "TaskSimplePlayerOnFoot.h"
#include "TaskSimpleFightingControl.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskComplexKillCriminal.h"
#include "TaskComplexProstituteSolicit.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
#include <reversiblehooks/ReversibleHook/Virtual.h>
namespace RequestedTaskTypeSixLive {
using namespace ReversibleHooks::ReversibleHook;
inline bool done{};
inline int fixtureAnimCalls{},fixtureRefCalls{};inline bool fixtureContracts=true;
inline int32 __cdecl AnimIndex(const char*name){++fixtureAnimCalls;fixtureContracts&=name&&std::strcmp(name,"playidles")==0;return 3;}
inline void __fastcall RegisterRef(CEntity*self,void*,CEntity**slot){++fixtureRefCalls;fixtureContracts&=self&&slot&&*slot==self;}
__declspec(noinline) inline eTaskType Dispatch(const CTask*obj){return obj->GetTaskType();}
inline bool JumpEquals(const void*from,const void*to){std::array<uint8,5>expected{0xE9};const uint32 delta=uint32(reinterpret_cast<uintptr_t>(to)-reinterpret_cast<uintptr_t>(from)-5);std::memcpy(expected.data()+1,&delta,4);return !std::memcmp(from,expected.data(),5);}
struct Range{const void*address;std::vector<uint8>bytes;};
struct Spec{const char*cls;uintptr_t entry,table;int slots,type;};
inline const Spec specs[]{
 {"CTaskSimplePlayerOnFoot",0x6857C0,0x8708EC,9,0},
 {"CTaskSimpleFightingControl",0x61DC90,0x86D700,9,1019},
 {"CTaskComplexPlayHandSignalAnim",0x61B2E0,0x86D5DC,11,426},
 {"CTaskComplexKillPedOnFootArmed",0x621240,0x86D918,11,1002},
 {"CTaskComplexKillCriminal",0x68BF20,0x870A00,11,1105},
 {"CTaskComplexProstituteSolicit",0x661AE0,0x86FB88,11,1309}};
static_assert(CTaskSimplePlayerOnFoot::Type==0&&CTaskSimpleFightingControl::Type==1019&&CTaskComplexPlayHandSignalAnim::Type==426&&CTaskComplexKillPedOnFootArmed::Type==1002&&CTaskComplexKillCriminal::Type==1105&&CTaskComplexProstituteSolicit::Type==1309);
inline CTask* Construct(int which,void*storage,CPed*client,int seed){
 switch(which){
 case 0:return ::new(storage)CTaskSimplePlayerOnFoot{};
 case 1:return ::new(storage)CTaskSimpleFightingControl{nullptr,float(seed%7)*.25f,float(seed%13)*.5f};
 case 2:return ::new(storage)CTaskComplexPlayHandSignalAnim{ANIM_ID_UNDEFINED,float(seed%17)*.25f};
 case 3:return ::new(storage)CTaskComplexKillPedOnFootArmed{nullptr,uint32(seed%16),uint32(seed),uint32(seed%101),int8(seed%101)};
 case 4:return ::new(storage)CTaskComplexKillCriminal{nullptr,(seed&1)!=0};
 default:return ::new(storage)CTaskComplexProstituteSolicit{client};
 }
}
inline size_t ObjectSize(int which){constexpr size_t sizes[]{sizeof(CTaskSimplePlayerOnFoot),sizeof(CTaskSimpleFightingControl),sizeof(CTaskComplexPlayHandSignalAnim),sizeof(CTaskComplexKillPedOnFootArmed),sizeof(CTaskComplexKillCriminal),sizeof(CTaskComplexProstituteSolicit)};return sizes[which];}
inline void Update(){
 if(done)return;done=true;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);const auto dir=std::filesystem::path(executable).parent_path();if(!std::filesystem::exists(dir/"requested-task-type-six-runtime.start"))return;
 std::ofstream log(dir/"requested-task-type-six-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
 const auto flags=SmallPendingSupport::SnapshotHooks();std::vector<Range>entries,tables;std::array<Virtual*,6>targets{};
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){for(auto&item:cat.Items()){
  Base*h=item.get();void*a{};void*b{};
  if(h->Type()==Base::HookType::Simple){auto*s=static_cast<Simple*>(h);a=s->GetHookGTAAddress();b=s->GetHookOurAddress();}
  if(h->Type()==Base::HookType::Virtual){auto*v=static_cast<Virtual*>(h);a=v->GetHookGTAAddress();b=v->GetHookOurAddress();for(size_t i=0;i<6;++i)if(cat.Name()==specs[i].cls&&h->Name()=="GetTaskType"&&a==reinterpret_cast<void*>(specs[i].entry))targets[i]=v;}
  if(a)entries.push_back({a,ReversingProbe::Bytes(a,52)});if(b)entries.push_back({b,ReversingProbe::Bytes(b,52)});
 }});
 for(auto*h:targets)if(!h){write({{"event","error"},{"reason","missing exact runtime getter identity"}});return;}
 HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(targets[0]->GetHookOurAddress()),&module);
 std::array<void**,6>ours{};
 for(size_t i=0;i<6;++i){const std::string symbol="??_7"+std::string(specs[i].cls)+"@@6B@";ours[i]=reinterpret_cast<void**>(GetProcAddress(module,symbol.c_str()));if(!ours[i]){write({{"event","error"},{"reason","named DLL vtable export missing"},{"symbol",symbol}});return;}
  tables.push_back({reinterpret_cast<void*>(specs[i].table),ReversingProbe::Bytes(reinterpret_cast<void*>(specs[i].table),specs[i].slots*4)});tables.push_back({ours[i],ReversingProbe::Bytes(ours[i],specs[i].slots*4)});
 }
 const auto frameBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB4C),4);
 const std::array<uintptr_t,4>helpers{0x4D3990,reinterpret_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),0x571B70,std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference))};
 std::array<std::vector<uint8>,4>helperBefore;for(size_t i=0;i<4;++i)helperBefore[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[i]),5);
 bool passed=true;int pair{},failures{};
 {
  ReversingProbe::EntryPatch a0(helpers[0],AnimIndex),a1(helpers[1],AnimIndex),r0(helpers[2],RegisterRef),r1(helpers[3],RegisterRef);
  alignas(CPed)std::array<uint8,sizeof(CPed)>clientStorage{};CPed*client=reinterpret_cast<CPed*>(clientStorage.data());const auto clientBefore=clientStorage;
  for(int which=0;which<6;++which){auto*h=targets[which];const auto&spec=specs[which];h->LockState(false);h->State(true);const auto candidateBytes=ReversingProbe::Bytes(h->GetHookOurAddress(),5);
   std::array<uint8,6> originalCode{0xB8};std::memcpy(originalCode.data()+1,&spec.type,4);originalCode[5]=0xC3;const size_t codeSize=spec.type?6:3;if(!spec.type){originalCode[0]=0x33;originalCode[1]=0xC0;originalCode[2]=0xC3;}
   const auto ctorCallsBefore=fixtureAnimCalls,refCallsBefore=fixtureRefCalls,failuresBefore=failures;
   for(int seed=0;seed<256;++seed){
    alignas(16)std::array<uint8,256>left{},right{};left.fill(uint8(seed));right=left;
    CTask*a=Construct(which,left.data(),client,seed);CTask*b=Construct(which,right.data(),client,seed);
    *reinterpret_cast<void***>(a)=reinterpret_cast<void**>(spec.table);*reinterpret_cast<void***>(b)=ours[which];const auto ab=ReversingProbe::Bytes(a,ObjectSize(which)),bb=ReversingProbe::Bytes(b,ObjectSize(which));
    for(int route=0;route<5;++route){using Fn=eTaskType(__thiscall*)(const CTask*);auto call=[&](const CTask*obj){if(route==0)return reinterpret_cast<Fn>(h->GetHookGTAAddress())(obj);if(route==1)return reinterpret_cast<Fn>(h->GetHookOurAddress())(obj);if(route==2)return reinterpret_cast<Fn>(reinterpret_cast<void**>(spec.table)[4])(obj);if(route==3)return reinterpret_cast<Fn>(ours[which][4])(obj);return Dispatch(obj);};
     h->State(false);const bool ogate=!std::memcmp(h->GetHookGTAAddress(),originalCode.data(),codeSize)&&JumpEquals(h->GetHookOurAddress(),h->GetHookGTAAddress())&&reinterpret_cast<void**>(spec.table)[4]==h->GetHookGTAAddress()&&ours[which][4]==h->GetHookGTAAddress();const int32 original=int32(call(a));
     h->State(true);const bool cgate=!std::memcmp(h->GetHookOurAddress(),candidateBytes.data(),5)&&JumpEquals(h->GetHookGTAAddress(),h->GetHookOurAddress())&&reinterpret_cast<void**>(spec.table)[4]==h->GetHookOurAddress()&&ours[which][4]==h->GetHookOurAddress();const int32 candidate=int32(call(b));
     const bool objects=ab==ReversingProbe::Bytes(a,ObjectSize(which))&&bb==ReversingProbe::Bytes(b,ObjectSize(which));const bool equal=ogate&&cgate&&objects&&original==spec.type&&candidate==spec.type;
     passed&=equal;failures+=!equal;write({{"event","comparison"},{"function",std::string(spec.cls)+"::GetTaskType"},{"pair",pair++},{"seed",seed},{"route",route},{"expected",spec.type},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal},{"original_gate",ogate},{"candidate_gate",cgate},{"objects_unchanged",objects}});
    }
    // Restore actual named DLL vptr before deliberately ending manual-storage fixtures without class cleanup.
    *reinterpret_cast<void***>(a)=ours[which];*reinterpret_cast<void***>(b)=ours[which];
   }
   const bool ctorTrace=fixtureAnimCalls-ctorCallsBefore==(which==0?512:0)&&fixtureRefCalls-refCallsBefore==(which==5?512:0)&&fixtureContracts;
   passed&=ctorTrace;write({{"event","function_summary"},{"function",std::string(spec.cls)+"::GetTaskType"},{"cases",1280},{"failures",failures-failuresBefore+(ctorTrace?0:1)},{"fixture_contract",ctorTrace},{"scope","Getter only; source placement constructors are fixtures, manual arenas release without class destructor/resource integration"}});
  }
  passed&=clientStorage==clientBefore;
 }
 for(const auto&f:flags)if(std::find(targets.begin(),targets.end(),f.hook)!=targets.end()){f.hook->LockState(false);f.hook->State(f.enabled);f.hook->LockState(f.locked);}
 bool entriesRestored=true,tablesRestored=true,helpersRestored=true;for(const auto&r:entries)entriesRestored&=r.bytes==ReversingProbe::Bytes(r.address,r.bytes.size());for(const auto&r:tables)tablesRestored&=r.bytes==ReversingProbe::Bytes(r.address,r.bytes.size());for(size_t i=0;i<4;++i)helpersRestored&=helperBefore[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[i]),5);
 const bool frameUnchanged=frameBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB4C),4),restored=entriesRestored&&tablesRestored&&helpersRestored&&frameUnchanged&&SmallPendingSupport::HooksUnchanged(flags);
 write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"failures",failures},{"state_restored",restored},{"entries_restored",entriesRestored},{"tables_restored",tablesRestored},{"helpers_restored",helpersRestored},{"frame_unchanged",frameUnchanged},{"fixture_contracts",fixtureContracts}});
}
}
