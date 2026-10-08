#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RunningScript.h"
#include "AnimManager.h"
#include "TaskSimpleHoldEntity.h"
#include "PedScriptedTaskRecord.h"
#include "TaskSequences.h"
#include "Pools/PedPool.h"
#include "Pools/ObjectPool.h"
#include "TaskSimpleAffectSecondaryBehaviour.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace ScriptPickup355Live {
inline json trace;inline uint32 scenario{},collects{},labels{},allocations{};
inline std::array<std::array<uint8,128>,2> tasks{};
inline void __fastcall Collect(CRunningScript* s,void*,int16 n){trace.push_back({"collect",n});s->m_IP+=n;if(collects++==0){ScriptParams[0].iParam=scenario%3==0?-1:0;ScriptParams[1].iParam=scenario%5==0?1:0;for(int j=2;j<5;++j)ScriptParams[j].fParam=float(int32(scenario%17)-8)*float(j)*.125f;ScriptParams[5].iParam=int32(scenario);ScriptParams[6].iParam=int32(scenario*13);}else ScriptParams[0].iParam=int32(scenario%2);}
inline void __fastcall Label(CRunningScript* s,void*,char* out,uint8 n){trace.push_back({"label",n});s->m_IP+=n;const bool nullName=(scenario%4==0&&labels==0)||(scenario%4==1&&labels==1);std::strcpy(out,nullName?"NULL":scenario%4==2?"null":"fixture");++labels;}
inline void* __cdecl New(size_t n){trace.push_back({"new",n});const auto index=allocations++;return scenario%7==0||scenario%7==1&&index==1?nullptr:tasks[index].data();}
inline CAnimBlock* __cdecl Block(const char* name){trace.push_back({"block",name});return reinterpret_cast<CAnimBlock*>(0xB5D4A0)+3;}
inline CAnimBlendHierarchy* __cdecl Animation(const char* name,const CAnimBlock* block){trace.push_back({"animation",name,uintptr_t(block)});return reinterpret_cast<CAnimBlendHierarchy*>(0x123400);}
inline void __cdecl Ref(int32 index){trace.push_back({"anim_ref",index});}
inline int32 TaskIndex(CTask* task){return task?int((uintptr_t(task)-uintptr_t(tasks.data()))/128):-1;}
inline void __fastcall EntityRef(CEntity* entity,void*,CEntity** ref){trace.push_back({"entity_ref",uintptr_t(entity),uintptr_t(ref)});}
inline void __fastcall Secondary(CTaskManager* mgr,void*,CTask* task,eSecondaryTask index){trace.push_back({"secondary",int32(index),TaskIndex(task)});}
struct ManagerAccess:CTaskManager{using CTaskManager::ChangeTaskInSlot;};
inline void __fastcall Change(CTaskManager* mgr,void*,CTask*& slot,CTask* task){trace.push_back({"secondary",int32(&slot-mgr->m_aSecondaryTasks.data()),TaskIndex(task)});}
inline bool __fastcall Sequence(CTaskComplexSequence*,void*,CTask* task){trace.push_back({"sequence",TaskIndex(task)});return true;}
inline int32 __cdecl Vacant(){trace.push_back({"vacant"});return int32(scenario%3);}
inline void __fastcall Record(CPedScriptedTaskRecordData* record,void*,CPed* ped,int32 opcode,CTask* task){trace.push_back({"record",uintptr_t(record),uintptr_t(ped),opcode,TaskIndex(task)});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CRunningScript","ScriptTaskPickUpObject",0x46AF50,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-pickup355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-pickup355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-pickup355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedPedPool=StaticRef<CPedPool*>(0xB74490);const auto savedObjectPool=StaticRef<CObjectPool*>(0xB7449C);const auto savedParams=ScriptParams;const auto savedSequence=StaticRef<int32>(0x8D2E98);
 EntryPatch a(0x464080,Collect),ad(bit_cast<uintptr_t>(&CRunningScript::CollectParameters),Collect);
 EntryPatch b(0x463D50,Label),bd(bit_cast<uintptr_t>(&CRunningScript::ReadTextLabelFromScript),Label);
 EntryPatch c(0x61A5A0,New),cd(bit_cast<uintptr_t>(&CTask::operator new),New);
 EntryPatch d(0x4D3940,Block),dd(bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),Block);
 EntryPatch e(0x4D42F0,Animation),ed(bit_cast<uintptr_t>(static_cast<CAnimBlendHierarchy*(*)(const char*,const CAnimBlock*)>(&CAnimManager::GetAnimation)),Animation);
 EntryPatch f(0x4D3FB0,Ref),fd(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Ref);
 EntryPatch g(0x571B70,EntityRef),gd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),EntityRef);
 EntryPatch h(0x681B60,Secondary),hd(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),Secondary),hc(bit_cast<uintptr_t>(&ManagerAccess::ChangeTaskInSlot),Change);
 EntryPatch j(0x632D10,Sequence),jd(bit_cast<uintptr_t>(static_cast<bool(CTaskComplexSequence::*)(CTask*)>(&CTaskComplexSequence::AddTask)),Sequence);
 EntryPatch k(0x608540,Vacant),kd(bit_cast<uintptr_t>(&CPedScriptedTaskRecord::GetVacantSlot),Vacant);
 EntryPatch l(0x6083E0,Record),ld(bit_cast<uintptr_t>(static_cast<void(CPedScriptedTaskRecordData::*)(CPed*,int32,CTask*)>(&CPedScriptedTaskRecordData::Set)),Record);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();collects=labels=allocations=0;
    alignas(CRunningScript) std::array<uint8,sizeof(CRunningScript)> raw{};auto* self=reinterpret_cast<CRunningScript*>(raw.data());self->m_IP=reinterpret_cast<uint8*>(0x1200);for(auto& task:tasks)task.fill(0xCD);
    alignas(16) std::array<uint8,0x7C4> ped{};alignas(16) std::array<uint8,0x19C> object{};alignas(16) std::array<uint8,0x300> intelligence{};alignas(16) std::array<uint8,32> pp{},op{};uint8 state=0;At<void*>(ped.data(),0x47C)=intelligence.data();At<void*>(pp.data(),0)=ped.data();At<void*>(pp.data(),4)=&state;At<int32>(pp.data(),8)=1;At<void*>(op.data(),0)=object.data();At<void*>(op.data(),4)=&state;At<int32>(op.data(),8)=1;StaticRef<CPedPool*>(0xB74490)=reinterpret_cast<CPedPool*>(pp.data());StaticRef<CObjectPool*>(0xB7449C)=reinterpret_cast<CObjectPool*>(op.data());ScriptParams.fill({});for(size_t j=0;j<8;++j)ScriptParams[j].iParam=int32(j+i);StaticRef<int32>(0x8D2E98)=3;
    const std::array<int32,2> commands{0x70A,0x605};const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__thiscall*)(CRunningScript*,int32)>(fn.address)(self,commands[(i/128)%2]);
    return json{{"script",Digest(raw.data(),raw.size())},{"tasks",Digest(tasks.data(),sizeof(tasks))},{"params",Digest(ScriptParams.data(),sizeof(ScriptParams))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<CPedPool*>(0xB74490)=savedPedPool;StaticRef<CObjectPool*>(0xB7449C)=savedObjectPool;ScriptParams=savedParams;StaticRef<int32>(0x8D2E98)=savedSequence;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
