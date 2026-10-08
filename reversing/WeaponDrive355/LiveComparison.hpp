#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Weapon.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace WeaponDrive355Live {
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
inline const std::array<Fn,1> fns{{{"CWeapon","DoDriveByAutoAiming",0x73D720,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"weapon-drive355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"weapon-drive355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapon-drive355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x564A20,Find),ad(bit_cast<uintptr_t>(&CWorld::FindObjectsInRange),Find);
 EntryPatch b(0x417610,Dist),bd(bit_cast<uintptr_t>(&CCollision::DistToLine),Dist);
 EntryPatch c(0x6D3E00,Angle),cd(bit_cast<uintptr_t>(&CVehicle::GetPlaneGunsAutoAimAngle),Angle);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());veh->m_nVehicleSubType=eVehicleType((i/32)%5);veh->GetPosition()=CVector{0.f,0.f,0.f};
    alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,4> raw{};
    for(size_t k=0;k<4;++k){auto* ped=reinterpret_cast<CPed*>(raw[k].data());raw[k][0x36]=uint8((i+k)%2?3:2);ped->m_nPedState=(i+k)%5==0?PEDSTATE_DEAD:(i+k)%5==1?PEDSTATE_DIE:PEDSTATE_IDLE;ped->m_pVehicle=(i+k)%7==0?veh:nullptr;ped->m_pAttachedTo=(i+k)%11==0?veh:nullptr;ped->GetPosition()=CVector{float(k+1)*float(i%5+1),float(int(k)-2)*.125f,float(i%3)*.0625f};entities[k]=ped;}
    auto* owner=entities[3];CVector start{0.f,0.f,0.f},end{float(i%2?20:-20),float(i%7)*.125f,float(i%11)*.0625f};
    scenario=uint32(i);trace=json::array();const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__cdecl*)(CEntity*,CVehicle*,CVector*,CVector*,bool)>(fn.address)(i%13==0?nullptr:owner,veh,&start,&end,i%2!=0);
    return json{{"start",Digest(&start,sizeof(start))},{"end",Digest(&end,sizeof(end))},{"vehicle",Digest(vehicle.data(),vehicle.size())},{"entities",Digest(raw.data(),sizeof(raw))},{"trace",trace},{"switches",switched}};
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
