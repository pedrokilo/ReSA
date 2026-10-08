#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorControl355Live {
inline json trace;inline uint32 scenario{};
inline void __fastcall Calc(InteriorGroup_c* g,void*){trace.push_back({"visible"});g->m_lastIsVisible=g->m_isVisible;g->m_isVisible=scenario%2!=0;}
inline int8 __fastcall Paths(InteriorGroup_c* g,void*){trace.push_back({"paths"});g->m_pathSetupComplete=(scenario/2)%2;return g->m_pathSetupComplete;}
inline void __fastcall Setup(InteriorGroup_c* g,void*){trace.push_back({"setup"});g->m_updatePeds=(scenario/4)%2;}
inline void __fastcall UpdatePeds(InteriorGroup_c*,void*){trace.push_back({"update_peds"});}
inline void __fastcall Anim(InteriorGroup_c*,void*){trace.push_back({"anims"});}
inline int32 __fastcall House(InteriorGroup_c*,void*){trace.push_back({"house"});return 0;}
inline int32 __fastcall Shop(InteriorGroup_c*,void*){trace.push_back({"shop"});return 0;}
inline void __fastcall Office(InteriorGroup_c*,void*){trace.push_back({"office"});}
inline void __fastcall OfficeUpdate(InteriorGroup_c*,void*){trace.push_back({"office_update"});}
inline bool __fastcall Valid(CPed* p,void*){trace.push_back({"valid",uintptr_t(p)});return ((uintptr_t(p)>>8)+scenario)%3!=0;}
inline void __fastcall Remove(InteriorGroup_c*,void*,CPed* p){trace.push_back({"remove",uintptr_t(p)});}
inline void __cdecl Population(CPed* p){trace.push_back({"population",uintptr_t(p)});}
inline void __cdecl Stream(int32 type){trace.push_back({"stream",type});}
inline void __cdecl Load(bool b){trace.push_back({"load",b});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,5> fns{{
{"InteriorGroup_c","Update",0x5968E0,0,-1,512},
{"InteriorGroup_c","SetupPeds",0x596890,0,-1,512},
{"InteriorGroup_c","UpdatePeds",0x596830,0,-1,512},
{"InteriorGroup_c","RemovePed",0x594E30,0,-1,512},
{"InteriorGroup_c","SetupOfficePeds",0x594BF0,0,-1,512},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-control355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-control355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-control355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;const auto savedActive=*reinterpret_cast<uint8*>(0xBB3DC2);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  using ReversingProbe::EntryPatch;using std::bit_cast;
  std::vector<std::unique_ptr<EntryPatch>> patches;
  const auto patch=[&](uintptr_t address,uintptr_t dll,auto stub){if(address!=fn.address){patches.push_back(std::make_unique<EntryPatch>(address,stub));patches.push_back(std::make_unique<EntryPatch>(dll,stub));}};
  patch(0x595200,bit_cast<uintptr_t>(&InteriorGroup_c::CalcIsVisible),Calc);
  patch(0x595590,bit_cast<uintptr_t>(&InteriorGroup_c::SetupPaths),Paths);
  patch(0x596890,bit_cast<uintptr_t>(&InteriorGroup_c::SetupPeds),Setup);
  patch(0x596830,bit_cast<uintptr_t>(&InteriorGroup_c::UpdatePeds),UpdatePeds);
  patch(0x5950D0,bit_cast<uintptr_t>(&InteriorGroup_c::ReferenceAnims),Anim);
  patch(0x5965E0,bit_cast<uintptr_t>(&InteriorGroup_c::SetupHousePeds),House);
  patch(0x594C10,bit_cast<uintptr_t>(&InteriorGroup_c::SetupShopPeds),Shop);
  patch(0x594BF0,bit_cast<uintptr_t>(&InteriorGroup_c::SetupOfficePeds),Office);
  patch(0x594E90,bit_cast<uintptr_t>(&InteriorGroup_c::UpdateOfficePeds),OfficeUpdate);
  patch(0x5E4220,bit_cast<uintptr_t>(&CPed::IsPointerValid),Valid);
  patch(0x594E30,bit_cast<uintptr_t>(&InteriorGroup_c::RemovePed),Remove);
  patch(0x610F20,bit_cast<uintptr_t>(&CPopulation::RemovePed),Population);
  patch(0x40BBB0,bit_cast<uintptr_t>(&CStreaming::StreamPedsForInterior),Stream);
  patch(0x40EA10,bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),Load);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(InteriorGroup_c) std::array<uint8,sizeof(InteriorGroup_c)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<InteriorGroup_c*>(raw.data());
    scenario=uint32(i);trace=json::array();*reinterpret_cast<uint8*>(0xBB3DC2)=uint8(i%2);
    self->m_EnEx=(i/2)%2?reinterpret_cast<CEntryExit*>(0x123):nullptr;self->m_groupType=uint8((i/4)%5);
    self->m_pathSetupComplete=(i/8)%2;self->m_updatePeds=(i/16)%2;self->m_numPeds=int8(i);
    self->m_isVisible=i%2!=0;self->m_lastIsVisible=(i/2)%2!=0;
    for(size_t k=0;k<16;++k){self->m_peds[k]=(i+k)%3==0?nullptr:reinterpret_cast<CPed*>(0x100000+uint32(k)*256);self->m_pedsToRemove[k]=(i+k)%4==0?nullptr:reinterpret_cast<CPed*>(0x100000+uint32(k)*256);}
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    if(fn.address==0x594E30)reinterpret_cast<void(__thiscall*)(InteriorGroup_c*,CPed*)>(fn.address)(self,(i/32)%2==0?reinterpret_cast<CPed*>(0x100000+uint32(i%16)*256):nullptr);
    else reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"bytes",Digest(raw.data(),raw.size())},{"trace",trace},{"active",*reinterpret_cast<uint8*>(0xBB3DC2)},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 *reinterpret_cast<uint8*>(0xBB3DC2)=savedActive;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
