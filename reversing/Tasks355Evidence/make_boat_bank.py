from pathlib import Path
p=Path('reversing/Boat355');p.mkdir(exist_ok=True)
s=Path('reversing/GangJoin355/LiveComparison.hpp').read_text()
s=s.replace('GangJoin355Live','Boat355Live').replace('gang-join355','boat355').replace('TaskComplexGangJoinRespond','TaskComplexKillPedFromBoat').replace('#include "TaskComplexGangLeader.h"','#include "TaskComplexKillPedOnFoot.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexKillPedFromBoat","Clone",0x6238A0,0x86DA98,1,256},
{"CTaskComplexKillPedFromBoat","GetTaskType",0x622820,0x86DA98,4,256},
{"CTaskComplexKillPedFromBoat","CreateNextSubTask",0x622890,0x86DA98,8,256},
{"CTaskComplexKillPedFromBoat","CreateFirstSubTask",0x622900,0x86DA98,9,256},
{"CTaskComplexKillPedFromBoat","ControlSubTask",0x622980,0x86DA98,10,1024}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted;\nalignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;\ninline std::array<void*,11> subVT{};')
s=s.replace('P(subtask)','P(subtask) P(wanted) P(vehicle)')
a=s.index('inline bool __cdecl Load()');b=s.index('inline void __fastcall Ref',a)
s=s[:a]+'''inline bool __fastcall IsPlayer(const CPed*,void*){bool b=(scenario&1)!=0;trace.push_back({"is_player",b});return b;}
inline CWanted* __cdecl Wanted(int id){trace.push_back({"wanted",id});return reinterpret_cast<CWanted*>(wanted.data());}
inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=(scenario&2)!=0;trace.push_back({"abort",priority,b});return b;}
inline void __fastcall Say(CAudioEngine*,void*,int event,int context,CEntity* p,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",event,context,Ptr(uintptr_t(p)),delay,std::bit_cast<uint32>(probability),a,b,c});}
'''+s[b:]
s=s.replace('subVT[4]=reinterpret_cast<void*>(&Type);','subVT[4]=reinterpret_cast<void*>(&Type);subVT[6]=reinterpret_cast<void*>(&Abort);')
s=s.replace('auto& block=CAnimManager::GetAnimBlocks()[0];const auto savedLoaded=block.IsLoaded;','')
a=s.index('EntryPatch le(');b=s.index('EntryPatch refe(',a)
s=s[:a]+'''EntryPatch ie(0x5DF8F0,IsPlayer),id(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
EntryPatch we(0x56E230,Wanted),wd(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
EntryPatch se(0x5073C0,Say),sd(bit_cast<uintptr_t>(&CAudioEngine::SayPedless),Say);
'''+s[b:]
s=s.replace('scenario=i%32;subType=i%3==0?0x190:i%3==1?TASK_COMPLEX_TURN_TO_FACE_ENTITY:TASK_SIMPLE_STAND_STILL;', 'scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;')
a=s.index('t->m_response=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''t->m_Ped=(scenario&4)?q:nullptr;wanted.fill(0);vehicle.fill(0);reinterpret_cast<CWanted*>(wanted.data())->m_WantedLevel=static_cast<eWantedLevel>((scenario&8)?1:0);p->GetActiveWeapon().m_Type=static_cast<eWeaponType>((scenario&16)?6:22);q->bInVehicle=(scenario&32)!=0;q->m_pVehicle=(scenario&64)?reinterpret_cast<CVehicle*>(vehicle.data()):nullptr;q->physicalFlags.bSubmergedInWater=(i&128)!=0;vehicle[0x1B8]=uint8((i&256)?3:0);
'''+s[b:]
s=s.replace('fn.slot==6?uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(address)(t,p,i%3,nullptr))','fn.slot==4?uintptr_t(reinterpret_cast<int(__thiscall*)(void*)>(address)(t))')
s=s.replace('fn.slot==6?json(ret)','fn.slot==4?json(ret)')
s=s.replace('}block.IsLoaded=savedLoaded;','}')
a=s.index('if(fn.slot==8){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==10){require("wanted");require("abort");require("say");require("new");}'''+s[b:]
s=s.replace('block.IsLoaded==savedLoaded','true')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index("if __name__",a)
s=s[:a]+'''SOURCES=[G+x for x in ('Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp','Tasks/TaskTypes/TaskComplexKillPedFromBoat.h','Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp','Tasks/TaskTypes/TaskComplexKillPedOnFoot.h','Tasks/TaskTypes/TaskSimpleStandStill.cpp','Tasks/TaskTypes/TaskSimpleStandStill.h')]
'''+s[b:];s=s.replace('gang-join355','boat355').replace('GangJoin355Live','Boat355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','boat355').replace('GangJoinRespond pendientes','KillPedFromBoat pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexKillPedFromBoat: cinco pendientes

Banco pendiente de ejecución: Clone256, GetTaskType256, Next256, First256, Control1024: 2.048 pares. Evidencia ASM en ../Tasks355Evidence/Boat_*.txt.

Compara tareas, ped, asignaciones y trazas originales/candidatas. Control cubre arma6, jugador, estados de vehículo/agua, búsqueda del nivel de búsqueda, aborto y transición entre StandStill y KillPedOnFoot. Se aíslan jugador, wanted, referencias, audio y asignación; no reproduce audio real ni combate. First activa bStayInSamePlace. Constructor auxiliar de KillPedOnFoot es dependencia, no validación adicional.
''',encoding='utf-8')
