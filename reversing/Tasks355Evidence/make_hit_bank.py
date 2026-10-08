from pathlib import Path
p=Path('reversing/HitCar355');p.mkdir(exist_ok=True)
s=Path('reversing/Boat355/LiveComparison.hpp').read_text().replace('Boat355Live','HitCar355Live').replace('boat355','hit-car355').replace('TaskComplexKillPedFromBoat','TaskComplexHitPedWithCar').replace('#include "TaskComplexKillPedOnFoot.h"','#include "PedGeometryAnalyser.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexHitPedWithCar","ComputeEvasiveStepMoveDir",0x653B40,0,-1,512},
{"CTaskComplexHitPedWithCar","HitHurtsPed",0x653AE0,0,-1,512},
{"CTaskComplexHitPedWithCar","CreateSubTask",0x6560E0,0,-1,512},
{"CTaskComplexHitPedWithCar","Clone",0x6559B0,0x86F294,1,256},
{"CTaskComplexHitPedWithCar","GetTaskType",0x653A20,0x86F294,4,256},
{"CTaskComplexHitPedWithCar","CreateNextSubTask",0x657AF0,0x86F294,8,512},
{"CTaskComplexHitPedWithCar","CreateFirstSubTask",0x656300,0x86F294,9,512},
{"CTaskComplexHitPedWithCar","ControlSubTask",0x653A90,0x86F294,10,256}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix;inline std::array<void*,11> subVT{};')
s=s.replace('P(vehicle)','P(vehicle) P(matrix)')
a=s.index('inline CWanted* __cdecl Wanted(');b=s.index('inline void __fastcall Ref',a)
s=s[:a]+'''inline int __cdecl EntitySide(const CPed&,CEntity&){int r=scenario%4;trace.push_back({"entity_side",r});return r;}
inline int __cdecl PedSide(const CPed&,const CPhysical&){int r=scenario%4;trace.push_back({"ped_side",r});return r;}
inline CTask* __fastcall Sub(CTaskComplexHitPedWithCar*,void*,int type){trace.push_back({"sub",type});return type==0x516?nullptr:reinterpret_cast<CTask*>(arena.data());}
'''+s[b:]
s=s.replace('subVT[6]=reinterpret_cast<void*>(&Abort);','')
s=s.replace('auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);', 'auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);')
a=s.index('EntryPatch we(');b=s.index('EntryPatch refe',a)
s=s[:a]+'''EntryPatch ese(0x5F3BC0,EntitySide),esd(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide)),EntitySide);
EntryPatch pse(0x5F3640,PedSide),psd(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,const CPhysical&)>(&CPedGeometryAnalyser::ComputePedHitSide)),PedSide);
std::optional<EntryPatch> sube,subd;if(fn.slot==8||fn.slot==9){sube.emplace(0x6560E0,Sub);subd.emplace(bit_cast<uintptr_t>(&CTaskComplexHitPedWithCar::CreateSubTask),Sub);}
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;', 'scenario=i%32;const int types[]{0x1FA,0x1FB,0x1F6,0x19E,0xD0,0xC8,0x516,12345};subType=types[(i/16)%8];')
a=s.index('t->m_Ped=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''wanted.fill(0);vehicle.fill(0);matrix.fill(0);auto*v=reinterpret_cast<CVehicle*>(vehicle.data());Put(vehicle.data(),0x14,matrix.data());auto*m=reinterpret_cast<CMatrix*>(matrix.data());m->GetRight()=CVector{float(i%7)/3.f,float(i%5)/2.f,-0.25f};m->GetForward()=CVector{float(i%11)/4.f,-float(i%9)/3.f,0.5f};t->m_Veh=(fn.slot==1&&(scenario&16))?nullptr:v;t->m_ImpulseMag=float(i%30);if(i%31==0)t->m_ImpulseMag=std::numeric_limits<float>::quiet_NaN();t->m_PedHitSide=i%6;t->m_DownTime=(i%2)?1000:3000;t->m_MoveDir=CVector{0.25f,-0.5f,1.f};p->m_vecLastCollisionImpactVelocity.z=float(i%9)/4.f-1.f;Put<uint8>(subtask.data(),0x10,uint8(i%2));
'''+s[b:]
a=s.index('uintptr_t ret=');b=s.index('\nif(candidate)',a)
s=s[:a]+'''uintptr_t ret=0;json vector=nullptr;
if(std::string(fn.name)=="ComputeEvasiveStepMoveDir"){auto r=reinterpret_cast<CVector(__cdecl*)(const CPed*,CVehicle*)>(address)(p,v);vector=json::array({std::bit_cast<uint32>(r.x),std::bit_cast<uint32>(r.y),std::bit_cast<uint32>(r.z)});}
else if(std::string(fn.name)=="HitHurtsPed")ret=uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(address)(t,p));
else if(std::string(fn.name)=="CreateSubTask")ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int)>(address)(t,types[i%8]));
else if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));
else if(fn.slot==4)ret=uintptr_t(reinterpret_cast<int(__thiscall*)(void*)>(address)(t));
else ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));
'''+s[b:]
s=s.replace('fn.slot==4?json(ret):Ptr(ret)', '(fn.slot==4||std::string(fn.name)=="HitHurtsPed")?json(ret):Ptr(ret)')
s=s.replace('{"task",Words(task.data(),task.size())}', '{"vector",vector},{"task",Words(task.data(),task.size())}')
a=s.index('if(fn.slot==10){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==9){require("ped_side");require("entity_side");require("sub");}'''+s[b:]
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index("if __name__",a)
files=['Tasks/TaskTypes/TaskComplexHitPedWithCar','Tasks/TaskTypes/TaskSimpleKillPedWithCar','Tasks/TaskTypes/TaskSimpleHurtPedWithCar','Tasks/TaskTypes/TaskComplexEvasiveStep','Tasks/TaskTypes/TaskSimpleHitFromBehind','Tasks/TaskTypes/TaskComplexFallAndGetUp','Tasks/TaskTypes/TaskSimpleLeaveGroup','Tasks/TaskTypes/TaskSimpleNone','PedGeometryAnalyser']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','hit-car355').replace('GangJoin355Live','HitCar355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','hit-car355').replace('GangJoinRespond pendientes','HitPedWithCar pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexHitPedWithCar: ocho pendientes

Banco previsto de3.328pares, pendiente de ejecución. Compara tarea,ped,memoriaCD de nuevas tareas,trazas y vectores bit a bit. HitHurts/First cubren umbrales3/6/10/12/20 y velocidad de impacto z−0.8, con NaN quietos. Next cubre todos los estados y el byte de herida. Clone deja Side/MoveDir intactos como el EXE.

Los cálculos geométricos de lado se aíslan con retornos0..3, ambas entradas EXE/DLL. No se cubren lados fuera de0..3 porque el original devuelve vector de pila indeterminado. Constructor de auxiliares real; audio/física/juego de colisión no se ejecutan. Evidencia ../Tasks355Evidence.
''',encoding='utf-8')
