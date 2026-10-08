#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Weapon.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "CollisionData.h"
#include "Automobile.h"
#include "SurfaceInfos_c.h"
#include "Collision/ColLine.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace WeaponOccupant355Live {
inline json trace;inline uint32 scenario{},sphereCall{};inline std::array<RwMatrix,9> heads;
inline CColModel* activeCol{};
inline CColModel* __fastcall GetCol(CEntity*,void*){trace.push_back({"col"});return activeCol;}
inline RpHAnimHierarchy* __cdecl Hierarchy(RpClump* clump){trace.push_back({"hierarchy",uintptr_t(clump)});return reinterpret_cast<RpHAnimHierarchy*>(0x11223344);}
inline int32 __cdecl Index(RpHAnimHierarchy*,int32 id){trace.push_back({"index",id});return id;}
inline RwMatrix* __cdecl Matrices(RpHAnimHierarchy*){trace.push_back({"matrices"});return heads.data();}
inline bool __cdecl Sphere(const CColLine& line,const CColSphere& sphere,CColPoint& out,float& depth){trace.push_back({"sphere",std::bit_cast<uint32>(line.m_vecStart.x),std::bit_cast<uint32>(line.m_vecEnd.y),std::bit_cast<uint32>(sphere.m_vecCenter.x),std::bit_cast<uint32>(sphere.m_vecCenter.y),std::bit_cast<uint32>(sphere.m_vecCenter.z),std::bit_cast<uint32>(sphere.m_fRadius),std::bit_cast<uint32>(depth)});const bool hit=(sphereCall+++scenario)%3==0;if(hit){depth=float(sphereCall)*.0625f;out.m_vecPoint=CVector{sphere.m_vecCenter.x,2,3};out.m_vecNormal=CVector{0,1,0};out.m_nPieceTypeB=9;out.m_fDepth=depth;}return hit;}
inline bool __fastcall Glass(SurfaceInfos_c*,void*,SurfaceId material){trace.push_back({"glass",int(material)});return (uint32(material)+scenario)%3!=0;}
inline void __cdecl Planes(CColModel*){trace.push_back({"planes"});}
inline bool __cdecl Triangle(const CColLine& line,const CompressedVector*,const CColTriangle& tri,const CColTrianglePlane&){trace.push_back({"triangle",int(tri.m_nMaterial),std::bit_cast<uint32>(line.m_vecStart.x),std::bit_cast<uint32>(line.m_vecEnd.z)});return (uint32(tri.m_nMaterial)+scenario)%2!=0;}
inline bool __fastcall Progress(CDamageManager* damage,void*,ePanels panel){trace.push_back({"progress",int(panel)});reinterpret_cast<uint8*>(damage)[0]++;return scenario%4!=0;}
inline ePanelDamageState __fastcall Panel(CDamageManager*,void*,ePanels panel){trace.push_back({"panel",int(panel)});return static_cast<ePanelDamageState>(scenario%3);}
inline void __fastcall SetPanel(CAutomobile*,void*,ePanels panel,bool glass){trace.push_back({"set_panel",int(panel),glass});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CWeapon","CheckForShootingVehicleOccupant",0x73F480,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"weapon-occupant355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"weapon-occupant355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapon-occupant355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch getCol(0x535300,GetCol),getColDll(bit_cast<uintptr_t>(&CEntity::GetColModel),GetCol);
 EntryPatch h(0x734A40,Hierarchy),hd(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy),idx(0x7C51A0,Index),ma(0x7C5120,Matrices);
 EntryPatch sp(0x412AA0,Sphere),spd(bit_cast<uintptr_t>(&CCollision::ProcessLineSphere),Sphere);
 EntryPatch gl(0x55E790,Glass),gld(bit_cast<uintptr_t>(&SurfaceInfos_c::IsGlass),Glass);
 EntryPatch pl(0x418580,Planes),pld(bit_cast<uintptr_t>(static_cast<void(*)(CColModel*)>(&CCollision::CalculateTrianglePlanes)),Planes);
 EntryPatch tr(0x413AC0,Triangle),trd(bit_cast<uintptr_t>(&CCollision::TestLineTriangle),Triangle);
 EntryPatch pr(0x6C23C0,Progress),prd(bit_cast<uintptr_t>(&CDamageManager::ProgressPanelDamage),Progress);
 EntryPatch pa(0x6C2180,Panel),pad(bit_cast<uintptr_t>(&CDamageManager::GetPanelStatus),Panel);
 EntryPatch se(0x6B1480,SetPanel),sed(bit_cast<uintptr_t>(&CAutomobile::SetPanelDamage),SetPanel);
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CAutomobile) std::array<uint8,sizeof(CAutomobile)> car{};alignas(CPed) std::array<uint8,sizeof(CPed)> peds[9]{};std::array<uint8,32> bones[9]{};std::array<uint8,64> clumps[9]{};
    alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(CBaseModelInfo) std::array<uint8,sizeof(CBaseModelInfo)> model{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};alignas(CCollisionData) std::array<uint8,sizeof(CCollisionData)> dataRaw{};auto* data=reinterpret_cast<CCollisionData*>(dataRaw.data());
    std::array<CColTriangle,3> triangles{};alignas(CColTrianglePlane) std::array<uint8,sizeof(CColTrianglePlane)*3> planeBytes{};std::array<CompressedVector,3> vertices{};
    activeCol=reinterpret_cast<CColModel*>(col.data());auto* vehicle=reinterpret_cast<CAutomobile*>(car.data());car[0x36]=uint8(i%13==0?3:2);car[0x429]=uint8(i%4==0?4:0);vehicle->m_nVehicleType=static_cast<eVehicleType>((i/7)%2?5:0);vehicle->m_nVehicleSubType=static_cast<eVehicleType>(i%5==0?5:0);At<int16>(car.data(),0x22)=2000;At<uintptr_t>(car.data(),0x14)=uintptr_t(matrix.data());At<uintptr_t>(model.data(),0x14)=uintptr_t(col.data());reinterpret_cast<CColModel*>(col.data())->m_pColData=data;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());
    At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),0x10)=CVector{0,1,0};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%7),float(i%5),2};
    data->m_nNumTriangles=uint16(i%4);data->m_pTriangles=triangles.data();data->m_pTrianglePlanes=reinterpret_cast<CColTrianglePlane*>(planeBytes.data());data->m_pVertices=vertices.data();
    for(int k=0;k<3;++k)triangles[k].m_nMaterial=static_cast<eSurfaceType>(k+1);
    for(int k=0;k<9;++k){auto* ped=reinterpret_cast<CPed*>(peds[k].data());At<uint32>(peds[k].data(),0x470)=((i+k)%4)?0x4000000:0;clumps[k][0]=2;At<uintptr_t>(peds[k].data(),0x18)=uintptr_t(clumps[k].data());At<uintptr_t>(peds[k].data(),0x490)=uintptr_t(bones[k].data());At<int32>(bones[k].data(),0x14)=k;std::memset(&heads[k],0,sizeof(heads[k]));heads[k].right={1,0,0};heads[k].up={0,1,0};heads[k].at={0,0,1};heads[k].pos={float(k),float(i%7),float(i%3)};
      if(k==0)vehicle->m_pDriver=i%3?ped:nullptr;else vehicle->m_apPassengers[k-1]=(i+k)%3?ped:nullptr;
    }
    CEntity* hitEntity=vehicle;CColPoint point;std::memset(&point,uint8(i*17),sizeof(point));const CVector origin{1,2,3},target{float(i%5)-2,float(i%7)-3,float(i%9)-4};sphereCall=0;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)(CEntity**,CColPoint*,eWeaponType,const CVector&,const CVector&)>(fn.address)(&hitEntity,&point,WEAPON_PISTOL,origin,target);
    const auto pedHash=Digest(peds,sizeof(peds));
    return json{{"vehicle",Digest(car.data(),car.size())},{"peds",pedHash},{"point",Digest(&point,sizeof(point))},{"hit",uintptr_t(hitEntity)},{"trace",trace},{"switches",switched}};
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
