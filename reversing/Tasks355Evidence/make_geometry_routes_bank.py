from pathlib import Path
p=Path('reversing/GeometryRoutes355');p.mkdir(exist_ok=True)
s=r'''#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGeometryAnalyser.h"
#include "Interior/InteriorManager_c.h"
#include "../ProbeRuntime.hpp"
namespace GeometryRoutes355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},lineCalls{};
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CPedGeometryAnalyser","ComputeEntityBoundingBoxCornersUncached",0x5F1FA0,0,0,1024},
{"CPedGeometryAnalyser","ComputeRouteRoundEntityBoundingBox-1",0x5F6110,0,1,1024},
{"CPedGeometryAnalyser","ComputeRouteRoundEntityBoundingBox-2",0x5F3DD0,0,2,1024}};
alignas(16)inline std::array<uint8,sizeof(CPed)>ped,obstacle;
alignas(16)inline std::array<uint8,sizeof(CAutomobile)>entity;
alignas(16)inline std::array<uint8,sizeof(CMatrixLink)>matrix,entityMatrix,obstacleMatrix;
alignas(16)inline std::array<uint8,sizeof(CBaseModelInfo)>modelInfo,obstacleInfo;
alignas(16)inline std::array<uint8,sizeof(CColModel)>colModel,obstacleCol;
alignas(16)inline std::array<uint8,sizeof(CCollisionData)>colData;
inline std::array<CColSphere,2>spheres;
inline std::array<CColBox,2>boxes;
inline std::array<CVector,4>output;
inline CVector from,target;inline CPointRoute route;
template<class T>inline void Put(void*p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+sizeof(x))return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(obstacle) P(entity) P(matrix) P(entityMatrix) P(obstacleMatrix) P(modelInfo) P(obstacleInfo) P(colModel) P(obstacleCol) P(colData) P(spheres) P(boxes) P(output)
#undef P
return p;}
inline json Words(const void*p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(Ptr(static_cast<const uint32*>(p)[i]));return j;}
inline uint32 B(float x){return std::bit_cast<uint32>(x);}inline json V(const CVector&v){return json::array({B(v.x),B(v.y),B(v.z)});}
inline bool __fastcall Interior(InteriorManager_c*,void*,CEntity*e,CVector*v){bool success=(scenario&4)!=0;trace.push_back({"interior",Ptr(uintptr_t(e)),success});if(success){v[0]=CVector{2.f,2.f,9.f};v[1]=CVector{-2.f,2.f,9.f};v[2]=CVector{-2.f,-2.f,9.f};v[3]=CVector{2.f,-2.f,9.f};}return success;}
inline void __cdecl Corners(float z,CEntity&e,CVector*v){trace.push_back({"corners",B(z),Ptr(uintptr_t(&e)),B(*reinterpret_cast<float*>(0x8D22B0))});const auto p=e.GetPosition();const auto padding=*reinterpret_cast<float*>(0x8D22B0);const auto x=1.f+padding,y=2.f+padding;v[0]=CVector{p.x+x,p.y+y,z};v[1]=CVector{p.x-x,p.y+y,z};v[2]=CVector{p.x-x,p.y-y,z};v[3]=CVector{p.x+x,p.y-y,z};}
inline bool __cdecl Line(const CVector&a,const CVector&b,CColPoint&out,CEntity*&e,bool c,bool d,bool f,bool g,bool h,bool j,bool k,bool l){++lineCalls;bool success=(scenario&8)!=0 && (lineCalls%3==(scenario/16)%3);e=(scenario&32)?reinterpret_cast<CEntity*>(entity.data()):(scenario&64)?reinterpret_cast<CEntity*>(ped.data()):reinterpret_cast<CEntity*>(obstacle.data());out.m_vecPoint=b;trace.push_back({"line",V(a),V(b),c,d,f,g,h,j,k,l,success,Ptr(uintptr_t(e))});return success;}
inline void __cdecl BoundSphere(const CPed&,CEntity&e,CColSphere&s){trace.push_back({"sphere",Ptr(uintptr_t(&e))});s.m_vecCenter=e.GetPosition();s.m_fRadius=3.f;}
inline ReversibleHooks::ReversibleHook::Base*FindHook(const char*c,const char*n){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()==c)for(auto&h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"geometry-routes355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"geometry-routes355-runtime.jsonl");ReversingProbe::InstallFailureCapture("geometry-routes355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"geometry-routes355-only.txt");if(f)std::getline(f,only);}bool pass=true,selected=false;int total=0;
for(auto&fn:fns){auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto*h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ie(0x598090,Interior),id(bit_cast<uintptr_t>(&InteriorManager_c::GetBoundingBox),Interior);
EntryPatch le(0x56BA00,Line),ld(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
std::unique_ptr<EntryPatch>ce,cd,se,sd;if(fn.slot!=0){ce=std::make_unique<EntryPatch>(0x5F1FA0,Corners);cd=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached),Corners);se=std::make_unique<EntryPatch>(0x5F3C20,BoundSphere);sd=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingSphere),BoundSphere);}
auto oldModel=CModelInfo::ms_modelInfoPtrs[100],oldObstacle=CModelInfo::ms_modelInfoPtrs[101];CModelInfo::ms_modelInfoPtrs[100]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());CModelInfo::ms_modelInfoPtrs[101]=reinterpret_cast<CBaseModelInfo*>(obstacleInfo.data());const auto oldPadding=*reinterpret_cast<float*>(0x8D22B0);
for(int i=0;i<fn.cases;++i){scenario=i%256;json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);ped.fill(0);obstacle.fill(0);entity.fill(0);matrix.fill(0);entityMatrix.fill(0);obstacleMatrix.fill(0);modelInfo.fill(0);obstacleInfo.fill(0);colModel.fill(0);obstacleCol.fill(0);colData.fill(0);trace=json::array();lineCalls=0;*reinterpret_cast<float*>(0x8D22B0)=.35f;
auto*p=reinterpret_cast<CPed*>(ped.data());auto*e=reinterpret_cast<CEntity*>(entity.data());auto*o=reinterpret_cast<CPed*>(obstacle.data());auto*m=reinterpret_cast<CMatrixLink*>(matrix.data());auto*em=reinterpret_cast<CMatrixLink*>(entityMatrix.data());auto*om=reinterpret_cast<CMatrixLink*>(obstacleMatrix.data());auto*cm=reinterpret_cast<CColModel*>(colModel.data());auto*oc=reinterpret_cast<CColModel*>(obstacleCol.data());auto*cc=reinterpret_cast<CCollisionData*>(colData.data());p->m_matrix=m;e->m_matrix=em;o->m_matrix=om;p->m_nModelIndex=e->m_nModelIndex=100;o->m_nModelIndex=101;Put(modelInfo.data(),0x14,cm);Put(obstacleInfo.data(),0x14,oc);cm->m_pColData=cc;cm->m_boundSphere.m_fRadius=3.f;oc->m_boundSphere.m_fRadius=float((i/32)%4)+1.f;
for(auto*t:{m,em,om}){t->GetRight()=CVector{1.f,0.f,0.f};t->GetForward()=CVector{0.f,1.f,0.f};t->GetUp()=CVector{0.f,0.f,1.f};}
if(fn.slot==0){const auto mode=(i/8)%4;if(mode==1){em->GetRight()=CVector{0.f,0.f,1.f};em->GetUp()=CVector{1.f,0.f,0.f};}if(mode==2){em->GetForward()=CVector{0.f,0.f,1.f};em->GetUp()=CVector{0.f,1.f,0.f};}if(mode==3){em->GetRight()=CVector{.5f,.5f,0.f};em->GetForward()=CVector{-.5f,.5f,0.f};}}
em->GetPosition()=CVector{float((i/128)%3)-1.f,float((i/384)%3),0.f};cm->m_boundBox.m_vecMin=CVector{-1.f,-2.f,-1.f};cm->m_boundBox.m_vecMax=CVector{1.f,2.f,float((i/16)%3)*4.f+1.f};cc->m_pSpheres=spheres.data();cc->m_pBoxes=boxes.data();cc->m_nNumSpheres=(scenario&16)?2:0;cc->m_nNumBoxes=(scenario&32)?2:0;
for(auto k=0;k<2;++k){spheres[k].m_vecCenter=CVector{float(k),0.f,float(k*3)};spheres[k].m_fRadius=.5f;boxes[k].m_vecMin=CVector{-1.f,-1.f,float(k*3)};boxes[k].m_vecMax=CVector{1.f,1.f,float(k*3)+1.f};}
e->m_info.m_nType=(scenario&1)?ENTITY_TYPE_BUILDING:ENTITY_TYPE_OBJECT;e->m_bIsTempBuilding=(scenario&2)!=0;o->m_info.m_nType=(scenario&128)?ENTITY_TYPE_PED:ENTITY_TYPE_VEHICLE;o->m_nPedState=static_cast<ePedState>((scenario&4)?1:0);
const auto direction=i%8;const CVector starts[]{CVector{-4.f,0.f,0.f},CVector{4.f,0.f,0.f},CVector{0.f,-5.f,0.f},CVector{0.f,5.f,0.f},CVector{-4.f,-4.f,0.f},CVector{4.f,4.f,0.f},CVector{0.f,0.f,0.f},CVector{4.f,5.f,0.f}};from=starts[direction];target=-from;target.z=0.f;m->GetPosition()=from;for(auto&v:output)v=CVector{99.f,98.f,97.f};std::memset(&route,0xCD,sizeof(route));
int32 ret{};if(fn.slot==0)reinterpret_cast<void(__cdecl*)(float,CEntity&,CVector*)>(fn.address)(float((i/64)%5)-1.f,*e,output.data());else if(fn.slot==1)ret=reinterpret_cast<int32(__cdecl*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32)>(fn.address)(*p,*e,target,route,(i/8)%4);else ret=reinterpret_cast<int32(__cdecl*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32)>(fn.address)(*p,from,*e,target,route,(i/8)%4);
if(candidate){auto n=std::to_string(ret);coverage[n]=coverage.value(n,0)+1;for(auto&t:trace){auto k=t[0].get<std::string>();coverage[k]=coverage.value(k,0)+1;}}
result[candidate]={{"return",ret},{"corners",Words(output.data(),sizeof(output))},{"route",Words(&route,sizeof(route))},{"ped",Words(ped.data(),ped.size())},{"entity",Words(entity.data(),entity.size())},{"obstacle",Words(obstacle.data(),obstacle.size())},{"matrix",Words(matrix.data(),matrix.size())},{"entity_matrix",Words(entityMatrix.data(),entityMatrix.size())},{"padding",B(*reinterpret_cast<float*>(0x8D22B0))},{"trace",trace}};
}bool equal=result[0]==result[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total++},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
CModelInfo::ms_modelInfoPtrs[100]=oldModel;CModelInfo::ms_modelInfoPtrs[101]=oldObstacle;*reinterpret_cast<float*>(0x8D22B0)=oldPadding;
}h->State(hs);h->LockState(hl);Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",true}});
}
}
'''
p.joinpath('LiveComparison.hpp').write_text(s)
run=Path('reversing/Boat355/run_local.py').read_text().replace('boat355','geometry-routes355').replace('KillPedFromBoat pendientes','Rutas y corners pendientes');p.joinpath('run_local.py').write_text(run)
import runpy
sources=runpy.run_path('reversing/GeometryQueries355/build_probe.py')['SOURCES']+['source/game_sa/Collision/CollisionData.cpp','source/game_sa/Collision/CollisionData.h','source/game_sa/Collision/ColBox.cpp','source/game_sa/Collision/ColBox.h','source/game_sa/Route.hpp']
p.joinpath('build_probe.py').write_text("import json,sys\nfrom pathlib import Path\nsys.path.insert(0,str(Path(__file__).resolve().parents[1]))\nfrom probe_build import build\nSOURCES="+repr(sources)+"\nif __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'geometry-routes355','GeometryRoutes355Live',SOURCES)))\n")
p.joinpath('README.md').write_text('# GeometryRoutes355\n\n3072 pares preparados: corners 1024, ambas rutas 1024 cada una. Corners usa matrices orientadas por tres ejes, cajas altas con selección por Z, esferas, cajas y fallback de InteriorManager, con cuatro vectores completos. Rutas incluyen puntos en ocho posiciones, elección automática y forzada, obstáculos vehículo/ped, ignorados ped/entidad, ruta completa incluyendo entradas inactivas y padding restaurado. Mundo/InteriorManager aislados por pares; rutas aíslan corners y bounding sphere y ejecutan realmente sus decisiones y plano/segmento. Devuelven memoria completa y trazas; no comprueba navegación física en mapa. Pendiente de ejecución. Hooks false.\n')
print('missing',[x for x in sources if not Path(x).exists()])
