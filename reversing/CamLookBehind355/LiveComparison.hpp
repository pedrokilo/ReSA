#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "BaseModelInfo.h"
#include "ColModel.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "World.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamLookBehind355Live {
inline json trace;inline uint32 scenario{};inline void* hold{};
inline void __fastcall Ready(CCam* cam,void*){trace.push_back({"ready",std::bit_cast<uint32>(cam->m_vecFront.x),std::bit_cast<uint32>(cam->m_vecFront.y),std::bit_cast<uint32>(cam->m_vecFront.z),std::bit_cast<uint32>(cam->m_vecSource.z),std::bit_cast<uint32>(cam->m_vecUp.z)});}
inline void __fastcall VehicleSpecial(CCamera*,void*,CVehicle* vehicle){trace.push_back({"vehicle_special",uintptr_t(vehicle)});}
inline bool __fastcall Collision(CCamera*,void*,CVector* source,CVector* target){trace.push_back({"collision",std::bit_cast<uint32>(source->x),std::bit_cast<uint32>(source->y),std::bit_cast<uint32>(source->z),std::bit_cast<uint32>(target->x),std::bit_cast<uint32>(target->y),std::bit_cast<uint32>(target->z),uintptr_t(CWorld::pIgnoreEntity)});source->x+=.125f;target->y+=.25f;return true;}
inline void __fastcall Improve(CCamera*,void*,CVehicle* vehicle,CPed* ped,CVector* source,CVector* target){trace.push_back({"improve",uintptr_t(vehicle),uintptr_t(ped),std::bit_cast<uint32>(source->x),std::bit_cast<uint32>(target->y)});}
inline void __fastcall Duck(CCamera*,void*,CPed* ped,CVector* source,CVector* target,bool flag){trace.push_back({"duck",uintptr_t(ped),std::bit_cast<uint32>(source->z),std::bit_cast<uint32>(target->z),flag});source->z+=.25f;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back({"appearance"});return static_cast<eVehicleAppearance>((scenario/7)%5);}
inline CTaskSimpleSwim* __fastcall Swim(CPedIntelligence*,void*){trace.push_back({"swim"});return scenario%3==0?reinterpret_cast<CTaskSimpleSwim*>(0x5678):nullptr;}
inline CTaskSimpleHoldEntity* __fastcall Hold(CPedIntelligence*,void*,bool ignore){trace.push_back({"hold",ignore});return scenario%4==0?reinterpret_cast<CTaskSimpleHoldEntity*>(hold):nullptr;}
inline RwCamera* __cdecl Near(RwCamera* camera,float value){trace.push_back({"near",std::bit_cast<uint32>(value)});camera->nearPlane=value;return camera;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","LookBehind",0x520690,0,-1,8192}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-look-behind355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-look-behind355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-look-behind355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedRw=StaticRef<uintptr_t>(0xC1703C),savedIgnore=uintptr_t(CWorld::pIgnoreEntity);const auto savedPreset=StaticRef<int32>(0xB6F0F0);const auto savedDist=StaticRef<float>(0xB6F0FC);const auto savedFlags=StaticRef<uint32>(0x8A5B14);std::array<uint8,56> savedParameters;std::memcpy(savedParameters.data(),reinterpret_cast<void*>(0x8CCE18),56);std::array<uint8,28> savedCache;std::memcpy(savedCache.data(),reinterpret_cast<void*>(0xB6FC70),28);std::array<uint8,12> savedVehicleTarget;std::memcpy(savedVehicleTarget.data(),reinterpret_cast<void*>(0xB6F018),12);
 EntryPatch r(0x509CE0,Ready),rd(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Ready);
 EntryPatch v(0x50CDE0,VehicleSpecial),vd(bit_cast<uintptr_t>(&CCamera::CameraVehicleModeSpecialCases),VehicleSpecial);
 EntryPatch c(0x520190,Collision),cd(bit_cast<uintptr_t>(&CCamera::CameraColDetAndReact),Collision);
 EntryPatch im(0x516B20,Improve),imd(bit_cast<uintptr_t>(&CCamera::ImproveNearClip),Improve);
 EntryPatch d(0x50CFA0,Duck),dd(bit_cast<uintptr_t>(&CCamera::HandleCameraMotionForDucking),Duck);
 EntryPatch ap(0x6D1080,Appearance),apd(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance);
 EntryPatch sw(0x601070,Swim),swd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),Swim);
 EntryPatch ho(0x600FF0,Hold),hod(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskHold),Hold);
 EntryPatch ne(0x7EE1D0,Near),ned(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    alignas(16) std::array<uint8,4096> entity{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(RwCamera) std::array<uint8,sizeof(RwCamera)> rwRaw{};alignas(4) std::array<uint8,32> taskRaw{};
    auto* target=reinterpret_cast<CEntity*>(entity.data());entity[0x36]=uint8(i%3==0?3:i%3==1?2:4);At<uintptr_t>(entity.data(),0x14)=uintptr_t(matrix.data());At<uintptr_t>(entity.data(),0x47C)=0x1234;At<int32>(entity.data(),0x590)=i%5==0?5:0;
    At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),0x10)=CVector{float(i%3)*.25f,1,float(i%2)*.125f};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%11)-5,float(i%7),float(i%5)};
    At<CVector>(entity.data(),0x578)=CVector{float(i%5)*.125f,float(i%3)*.25f,1};At<uintptr_t>(taskRaw.data(),8)=i%8==0?uintptr_t(target):0;hold=taskRaw.data();
    auto* camera=reinterpret_cast<RwCamera*>(rwRaw.data());camera->nearPlane=float(i%4)*.05f;StaticRef<uintptr_t>(0xC1703C)=uintptr_t(camera);StaticRef<int32>(0xB6F0F0)=i%3;StaticRef<float>(0xB6F0FC)=float(i%5)*.125f;StaticRef<uint32>(0x8A5B14)=0x33445566;std::memcpy(reinterpret_cast<void*>(0xB6FC70),savedCache.data(),28);StaticRef<CVector>(0xB6F018)=CVector{3,4,5};CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(0x5566);
    for(int k=0;k<3;++k){StaticRef<float>(0x8CCE18+k*4)=float(k)*.25f;StaticRef<float>(0x8CCE24+k*4)=float(k)*.125f+.5f;StaticRef<float>(0x8CCE30+k*4)=float(k)*.25f+.1f;StaticRef<float>(0x8CCE3C+k*4)=float(k)*.25f+.2f;}StaticRef<float>(0x8CCE4C)=float(i%7)*.125f;
    constexpr int modes[]{0x12,0x16,3,0x10,4,6};self->m_nMode=static_cast<eCamMode>(modes[(i/3)%6]);self->m_pCamTargetEntity=target;self->m_vecSource=CVector{float(i%13)-6,float(i%17)-8,2.0f+float(i%7)*.25f};self->m_fFOV=70;self->m_fHorizontalAngle=float(i%25)*.25f-3;At<float>(raw.data(),0xD0)=float(i%8)+1;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CCam*)>(fn.address)(self);
    return json{{"cam",Digest(raw.data(),raw.size())},{"entity",Digest(entity.data(),entity.size())},{"rw",Digest(rwRaw.data(),rwRaw.size())},{"cache",Digest(reinterpret_cast<void*>(0xB6FC70),28)},{"flags",StaticRef<uint32>(0x8A5B14)},{"ignore",uintptr_t(CWorld::pIgnoreEntity)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xC1703C)=savedRw;CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(savedIgnore);StaticRef<int32>(0xB6F0F0)=savedPreset;StaticRef<float>(0xB6F0FC)=savedDist;StaticRef<uint32>(0x8A5B14)=savedFlags;std::memcpy(reinterpret_cast<void*>(0x8CCE18),savedParameters.data(),56);std::memcpy(reinterpret_cast<void*>(0xB6FC70),savedCache.data(),28);std::memcpy(reinterpret_cast<void*>(0xB6F018),savedVehicleTarget.data(),12);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
