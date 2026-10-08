#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "DecisionMakers/Decision.h"
#include "../ProbeRuntime.hpp"
namespace DecisionPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
    {"CDecision","SetDefault",0x600530,0,-1,1024},
    {"CDecision","Set",0x600570,0,-1,2048},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"decision-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"decision-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("decision-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CDecision) std::array<uint8,sizeof(CDecision)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CDecision*>(raw.data());
    notsa::mdarray<int32,6> tasks{};notsa::mdarray<float,6,4> probs{};notsa::mdarray<int32,6,2> bools{};notsa::mdarray<float,6,6> facial{};
    constexpr std::array<uint32,20> edges{0,0x80000000,0x3f7fffff,0x3f800000,0xbf800000,0x437fffff,0x43800000,0x43800001,0xc3800000,0xc3800001,0x7fc00000,0x7f800000,0xff800000,0x5f000000,0xdf000000,0x4f000000,0xcf000000,0x3effffff,0xbeffffff,0x477fff80};
    for(size_t k=0;k<6;++k){tasks[k]=int32(uint32(i)*0x1020304u+uint32(k));for(size_t j=0;j<4;++j)probs[k][j]=i%3==0?std::bit_cast<float>(edges[(i/3+k*4+j)%edges.size()]):float((i*19+int(k)*7+int(j)*13)%2048-1024)*0.25f;for(size_t j=0;j<2;++j)bools[k][j]=(i+k+j)%4==0?0:(i+k+j)%4==1?-1:int32(0x80000000u+uint32(i));}
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    if(fn.address==0x600530)reinterpret_cast<void(__thiscall*)(CDecision*)>(fn.address)(self);
    else reinterpret_cast<void(__thiscall*)(CDecision*,decltype(tasks)*,decltype(probs)*,decltype(bools)*,decltype(facial)*)>(fn.address)(self,&tasks,&probs,&bools,&facial);
    return json{{"bytes",ReversingProbe::Bytes(raw.data(),raw.size())},{"tasks",ReversingProbe::Bytes(&tasks,sizeof(tasks))},{"probs",ReversingProbe::Bytes(&probs,sizeof(probs))},{"bools",ReversingProbe::Bytes(&bools,sizeof(bools))},{"facial",ReversingProbe::Bytes(&facial,sizeof(facial))},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
