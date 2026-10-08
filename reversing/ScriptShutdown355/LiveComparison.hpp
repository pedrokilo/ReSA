#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Scripts/RunningScript.h"
#include "StreamedScripts.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "Pools/PedPool.h"
#include "Pools/ObjectPool.h"
#include "../ProbeRuntime.hpp"
namespace ScriptShutdown355Live {
inline json trace;inline uint32 scenario{},allocations{};alignas(16) inline std::array<uint8,8> task{};
inline uint32 __fastcall Index(CStreamedScripts*,void*,uint8* ip){trace.push_back({"index",uintptr_t(ip)});return scenario%4;}
inline void* __cdecl NewTask(size_t size){++allocations;return scenario%3==0?nullptr:task.data();}
inline CTask* __cdecl SetTask(CPed* p,CTask* t){trace.push_back({"set_task",uintptr_t(p),t!=nullptr,t?*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(t)+4):0});return nullptr;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CRunningScript","ShutdownThisScript",0x465AA0,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-shutdown355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-shutdown355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-shutdown355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 auto& pedPool=*reinterpret_cast<CPedPool**>(0xB74490);auto* savedPedPool=pedPool;auto& objectPool=*reinterpret_cast<CObjectPool**>(0xB7449C);auto* savedObjectPool=objectPool;
 std::array<uint8,128> savedStream{};memcpy(savedStream.data(),reinterpret_cast<void*>(0xA47B60),128);const auto savedLocal=*reinterpret_cast<int32*>(0xA48960);
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x470910,Index),ad(bit_cast<uintptr_t>(&CStreamedScripts::GetStreamedScriptWithThisStartAddress),Index);
 EntryPatch b(0x635720,SetTask),bd(bit_cast<uintptr_t>(&CScriptedBrainTaskStore::SetTask),SetTask);
 EntryPatch c(0x61A5A0,NewTask),cd(bit_cast<uintptr_t>(&CTask::operator new),NewTask);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CRunningScript) std::array<uint8,sizeof(CRunningScript)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CRunningScript*>(raw.data());
    alignas(16) std::array<uint8,0x7C4*4> peds{};alignas(16) std::array<uint8,0x19C*4> objects{};
    std::array<uint8,4> pedStates{},objectStates{};alignas(16) std::array<uint8,sizeof(CPedPool)> fakePedPool{};alignas(16) std::array<uint8,sizeof(CObjectPool)> fakeObjectPool{};
    At<void*>(fakePedPool.data(),0)=peds.data();At<void*>(fakePedPool.data(),4)=pedStates.data();At<int32>(fakePedPool.data(),8)=4;At<void*>(fakeObjectPool.data(),0)=objects.data();At<void*>(fakeObjectPool.data(),4)=objectStates.data();At<int32>(fakeObjectPool.data(),8)=4;
    for(size_t k=0;k<4;++k){pedStates[k]=uint8(i+k);objectStates[k]=uint8(i+k);At<uint32>(peds.data()+0x7C4*k,0x474)=0xFFFFFFFF;At<uint32>(objects.data()+0x19C*k,0x140)=0xFFFFFFFF;}
    pedPool=reinterpret_cast<CPedPool*>(fakePedPool.data());objectPool=reinterpret_cast<CObjectPool*>(fakeObjectPool.data());
    self->m_IsActive=true;self->m_IsExternal=(i/4)%2!=0;self->m_ExternalType=int8(i%8);self->m_ThisMustBeTheOnlyMissionRunning=(i/8)%2!=0;self->m_BaseIP=reinterpret_cast<uint8*>(0x123450);
    const auto index=(i/16)%4;const int32 reference=index*256+int32(uint8(i+index+(i%3==0?1:0)));self->m_LocalVars[0].iParam=reference;*reinterpret_cast<int32*>(0xA48960)=reference;
    memset(reinterpret_cast<void*>(0xA47B60),uint8(i),128);scenario=uint32(i);trace=json::array();allocations=0;task.fill(0xCD);
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CRunningScript*)>(fn.address)(self);
    return json{{"bytes",Digest(raw.data(),raw.size())},{"peds",Digest(peds.data(),peds.size())},{"objects",Digest(objects.data(),objects.size())},{"stream",Digest(reinterpret_cast<void*>(0xA47B60),128)},{"allocations",allocations},{"task_parent",Digest(task.data()+4,4)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 pedPool=savedPedPool;objectPool=savedObjectPool;memcpy(reinterpret_cast<void*>(0xA47B60),savedStream.data(),128);*reinterpret_cast<int32*>(0xA48960)=savedLocal;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
