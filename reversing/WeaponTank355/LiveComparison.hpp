#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Weapon.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace WeaponTank355Live {
inline json trace;inline uint32 scenario{};inline std::array<CEntity*,4> entities{};
inline void __cdecl Find(const CVector& p,float radius,bool twoD,int16* count,int16 maximum,CEntity** out,bool buildings,bool vehicles,bool peds,bool objects,bool dummies){trace.push_back({"find",std::bit_cast<uint32>(radius),twoD,maximum,buildings,vehicles,peds,objects,dummies});*count=int16((scenario/2)%4);for(int16 k=0;k<*count;++k)out[k]=entities[k];}
inline float __cdecl Dist(const CVector& a,const CVector& b,const CVector& p){trace.push_back({"dist",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});return float((scenario+uint32(int(p.x)))%7)*.125f;}
inline float __fastcall Angle(CVehicle*,void*){trace.push_back({"angle"});constexpr std::array<float,5> values{0.f,.5f,1.f,30.f,60.f};return values[(scenario/8)%5];}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CWeapon","DoTankDoomAiming",0x73D1E0,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"weapon-tank355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"weapon-tank355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapon-tank355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x564A20,Find),ad(bit_cast<uintptr_t>(&CWorld::FindObjectsInRange),Find);
 EntryPatch b(0x417610,Dist),bd(bit_cast<uintptr_t>(&CCollision::DistToLine),Dist);
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CVehicle) std::array<std::array<uint8,sizeof(CVehicle)>,5> raw{};alignas(CMatrix) std::array<std::array<uint8,sizeof(CMatrix)>,5> matrices{};alignas(CBaseModelInfo) std::array<uint8,sizeof(CBaseModelInfo)> model{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};
    At<uintptr_t>(model.data(),0x14)=uintptr_t(col.data());At<float>(col.data(),0x24)=float((i/128)%7)*.125f;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());
    for(size_t k=0;k<5;++k){auto* e=reinterpret_cast<CEntity*>(raw[k].data());raw[k][0x36]=uint8(2|(((i+int(k))%9)<<3));At<int16>(raw[k].data(),0x22)=2000;At<uintptr_t>(raw[k].data(),0x14)=uintptr_t(matrices[k].data());At<CVector>(matrices[k].data(),0)=CVector{1,0,0};At<CVector>(matrices[k].data(),16)=CVector{0,1,0};At<CVector>(matrices[k].data(),32)=CVector{0,0,1};At<CVector>(matrices[k].data(),48)=CVector{float(k)*float(i%5+1),float(int(k)-2)*.125f,float((i/16+k)%7)*.25f};At<uint32>(raw[k].data(),0x40)=(i+k)%5==0?0x20000000u:0;if(k<4)entities[k]=e;}
    auto* vehicle=reinterpret_cast<CEntity*>(raw[4].data());auto* owner=i%3==0?entities[0]:nullptr;CVector start{0,0,0},end{float(i%2?20:-20),float(i%7)*.125f,float(i%11)*.0625f};
    scenario=uint32(i);trace=json::array();const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__cdecl*)(CEntity*,CEntity*,CVector*,CVector*)>(fn.address)(vehicle,owner,&start,&end);
    return json{{"start",Digest(&start,sizeof(start))},{"end",{F(end.x),F(end.y),F(end.z)}},{"entities",Digest(raw.data(),sizeof(raw))},{"matrices",Digest(matrices.data(),sizeof(matrices))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
