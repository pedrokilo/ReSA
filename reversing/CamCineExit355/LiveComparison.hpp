#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "../ProbeRuntime.hpp"
namespace CamCineExit355Live {
inline json trace;inline uint32 scenario{};
inline bool __cdecl Line(const CVector& a,const CVector& b,CColPoint&,CEntity*&,bool bu,bool ve,bool pe,bool ob,bool du,bool se,bool ca,bool sh){trace.push_back({"line",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),uintptr_t(CWorld::pIgnoreEntity),bu,ve,pe,ob,du,se,ca,sh});return scenario%3==0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","IsTimeToExitThisDWCineyCamMode",0x517400,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-cine-exit355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-cine-exit355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-cine-exit355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 std::array<uint8,32> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xB6EC5C),32);const auto savedTime=CTimer::GetTimeInMS(),savedDeadline=StaticRef<uint32>(0x8CCBA4);const auto savedIgnore=CWorld::pIgnoreEntity;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch l(0x56BA00,Line),ld(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    scenario=uint32(i);trace=json::array();memset(reinterpret_cast<void*>(0xB6EC5C),0,32);const auto id=19+i%11;StaticRef<uint8>(0xB6EC5C+id)=uint8(i%7==0);CTimer::SetTimeInMS(uint32(i%4==0?0:i%4==1?100:i%4==2?101:0xFFFFFFFE));StaticRef<uint32>(0x8CCBA4)=100;CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(0x567800);self->m_pCamTargetEntity=reinterpret_cast<CEntity*>(0x123400);
    const CVector source{1,2,3},target{float(i%37),float(i%21),float(i%15)};
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    const auto result=reinterpret_cast<bool(__thiscall*)(CCam*,int32,const CVector&,const CVector&,float,bool)>(fn.address)(self,id,source,target,float(i),i%2!=0);
    return json{{"bytes",Digest(raw.data(),raw.size())},{"ignore",uintptr_t(CWorld::pIgnoreEntity)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 memcpy(reinterpret_cast<void*>(0xB6EC5C),saved.data(),32);CTimer::SetTimeInMS(savedTime);StaticRef<uint32>(0x8CCBA4)=savedDeadline;CWorld::pIgnoreEntity=savedIgnore;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
