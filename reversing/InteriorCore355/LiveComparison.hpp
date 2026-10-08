#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorCore355Live {
inline json trace;inline uint32 rngValue{};inline uintptr_t base{};
inline int32 __cdecl Random(){trace.push_back({"random",rngValue});return int32(rngValue);}
inline bool __fastcall Visible(Interior_c* self,void*){const auto index=(uintptr_t(self)-base)/sizeof(Interior_c);trace.push_back({"visible",index});return index%3==rngValue%3;}
inline void __fastcall Anims(InteriorGroup_c*,void*){trace.push_back({"reference_anims"});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,10> fns{{
{"InteriorGroup_c","Init",0x5947E0,0,-1,512},
{"InteriorGroup_c","AddInterior",0x594840,0,-1,512},
{"InteriorGroup_c","GetPed",0x594B90,0,-1,512},
{"InteriorGroup_c","GetEntity",0x594BD0,0,-1,512},
{"InteriorGroup_c","ContainsInteriorType",0x595250,0,-1,512},
{"InteriorGroup_c","GetNumInteriorInfos",0x594920,0,-1,512},
{"InteriorGroup_c","GetRandomInterior",0x5948C0,0,-1,512},
{"InteriorGroup_c","FindInteriorInfo",0x594970,0,-1,512},
{"InteriorGroup_c","CalcIsVisible",0x595200,0,-1,512},
{"InteriorGroup_c","Setup",0x595320,0,-1,512},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-core355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-core355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-core355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x821B1E,Random),ad(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch b(0x5929F0,Visible),bd(bit_cast<uintptr_t>(&Interior_c::IsVisible),Visible);
 EntryPatch c(0x5950D0,Anims),cd(bit_cast<uintptr_t>(&InteriorGroup_c::ReferenceAnims),Anims);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(InteriorGroup_c) std::array<uint8,sizeof(InteriorGroup_c)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<InteriorGroup_c*>(raw.data());
    alignas(Interior_c) std::array<std::array<uint8,sizeof(Interior_c)>,8> interiors{};
    std::array<std::array<uint8,64>,8> boxes{};
    for(size_t k=0;k<8;++k){auto* in=reinterpret_cast<Interior_c*>(interiors[k].data());self->m_interiors[k]=(i+k)%3==0?nullptr:in;in->m_box=reinterpret_cast<tEffectInterior*>(boxes[k].data());boxes[k][0]=uint8((i+k)%8);in->m_interiorInfosCount=int8((i+k)%9-2);for(int j=0;j<16;++j){auto* info=&in->m_interiorInfos[j];*reinterpret_cast<int8*>(&info->Type)=int8((i+j+k)%13-2);info->IsInUse=(i+j)%3==0;}}
    self->m_numInteriors=uint8(i%9);self->m_isVisible=(i%2)!=0;self->m_lastIsVisible=(i%3)!=0;
    if(fn.address==0x595200)for(size_t k=0;k<8;++k)self->m_interiors[k]=reinterpret_cast<Interior_c*>(interiors[k].data());
    for(size_t k=0;k<16;++k)self->m_peds[k]=reinterpret_cast<CPed*>(0x100000+uint32(k)*256);
    self->m_pEntity=reinterpret_cast<CEntity*>(0x223344);
    rngValue=uint32((i*53)%32768);trace=json::array();base=uintptr_t(interiors.data());
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    uintptr_t result=0;InteriorInfo_t* outInfo=reinterpret_cast<InteriorInfo_t*>(0x123);Interior_c* outInterior=reinterpret_cast<Interior_c*>(0x456);
    if(fn.address==0x5947E0)reinterpret_cast<void(__thiscall*)(InteriorGroup_c*,CEntity*,int32)>(fn.address)(self,reinterpret_cast<CEntity*>(0x665544),i*37);
    else if(fn.address==0x594840)result=reinterpret_cast<int32(__thiscall*)(InteriorGroup_c*,Interior_c*)>(fn.address)(self,reinterpret_cast<Interior_c*>(interiors[0].data()));
    else if(fn.address==0x594B90)result=uintptr_t(reinterpret_cast<CPed*(__thiscall*)(InteriorGroup_c*,int32)>(fn.address)(self,i%16));
    else if(fn.address==0x594BD0)result=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(InteriorGroup_c*)>(fn.address)(self));
    else if(fn.address==0x595250)result=reinterpret_cast<int8(__thiscall*)(InteriorGroup_c*,int32)>(fn.address)(self,i%10-1);
    else if(fn.address==0x594920)result=reinterpret_cast<int32(__thiscall*)(InteriorGroup_c*,int32)>(fn.address)(self,i%13-2);
    else if(fn.address==0x5948C0)result=uintptr_t(reinterpret_cast<int32(__thiscall*)(InteriorGroup_c*)>(fn.address)(self));
    else if(fn.address==0x594970)result=reinterpret_cast<bool(__thiscall*)(InteriorGroup_c*,eInteriorInfoType,InteriorInfo_t**,Interior_c**)>(fn.address)(self,eInteriorInfoType(i%13-2),&outInfo,&outInterior);
    else reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"bytes",Digest(raw.data(),raw.size())},{"result",result},{"out_info",uintptr_t(outInfo)},{"out_interior",uintptr_t(outInterior)},{"trace",trace},{"switches",switched}};
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
