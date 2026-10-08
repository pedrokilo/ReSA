#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "Cam.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamPlane355Live {
inline json trace;inline uint32 scenario{};
inline uintptr_t fakeEntity{};inline uint32 losCalls{};
inline void __fastcall Core(CCam* cam,void*,CEntity*& entity,CVehicle*& vehicle,CVector& dest,CVector& src,CVector& up,CVector& right,CVector& forward,CVector& velocity,float& speed,CVector& angular,float& angularSpeed,CColSphere& sphere){trace.push_back({"core",uintptr_t(cam)});entity=reinterpret_cast<CEntity*>(fakeEntity);vehicle=reinterpret_cast<CVehicle*>(entity);dest=CVector{1,2,float(int32((scenario/16)%7)-3)};src=CVector{4,5,6};up=CVector{.25f,.5f,1};right=CVector{1,0,0};forward=CVector{0,1,.125f};velocity=CVector{1,2,3};speed=4;angular=CVector{0,0,0};angularSpeed=0;sphere.m_vecCenter=CVector{0,0,0};sphere.m_fRadius=1;}
inline bool __cdecl Line(const CVector& a,const CVector& b,CColPoint&,CEntity*&,bool bu,bool ve,bool pe,bool ob,bool du,bool se,bool ca,bool sh){trace.push_back({"line",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),uintptr_t(CWorld::pIgnoreEntity),bu,ve,pe,ob,du,se,ca,sh});return (scenario+losCalls++)%5==0;}
inline bool __fastcall Exit(CCam*,void*,int32 id,const CVector& src,const CVector& dest,float progress,bool check){trace.push_back({"exit",id,std::bit_cast<uint32>(src.x),std::bit_cast<uint32>(src.y),std::bit_cast<uint32>(src.z),std::bit_cast<uint32>(dest.x),std::bit_cast<uint32>(dest.y),std::bit_cast<uint32>(dest.z),std::bit_cast<uint32>(progress),check});return scenario%7==0;}
inline void __fastcall Final(CCam* cam,void*,const CVector& src,const CVector& dest,float roll,float fov,float clip,float shake){trace.push_back({"final",std::bit_cast<uint32>(roll),std::bit_cast<uint32>(fov),std::bit_cast<uint32>(clip),std::bit_cast<uint32>(shake)});cam->m_vecSource=src;cam->m_vecFront=dest-src;cam->m_fFOV=fov;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","Process_DW_PlaneCam3",0x51D100,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-plane355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-plane355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-plane355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedMode=StaticRef<int32>(0x8CC488);const auto savedDuration=StaticRef<uint32>(0x8CCBC8);const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedIgnore=CWorld::pIgnoreEntity;const auto savedFailed=StaticRef<uint8>(0xB6EC78),savedActive=StaticRef<uint8>(0xB6F059);const auto savedMS=CTimer::GetTimeInMS(),savedFrame=CTimer::GetFrameCounter();std::array<uint8,12> savedTimes{};std::memcpy(savedTimes.data(),reinterpret_cast<void*>(0x8CCB9C),12);std::array<uint8,16> savedParameters{};std::memcpy(savedParameters.data(),reinterpret_cast<void*>(0x8CCDE0),16);std::array<uint8,8> savedCounter{};std::memcpy(savedCounter.data(),reinterpret_cast<void*>(0xB700D4),8);
 EntryPatch a(0x517130,Core),ad(bit_cast<uintptr_t>(&CCam::GetCoreDataForDWCineyCamMode),Core);
 EntryPatch b(0x56BA00,Line),bd(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
 EntryPatch c(0x517400,Exit),cd(bit_cast<uintptr_t>(&CCam::IsTimeToExitThisDWCineyCamMode),Exit);
 EntryPatch d(0x50DD70,Final),dd(bit_cast<uintptr_t>(&CCam::Finalise_DW_CineyCams),Final);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());alignas(16) std::array<uint8,sizeof(CVehicle)> entity{};alignas(16) std::array<uint8,sizeof(CBaseModelInfo)> model{};alignas(16) std::array<uint8,sizeof(CColModel)> col{};
    entity[0x36]=uint8(i%4==1?3:2);At<int16>(entity.data(),0x22)=2000;fakeEntity=uintptr_t(entity.data());self->m_pCamTargetEntity=i%4==0?nullptr:reinterpret_cast<CEntity*>(entity.data());At<void*>(model.data(),0x14)=col.data();At<CVector>(col.data(),0)=CVector{-1,-2,-3};At<CVector>(col.data(),12)=CVector{1,float(i%5)+2,3};StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());
    StaticRef<int32>(0x8CC488)=i%3==0?0x40:0x41;StaticRef<uint32>(0x8CCBC8)=i%7==0?0:100;StaticRef<uint32>(0x8CCBA0)=20;StaticRef<uint32>(0x8CCBA4)=i%7==0?20:120;StaticRef<uint32>(0x8CCB9C)=i%5==0?1:100;StaticRef<uint32>(0xB7CB4C)=100;StaticRef<uint32>(0xB7CB84)=uint32(i%201);StaticRef<uint8>(0xB6EC78)=uint8(i%2);StaticRef<uint8>(0xB6F059)=uint8(i%3);StaticRef<uint32>(0xB700D8)=uint32((i/4)%2)|0xABCD0000u;StaticRef<int32>(0xB700D4)=int32((i/8)%6)-1;StaticRef<int32>(0x8CCDE0)=3;StaticRef<float>(0x8CCDE4)=float((i/112)%5);StaticRef<float>(0x8CCDE8)=-99;StaticRef<float>(0x8CCDEC)=float((i/560)%3);CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(0x1234);losCalls=0;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CCam*,bool)>(fn.address)(self,i%2!=0);
    return json{{"cam",Digest(raw.data(),raw.size())},{"result",result},{"mode",StaticRef<int32>(0x8CC488)},{"times",Digest(reinterpret_cast<void*>(0x8CCB9C),12)},{"parameters",Digest(reinterpret_cast<void*>(0x8CCDE0),16)},{"counter",Digest(reinterpret_cast<void*>(0xB700D4),8)},{"active",StaticRef<uint8>(0xB6F059)},{"failed",StaticRef<uint8>(0xB6EC78)},{"ignore",uintptr_t(CWorld::pIgnoreEntity)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<int32>(0x8CC488)=savedMode;StaticRef<uint32>(0x8CCBC8)=savedDuration;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;CWorld::pIgnoreEntity=savedIgnore;StaticRef<uint8>(0xB6EC78)=savedFailed;StaticRef<uint8>(0xB6F059)=savedActive;StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<uint32>(0xB7CB4C)=savedFrame;std::memcpy(reinterpret_cast<void*>(0x8CCB9C),savedTimes.data(),12);std::memcpy(reinterpret_cast<void*>(0x8CCDE0),savedParameters.data(),16);std::memcpy(reinterpret_cast<void*>(0xB700D4),savedCounter.data(),8);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
