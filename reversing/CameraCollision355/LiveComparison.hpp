#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraCollision355Live {
inline json trace;inline uint32 scenario{};
inline uintptr_t fakeMatrix{};inline CColModel* fakeCol{};
inline CColModel* __fastcall Col(CEntity* entity,void*){trace.push_back({"col",uintptr_t(entity)});return fakeCol;}
inline void __fastcall Allocate(CPlaceable* p,void*){trace.push_back({"allocate",uintptr_t(p)});*reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8*>(p)+0x14)=fakeMatrix;}
inline void __fastcall UpdateMatrix(CSimpleTransform* p,void*,CMatrix* matrix){trace.push_back({"update_matrix",uintptr_t(p),uintptr_t(matrix)});}
inline bool __fastcall Cone(CCamera* camera,void*,const CVector& source,const CVector& target,CVector& out,float radius,float minDistance,float& fraction){trace.push_back({"cone",uintptr_t(camera),std::bit_cast<uint32>(source.x),std::bit_cast<uint32>(source.y),std::bit_cast<uint32>(source.z),std::bit_cast<uint32>(target.x),std::bit_cast<uint32>(target.y),std::bit_cast<uint32>(target.z),std::bit_cast<uint32>(radius),std::bit_cast<uint32>(minDistance)});const std::array<float,7> values{0,.25f,.5f,1,2,-1,std::bit_cast<float>(0x7FC00001u)};fraction=values[(scenario/16)%7];out=CVector{3,4,5};return scenario%3!=0;}
inline RwCamera* __cdecl Near(RwCamera* camera,float clip){trace.push_back({"near",uintptr_t(camera),std::bit_cast<uint32>(clip)});return camera;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","CameraColDetAndReact",0x520190,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-collision355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-collision355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-collision355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,24> savedHistory{};std::memcpy(savedHistory.data(),reinterpret_cast<void*>(0xB700DC),24);std::array<uint8,8> savedVars{};std::memcpy(savedVars.data(),reinterpret_cast<void*>(0x8CCB80),8);const auto savedPointer=gpCamColVars;const auto savedIgnore=CWorld::pIgnoreEntity;const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedRadius=StaticRef<float>(0xB6EC6C);const auto savedStep=CTimer::GetTimeStep();
 EntryPatch a(0x535300,Col),ad(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch b(0x54F560,Allocate),bd(bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix),Allocate);
 EntryPatch c(0x54F1B0,UpdateMatrix),cd(bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix),UpdateMatrix);
 EntryPatch d(0x51A5D0,Cone),dd(bit_cast<uintptr_t>(&CCamera::ConeCastCollisionResolve),Cone);
 EntryPatch e(0x7EE1D0,Near),ed(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    alignas(16) std::array<uint8,sizeof(CVehicle)> entity{};alignas(16) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(16) std::array<uint8,sizeof(CBaseModelInfo)> model{};alignas(16) std::array<uint8,sizeof(CColModel)> col{};alignas(16) std::array<uint8,sizeof(CCollisionData)> data{};alignas(16) std::array<uint8,sizeof(CColSphere)*3> spheres{};
    auto* vehicle=reinterpret_cast<CVehicle*>(entity.data());entity[0x36]=uint8((i/2)%4==3?3:2);vehicle->m_nVehicleSubType=eVehicleType((i/2)%4==2?9:0);vehicle->m_nVehicleType=eVehicleType((i/2)%4==2?9:0);At<int16>(entity.data(),0x22)=2000;At<uintptr_t>(entity.data(),0x14)=i%2?uintptr_t(matrix.data()):0;fakeMatrix=uintptr_t(matrix.data());fakeCol=reinterpret_cast<CColModel*>(col.data());CWorld::pIgnoreEntity=(i/2)%4==0?nullptr:reinterpret_cast<CEntity*>(entity.data());
    At<CVector>(matrix.data(),0x20)=CVector{float(i%3)*.25f,float(i%5)*.125f,1};At<CVector>(matrix.data(),0x30)=CVector{1,2,3};At<void*>(model.data(),0x14)=col.data();StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());At<CVector>(col.data(),0)=CVector{-1,-2,-3};At<CVector>(col.data(),12)=CVector{1,2,3};At<void*>(col.data(),0x2C)=i%5==0?nullptr:data.data();At<int16>(data.data(),0)=int16(int32((i/5)%5)-1);At<void*>(data.data(),8)=spheres.data();for(int j=0;j<3;++j){At<CVector>(spheres.data()+j*sizeof(CColSphere),0)=CVector{0,0,float(j)-1};At<float>(spheres.data()+j*sizeof(CColSphere),12)=float(j+1)*.25f;}
    std::array<float,6> vars{float((i/128)%5)*.125f,float((i/640)%5)*.5f,.1f,.5f,.05f,.125f};gpCamColVars=vars.data();gCurCamColVars=uint8((i/8)%14);StaticRef<float>(0x8CCB84)=float((i/112)%7)*.25f;StaticRef<uint32>(0xB700E8)=uint32((i/4)%2)|0xABCD0000u;StaticRef<CVector>(0xB700DC)=CVector{float(i%7),float(i%3),float(i%5)};StaticRef<float>(0xB700EC)=float((i/256)%5)-2;StaticRef<int32>(0xB700F0)=i%3==0?2000:-1;CTimer::SetTimeStep(float(i%5)*.5f);
    CVector source{float(i%17)-8,float((i/17)%17)-8,float((i/289)%7)-3},target{.5f,-.5f,.25f};
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CCamera*,CVector*,CVector*)>(fn.address)(self,&source,&target);
    return json{{"camera",Digest(raw.data(),raw.size())},{"entity",Digest(entity.data(),entity.size())},{"matrix",Digest(matrix.data(),matrix.size())},{"source",{F(source.x),F(source.y),F(source.z)}},{"target",{F(target.x),F(target.y),F(target.z)}},{"radius",F(StaticRef<float>(0xB6EC6C))},{"distance",F(StaticRef<float>(0x8CCB84))},{"history",Digest(reinterpret_cast<void*>(0xB700DC),24)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 std::memcpy(reinterpret_cast<void*>(0xB700DC),savedHistory.data(),24);std::memcpy(reinterpret_cast<void*>(0x8CCB80),savedVars.data(),8);gpCamColVars=savedPointer;CWorld::pIgnoreEntity=savedIgnore;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;StaticRef<float>(0xB6EC6C)=savedRadius;CTimer::SetTimeStep(savedStep);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
