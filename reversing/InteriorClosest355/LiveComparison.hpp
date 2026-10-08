#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorClosest355Live {
inline json trace;inline uint32 rngValue{};inline uintptr_t base{};
inline int32 __cdecl Random(){trace.push_back({"random",rngValue});return int32(rngValue);}
inline bool __fastcall Visible(Interior_c* self,void*){const auto index=(uintptr_t(self)-base)/sizeof(Interior_c);trace.push_back({"visible",index});return index%3==rngValue%3;}
inline void __fastcall Anims(InteriorGroup_c*,void*){trace.push_back({"reference_anims"});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool __fastcall Inside(Interior_c* self,void*,const CVector& pos,CVector bias){const auto index=(uintptr_t(self)-base)/sizeof(Interior_c);trace.push_back({"inside",index,std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),std::bit_cast<uint32>(bias.x),std::bit_cast<uint32>(bias.y),std::bit_cast<uint32>(bias.z)});return (index+rngValue)%4!=0;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"InteriorGroup_c","FindClosestInteriorInfo",0x594A50,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-closest355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-closest355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-closest355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch b(0x5913E0,Inside),bd(bit_cast<uintptr_t>(&Interior_c::IsPtInside),Inside);
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
    for(size_t k=0;k<8;++k){auto* in=reinterpret_cast<Interior_c*>(interiors[k].data());self->m_interiors[k]=(i+k)%3==0?nullptr:in;in->m_box=reinterpret_cast<tEffectInterior*>(boxes[k].data());boxes[k][0]=uint8((i+k)%8);in->m_interiorInfosCount=int8((i+k)%9-2);for(int j=0;j<16;++j){auto* info=&in->m_interiorInfos[j];*reinterpret_cast<int8*>(&info->Type)=int8((i+j+k)%13-2);info->IsInUse=(i+j)%3==0;info->Pos=CVector{float(j*3)-float(k),float(j)-float(k*2),float(k+j)*0.25f};}}
    self->m_numInteriors=uint8(i%9);self->m_isVisible=(i%2)!=0;self->m_lastIsVisible=(i%3)!=0;
    if(fn.address==0x595200)for(size_t k=0;k<8;++k)self->m_interiors[k]=reinterpret_cast<Interior_c*>(interiors[k].data());
    for(size_t k=0;k<16;++k)self->m_peds[k]=reinterpret_cast<CPed*>(0x100000+uint32(k)*256);
    self->m_pEntity=reinterpret_cast<CEntity*>(0x223344);
    rngValue=uint32((i*53)%32768);trace=json::array();base=uintptr_t(interiors.data());
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    uintptr_t result=0;InteriorInfo_t* outInfo=reinterpret_cast<InteriorInfo_t*>(0x123);Interior_c* outInterior=reinterpret_cast<Interior_c*>(0x456);
    float distance=std::bit_cast<float>(uint32(0x7FC00001));
    CVector point{float(i%17)-3,float(i%13)*0.5f,float(i%11)*0.25f};
    const float radius=std::array<float,8>{0,1,-1,5,10,1000,std::bit_cast<float>(uint32(0x7FC00000)),std::bit_cast<float>(uint32(0x7F800000))}[i%8];
    result=reinterpret_cast<bool(__thiscall*)(InteriorGroup_c*,int32,CVector,float,InteriorInfo_t**,Interior_c**,float*)>(fn.address)(self,i%13-2,point,radius,&outInfo,&outInterior,&distance);
    return json{{"distance",F(distance)},{"bytes",Digest(raw.data(),raw.size())},{"result",result},{"out_info",uintptr_t(outInfo)},{"out_interior",uintptr_t(outInterior)},{"trace",trace},{"switches",switched}};
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
