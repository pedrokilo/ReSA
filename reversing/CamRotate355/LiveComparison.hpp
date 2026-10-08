#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "WeaponInfo.h"
#include "Pad.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamRotate355Live {
inline json trace;inline uint32 scenario{};inline CPad pad;
inline CPad* __cdecl Pad(int32 index){trace.push_back({"pad",index});return &pad;}
inline bool __fastcall Behind(const CPad*,void*){trace.push_back({"behind"});return scenario%7==0;}
inline bool __fastcall Left(const CPad*,void*){trace.push_back({"left"});return scenario%5==0;}
inline bool __fastcall Right(const CPad*,void*){trace.push_back({"right"});return scenario%3==0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","RotCamIfInFrontCar",0x50A4F0,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-rotate355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-rotate355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-rotate355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,sizeof(CCam)> savedCam{};std::memcpy(savedCam.data(),reinterpret_cast<void*>(0xB6F19C),savedCam.size());const auto savedStep=CTimer::GetTimeStep();const auto savedFront=StaticRef<uint8>(0xB6F042),savedRear=StaticRef<uint8>(0xB6F043),savedBack=StaticRef<uint8>(0xB6F05F),savedTransition=StaticRef<uint8>(0xB6F080),savedActive=StaticRef<uint8>(0xB6F081);
 EntryPatch a(0x53FB70,Pad),ad(bit_cast<uintptr_t>(&CPad::GetPad),Pad);
 EntryPatch b(0x53FE70,Behind),bd(bit_cast<uintptr_t>(&CPad::GetLookBehindForCar),Behind);
 EntryPatch c(0x53FDD0,Left),cd(bit_cast<uintptr_t>(&CPad::GetLookLeft),Left);
 EntryPatch d(0x53FE10,Right),dd(bit_cast<uintptr_t>(&CPad::GetLookRight),Right);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();std::memset(&pad,0,sizeof(pad));
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};auto* self=i%2==0?reinterpret_cast<CCam*>(0xB6F19C):reinterpret_cast<CCam*>(raw.data());std::memset(self,0,sizeof(CCam));
    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());vehicle[0x36]=uint8(i%4==0?3:2);At<uintptr_t>(vehicle.data(),0x14)=uintptr_t(matrix.data());At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),16)=CVector{0,1,0};At<CVector>(matrix.data(),32)=CVector{0,0,1};veh->m_vecMoveSpeed=CVector{float(int32(i%7)-3)*.125f,float((i/7)%5)*.125f,0};self->m_pCamTargetEntity=veh;
    self->m_vecSource=CVector{float(i%5)-2,float((i/5)%5)-2,3};self->m_fHorizontalAngle=float(int32((i/25)%7)-3);self->m_fBetaSpeed=0;self->m_bFixingBeta=i%3==0;self->m_nDirectionWasLooking=uint32(i%4);*reinterpret_cast<float*>(reinterpret_cast<uint8*>(self)+0x9C)=.125f;
    StaticRef<uint8>(0xB6F042)=uint8((i/4)%3);StaticRef<uint8>(0xB6F043)=uint8((i/12)%3);StaticRef<uint8>(0xB6F05F)=uint8((i/36)%3);StaticRef<uint8>(0xB6F080)=uint8((i/108)%2);StaticRef<uint8>(0xB6F081)=0;CTimer::SetTimeStep(float(i%5)*.5f);
    const CVector target{0,0,0};const float orientation=float(int32((i/216)%5)-2)*.5f;const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CCam*,const CVector&,float)>(fn.address)(self,target,orientation);
    return json{{"cam",Digest(self,sizeof(CCam))},{"vehicle",Digest(vehicle.data(),vehicle.size())},{"front",StaticRef<uint8>(0xB6F042)},{"rear",StaticRef<uint8>(0xB6F043)},{"back",StaticRef<uint8>(0xB6F05F)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 std::memcpy(reinterpret_cast<void*>(0xB6F19C),savedCam.data(),savedCam.size());CTimer::SetTimeStep(savedStep);StaticRef<uint8>(0xB6F042)=savedFront;StaticRef<uint8>(0xB6F043)=savedRear;StaticRef<uint8>(0xB6F05F)=savedBack;StaticRef<uint8>(0xB6F080)=savedTransition;StaticRef<uint8>(0xB6F081)=savedActive;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
