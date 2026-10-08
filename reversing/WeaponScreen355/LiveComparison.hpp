#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Weapon.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "Darkel.h"
#include "Sprite.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace WeaponScreen355Live {
inline json trace;inline uint32 scenario{};
inline std::array<CEntity*,8> entities{};
inline bool __cdecl PedEligible(const CPed& ped){trace.push_back({"ped_eligible",uintptr_t(&ped)});return scenario%5!=0;}
inline bool __cdecl VehicleEligible(const CVehicle& vehicle){trace.push_back({"vehicle_eligible",uintptr_t(&vehicle)});return scenario%7!=0;}
inline CVehicle* __cdecl Player(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote});return scenario%3==0?reinterpret_cast<CVehicle*>(entities[5]):nullptr;}
inline bool __cdecl Screen(const RwV3d& pos,RwV3d* out,float* width,float* height,bool farClip,bool nearClip){trace.push_back({"screen",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),farClip,nearClip});*out=RwV3d{pos.x,pos.y,pos.z};*width=2;*height=3;return scenario%11!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CWeapon","FindNearestTargetEntityWithScreenCoors",0x73E240,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"weapon-screen355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"weapon-screen355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapon-screen355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedPed=StaticRef<CPedPool*>(0xB74490);const auto savedVeh=StaticRef<CVehiclePool*>(0xB74494);const auto savedWidth=RsGlobal.maximumWidth,savedHeight=RsGlobal.maximumHeight;
 EntryPatch a(0x43D2F0,PedEligible),ad(bit_cast<uintptr_t>(&CDarkel::ThisPedShouldBeKilledForFrenzy),PedEligible);
 EntryPatch b(0x43D350,VehicleEligible),bd(bit_cast<uintptr_t>(&CDarkel::ThisVehicleShouldBeKilledForFrenzy),VehicleEligible);
 EntryPatch c(0x56E0D0,Player),cd(bit_cast<uintptr_t>(&FindPlayerVehicle),Player);
 EntryPatch d(0x70CE30,Screen),dd(bit_cast<uintptr_t>(&CSprite::CalcScreenCoors),Screen);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(16) std::array<uint8,0x7C4*4> peds{};alignas(16) std::array<uint8,0xA18*4> vehicles{};alignas(16) std::array<uint8,sizeof(CPedPool)> pp{},vp{};std::array<uint8,4> ps{},vs{};
    At<void*>(pp.data(),0)=peds.data();At<void*>(pp.data(),4)=ps.data();At<int32>(pp.data(),8)=4;At<void*>(vp.data(),0)=vehicles.data();At<void*>(vp.data(),4)=vs.data();At<int32>(vp.data(),8)=4;
    StaticRef<CPedPool*>(0xB74490)=reinterpret_cast<CPedPool*>(pp.data());StaticRef<CVehiclePool*>(0xB74494)=reinterpret_cast<CVehiclePool*>(vp.data());
    const std::array<int32,4> widths{0,1,640,1920},heights{0,1,480,1080};RsGlobal.maximumWidth=widths[(i/128)%4];RsGlobal.maximumHeight=heights[(i/512)%4];
    for(int k=0;k<4;++k){ps[k]=uint8((i+k)%5==0?0x80:0);vs[k]=uint8((i+k)%7==0?0x80:0);auto* ped=reinterpret_cast<CPed*>(peds.data()+k*0x7C4);ped->m_nPedState=(i+k)%4==0?PEDSTATE_DEAD:(i+k)%4==1?PEDSTATE_DIE:PEDSTATE_IDLE;ped->bInVehicle=(i+k)%6==0;entities[k]=ped;entities[k+4]=reinterpret_cast<CEntity*>(vehicles.data()+k*0xA18);const CVector pos{float(RsGlobal.maximumWidth)*.5f+float(int32((i+k*3)%17)-8),float(RsGlobal.maximumHeight)*.5f+float(int32((i/17+k*3)%17)-8),float(k)};ped->GetPosition()=pos;entities[k+4]->GetPosition()=pos+CVector{.5f,.5f,0};}
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;float outX=std::bit_cast<float>(0x7FC00001u),outY=-99;
    const CVector origin{float(RsGlobal.maximumWidth)*.5f,float(RsGlobal.maximumHeight)*.5f,0};const std::array<float,6> ranges{0,1,-1,5,100,std::bit_cast<float>(0x7FC00001u)};
    auto* result=reinterpret_cast<CEntity*(__cdecl*)(float,float,float,CVector,float*,float*)>(fn.address)(float(int32(i%3)-1)*.01f,float(int32((i/3)%3)-1)*.01f,ranges[(i/9)%6],origin,i%2?&outX:nullptr,i%2?&outY:nullptr);
    return json{{"peds",Digest(peds.data(),peds.size())},{"vehicles",Digest(vehicles.data(),vehicles.size())},{"result",uintptr_t(result)},{"out_x",F(outX)},{"out_y",F(outY)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<CPedPool*>(0xB74490)=savedPed;StaticRef<CVehiclePool*>(0xB74494)=savedVeh;RsGlobal.maximumWidth=savedWidth;RsGlobal.maximumHeight=savedHeight;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
