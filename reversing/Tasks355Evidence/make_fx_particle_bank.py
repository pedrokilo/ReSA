from pathlib import Path
p=Path('reversing/FxEmitterParticle355');p.mkdir(exist_ok=True)
s=r'''#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Fx/FxEmitterBP.h"
#include "Fx/FxEmitterPrt.h"
#include "Fx/MovementInfo.h"
#include "WaterLevel.h"
#include "../ProbeRuntime.hpp"
namespace FxEmitterParticle355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
alignas(16)inline std::array<uint8,sizeof(FxEmitterBP_c)>object;
alignas(16)inline std::array<uint8,sizeof(FxEmitterPrt_c)>particle;
alignas(16)inline std::array<uint8,sizeof(FxSystem_c)>system;
alignas(16)inline std::array<uint8,sizeof(FxSystemBP_c)>systemBP;
inline uint32 B(float x){return std::bit_cast<uint32>(x);}
inline json Words(const void*p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(static_cast<const uint32*>(p)[i]);return j;}
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline void __fastcall Movement(FxInfoManager_c*,void*,float current,float age,float step,float length,bool constant,MovementInfo_t*out){trace.push_back({"movement",B(current),B(age),B(step),B(length),constant,Words(out,24)});out->m_Pos.x+=.5f;out->m_Vel.y=-out->m_Vel.y;out->m_Rot={(scenario&1)?1.f:-1.f,(scenario&2)?2.f:0.f,(scenario&4)?1.5f:-2.f,(scenario&8)?3.f:0.f};out->m_bHasFloatInfo=(scenario&16)!=0;out->m_bHasUnderwaterInfo=(scenario&32)!=0;}
inline bool __cdecl Water(float x,float y,float z,float&out,uint8 touch,CVector*){bool found=(scenario&64)!=0;out=float((scenario/128)%4)-1.f;trace.push_back({"water",B(x),B(y),B(z),touch,found,B(out)});return found;}
inline auto*FindHook(){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()=="FxEmitterBP_c")for(auto&h:cat.Items())if(h->Name()=="UpdateParticle")r=h.get();});return r;}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"FxEmitterBP_c","UpdateParticle",0x4A21D0,0,0,1024}};
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"fx-emitter-particle355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"fx-emitter-particle355-runtime.jsonl");ReversingProbe::InstallFailureCapture("fx-emitter-particle355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;auto*h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);bool pass=true;int failures=0;json coverage=json::object();{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch me(0x4A4A10,Movement),md(bit_cast<uintptr_t>(&FxInfoManager_c::ProcessMovementInfo),Movement);
EntryPatch we(0x6EB690,Water),wd(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
for(int i=0;i<1024;++i){scenario=i;json results[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);object.fill(0xCD);particle.fill(0);system.fill(0);systemBP.fill(0);trace=json::array();auto*self=reinterpret_cast<FxEmitterBP_c*>(object.data());auto*p=reinterpret_cast<FxEmitterPrt_c*>(particle.data());auto*s=reinterpret_cast<FxSystem_c*>(system.data());auto*bp=reinterpret_cast<FxSystemBP_c*>(systemBP.data());p->m_System=s;s->m_SystemBP=bp;s->m_nTimeMult=uint16((i/128)%8*250);s->m_fCurrentTime=float(i%7)*.25f;bp->m_fLength=2.f;p->m_fTotalLife=float((i/256)%4)+.5f;p->m_fCurrentLife=float((i/16)%8)*.25f;p->m_Pos=CVector{.25f,-.5f,float((i/64)%4)-1.f};p->m_Velocity=CVector{.5f,.25f,-.5f};p->m_CurrentRotation=.125f;particle[0x31]=uint8((i/4)%256);particle[0x32]=uint8(i%256);const auto step=float((i/32)%8)*.125f;
bool result=reinterpret_cast<bool(__thiscall*)(FxEmitterBP_c*,float,FxEmitterPrt_c*)>(0x4A21D0)(self,step,p);if(candidate){auto n=result?"dead":"alive";coverage[n]=coverage.value(n,0)+1;for(auto&t:trace){auto k=t[0].get<std::string>();coverage[k]=coverage.value(k,0)+1;}}
results[candidate]={{"return",result},{"particle",Words(particle.data(),particle.size())},{"object",Words(object.data(),object.size())},{"system",Words(system.data(),system.size())},{"system_bp",Words(systemBP.data(),systemBP.size())},{"trace",trace}};
}bool equal=results[0]==results[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function","FxEmitterBP_c::UpdateParticle"},{"case",i},{"pair",i},{"equal",equal},{"accepted",equal},{"original",results[0]},{"candidate",results[1]}});}}
h->State(hs);h->LockState(hl);for(auto n:{"dead","alive","movement","water"})if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"reason","missing coverage"},{"branch",n}});}Write({{"event","function_summary"},{"function","FxEmitterBP_c::UpdateParticle"},{"cases",1024},{"failures",failures},{"coverage",coverage}});Write({{"event",pass?"complete":"error"},{"pairs",1024},{"state_restored",true}});}
}
'''
p.joinpath('LiveComparison.hpp').write_text(s)
run=Path('reversing/Boat355/run_local.py').read_text().replace('boat355','fx-emitter-particle355').replace('KillPedFromBoat pendientes','UpdateParticle pendientes');p.joinpath('run_local.py').write_text(run)
import runpy
sources=runpy.run_path('reversing/FxEmitterCore355/build_probe.py')['SOURCES']+['source/game_sa/Fx/FxInfoManager.cpp','source/game_sa/Fx/FxInfoManager.h','source/game_sa/Fx/MovementInfo.h','source/game_sa/Fx/FxSystemBP.cpp','source/game_sa/Fx/FxSystemBP.h','source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h','source/game_sa/Core/Vector.cpp','source/game_sa/Core/Vector.h']
p.joinpath('build_probe.py').write_text("import json,sys\nfrom pathlib import Path\nsys.path.insert(0,str(Path(__file__).resolve().parents[1]))\nfrom probe_build import build\nSOURCES="+repr(sources)+"\nif __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'fx-emitter-particle355','FxEmitterParticle355Live',SOURCES)))\n")
p.joinpath('README.md').write_text('# FxEmitterParticle355\n\n1024 pares de UpdateParticle: vida/escala temporal, posición/velocidad, rotación positiva/negativa/doble, bytes aleatorios, flotación y muerte bajo agua. Movimiento y agua aislados simétricamente con entradas y salidas positivas. Compara toda la partícula, sistema, blueprint, emitter y trazas. Pendiente de runtime; hook false. No valida módulos FxInfo reales ni agua del mapa.\n')
print([x for x in sources if not Path(x).exists()])
