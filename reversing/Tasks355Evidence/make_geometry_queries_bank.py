from pathlib import Path
p=Path('reversing/GeometryQueries355');p.mkdir(exist_ok=True)
s=r'''#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGeometryAnalyser.h"
#include "SurfaceInfos_c.h"
#include "WaterLevel.h"
#include "../ProbeRuntime.hpp"
namespace GeometryQueries355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},verticalCalls{},rayCalls{};
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CPedGeometryAnalyser","CanPedJumpObstacle-",0x5F1B00,0,0,512},
{"CPedGeometryAnalyser","CanPedJumpObstacle-contacted",0x5F32D0,0,1,1024},
{"CPedGeometryAnalyser","CanPedTargetPoint",0x5F1B70,0,2,512},
{"CPedGeometryAnalyser","ComputeBuildingHitPoints",0x5F1E30,0,3,256},
{"CPedGeometryAnalyser","ComputeClearTarget",0x5F5D80,0,4,512},
{"CPedGeometryAnalyser","ComputeClosestSurfacePoint-posn",0x5F36F0,0,5,512},
{"CPedGeometryAnalyser","ComputeEntityBoundingBoxPlanes",0x5F3660,0,6,512},
{"CPedGeometryAnalyser","ComputeEntityBoundingSphere",0x5F3C20,0,7,512},
{"CPedGeometryAnalyser","ComputeMoveDirToAvoidEntity",0x5F3730,0,8,512},
{"CPedGeometryAnalyser","ComputeEntityHitSide-1",0x5F3BC0,0,9,512},
{"CPedGeometryAnalyser","ComputeEntityHitSide-3",0x5F3AC0,0,10,512},
{"CPedGeometryAnalyser","ComputePedHitSide-physical",0x5F3640,0,11,512},
{"CPedGeometryAnalyser","ComputeRouteRoundSphere",0x5F1890,0,12,512},
{"CPedGeometryAnalyser","GetIsLineOfSightClear-ped",0x5F5A30,0,13,1024},
{"CPedGeometryAnalyser","GetIsLineOfSightClear-v3d",0x5F2F00,0,14,512},
{"CPedGeometryAnalyser","GetNearestPed",0x5F3590,0,15,512},
{"CPedGeometryAnalyser","IsEntityBlockingTarget",0x5F3970,0,16,512},
{"CPedGeometryAnalyser","IsInAir",0x5F1CB0,0,17,1024},
{"CPedGeometryAnalyser","IsWanderPathClear",0x5F2F70,0,18,1024},
{"CPedGeometryAnalyser","LiesInsideBoundingBox",0x5F3880,0,19,512}};
alignas(16)inline std::array<uint8,sizeof(CPed)>ped;
alignas(16)inline std::array<uint8,sizeof(CAutomobile)>entity;
alignas(16)inline std::array<uint8,sizeof(CMatrixLink)>matrix,entityMatrix;
alignas(16)inline std::array<uint8,sizeof(CPedIntelligence)>intel;
alignas(16)inline std::array<uint8,sizeof(CBaseModelInfo)>modelInfo;
alignas(16)inline std::array<uint8,sizeof(CColModel)>colModel;
alignas(16)inline std::array<uint8,sizeof(CCollisionData)>colData;
alignas(16)inline std::array<uint8,sizeof(CColSphere)>colSphere;
alignas(16)inline std::array<uint8,64>subtask,pool;
alignas(16)inline std::array<uint8,4*0x7C4>poolObjects;
inline std::array<uint8,4>poolFlags;
inline std::array<void*,11>subVT;inline int subtype{};
inline std::array<CVector,4>output;
inline std::array<float,4>dots;
inline CVector point,from,contact;inline CColSphere sphere;
template<class T>inline void Put(void*p,size_t off,T v){std::memcpy(static_cast<uint8*>(p)+off,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+sizeof(x))return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(entity) P(matrix) P(entityMatrix) P(intel) P(modelInfo) P(colModel) P(colData) P(colSphere) P(subtask) P(pool) P(poolObjects) P(poolFlags) P(output) P(dots)
#undef P
return p;}
inline json Words(const void*p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(Ptr(static_cast<const uint32*>(p)[i]));return j;}
inline uint32 B(float v){return std::bit_cast<uint32>(v);}inline json V(const CVector&v){return json::array({B(v.x),B(v.y),B(v.z)});}
inline void __cdecl Corners(float z,CEntity&e,CVector*out){trace.push_back({"corners",B(z),Ptr(uintptr_t(&e))});auto center=e.GetPosition();const float x=float((scenario/4)%5)+.5f,y=float((scenario/20)%4)+.5f;out[0]=CVector{center.x+x,center.y+y,z};out[1]=CVector{center.x-x,center.y+y,z};out[2]=CVector{center.x-x,center.y-y,z};out[3]=CVector{center.x+x,center.y-y,z};}
inline bool __cdecl LOS(const CVector&a,const CVector&b,bool c,bool d,bool e,bool f,bool g,bool h,bool j){bool result=(scenario&1)!=0;trace.push_back({"los",V(a),V(b),c,d,e,f,g,h,j,result});return result;}
inline bool __cdecl Vertical(const CVector&a,float z,CColPoint&out,CEntity*&e,bool b,bool v,bool p,bool o,bool d,bool see,CStoredCollPoly*){++verticalCalls;out.m_vecPoint=CVector{a.x,a.y,float((scenario/8)%4)};e=reinterpret_cast<CEntity*>(entity.data());bool result=(scenario&2)!=0&&(!(scenario&64)||verticalCalls<2);trace.push_back({"vertical",V(a),B(z),b,v,p,o,d,see,result});return result;}
inline CEntity* __cdecl SphereWorld(CVector a,float r,CEntity*ignore,bool b,bool v,bool p,bool o,bool d,bool c){bool result=(scenario&4)!=0;trace.push_back({"sphere_world",V(a),B(r),Ptr(uintptr_t(ignore)),b,v,p,o,d,c,result});return result?reinterpret_cast<CEntity*>(entity.data()):nullptr;}
inline bool __cdecl Line(const CVector&a,const CVector&b,CColPoint&out,CEntity*&e,bool c,bool d,bool f,bool g,bool h,bool j,bool k,bool l){CWorld::ms_iProcessLineNumCrossings=(scenario/8)%4;out.m_vecPoint=b;e=reinterpret_cast<CEntity*>(entity.data());bool result=(scenario&1)!=0;trace.push_back({"line",V(a),V(b),c,d,f,g,h,j,k,l,result});return result;}
inline bool __cdecl Water(float x,float y,float z,float&out,uint8 t,CVector*){out=1.5f;bool result=(scenario&8)!=0;trace.push_back({"water",B(x),B(y),B(z),t,result});return result;}
inline bool __cdecl Collision(const CColLine&,const CMatrix&m,CColModel&,bool a,bool b){bool result=(scenario&2)!=0;trace.push_back({"collision",Ptr(uintptr_t(&m)),a,b,result});return result;}
inline bool __fastcall Shallow(SurfaceInfos_c*,void*,SurfaceId id){bool result=(scenario&16)!=0;trace.push_back({"shallow",int(id),result});return result;}
inline bool __cdecl InGroup(CPed*p){bool result=(scenario&32)!=0;trace.push_back({"group",Ptr(uintptr_t(p)),result});return result;}
inline float __cdecl Ground(CVector p,bool*ok,CEntity**){*ok=(scenario&64)!=0;float result=float((scenario/16)%5)-1.f;trace.push_back({"ground",V(p),*ok,B(result)});return result;}
inline CTask* __fastcall Active(CTaskManager*,void*){bool result=(scenario&16)!=0;trace.push_back({"active",result});return result?reinterpret_cast<CTask*>(subtask.data()):nullptr;}
inline CTask* __fastcall Simplest(CTaskManager*,void*){trace.push_back({"simplest"});return reinterpret_cast<CTask*>(subtask.data());}
inline CTaskSimpleSwim* __fastcall Swim(CPedIntelligence*,void*){bool result=(scenario&32)!=0;trace.push_back({"swim",result});return result?reinterpret_cast<CTaskSimpleSwim*>(subtask.data()):nullptr;}
inline CTaskSimpleJetPack* __fastcall Jet(CPedIntelligence*,void*){bool result=(scenario&64)!=0;trace.push_back({"jet",result});return result?reinterpret_cast<CTaskSimpleJetPack*>(subtask.data()):nullptr;}
inline int __fastcall Type(CTask*,void*){trace.push_back({"type",subtype});return subtype;}
inline bool __fastcall Ray(CColSphere*,void*,const CVector&a,const CVector&d,CVector&p,CVector&q){++rayCalls;p=a+d*.5f;q=a+d*2.f;bool result=(scenario&1)!=0&&(!(scenario&128)||rayCalls<2);trace.push_back({"ray",V(a),V(d),result});return result;}
inline bool __fastcall Intersect(CColSphere*,void*,const CVector&p){bool result=(scenario&2)!=0;trace.push_back({"intersect",V(p),result});return result;}
inline bool __cdecl Inside(const CPed&,const CVector&p,CEntity&){bool result=(scenario&2)!=0;trace.push_back({"inside",V(p),result});return result;}
inline bool __cdecl LOSPed(const CPed&,const CVector&p,CEntity&,float&d){d=.5f;bool result=(scenario&4)!=0;trace.push_back({"los_ped",V(p),result});return result;}
inline ReversibleHooks::ReversibleHook::Base*FindHook(const char*c,const char*n){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()==c)for(auto&h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"geometry-queries355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"geometry-queries355-runtime.jsonl");ReversingProbe::InstallFailureCapture("geometry-queries355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"geometry-queries355-only.txt");if(f)std::getline(f,only);}subVT[4]=reinterpret_cast<void*>(&Type);
bool pass=true,selected=false;int total=0;
for(auto&fn:fns){auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto*h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ce(0x5F1FA0,Corners),cd(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached),Corners);
EntryPatch le(0x56A490,LOS),ld(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),LOS);
EntryPatch ve(0x5674E0,Vertical),vd(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),Vertical);
EntryPatch se(0x569E20,SphereWorld),sd(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),SphereWorld);
EntryPatch pe(0x56BA00,Line),pd(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
EntryPatch we(0x6EB690,Water),wd(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
EntryPatch coe(0x417730,Collision),cod(bit_cast<uintptr_t>(&CCollision::TestLineOfSight),Collision);
EntryPatch she(0x55E730,Shallow),shd(bit_cast<uintptr_t>(&SurfaceInfos_c::IsShallowWater),Shallow);
EntryPatch ge(0x5F7F10,InGroup),gd(bit_cast<uintptr_t>(&CPedGroups::IsInPlayersGroup),InGroup);
EntryPatch gze(0x5696C0,Ground),gzd(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
EntryPatch ae(0x681720,Active),ad(bit_cast<uintptr_t>(&CTaskManager::GetActiveTask),Active);
EntryPatch ase(0x6819D0,Simplest),asd(bit_cast<uintptr_t>(&CTaskManager::GetSimplestActiveTask),Simplest);
EntryPatch swe(0x601070,Swim),swd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),Swim);
EntryPatch je(0x601110,Jet),jd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),Jet);
EntryPatch re(0x40FF20,Ray),rd(bit_cast<uintptr_t>(&CColSphere::IntersectRay),Ray);
EntryPatch ie(0x410040,Intersect),id(bit_cast<uintptr_t>(&CColSphere::IntersectPoint),Intersect);
std::unique_ptr<EntryPatch> insideE,insideD,losE,losD;if(fn.slot==4){insideE=std::make_unique<EntryPatch>(0x5F3880,Inside);insideD=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CPedGeometryAnalyser::LiesInsideBoundingBox),Inside);losE=std::make_unique<EntryPatch>(0x5F5A30,LOSPed);losD=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,const CVector&,CEntity&,float&)>(&CPedGeometryAnalyser::GetIsLineOfSightClear)),LOSPed);}
auto savedModel=CModelInfo::ms_modelInfoPtrs[100];CModelInfo::ms_modelInfoPtrs[100]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());auto oldPool=*reinterpret_cast<uintptr_t*>(0xB74490);*reinterpret_cast<uintptr_t*>(0xB74490)=uintptr_t(pool.data());auto crossings=CWorld::ms_iProcessLineNumCrossings;const auto oldPadding=*reinterpret_cast<float*>(0x8D22B0);
for(int i=0;i<fn.cases;++i){scenario=i%256;subtype=(i/64)%3==0?0xD3:(i/64)%3==1?0xFE:0xCB;json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);ped.fill(0);entity.fill(0);matrix.fill(0);entityMatrix.fill(0);intel.fill(0);modelInfo.fill(0);colModel.fill(0);colData.fill(0);colSphere.fill(0);subtask.fill(0);pool.fill(0);poolObjects.fill(0);poolFlags.fill(0);trace=json::array();verticalCalls=rayCalls=0;CWorld::ms_iProcessLineNumCrossings=0;*reinterpret_cast<float*>(0x8D22B0)=.35f;
auto*p=reinterpret_cast<CPed*>(ped.data());auto*e=reinterpret_cast<CEntity*>(entity.data());auto*m=reinterpret_cast<CMatrixLink*>(matrix.data());auto*em=reinterpret_cast<CMatrixLink*>(entityMatrix.data());auto*cm=reinterpret_cast<CColModel*>(colModel.data());auto*cc=reinterpret_cast<CCollisionData*>(colData.data());auto*cs=reinterpret_cast<CColSphere*>(colSphere.data());p->m_matrix=m;e->m_matrix=em;p->m_pIntelligence=reinterpret_cast<CPedIntelligence*>(intel.data());p->m_nModelIndex=e->m_nModelIndex=100;Put(modelInfo.data(),0x14,cm);cm->m_boundSphere.m_fRadius=float((i/16)%5)+.5f;cm->m_pColData=cc;cc->m_pSpheres=cs;cs->m_vecCenter=CVector{0.f,0.f,.5f};cs->m_fRadius=.5f;
for(auto*t:{m,em}){t->GetRight()=CVector{1.f,0.f,0.f};t->GetForward()=CVector{0.f,1.f,0.f};t->GetUp()=CVector{0.f,0.f,1.f};}m->GetPosition()=CVector{float(i%9)-4.f,float((i/9)%7)-3.f,float((i/63)%3)};em->GetPosition()=CVector{float(i%5)-2.f,float((i/5)%5)-2.f,0.f};p->bInVehicle=(scenario&8)!=0;e->m_bIsTempBuilding=(scenario&128)!=0;Put(subtask.data(),0,subVT.data());p->m_nContactSurface=static_cast<eSurfaceType>(0);
point=CVector{float(i%17)-8.f,float((i/17)%13)-6.f,float((i/221)%5)-2.f};from=CVector{0.f,0.f,0.f};contact=CVector{.3f,.4f,float(i%8)*.15f};sphere.m_vecCenter=CVector{.5f,0.f,0.f};sphere.m_fRadius=1.f;reinterpret_cast<CPhysical*>(entity.data())->m_vecMoveSpeed=point;for(auto&v:output)v=CVector{99.f,98.f,97.f};dots.fill(96.f);
if(fn.slot==4){auto*in=p->GetIntelligence();in->m_vehicleScanner.m_apEntities[0]=(scenario&16)?e:nullptr;in->m_pedScanner.m_apEntities[0]=(scenario&32)?e:nullptr;}
Put(pool.data(),0,poolObjects.data());Put(pool.data(),4,poolFlags.data());Put<int>(pool.data(),8,4);for(int k=0;k<4;++k){poolFlags[k]=(scenario&(1<<k))?0x80:0;auto*q=reinterpret_cast<CPed*>(poolObjects.data()+k*0x7C4);q->SetPosition(CVector{float(k),float(k%2),0.f});}
uintptr_t ret{};switch(fn.slot){
case 0:ret=reinterpret_cast<bool(__cdecl*)(const CPed&,const CEntity&)>(fn.address)(*p,*e);break;
case 1:ret=reinterpret_cast<bool(__cdecl*)(const CPed&,const CEntity&,const CVector&,const CVector&)>(fn.address)(*p,*e,contact,point);break;
case 2:ret=reinterpret_cast<bool(__cdecl*)(const CPed&,const CVector&,bool)>(fn.address)(*p,point,(scenario&64)!=0);break;
case 3:ret=reinterpret_cast<int(__cdecl*)(const CVector&,const CVector&)>(fn.address)(from,point);break;
case 4:reinterpret_cast<void(__cdecl*)(const CPed&,const CVector&,CVector&)>(fn.address)(*p,point,output[0]);break;
case 5:ret=reinterpret_cast<bool(__cdecl*)(const CVector&,CEntity&,CVector&)>(fn.address)(point,*e,output[0]);break;
case 6:reinterpret_cast<void(__cdecl*)(float,CEntity&,CVector(*)[4],float*)>(fn.address)(point.z,*e,reinterpret_cast<CVector(*)[4]>(output.data()),dots.data());break;
case 7:reinterpret_cast<void(__cdecl*)(const CPed&,CEntity&,CColSphere&)>(fn.address)(*p,*e,sphere);break;
case 8:ret=reinterpret_cast<int32(__cdecl*)(const CPed&,CEntity&,CVector&)>(fn.address)(*p,*e,output[0]);break;
case 9:ret=reinterpret_cast<int32(__cdecl*)(const CPed&,CEntity&)>(fn.address)(*p,*e);break;
case 10:ret=reinterpret_cast<int32(__cdecl*)(const CVector&,CEntity&)>(fn.address)(point,*e);break;
case 11:ret=reinterpret_cast<int32(__cdecl*)(const CPed&,const CPhysical&)>(fn.address)(*p,*reinterpret_cast<CPhysical*>(entity.data()));break;
case 12:ret=reinterpret_cast<bool(__cdecl*)(const CPed&,const CColSphere&,const CVector&,const CVector&,CVector&,CVector&)>(fn.address)(*p,sphere,from,point,output[0],output[1]);break;
case 13:ret=reinterpret_cast<bool(__cdecl*)(const CPed&,const CVector&,CEntity&,float&)>(fn.address)(*p,point,*e,dots[0]);break;
case 14:ret=reinterpret_cast<bool(__cdecl*)(const CVector&,const CVector&,CEntity&)>(fn.address)(from,point,*e);break;
case 15:ret=uintptr_t(reinterpret_cast<CPed*(__cdecl*)(const CVector&)>(fn.address)(point));break;
case 16:ret=reinterpret_cast<bool(__cdecl*)(CEntity*,const CVector&,float)>(fn.address)(e,point,float((i/16)%5)*.5f);break;
case 17:ret=reinterpret_cast<bool(__cdecl*)(const CPed&)>(fn.address)(*p);break;
case 18:ret=uintptr_t(reinterpret_cast<uint32(__cdecl*)(const CVector&,const CVector&,float,int32)>(fn.address)(from,point,float((i/32)%5),int32((i/128)%5)));break;
case 19:ret=reinterpret_cast<bool(__cdecl*)(const CPed&,const CVector&,CEntity&)>(fn.address)(*p,point,*e);break;
}
if(candidate){auto n=std::to_string(fn.slot==15?int(ret!=0):int(ret));coverage[n]=coverage.value(n,0)+1;for(auto&t:trace){auto k=t[0].get<std::string>();coverage[k]=coverage.value(k,0)+1;}}
result[candidate]={{"return",fn.slot==15||fn.slot==8?Ptr(ret):json(int32(ret))},{"output",Words(output.data(),sizeof(output))},{"dots",Words(dots.data(),sizeof(dots))},{"sphere",Words(&sphere,sizeof(sphere))},{"ped",Words(ped.data(),ped.size())},{"entity",Words(entity.data(),entity.size())},{"intel",Words(intel.data(),intel.size())},{"matrix",Words(matrix.data(),matrix.size())},{"entity_matrix",Words(entityMatrix.data(),entityMatrix.size())},{"pool",Words(poolObjects.data(),poolObjects.size())},{"crossings",CWorld::ms_iProcessLineNumCrossings},{"trace",trace}};
}bool equal=result[0]==result[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total++},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
CModelInfo::ms_modelInfoPtrs[100]=savedModel;*reinterpret_cast<uintptr_t*>(0xB74490)=oldPool;CWorld::ms_iProcessLineNumCrossings=crossings;*reinterpret_cast<float*>(0x8D22B0)=oldPadding;
}h->State(hs);h->LockState(hl);Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",true}});
}
}
'''
p.joinpath('LiveComparison.hpp').write_text(s)
run=Path('reversing/Boat355/run_local.py').read_text().replace('boat355','geometry-queries355').replace('KillPedFromBoat pendientes','Consultas geometría pendientes');p.joinpath('run_local.py').write_text(run)
import runpy
sources=runpy.run_path('reversing/GeometryCore355/build_probe.py')['SOURCES']+['source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h','source/game_sa/SurfaceInfos_c.cpp','source/game_sa/SurfaceInfos_c.h','source/game_sa/Interior/InteriorManager_c.cpp','source/game_sa/Interior/InteriorManager_c.h','source/game_sa/PedGroups.cpp','source/game_sa/PedGroups.h']
p.joinpath('build_probe.py').write_text("import json,sys\nfrom pathlib import Path\nsys.path.insert(0,str(Path(__file__).resolve().parents[1]))\nfrom probe_build import build\nSOURCES="+repr(sources)+"\nif __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'geometry-queries355','GeometryQueries355Live',SOURCES)))\n")
p.joinpath('README.md').write_text('# GeometryQueries355\n\n20 consultas pendientes, 12032 pares preparados. Compara retornos, vectores/planos/esfera, ped, entidad, ambas matrices, inteligencia, pool y trazas de dependencias completas. Fixtures positivos de LOS, contacto, colisión, agua, altura, tareas y escáneres. Pool aislado de cuatro slots reales de 0x7C4 y mapas ocupado/vacío, restaura pool/modelo/padding/cruces. Dependencias mundo/collision y generación de corners aisladas simétricamente; ClearTarget también aísla pruebas de entidad y segmento. Las operaciones geométricas restantes son reales. No valida streaming del mundo ni animaciones. Pendiente de ejecución; hooks false.\n')
print('missing',[x for x in sources if not Path(x).exists()])
