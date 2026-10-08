#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "Cam.h"
#include "HandShaker.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamHeliChase355Live {
inline json trace;inline uint32 scenario{};
inline uintptr_t fakeEntity{};inline uint32 losCalls{};
inline void __fastcall Core(CCam* cam,void*,CEntity*& entity,CVehicle*& vehicle,CVector& dest,CVector& src,CVector& up,CVector& right,CVector& forward,CVector& velocity,float& speed,CVector& angular,float& angularSpeed,CColSphere& sphere){trace.push_back({"core",uintptr_t(cam)});entity=reinterpret_cast<CEntity*>(fakeEntity);vehicle=reinterpret_cast<CVehicle*>(entity);dest=CVector{1,2,float(int32((scenario/16)%7)-3)};src=CVector{4,5,6};up=CVector{.25f,.5f,1};right=CVector{1,0,0};forward=CVector{0,1,.125f};velocity=CVector{1,2,3};speed=4;angular=CVector{0,0,0};angularSpeed=0;sphere.m_vecCenter=CVector{0,0,0};sphere.m_fRadius=1;}
inline bool __cdecl Line(const CVector& a,const CVector& b,CColPoint& point,CEntity*&,bool bu,bool ve,bool pe,bool ob,bool du,bool se,bool ca,bool sh){trace.push_back({"line",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),uintptr_t(CWorld::pIgnoreEntity),bu,ve,pe,ob,du,se,ca,sh});point.m_vecPoint=CVector{float(int32((scenario/8)%5)-2)*5,float((scenario/40)%7)*2,1};return (scenario+losCalls++)%3!=0;}
inline bool __fastcall Exit(CCam*,void*,int32 id,const CVector& src,const CVector& dest,float progress,bool check){trace.push_back({"exit",id,std::bit_cast<uint32>(src.x),std::bit_cast<uint32>(src.y),std::bit_cast<uint32>(src.z),std::bit_cast<uint32>(dest.x),std::bit_cast<uint32>(dest.y),std::bit_cast<uint32>(dest.z),std::bit_cast<uint32>(progress),check});return scenario%7==0;}
inline void __fastcall Final(CCam* cam,void*,const CVector& src,const CVector& dest,float roll,float fov,float clip,float shake){trace.push_back({"final",std::bit_cast<uint32>(roll),std::bit_cast<uint32>(fov),std::bit_cast<uint32>(clip),std::bit_cast<uint32>(shake)});cam->m_vecSource=src;cam->m_vecFront=dest-src;cam->m_fFOV=fov;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline uint32 randomCalls{};inline int __cdecl Random(){const int value=int((scenario*7919+randomCalls++*14033)%32768);trace.push_back({"rand",value});return value;}
inline uint32 sphereCalls{};
inline CEntity* __cdecl Sphere(CVector position,float radius,CEntity* ignore,bool b,bool v,bool p,bool o,bool d,bool c){trace.push_back({"sphere",std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),std::bit_cast<uint32>(radius),uintptr_t(ignore),b,v,p,o,d,c});return (sphereCalls+++scenario)%4==0?reinterpret_cast<CEntity*>(fakeEntity):nullptr;}
inline void __fastcall Reset(CHandShaker*,void*){trace.push_back({"reset"});}
inline void __fastcall StateInitial(void* state,void*){trace.push_back({"state_initial",uintptr_t(state)});}
inline void __cdecl StateStart(){trace.push_back({"state_start"});}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","Process_DW_HeliChaseCam",0x51A740,0,-1,16384}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-heli-chase355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-heli-chase355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-heli-chase355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedMode=StaticRef<int32>(0x8CC488);const auto savedDuration=StaticRef<uint32>(0x8CCBA8);const auto savedIgnore=CWorld::pIgnoreEntity;const auto savedFailed=StaticRef<uint8>(0xB6EC70),savedActive=StaticRef<uint8>(0xB6F059);const auto savedMS=CTimer::GetTimeInMS(),savedFrame=CTimer::GetFrameCounter();
 std::array<uint8,12> savedTimes;std::memcpy(savedTimes.data(),reinterpret_cast<void*>(0x8CCB9C),12);
 std::array<uint8,0x9C> savedState;std::memcpy(savedState.data(),reinterpret_cast<void*>(0xB6FEC0),savedState.size());
 std::array<uint8,16> savedScratch;std::memcpy(savedScratch.data(),reinterpret_cast<void*>(0xB70058),16);
 std::array<uint8,12> savedParameters;std::memcpy(savedParameters.data(),reinterpret_cast<void*>(0x8CCD20),12);
 EntryPatch a(0x517130,Core),ad(bit_cast<uintptr_t>(&CCam::GetCoreDataForDWCineyCamMode),Core);
 EntryPatch b(0x56BA00,Line),bd(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
 EntryPatch c(0x517400,Exit),cd(bit_cast<uintptr_t>(&CCam::IsTimeToExitThisDWCineyCamMode),Exit);
 EntryPatch reset(0x50D860,Reset),resetd(bit_cast<uintptr_t>(&CHandShaker::Reset),Reset);
 EntryPatch initial(0x50E180,StateInitial),start(0x50E090,StateStart);
 EntryPatch sp(0x569E20,Sphere),spd(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),Sphere);
 EntryPatch ra(0x821B1E,Random),rad(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch d(0x50DD70,Final),dd(bit_cast<uintptr_t>(&CCam::Finalise_DW_CineyCams),Final);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();randomCalls=0;
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());alignas(16) std::array<uint8,sizeof(CVehicle)> entity{};
    entity[0x36]=uint8(i%7==1?3:2);fakeEntity=uintptr_t(entity.data());self->m_pCamTargetEntity=i%7==0?nullptr:reinterpret_cast<CEntity*>(entity.data());
    StaticRef<int32>(0x8CC488)=i%3==0?0x38:0x41;StaticRef<uint32>(0x8CCBA8)=100;StaticRef<uint32>(0x8CCBA0)=20;StaticRef<uint32>(0x8CCBA4)=120;StaticRef<uint32>(0x8CCB9C)=i%5==0?1:100;StaticRef<uint32>(0xB7CB4C)=100;StaticRef<uint32>(0xB7CB84)=uint32(i%201);StaticRef<uint8>(0xB6EC70)=uint8(i%2);StaticRef<uint8>(0xB6F059)=uint8(i%3);
    auto* state=reinterpret_cast<uint8*>(0xB6FEC0);std::memset(state,0,0x9C);At<CVector>(state,0)=CVector{10,20,30};At<CVector>(state,0x0C)=CVector{-20,10,25};
    At<float>(state,0x18)=5;At<float>(state,0x1C)=6;At<float>(state,0x20)=3;At<float>(state,0x24)=2;At<float>(state,0x28)=.5f;At<float>(state,0x2C)=50;At<float>(state,0x30)=40;At<float>(state,0x34)=.25f;At<float>(state,0x38)=.1f;At<float>(state,0x3C)=.4f;
    At<uint8>(state,0x40)=uint8((i/4)%2);At<int32>(state,0x44)=int32((i/7)%4)-1;At<int32>(state,0x48)=2;At<CVector>(state,0x4C)=CVector{3,4,5};At<int32>(state,0x58)=int32((i/13)%4);At<uint8>(state,0x5C)=uint8((i/5)%2);At<int32>(state,0x60)=3;At<int32>(state,0x64)=int32((i/11)%6)-1;
    At<float>(state,0x68)=5;At<float>(state,0x6C)=100;At<float>(state,0x70)=15;At<float>(state,0x74)=8;At<float>(state,0x78)=.3f;At<float>(state,0x7C)=10;At<uint8>(state,0x80)=uint8(i%2);At<uint8>(state,0x81)=uint8((i/17)%2);At<float>(state,0x84)=60;At<uint8>(state,0x88)=uint8((i/19)%2);At<int32>(state,0x8C)=20;At<int32>(state,0x90)=120;At<float>(state,0x94)=.8f;At<int32>(state,0x98)=100;
    StaticRef<CVector>(0xB70058)=CVector{1,2,3};StaticRef<uint32>(0xB70064)=uint32(i%2);StaticRef<float>(0x8CCD20)=.75f;StaticRef<int32>(0x8CCD24)=int32((i/23)%5)-1;StaticRef<float>(0x8CCD28)=.5f;CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(0x1234);losCalls=0;sphereCalls=0;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CCam*,bool)>(fn.address)(self,i%2!=0);
    return json{{"cam",Digest(raw.data(),raw.size())},{"entity",Digest(entity.data(),entity.size())},{"result",result},{"mode",StaticRef<int32>(0x8CC488)},{"times",Digest(reinterpret_cast<void*>(0x8CCB9C),12)},{"parameters",Digest(reinterpret_cast<void*>(0x8CCD20),12)},{"state",Digest(state,0x9C)},{"scratch",Digest(reinterpret_cast<void*>(0xB70058),16)},{"active",StaticRef<uint8>(0xB6F059)},{"failed",StaticRef<uint8>(0xB6EC70)},{"ignore",uintptr_t(CWorld::pIgnoreEntity)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<int32>(0x8CC488)=savedMode;StaticRef<uint32>(0x8CCBA8)=savedDuration;CWorld::pIgnoreEntity=savedIgnore;StaticRef<uint8>(0xB6EC70)=savedFailed;StaticRef<uint8>(0xB6F059)=savedActive;StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<uint32>(0xB7CB4C)=savedFrame;std::memcpy(reinterpret_cast<void*>(0x8CCB9C),savedTimes.data(),12);std::memcpy(reinterpret_cast<void*>(0x8CCD20),savedParameters.data(),12);std::memcpy(reinterpret_cast<void*>(0xB6FEC0),savedState.data(),savedState.size());std::memcpy(reinterpret_cast<void*>(0xB70058),savedScratch.data(),16);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
