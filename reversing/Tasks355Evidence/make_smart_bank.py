from pathlib import Path
p=Path('reversing/SmartFlee355');p.mkdir(exist_ok=True)
s=Path('reversing/HandSignal355/LiveComparison.hpp').read_text().replace('HandSignal355Live','SmartFlee355Live').replace('hand-signal355','smart-flee355').replace('TaskComplexPlayHandSignalAnim','TaskComplexSmartFleePoint')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexSmartFleePoint","SetDefaultTaskWanderDir",0x65BE00,0,-1,256},
{"CTaskComplexSmartFleePoint","ComputeFleeDir",0x65BE40,0,-1,512},
{"CTaskComplexSmartFleePoint","CreateSubTask",0x65BE80,0,-1,512},
{"CTaskComplexSmartFleePoint","SetFleePosition",0x65C3C0,0,-1,512},
{"CTaskComplexSmartFleePoint","Clone",0x65CED0,0x86F744,1,256},
{"CTaskComplexSmartFleePoint","GetTaskType",0x65BDA0,0x86F744,4,256},
{"CTaskComplexSmartFleePoint","MakeAbortable",0x65BDC0,0x86F744,6,512},
{"CTaskComplexSmartFleePoint","CreateNextSubTask",0x65C0C0,0x86F744,8,512},
{"CTaskComplexSmartFleePoint","CreateFirstSubTask",0x65C140,0x86F744,9,512},
{"CTaskComplexSmartFleePoint","ControlSubTask",0x65C1E0,0x86F744,10,1024}};
'''+s[b:]
s=s.replace('inline int subType{},scenario{};', 'alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel;alignas(16) inline std::array<uint8,128> defaultTask;alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;inline CVector changed;inline std::array<void*,11> defaultVT{};inline int subType{},scenario{};')
s=s.replace('P(subtask)','P(subtask) P(defaultTask) P(vehicle) P(intel)')
a=s.index('inline bool __stdcall Arm');b=s.index('inline ReversibleHooks::',a)
s=s[:a]+'''inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=(scenario&1)!=0;trace.push_back({"abort",priority,b});return b;}
inline int __fastcall DefaultType(CTask*,void*){int type=(scenario&2)?0x390:12345;trace.push_back({"default_type",type});return type;}
inline bool __fastcall Looking(IKChainManager_c*,void*,CPed*){bool b=(scenario&4)!=0;trace.push_back({"looking",b});return b;}
inline void __fastcall AbortLook(IKChainManager_c*,void*,CPed*,uint32 time){trace.push_back({"abort_look",time});}
inline void __fastcall Look(IKChainManager_c*,void*,const char* purpose,CPed*,CEntity*,int time,eBoneTag32 bone,CVector* offset,bool torso,float speed,int blend,int priority,bool force){trace.push_back({"look",purpose,time,int(bone),Ptr(uintptr_t(offset)),torso,std::bit_cast<uint32>(speed),blend,priority,force});}
inline bool __fastcall Out(CTaskTimer*,void*){bool b=(scenario&8)!=0;trace.push_back({"out",b});return b;}
inline int16 __fastcall Say(CPed*,void*,int context,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",context,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline int __cdecl Random(){trace.push_back({"rand"});return scenario*701%32768;}
inline CTask* __fastcall Sub(CTaskComplexSmartFleePoint*,void*,int type,CPed*){trace.push_back({"sub",type});return type==0x516?nullptr:reinterpret_cast<CTask*>(arena.data());}
'''+s[b:]
s=s.replace('subVT[4]=reinterpret_cast<void*>(&Type);','subVT[4]=reinterpret_cast<void*>(&Type);subVT[6]=reinterpret_cast<void*>(&Abort);defaultVT[4]=reinterpret_cast<void*>(&DefaultType);')
a=s.index('auto& block=CAnimManager');b=s.index('bool pass=true',a)
s=s[:a]+'const auto savedTime=CTimer::GetTimeInMS();'+s[b:]
a=s.index('EntryPatch ike(');b=s.index('for(int i=0;',a)
s=s[:a]+'''EntryPatch loe(0x6181A0,Looking),lod(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),Looking);
EntryPatch ale(0x618280,AbortLook),ald(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLook);
EntryPatch lke(0x618970,Look),lkd(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),Look);
EntryPatch ote(0x420E30,Out),otd(bit_cast<uintptr_t>(&CTaskTimer::IsOutOfTime),Out);
EntryPatch se(0x5EFFE0,Say),sd(bit_cast<uintptr_t>(&CPed::Say),Say);
EntryPatch rne(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
std::optional<EntryPatch> sube,subd;if(fn.slot>=8){sube.emplace(0x65BE80,Sub);subd.emplace(bit_cast<uintptr_t>(&CTaskComplexSmartFleePoint::CreateSubTask),Sub);}
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%2?0xCB:0x1A9;', 'scenario=i%128;const int types[]{0x2CA,0x390,0xCB,0xF4,0x516,12345};subType=types[(i/16)%6];')
a=s.index('t->m_animationId=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''intel.fill(0);defaultTask.fill(0);vehicle.fill(0);Put(defaultTask.data(),0,defaultVT.data());Put(ped.data(),0x47C,intel.data());Put(intel.data(),0x14,(scenario&16)?defaultTask.data():nullptr);t->m_fleePoint=CVector{float(i%7),float(i%11),float(i%3)};t->m_initalPos=CVector{float(i%13)-4.f,float(i%5)-2.f,0.f};t->m_doScream=(scenario&32)!=0;t->m_fleeTimeMs=(i%3)?1500:-1;t->m_safeDist=float(i%9);t->m_moveState=static_cast<eMoveState>((scenario&64)?6:7);t->m_fleeDir=uint8(i%8);t->m_hasFleePointChanged=(i&128)!=0;t->m_timer.m_nStartTime=123;t->m_timer.m_nInterval=1500;t->m_timer.m_bStarted=(i&256)!=0;t->m_timer.m_bStopped=(i&512)!=0;p->GetPosition()=CVector{float(i%17)-8.f,float(i%19)-9.f,float(i%4)};p->bInVehicle=(scenario&1)!=0;p->m_pVehicle=(scenario&2)?reinterpret_cast<CVehicle*>(vehicle.data()):nullptr;changed=t->m_fleePoint;if(scenario&1)changed.x+=1.f;Put<uint32>(subtask.data(),0x18,uint32(i%4));Put<uint32>(subtask.data(),0x1C,uint32(i%5));Put<uint8>(subtask.data(),0x10,uint8(i%8));Put<uint32>(defaultTask.data(),0x18,uint32(i%3));Put<uint32>(defaultTask.data(),0x1C,uint32(i%4));*reinterpret_cast<uint32*>(0xB7CB84)=4000;
'''+s[b:]
a=s.index('uintptr_t ret=');b=s.index('\nif(candidate)',a)
s=s[:a]+'''uintptr_t ret=0;
if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));
else if(fn.slot==4)ret=uintptr_t(reinterpret_cast<int(__thiscall*)(void*)>(address)(t));
else if(fn.slot==6)ret=uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(address)(t,p,i%3,nullptr));
else if(std::string(fn.name)=="SetDefaultTaskWanderDir")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(address)(t,p);
else if(std::string(fn.name)=="SetFleePosition")reinterpret_cast<void(__thiscall*)(void*,const CVector&,float,bool)>(address)(t,changed,(i&2)?t->m_safeDist:t->m_safeDist+1.f,(i&4)!=0);
else if(std::string(fn.name)=="CreateSubTask")ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(address)(t,types[i%6],p));
else if(std::string(fn.name)=="ComputeFleeDir")ret=uintptr_t(reinterpret_cast<uint32(__thiscall*)(void*,CPed*)>(address)(t,p));
else ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));
'''+s[b:]
s=s.replace('std::string(fn.name)=="GetAnimIdForPed"?json(int32(ret)):Ptr(ret)', '(fn.slot==4||fn.slot==6||std::string(fn.name)=="ComputeFleeDir")?json(uint32(ret)):Ptr(ret)')
s=s.replace('{"ped",Words(ped.data(),ped.size())}', '{"ped",Words(ped.data(),ped.size())},{"subtask",Words(subtask.data(),128)},{"default_task",Words(defaultTask.data(),128)}')
a=s.index('}block.IsLoaded=savedLoaded;');b=s.index('h->State(hs)',a)
s=s[:a]+'}*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;'+s[b:]
a=s.index('if(fn.slot==9){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==9){require("sub");}if(fn.slot==10){require("out");require("abort_look");require("look");require("say");require("sub");}'''+s[b:]
s=s.replace('block.IsLoaded==savedLoaded','CTimer::GetTimeInMS()==savedTime')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index("if __name__",a)
files=['Tasks/TaskTypes/TaskComplexSmartFleePoint','Tasks/TaskTypes/TaskComplexLeaveAnyCar','Tasks/TaskTypes/TaskComplexWanderFlee','Tasks/TaskTypes/TaskComplexWander','Tasks/TaskTypes/TaskSimpleRunAnim','Tasks/TaskTypes/TaskSimpleTired','Tasks/TaskTypes/TaskSimpleStandStill','Tasks/TaskComplexSequence','Tasks/TaskTimer','General']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','smart-flee355').replace('GangJoin355Live','SmartFlee355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','smart-flee355').replace('GangJoinRespond pendientes','SmartFleePoint pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexSmartFleePoint: diez pendientes

Banco preparado para4.864pares; pendiente de ejecución. Fixtures comparan memoria de tareas/ped/subtarea/wander predeterminado y asignacionesCD. Auxiliares de dirección y actualización de posición se prueban directamente; retornos de dos métodos void del EXE fueron corregidos en la firma DLL y no se atribuye valor al EAX indeterminado.

Next/First/Control aíslan la creación de subtareas y recorren coche, timer, nuevo objetivo, distancia segura, orientación, grito y mirada. CreateSub construye tareas reales. Timer IsOutOfTime, audio, mirada y RNG se aíslan; no se valida reproducción ni navegación real. Todos los globals temporales son restaurados. Evidencia en ../Tasks355Evidence; falta confirmar paridad en ejecución.
''',encoding='utf-8')
