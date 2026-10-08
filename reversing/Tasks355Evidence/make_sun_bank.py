from pathlib import Path
p=Path('reversing/Sunbathe355');p.mkdir(exist_ok=True)
s=Path('reversing/Boat355/LiveComparison.hpp').read_text().replace('Boat355Live','Sunbathe355Live').replace('boat355','sunbathe355').replace('TaskComplexKillPedFromBoat','TaskComplexSunbathe').replace('#include "TaskComplexKillPedOnFoot.h"','#include "InterestingEvents.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexSunbathe","Clone",0x6366A0,0x86E0AC,1,256},
{"CTaskComplexSunbathe","GetTaskType",0x632040,0x86E0AC,4,256},
{"CTaskComplexSunbathe","MakeAbortable",0x6320F0,0x86E0AC,6,512},
{"CTaskComplexSunbathe","CreateNextSubTask",0x6399F0,0x86E0AC,8,1024},
{"CTaskComplexSunbathe","CreateFirstSubTask",0x639CB0,0x86E0AC,9,1024},
{"CTaskComplexSunbathe","ControlSubTask",0x6381A0,0x86E0AC,10,1024}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CAnimBlock)> beach,sun;alignas(16) inline std::array<uint8,64> event;inline std::array<void*,11> subVT{},eventVT{};')
s=s.replace('P(vehicle)','P(vehicle) P(beach) P(sun) P(event)')
a=s.index('inline CWanted* __cdecl Wanted');b=s.index('inline void __fastcall Ref',a)
s=s[:a]+'''inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=(scenario&2)!=0;trace.push_back({"abort",priority,b});return b;}
inline int __fastcall EventType(CEvent*,void*){int r=(scenario/4)%5;trace.push_back({"event_type",r});return r;}
inline int __cdecl BlockIndex(const char* name){int r=std::string(name)=="beach"?3:4;trace.push_back({"block_index",name,r});return r;}
inline CAnimBlock* __cdecl Block(const char* name){trace.push_back({"block",name});return reinterpret_cast<CAnimBlock*>(std::string(name)=="beach"?beach.data():sun.data());}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
inline int __cdecl Random(){int r=(scenario*701)%32768;trace.push_back({"rand",r});return r;}
inline bool __cdecl IsSunbather(int model){bool b=(scenario&32)!=0;trace.push_back({"sunbather",model,b});return b;}
inline bool __fastcall Out(CTaskTimer*,void*){bool b=(scenario&64)!=0;trace.push_back({"out",b});return b;}
inline void __fastcall Interesting(CInterestingEvents*,void*,int type,CEntity* ped){trace.push_back({"interesting",type,Ptr(uintptr_t(ped))});}
inline CTask* __fastcall Sub(CTaskComplexSunbathe*,void*,int type,CPed*){trace.push_back({"sub",type});return type==0x516?nullptr:reinterpret_cast<CTask*>(arena.data());}
'''+s[b:]
s=s.replace('subVT[6]=reinterpret_cast<void*>(&Abort);','subVT[6]=reinterpret_cast<void*>(&Abort);eventVT[1]=reinterpret_cast<void*>(&EventType);')
s=s.replace('bool pass=true,selected=false;int total=0;', 'const auto savedTime=CTimer::GetTimeInMS();const auto savedHour=CClock::ms_nGameClockHours;auto& weather=*reinterpret_cast<int16*>(0xC8131C);const auto savedWeather=weather;const auto savedFocus=CWorld::PlayerInFocus;const auto savedPlayer=CWorld::Players[0].m_pPed;bool pass=true,selected=false;int total=0;')
a=s.index('EntryPatch pe(');b=s.index('EntryPatch refe',a)
s=s[:a]+'''EntryPatch bi(0x4D3990,BlockIndex),bid(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),BlockIndex);
EntryPatch be(0x4D3940,Block),bd(bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
EntryPatch re(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch ise(0x611760,IsSunbather),isd(bit_cast<uintptr_t>(static_cast<bool(*)(eModelID)>(&CPopulation::IsSunbather)),IsSunbather);
EntryPatch oe(0x420E30,Out),od(bit_cast<uintptr_t>(&CTaskTimer::IsOutOfTime),Out);
EntryPatch ie(0x602590,Interesting),id(bit_cast<uintptr_t>(&CInterestingEvents::Add),Interesting);
std::optional<EntryPatch> sube,subd;if(fn.slot==8||fn.slot==9){sube.emplace(0x638290,Sub);subd.emplace(bit_cast<uintptr_t>(&CTaskComplexSunbathe::CreateSubTask),Sub);}
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;', 'scenario=i%256;const int types[]{0xCA,0x1A2,0x1AC,0x1AD,0x1AE,12345};subType=types[(i/16)%6];')
a=s.index('t->m_Ped=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''beach.fill(0);sun.fill(0);event.fill(0);vehicle.fill(0);Put(event.data(),0,eventVT.data());t->m_pTowel=nullptr;t->m_pBeachAnimBlock=reinterpret_cast<CAnimBlock*>(beach.data());t->m_pSunbatheAnimBlock=reinterpret_cast<CAnimBlock*>(sun.data());t->m_BeachAnimBlockIndex=3;t->m_SunbatheAnimBlockIndex=4;t->m_SunbatherType=static_cast<eSunbatherType>(i%5);t->m_bStartStanding=(i&1)!=0;t->m_bBathing=(i&2)!=0;t->m_bBeachAnimsReferenced=(i&4)!=0;t->m_bSunbatheAnimsReferenced=(i&8)!=0;t->m_bAborted=(i&16)!=0;beach[16]=uint8((i&32)!=0);sun[16]=uint8((i&64)!=0);t->m_BathingTimer.m_nStartTime=3000;t->m_BathingTimer.m_nInterval=20000;t->m_BathingTimer.m_bStarted=(i&128)!=0;t->m_BathingTimer.m_bStopped=false;p->m_nPedType=static_cast<ePedType>(i%3==0?5:i%3==1?22:4);p->m_nModelIndex=103;auto*q=reinterpret_cast<CPed*>(player.data());q->bInVehicle=(i&256)!=0;q->m_pVehicle=reinterpret_cast<CVehicle*>(vehicle.data());reinterpret_cast<CVehicle*>(vehicle.data())->m_vecMoveSpeed=CVector{float(i%4)/8.f,0,0};CWorld::PlayerInFocus=0;CWorld::Players[0].m_pPed=q;CTimer::m_snTimeInMilliseconds=4000;CClock::ms_nGameClockHours=uint8(8+(i/32)%12);weather=int16((i/16)%22);
'''+s[b:]
s=s.replace('uintptr_t ret=fn.slot==1?', 'uintptr_t ret=fn.slot==6?uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(address)(t,p,i%3,(i&1)?reinterpret_cast<CEvent*>(event.data()):nullptr)):fn.slot==1?')
s=s.replace('fn.slot==4?json(ret):Ptr(ret)', '(fn.slot==4||fn.slot==6)?json(ret):Ptr(ret)')
a=s.index('if(fn.slot==10){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==6){require("abort");require("event_type");}if(fn.slot==8){require("interesting");require("sub");}if(fn.slot==9){require("rand");require("request");require("sub");}if(fn.slot==10){require("request");require("add_ref");require("abort");}'''+s[b:]
s=s.replace('Write({{"event",pass&&selected?', 'CTimer::m_snTimeInMilliseconds=savedTime;CClock::ms_nGameClockHours=savedHour;weather=savedWeather;CWorld::PlayerInFocus=savedFocus;CWorld::Players[0].m_pPed=savedPlayer;Write({{"event",pass&&selected?')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index('if __name__',a)
files=['Tasks/TaskTypes/TaskComplexSunbathe','Tasks/TaskTypes/TaskSimplePause','Tasks/TaskTypes/TaskSimpleRunAnim','Tasks/TaskTypes/TaskSimpleRunTimedAnim','Tasks/TaskTimer','InterestingEvents','Population','Core/Vector']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','sunbathe355').replace('GangJoin355Live','Sunbathe355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','sunbathe355').replace('GangJoinRespond pendientes','Sunbathe pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexSunbathe: seis pendientes

Banco preparado de 4.096 pares: Clone 256, GetTaskType 256, MakeAbortable 512, Next 1024, First 1024 y Control 1024. Pendiente de compilación y ejecución. Se comparan retorno, memoria de tarea y ped, asignaciones y trazas. Incluye horas, 22 tipos de clima, carga de bloques, interrupciones, estados de animación y vehículo del jugador; restaura reloj, clima y jugador.

CreateSubTask se reconstruye como dependencia nativa pero se aísla al probar las transiciones. Clone usa constructor real. No se afirma validar reproducción de animaciones o streaming real. Pseudocódigo REA recuperado con rangos completos en ../_handoff/rea/rea-x86/tasks355-pseudo y ASM en ../Tasks355Evidence.
''')
