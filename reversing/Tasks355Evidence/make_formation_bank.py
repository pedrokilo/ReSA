from pathlib import Path
p=Path('reversing/FormationFollow355');p.mkdir(exist_ok=True)
s=Path('reversing/Boat355/LiveComparison.hpp').read_text().replace('Boat355Live','FormationFollow355Live').replace('boat355','formation-follow355').replace('TaskComplexKillPedFromBoat','TaskComplexFollowLeaderInFormation').replace('#include "TaskComplexKillPedOnFoot.h"','#include "TaskSimpleStandStill.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexFollowLeaderInFormation","Constructor",0x6949A0,0,-1,256},
{"CTaskComplexFollowLeaderInFormation","Clone",0x695740,0x870C3C,1,256},
{"CTaskComplexFollowLeaderInFormation","GetTaskType",0x694A30,0x870C3C,4,256},
{"CTaskComplexFollowLeaderInFormation","CreateNextSubTask",0x696820,0x870C3C,8,1024},
{"CTaskComplexFollowLeaderInFormation","CreateFirstSubTask",0x6968E0,0x870C3C,9,512},
{"CTaskComplexFollowLeaderInFormation","ControlSubTask",0x696940,0x870C3C,10,1024}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group;alignas(16) inline std::array<uint8,sizeof(CBaseModelInfo)> model;alignas(16) inline std::array<uint8,sizeof(CColModel)> col;inline CVector constructorPos;inline std::array<void*,11> subVT{};')
s=s.replace('P(vehicle)','P(vehicle) P(group) P(model) P(col)')
a=s.index('inline CWanted* __cdecl Wanted');b=s.index('inline void __fastcall Ref',a)
s=s[:a]+'''inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=(scenario&1)!=0;trace.push_back({"abort",priority,b});return b;}
inline CPedGroup* __cdecl Group(CPed*){bool b=(scenario&2)!=0;trace.push_back({"group",b});return b?reinterpret_cast<CPedGroup*>(group.data()):nullptr;}
inline bool __fastcall Process(CTaskSimpleStandStill*,void*,CPed* p){trace.push_back({"process_stand"});p->m_nMoveState=PEDMOVE_STILL;return false;}
inline CTask* __fastcall Sub(CTaskComplexFollowLeaderInFormation*,void*,int type,CPed*){trace.push_back({"sub",type});return type==0x516?nullptr:reinterpret_cast<CTask*>(arena.data());}
'''+s[b:]
s=s.replace('bool pass=true,selected=false;int total=0;', 'const auto savedTime=CTimer::GetTimeInMS();auto*savedModel=CModelInfo::GetModelInfo(100);bool pass=true,selected=false;int total=0;')
s=s.replace('auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);','auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);')
a=s.index('EntryPatch pe(');b=s.index('EntryPatch refe',a)
s=s[:a]+'''EntryPatch ge(0x5F7E80,Group),gd(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),Group);
EntryPatch pe(0x62F370,Process),pd(bit_cast<uintptr_t>(&CTaskSimpleStandStill::ProcessPed),Process);
std::optional<EntryPatch> sube,subd;if(fn.slot>=8){sube.emplace(0x6962A0,Sub);subd.emplace(bit_cast<uintptr_t>(&CTaskComplexFollowLeaderInFormation::CreateSubTask),Sub);}
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;', 'scenario=i%64;const int types[]{0xCA,0xCB,700,0x2C0,0x2C5,0x38B,12345};subType=types[(i/16)%7];')
a=s.index('t->m_Ped=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''vehicle.fill(0);group.fill(0);model.fill(0);col.fill(0);auto*v=reinterpret_cast<CVehicle*>(vehicle.data());v->m_nModelIndex=100;Put(model.data(),0x14,col.data());CModelInfo::SetModelInfo(100,reinterpret_cast<CBaseModelInfo*>(model.data()));reinterpret_cast<CColModel*>(col.data())->m_boundSphere.m_fRadius=float(1+(i%3));v->m_nNumPassengers=uint8((i&128)?2:0);v->m_nMaxPassengers=2;Put(task.data(),0x0C,group.data());Put(task.data(),0x10,(i&512)?static_cast<void*>(nullptr):player.data());Put<CVector>(task.data(),0x14,CVector{.25f,-.5f,1.f});Put<uint32>(task.data(),0x20,4);Put<float>(task.data(),0x24,float(i%6)-1.f);Put<float>(subtask.data(),0x18,float(i%3));p->bInVehicle=(i&4)!=0;p->m_pVehicle=(i&8)?v:nullptr;q->bInVehicle=(i&16)!=0;q->m_pVehicle=(i&32)?v:nullptr;q->m_nMoveState=static_cast<eMoveState>(i%8);q->m_placement.m_vPosn=CVector{float(i%4),float(i%7),0.f};p->m_placement.m_vPosn=CVector{0.f,0.f,0.f};constructorPos=CVector{float(i%5),-.25f,1.f};CTimer::m_snTimeInMilliseconds=4000;
'''+s[b:]
s=s.replace('uintptr_t ret=fn.slot==1?', 'uintptr_t ret=fn.slot==-1?uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPedGroup*,CPed*,const CVector&,float)>(address)(t,reinterpret_cast<CPedGroup*>(group.data()),(i&1)?q:nullptr,constructorPos,float(i%6)-1.f)):fn.slot==1?')
s=s.replace('{"task",Words(task.data(),task.size())}', '{"leader",Words(player.data(),player.size())},{"subtask",Words(subtask.data(),128)},{"task",Words(task.data(),task.size())}')
a=s.index('if(fn.slot==10){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==9){require("sub");}if(fn.slot==10){require("sub");require("abort");require("process_stand");require("new");}'''+s[b:]
s=s.replace('Write({{"event",pass&&selected?', 'CTimer::m_snTimeInMilliseconds=savedTime;CModelInfo::SetModelInfo(100,savedModel);Write({{"event",pass&&selected?')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index('if __name__',a)
files=['Tasks/TaskTypes/TaskComplexFollowLeaderInFormation','Tasks/TaskTypes/TaskSimpleStandStill','Tasks/TaskTypes/TaskSimplePause','Tasks/TaskTypes/TaskSimpleAchieveHeading','Tasks/TaskTypes/TaskComplexEnterCarAsPassenger','Tasks/TaskTypes/TaskComplexEnterCar','Tasks/TaskTypes/TaskComplexLeaveCar','Tasks/TaskTypes/TaskSimpleCarDrive','PedGroups','PedGroupMembership','Core/Vector','General','Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset']
paths=['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')]+['source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h']
s=s[:a]+'SOURCES='+repr(paths)+'\n'+s[b:];s=s.replace('gang-join355','formation-follow355').replace('GangJoin355Live','FormationFollow355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','formation-follow355').replace('GangJoinRespond pendientes','FollowLeaderInFormation pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexFollowLeaderInFormation: seis pendientes

Banco preparado de 3.328 pares: Constructor/Clone/Type 256, Next 1024, First 512, Control 1024. Pendiente de ejecución. Compara tarea, ped, líder, subtarea completa y asignaciones, incluidos contador/radio de SeekEntity y pausa al separarse del líder en vehículo. Se reconstruyó CreateSubTask como dependencia nativa; se aísla en las transiciones. La creación directa del pasajero y pausa sigue real.

Fixture de modelo de colisión, vehículo y grupo; restaura modelo y reloj. ProcessPed de StandStill y búsqueda de grupo se aíslan por pares. No valida movimiento físico ni la geometría del seguimiento en ejecución real. Constructor hook antes devolvía this sin construir: ahora ejecuta el constructor nativo completo.
''')
