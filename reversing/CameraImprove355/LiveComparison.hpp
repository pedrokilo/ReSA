#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "PedModelInfo.h"
#include "CollisionData.h"
#include "ColModel.h"
#include "ColSphere.h"
#include "PedIntelligence.h"
#include "WaterLevel.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraImprove355Live {
inline json trace;inline uint32 scenario{};inline uintptr_t swimTask{};
inline RwCamera* __cdecl Near(RwCamera* camera,float clip){trace.push_back({"near",std::bit_cast<uint32>(clip)});camera->nearPlane=clip;return camera;}
inline bool __fastcall Parachute(CPedIntelligence*,void*){trace.push_back({"parachute"});return scenario%5==0;}
inline CTaskSimpleSwim* __fastcall Swim(CPedIntelligence*,void*){trace.push_back({"swim"});return scenario%3==0?reinterpret_cast<CTaskSimpleSwim*>(swimTask):nullptr;}
inline CTaskSimpleJetPack* __fastcall Jet(CPedIntelligence*,void*){trace.push_back({"jet"});return scenario%7==0?reinterpret_cast<CTaskSimpleJetPack*>(swimTask):nullptr;}
inline bool __cdecl Water(float,float,float z,float& out,uint8 touching,CVector*){trace.push_back({"water",touching});out=z+float(scenario%3)*.25f;return scenario%5!=0;}
inline float __fastcall Ground(CCamera*,void*,eGroundHeightType type){trace.push_back({"ground",int(type)});return float(scenario%5);}
inline float __fastcall Rough(CCamera*,void*){trace.push_back({"rough"});return float(scenario%7);}
inline CColModel* __fastcall Animate(CPedModelInfo* model,void*,RpClump*){trace.push_back({"animate"});return model->m_pHitColModel;}
inline bool __cdecl Check(const CVector& src,const CVector& front,float& nearest){trace.push_back({"check",std::bit_cast<uint32>(src.x),std::bit_cast<uint32>(front.x),std::bit_cast<uint32>(nearest)});nearest=0;return false;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","ImproveNearClip",0x516B20,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-improve355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-improve355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-improve355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedScene=Scene.m_pRwCamera;const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedDistance=StaticRef<float>(0x8CCB84),savedRadius=StaticRef<float>(0xB6EC6C);const auto savedActive=StaticRef<uint8>(0xB6F081);std::array<uint8,sizeof(CCam)> savedCam{};std::memcpy(savedCam.data(),reinterpret_cast<void*>(0xB6F19C),savedCam.size());const auto savedSwim=StaticRef<int32>(0xB6F0F0);
 EntryPatch a(0x7EE1D0,Near),ad(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near);
 EntryPatch b(0x6011B0,Parachute),bd(bit_cast<uintptr_t>(&CPedIntelligence::GetUsingParachute),Parachute);
 EntryPatch c(0x601070,Swim),cd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),Swim);
 EntryPatch d(0x601110,Jet),dd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),Jet);
 EntryPatch e(0x6EB690,Water),ed(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
 EntryPatch f(0x514B80,Ground),fd(bit_cast<uintptr_t>(&CCamera::CalculateGroundHeight),Ground);
 EntryPatch g(0x516B00,Rough),gd(bit_cast<uintptr_t>(&CCamera::GetRoughDistanceToGround),Rough);
 EntryPatch h(0x4C7170,Animate),hd(bit_cast<uintptr_t>(&CPedModelInfo::AnimatePedColModelSkinnedWorld),Animate);
 EntryPatch j(0x4154A0,Check),jd(bit_cast<uintptr_t>(&CCollision::CheckPeds),Check);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};auto* self=reinterpret_cast<CCamera*>(raw.data());self->m_nActiveCam=0;self->GetActiveCam().m_vecSource=CVector{1,2,float((i/32)%7)};self->GetActiveCam().m_vecFront=CVector{.125f,.5f,1};alignas(CPed) std::array<uint8,sizeof(CPed)> pedRaw{};auto* ped=reinterpret_cast<CPed*>(pedRaw.data());At<int16>(pedRaw.data(),0x22)=2000;ped->bIsStanding=i%2!=0;At<uintptr_t>(pedRaw.data(),0x47C)=0x1234;alignas(CTask) std::array<uint8,32> task{};At<int16>(task.data(),0xA)=i%4==0?4:0;swimTask=uintptr_t(task.data());alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());veh->m_nVehicleSubType=eVehicleType((i/4)%5);
    alignas(CPedModelInfo) std::array<uint8,sizeof(CPedModelInfo)> modelRaw{};auto* model=reinterpret_cast<CPedModelInfo*>(modelRaw.data());alignas(CColModel) std::array<uint8,sizeof(CColModel)> colRaw{};auto* col=reinterpret_cast<CColModel*>(colRaw.data());alignas(CCollisionData) std::array<uint8,sizeof(CCollisionData)> dataRaw{};auto* data=reinterpret_cast<CCollisionData*>(dataRaw.data());std::array<CColSphere,12> spheres{};for(size_t k=0;k<12;++k){spheres[k].m_vecCenter=CVector{float(k)*.125f,float((i/8)%5),float(i%7)*.25f};spheres[k].m_fRadius=.2f;spheres[k].m_Surface.m_nPiece=uint8(k%3==0?9:0);}data->m_pSpheres=spheres.data();col->m_pColData=data;model->m_pHitColModel=col;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model);
    alignas(RwCamera) std::array<uint8,sizeof(RwCamera)> rwRaw{};auto* rw=reinterpret_cast<RwCamera*>(rwRaw.data());rw->nearPlane=float((i/128)%5)*.125f;Scene.m_pRwCamera=rw;StaticRef<float>(0x8CCB84)=float((i/256)%7)*.25f;StaticRef<float>(0xB6EC6C)=float((i/1792)%3)*.25f;StaticRef<uint8>(0xB6F081)=0;auto& cam=StaticRef<CCam>(0xB6F19C);std::memset(&cam,0,sizeof(cam));cam.m_vecFront=CVector{.125f,.5f,1};cam.m_vecSource=CVector{1,2,3};cam.m_fFOV=float((i/16)%5)*15;StaticRef<int32>(0xB6F0F0)=i%3==0?1:0;
    CVector source{float(i%7),float((i/7)%5),float((i/35)%7)},target{0,0,0};const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCamera*,CVehicle*,CPed*,CVector*,CVector*)>(fn.address)(self,i%4==0?veh:nullptr,i%11==0?nullptr:ped,&source,&target);
    return json{{"camera",Digest(raw.data(),raw.size())},{"ped",Digest(pedRaw.data(),pedRaw.size())},{"rw",Digest(rwRaw.data(),rwRaw.size())},{"source",{F(source.x),F(source.y),F(source.z)}},{"spheres",Digest(spheres.data(),sizeof(spheres))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 Scene.m_pRwCamera=savedScene;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;StaticRef<float>(0x8CCB84)=savedDistance;StaticRef<float>(0xB6EC6C)=savedRadius;StaticRef<uint8>(0xB6F081)=savedActive;std::memcpy(reinterpret_cast<void*>(0xB6F19C),savedCam.data(),savedCam.size());StaticRef<int32>(0xB6F0F0)=savedSwim;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
