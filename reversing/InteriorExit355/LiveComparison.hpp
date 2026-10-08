#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorManager_c.h"
#include "PathFind.h"
#include "AnimManager.h"
#include "../ProbeRuntime.hpp"
namespace InteriorExit355Live {
inline json trace;inline uint32 rngValue{};inline uintptr_t base{};
inline void __fastcall Leave(Interior_c* p,void*){trace.push_back({"exit",uintptr_t(p)});}
inline void __fastcall Pool(InteriorManager_c* g,void*,Interior_c* p){trace.push_back({"pool",uintptr_t(g),uintptr_t(p)});}
inline bool __fastcall Valid(CPed* p,void*){trace.push_back({"valid",uintptr_t(p)});return (uintptr_t(p)/256+rngValue)%3!=0;}
inline bool __fastcall Alive(const CPed* p,void*){trace.push_back({"alive",uintptr_t(p)});return (uintptr_t(p)/256+rngValue)%2!=0;}
inline void __cdecl Remove(CPed* p){trace.push_back({"remove",uintptr_t(p)});}
inline void __fastcall Paths(CPathFind* p,void*,uint32 id){trace.push_back({"paths",uintptr_t(p),id});}
inline int32 __cdecl Index(const char* name){trace.push_back({"index",name});return int32(rngValue%50);}
inline void __cdecl Anim(int32 id){trace.push_back({"remove_anim",id});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{{"InteriorGroup_c","Exit",0x595290,0,-1,1024},{"InteriorGroup_c","DereferenceAnims",0x595160,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-exit355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-exit355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-exit355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,16> savedFlags{};memcpy(savedFlags.data(),reinterpret_cast<void*>(0xBB3D9C),16);
 EntryPatch p0(0x592230,Leave),d0(bit_cast<uintptr_t>(&Interior_c::Exit),Leave);
 EntryPatch p1(0x5984B0,Pool),d1(bit_cast<uintptr_t>(&InteriorManager_c::ReturnInteriorToPool),Pool);
 EntryPatch p2(0x5E4220,Valid),d2(bit_cast<uintptr_t>(&CPed::IsPointerValid),Valid);
 EntryPatch p3(0x5E0170,Alive),d3(bit_cast<uintptr_t>(&CPed::IsAlive),Alive);
 EntryPatch p4(0x610F20,Remove),d4(bit_cast<uintptr_t>(&CPopulation::RemovePed),Remove);
 EntryPatch p5(0x44E1A0,Paths),d5(bit_cast<uintptr_t>(&CPathFind::RemoveInterior),Paths);
 EntryPatch p6(0x4D3990,Index),d6(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Index);
 EntryPatch p7(0x4D3FD0,Anim),d7(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Anim);
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
    for(size_t k=0;k<16;++k){self->m_peds[k]=(i+k)%4==0?nullptr:reinterpret_cast<CPed*>(0x100000+uint32(k)*256);StaticRef<uint8>(0xBB3D9C+k)=uint8(20+k);}
    self->m_id=uint8(i);self->m_groupType=uint8(i%3);self->m_animBlockReferenced=int8((i/3)%2);
    rngValue=uint32(i);trace=json::array();base=uintptr_t(interiors.data());
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"bytes",Digest(raw.data(),raw.size())},{"cache",ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3D9C),16)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 memcpy(reinterpret_cast<void*>(0xBB3D9C),savedFlags.data(),16);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
