#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RunningScript.h"
#include "AnimManager.h"
#include "TaskSimpleRunNamedAnim.h"
#include "TaskSimpleAffectSecondaryBehaviour.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace ScriptAnim355Live {
inline json trace;inline uint32 scenario{},collects{},labels{},allocations{};
inline std::array<std::array<uint8,128>,2> tasks{};
inline void __fastcall Collect(CRunningScript* s,void*,int16 count){trace.push_back({"collect",count});s->m_IP+=count;if(collects++==0){ScriptParams[0].iParam=int32(scenario%7)-1;return;}ScriptParams[0].fParam=float(scenario%5)*.125f;for(int j=1;j<8;++j)ScriptParams[j].iParam=int32((scenario>>(j-1))&1);ScriptParams[5].iParam=int32(scenario%5)-2;}
inline void __fastcall Label(CRunningScript* s,void*,char* out,uint8 n){trace.push_back({"label",n});s->m_IP+=n;std::strcpy(out,labels++?"fixture_block":"fixture_anim");}
inline void* __cdecl New(size_t n){trace.push_back({"new",n});const auto index=allocations++;return scenario%7==0||scenario%7==1&&index==1?nullptr:tasks[index].data();}
inline CAnimBlock* __cdecl Block(const char* name){trace.push_back({"block",name});return nullptr;}
inline void __fastcall Give(CRunningScript*,void*,int32 ped,CTask* task,int32 opcode){trace.push_back({"give",ped,task?int((uintptr_t(task)-uintptr_t(tasks.data()))/128):-1,opcode});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CRunningScript","PlayAnimScriptCommand",0x470150,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-anim355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-anim355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-anim355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedParams=ScriptParams;const auto savedSequence=StaticRef<int32>(0x8D2E98);
 EntryPatch a(0x464080,Collect),ad(bit_cast<uintptr_t>(&CRunningScript::CollectParameters),Collect);
 EntryPatch b(0x463D50,Label),bd(bit_cast<uintptr_t>(&CRunningScript::ReadTextLabelFromScript),Label);
 EntryPatch c(0x61A5A0,New),cd(bit_cast<uintptr_t>(&CTask::operator new),New);
 EntryPatch d(0x4D3940,Block),dd(bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),Block);
 EntryPatch e(0x465C20,Give),ed(bit_cast<uintptr_t>(static_cast<void(CRunningScript::*)(int32,CTask*,int32)>(&CRunningScript::GivePedScriptedTask)),Give);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();collects=labels=allocations=0;
    alignas(CRunningScript) std::array<uint8,sizeof(CRunningScript)> raw{};auto* self=reinterpret_cast<CRunningScript*>(raw.data());self->m_IP=reinterpret_cast<uint8*>(0x1200);for(auto& task:tasks)task.fill(0xCD);
    ScriptParams.fill({});for(size_t j=0;j<8;++j)ScriptParams[j].iParam=int32(j+i);StaticRef<int32>(0x8D2E98)=i%2?-1:3;
    const std::array<int32,5> commands{0x605,0x812,0x88A,0xA1A,0};const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__thiscall*)(CRunningScript*,int32)>(fn.address)(self,commands[(i/128)%5]);
    return json{{"script",Digest(raw.data(),raw.size())},{"tasks",Digest(tasks.data(),sizeof(tasks))},{"params",Digest(ScriptParams.data(),sizeof(ScriptParams))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 ScriptParams=savedParams;StaticRef<int32>(0x8D2E98)=savedSequence;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
