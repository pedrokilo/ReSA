from pathlib import Path
p=Path('reversing/VehicleLean355');p.mkdir(exist_ok=True)
s=Path('reversing/Boat355/LiveComparison.hpp').read_text().replace('Boat355Live','VehicleLean355Live').replace('boat355','vehicle-lean355').replace('TaskComplexKillPedFromBoat','TaskGoToVehicleAndLean').replace('#include "TaskComplexKillPedOnFoot.h"','#include "TaskComplexGangLeader.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskGoToVehicleAndLean","Clone",0x6621B0,0x86FAC8,1,256},
{"CTaskGoToVehicleAndLean","MakeAbortable",0x664500,0x86FAC8,6,512},
{"CTaskGoToVehicleAndLean","CreateNextSubTask",0x664590,0x86FAC8,8,1024},
{"CTaskGoToVehicleAndLean","CreateFirstSubTask",0x664D40,0x86FAC8,9,1024},
{"CTaskGoToVehicleAndLean","ControlSubTask",0x664E60,0x86FAC8,10,1024}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix;alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel;inline std::array<void*,11> subVT{};')
s=s.replace('P(vehicle)','P(vehicle) P(matrix) P(intel)')
a=s.index('inline CWanted* __cdecl Wanted');b=s.index('inline void __fastcall Ref',a)
s=s[:a]+'''inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent* event){bool b=(scenario&2)!=0;trace.push_back({"abort",priority,Ptr(uintptr_t(event)),b});return b;}
inline CVector* __fastcall Target(CTaskGoToVehicleAndLean* t,void*,CVector* out,CPed*){t->field_15=(scenario&4)!=0;*out=CVector{float(scenario%7),1.f,.5f};trace.push_back({"target",t->field_15});return out;}
inline CVector* __fastcall Speed(CPhysical*,void*,CVector* out,CVector point){*out=CVector{(scenario&8)?.02f:0.f,0.f,0.f};trace.push_back({"speed",std::bit_cast<uint32>(point.x),std::bit_cast<uint32>(point.y),std::bit_cast<uint32>(point.z)});return out;}
inline bool __cdecl Should(){bool b=(scenario&16)!=0;trace.push_back({"should",b});return b;}
inline int __cdecl Block(const char* name){trace.push_back({"block",name});return 0;}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Remove(int index){trace.push_back({"remove_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
'''+s[b:]
s=s.replace('bool pass=true,selected=false;int total=0;', 'auto& block=CAnimManager::GetAnimBlocks()[0];const auto savedLoaded=block.IsLoaded;bool pass=true,selected=false;int total=0;')
a=s.index('EntryPatch pe(');b=s.index('EntryPatch refe',a)
s=s[:a]+'''EntryPatch te(0x664770,Target),td(bit_cast<uintptr_t>(&CTaskGoToVehicleAndLean::CalcTargetPos),Target);
EntryPatch spe(0x542CE0,Speed),spd(bit_cast<uintptr_t>(&CPhysical::GetSpeed),Speed);
EntryPatch she(0x65E7F0,Should),shd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),Should);
EntryPatch be(0x4D3990,Block),bd(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch rme(0x4D3FD0,Remove),rmd(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Remove);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;', 'scenario=i%128;const int types[]{0x387,0x386,0xCB,12345};subType=types[(i/16)%4];')
a=s.index('t->m_Ped=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''vehicle.fill(0);matrix.fill(0);intel.fill(0);auto*v=reinterpret_cast<CVehicle*>(vehicle.data());Put(vehicle.data(),0x14,matrix.data());auto*m=reinterpret_cast<CMatrix*>(matrix.data());m->GetRight()=CVector{1.f,0.f,0.f};m->GetForward()=CVector{0.f,1.f,0.f};m->GetUp()=CVector{0.f,0.f,1.f};m->GetPosition()=CVector{0.f,0.f,0.f};t->m_Vehicle=(scenario&1)?nullptr:v;t->m_LeanAnimDurationInMs=i%2?500:3000;t->m_LeanOnVehicle=(scenario&32)!=0;t->field_15=(scenario&4)!=0;t->m_TargetPos=CVector{float((scenario%7)+(i%3==0?0:1)),1.f,.5f};v->vehicleFlags.bHasGangLeaningOn=(i&128)!=0;v->m_pDriver=(i&256)?p:nullptr;p->m_pIntelligence=reinterpret_cast<CPedIntelligence*>(intel.data());Put<int32>(intel.data(),0x274,(i&512)?31:30);block.IsLoaded=(scenario&64)!=0;
'''+s[b:]
s=s.replace('uintptr_t ret=fn.slot==1?', 'uintptr_t ret=fn.slot==6?uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(address)(t,p,i%3,(i&4)?reinterpret_cast<CEvent*>(player.data()):nullptr)):fn.slot==1?')
s=s.replace('fn.slot==4?json(ret):Ptr(ret)', 'fn.slot==6?json(ret):Ptr(ret)')
s=s.replace('{"task",Words(task.data(),task.size())}', '{"vehicle",Words(vehicle.data(),vehicle.size())},{"task",Words(task.data(),task.size())}')
a=s.index('if(fn.slot==10){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==6){require("abort");}if(fn.slot==9){require("target");require("new");}if(fn.slot==10){require("speed");require("target");require("add_ref");require("remove_ref");require("request");}'''+s[b:]
s=s.replace('Write({{"event",pass&&selected?', 'block.IsLoaded=savedLoaded;Write({{"event",pass&&selected?')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index('if __name__',a)
files=['Tasks/TaskTypes/TaskGoToVehicleAndLean','Tasks/TaskTypes/TaskLeanOnVehicle','Tasks/TaskTypes/TaskSimpleStandStill','Tasks/TaskTypes/TaskSimpleAchieveHeading','Tasks/TaskTypes/TaskComplexGoToPointAndStandStill','Tasks/TaskTypes/TaskComplexGangLeader','Core/Vector','Core/Matrix','General','Entity/Physical']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','vehicle-lean355').replace('GangJoin355Live','VehicleLean355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','vehicle-lean355').replace('GangJoinRespond pendientes','GoToVehicleAndLean pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskGoToVehicleAndLean: cinco pendientes

Banco preparado de 3.840 pares, pendiente de ejecución. Clone 256, Abort 512 y Next/First/Control 1024 cada una. Se corrigió el tamaño ABI a 0x24, con vector objetivo en +0x18 que Clone no inicializa. Compara tarea, ped, vehículo, asignaciones completas salvo vptr y trazas. Incluye vehículo nulo/reservado, doble interrupción, contador 30/31, estados de subtarea y carga de gangs.

CalcTargetPos se reconstruyó como dependencia nativa, pero sus resultados y GetSpeed se aíslan en las comparaciones de transición; no se afirma validar geometría ni física real. El estado de carga del bloque se restaura. Evidencia REA y ASM en ../Tasks355Evidence y ../_handoff/rea/rea-x86/tasks355-pseudo.
''')
