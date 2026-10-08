from pathlib import Path
p=Path('reversing/GeometryCore355');p.mkdir(exist_ok=True)
s=r'''#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGeometryAnalyser.h"
#include "../ProbeRuntime.hpp"
namespace GeometryCore355Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CPedGeometryAnalyser","ComputeEntityDirs",0x5F1500,0,0,512},
{"CPedGeometryAnalyser","ComputeEntityBoundingBoxSegmentPlanesUncached",0x5F1750,0,1,512},
{"CPedGeometryAnalyser","ComputeEntityHitSide-2",0x5F1450,0,2,1024},
{"CPedGeometryAnalyser","ComputeClosestSurfacePoint-rect",0x5F2C10,0,3,1024},
{"CPedGeometryAnalyser","ComputePedHitSide-posn",0x5F1E70,0,4,512},
{"CPedGeometryAnalyser","ComputePedShotSide",0x5F13F0,0,5,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped;
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matrix;
inline std::array<CVector,4> input,output;
inline std::array<float,4> dots;
inline CVector point,center;
inline json Words(const void* p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(static_cast<const uint32*>(p)[i]);return j;}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+sizeof(x))return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(matrix) P(input) P(output) P(dots)
#undef P
return p;}
inline CVector* __cdecl Dirs(const CEntity&,CVector* v){v[0]={0.f,1.f,0.f};v[1]={-1.f,0.f,0.f};v[2]={0.f,-1.f,0.f};v[3]={1.f,0.f,0.f};return v+3;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char*c,const char*n){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()==c)for(auto&h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"geometry-core355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"geometry-core355-runtime.jsonl");ReversingProbe::InstallFailureCapture("geometry-core355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"geometry-core355-only.txt");if(f)std::getline(f,only);}
bool pass=true,selected=false;int total=0;
for(auto&fn:fns){auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto*h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();{
std::unique_ptr<ReversingProbe::EntryPatch> de,dd;if(fn.slot==4){de=std::make_unique<ReversingProbe::EntryPatch>(0x5F1500,Dirs);dd=std::make_unique<ReversingProbe::EntryPatch>(std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityDirs),Dirs);}
for(int i=0;i<fn.cases;++i){json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);
ped.fill(0);matrix.fill(0);auto*p=reinterpret_cast<CPed*>(ped.data());auto*m=reinterpret_cast<CMatrixLink*>(matrix.data());m->GetRight()=CVector{1.f,0.f,0.f};m->GetForward()=CVector{0.f,1.f,0.f};m->GetUp()=CVector{0.f,0.f,1.f};m->GetPosition()=CVector{float(i%9)-4.f,float((i/9)%7)-3.f,.5f};p->m_matrix=(i&1)?m:nullptr;p->SetHeading(float(i%32)*.125f);p->m_fCurrentRotation=float(i%64)*.125f-4.f;
const auto x=float(i%17)-8.f,y=float((i/17)%13)-6.f,z=float((i/221)%5)-2.f;point=CVector{x*.5f,y*.5f,z*.5f};center=CVector{float(i%5),float((i/5)%3),.5f};const auto a=float((i/3)%7)+.5f,b=float((i/21)%5)+.5f;
input={CVector{center.x+a,center.y+b,center.z},CVector{center.x-a,center.y+b,center.z},CVector{center.x-a,center.y-b,center.z},CVector{center.x+a,center.y-b,center.z}};
if(fn.slot==2){input={CVector{1.f,0.f,0.f},CVector{0.f,1.f,0.f},CVector{-1.f,0.f,0.f},CVector{0.f,-1.f,0.f}};dots={-a,-b,-a,-b};}else dots.fill(std::bit_cast<float>(0xCDCDCDCDu));
for(auto&v:output)v=CVector{std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu)};
if(fn.slot==3&&(i%8)==0)input[1]=input[0];
uintptr_t ret{};switch(fn.slot){
case 0:ret=uintptr_t(reinterpret_cast<CVector*(__cdecl*)(const CEntity&,CVector*)>(fn.address)(*p,output.data()));break;
case 1:ret=uintptr_t(reinterpret_cast<CVector*(__cdecl*)(const CVector*,CVector&,CVector*,float*)>(fn.address)(input.data(),center,output.data(),dots.data()));break;
case 2:ret=reinterpret_cast<int32(__cdecl*)(const CVector&,const CVector*,const float*)>(fn.address)(point,input.data(),dots.data());break;
case 3:ret=reinterpret_cast<bool(__cdecl*)(const CVector&,const CVector*,CVector&)>(fn.address)(point,input.data(),output[0]);break;
case 4:case 5:ret=reinterpret_cast<int32(__cdecl*)(const CPed&,const CVector&)>(fn.address)(*p,point);break;
}
if(candidate){auto n=std::to_string(fn.slot<2?int(ret!=0):int(ret));coverage[n]=coverage.value(n,0)+1;}
result[candidate]={{"return",fn.slot<2?Ptr(ret):json(int32(ret))},{"output",Words(output.data(),sizeof(output))},{"dots",Words(dots.data(),sizeof(dots))},{"input",Words(input.data(),sizeof(input))},{"ped",Words(ped.data(),ped.size())},{"matrix",Words(matrix.data(),matrix.size())},{"point",Words(&point,sizeof(point))},{"center",Words(&center,sizeof(center))}};
}bool equal=result[0]==result[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total++},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
}}
h->State(hs);h->LockState(hl);Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",true}});
}
}
'''
p.joinpath('LiveComparison.hpp').write_text(s)
run=Path('reversing/Boat355/run_local.py').read_text().replace('boat355','geometry-core355').replace('KillPedFromBoat pendientes','Geometría básica pendientes');p.joinpath('run_local.py').write_text(run)
sources=['source/game_sa/PedGeometryAnalyser.cpp','source/game_sa/PedGeometryAnalyser.h','source/game_sa/Core/Vector.cpp','source/game_sa/Core/Vector.h','source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h','source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Placeable.h','source/game_sa/Collision/ColSphere.cpp','source/game_sa/Collision/ColSphere.h','source/game_sa/Collision/Collision.cpp','source/game_sa/Collision/Collision.h','source/game_sa/World.cpp','source/game_sa/World.h','source/game_sa/Pools/Pools.cpp','source/game_sa/Pools/Pools.h','source/game_sa/PedIntelligence.cpp','source/game_sa/PedIntelligence.h','source/game_sa/Tasks/TaskManager.cpp','source/game_sa/Tasks/TaskManager.h']
p.joinpath('build_probe.py').write_text("import json,sys\nfrom pathlib import Path\nsys.path.insert(0,str(Path(__file__).resolve().parents[1]))\nfrom probe_build import build\nSOURCES="+repr(sources)+"\nif __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'geometry-core355','GeometryCore355Live',SOURCES)))\n")
p.joinpath('README.md').write_text('# GeometryCore355\n\n4608 pares preparados: Dirs y SegmentPlanes 512, HitSide y ClosestSurfacePoint 1024, PedHitSide 512 y ShotSide 1024. Compara retornos, cuatro vectores de salida, planos/dots, entradas y ped/matriz completos. Usa rectángulos positivos, planos por cuadrantes, casos proyectados y de extremos, aristas degeneradas y posición/matriz alternativa. PedHitSide aísla ComputeEntityDirs por ambos lados; las demás operaciones son reales. ShotSide mantiene atan x87 y truncación del CRT del EXE como dependencia externa. Hooks false hasta aprobación runtime. Evidencia completa en Tasks355Evidence; no valida rutas, obstáculos ni consultas físicas.\n')
print(len(sources),all(Path(x).exists() for x in sources))
