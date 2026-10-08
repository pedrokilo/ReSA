from pathlib import Path
p=Path('reversing/Wander355');p.mkdir(exist_ok=True)
s=Path('reversing/GangJoin355/LiveComparison.hpp').read_text().replace('GangJoin355Live','Wander355Live').replace('gang-join355','wander355').replace('TaskComplexGangJoinRespond','TaskComplexWanderStandard').replace('#include "TaskComplexGangLeader.h"','#include "EventSexyVehicle.h"\n#include "EventChatPartner.h"\n#include "EventAcquaintancePedLike.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexWanderStandard","WillChat",0x66AED0,0,-1,512},
{"CTaskComplexWanderStandard","SetNextMinScanTime",0x66AF60,0,-1,256},
{"CTaskComplexWanderStandard","LookForSexyCars",0x66AFD0,0,-1,512},
{"CTaskComplexWanderStandard","LookForChatPartners",0x66FDA0,0,-1,1024},
{"CTaskComplexWanderStandard","LookForGangMembers",0x670100,0,-1,512},
{"CTaskComplexWanderStandard","Clone",0x48E530,0x85A200,1,256}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel,otherIntel;alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;alignas(16) inline std::array<uint8,sizeof(tHandlingData)> handling;alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix,otherMatrix,vehicleMatrix;alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group;inline std::array<void*,15> subVT{},taskVT{};')
s=s.replace('P(subtask)','P(subtask) P(intel) P(otherIntel) P(vehicle) P(handling) P(matrix) P(otherMatrix) P(vehicleMatrix) P(group)')
a=s.index('inline bool __cdecl Load()');b=s.index('inline ReversibleHooks::',a)
s=s[:a]+'''inline bool __fastcall IsPlayer(const CPed*p,void*){bool b=(scenario&(p==reinterpret_cast<CPed*>(ped.data())?1:2))!=0;trace.push_back({"player",Ptr(uintptr_t(p)),b});return b;}
inline bool __cdecl Gang(int type){bool b=type>=7&&type<=16;return b;}
inline bool __cdecl Friends(const CPed&a,const CPed&b){bool r=(scenario&4)==0;trace.push_back({"friends",Ptr(uintptr_t(&a)),Ptr(uintptr_t(&b)),r});return r;}
inline CTask* __fastcall Active(CTaskManager*p,void*){bool yes=(scenario&8)==0;trace.push_back({"active",Ptr(uintptr_t(p)),yes});return yes?reinterpret_cast<CTask*>(subtask.data()):nullptr;}
inline int __fastcall Kind(CTask*,void*){trace.push_back({"kind",0});return 0;}
inline bool __fastcall Pavement(SurfaceInfos_c*,void*,SurfaceId surface){bool r=(scenario&16)==0;trace.push_back({"pavement",int(surface),r});return r;}
inline bool __cdecl Busy(){bool r=(scenario&32)!=0;trace.push_back({"busy",r});return r;}
inline bool __cdecl Riot(){bool r=(scenario&64)!=0;trace.push_back({"riot",r});return r;}
inline CTask* __fastcall Find(CPedIntelligence*p,void*,int type){bool r=(scenario&128)!=0;trace.push_back({"find",Ptr(uintptr_t(p)),type,r});return r?reinterpret_cast<CTask*>(subtask.data()):nullptr;}
inline CEvent* __fastcall Event(const CEventGroup*,void*,int type){trace.push_back({"event_find",type});return nullptr;}
inline bool __cdecl LOS(const CVector&a,const CVector&b,bool c,bool d,bool e,bool f,bool g,bool h,bool i){bool r=(scenario&256)==0;trace.push_back({"los",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),c,d,e,f,g,h,i,r});return r;}
inline CPedGroup* __cdecl Group(const CPed*p){bool r=p==reinterpret_cast<CPed*>(player.data())&&(scenario&512)==0;trace.push_back({"group",Ptr(uintptr_t(p)),r});return r?reinterpret_cast<CPedGroup*>(group.data()):nullptr;}
inline int __fastcall Members(const CPedGroupMembership*,void*){trace.push_back({"members",1});return 1;}
inline void __fastcall Target(CTaskComplexWander*,void*,const CPed*,CVector& out,const CNodeAddress&){out=CVector{0.f,0.f,0.f};trace.push_back({"target"});}
inline CEvent* __fastcall AddEvent(CEventGroup*p,void*,CEvent* e,bool valid){int type=e->GetEventType();json payload=nullptr;auto*b=reinterpret_cast<uint8*>(e);if(type==EVENT_SEXY_VEHICLE)payload=Ptr(*reinterpret_cast<uintptr_t*>(b+12));else if(type==EVENT_CHAT_PARTNER)payload=json::array({bool(b[12]),Ptr(*reinterpret_cast<uintptr_t*>(b+16))});else payload=json::array({int(*reinterpret_cast<int16*>(b+14)),Ptr(*reinterpret_cast<uintptr_t*>(b+20))});trace.push_back({"add_event",Ptr(uintptr_t(p)),type,payload,valid});return nullptr;}
inline void __fastcall Ref(CEntity*p,void*,CEntity**){trace.push_back({"reference",Ptr(uintptr_t(p))});}
inline void __fastcall Clean(CEntity*p,void*,CEntity**){trace.push_back({"cleanup",Ptr(uintptr_t(p))});}
'''+s[b:]
s=s.replace('subVT[4]=reinterpret_cast<void*>(&Type);','subVT[4]=reinterpret_cast<void*>(&Type);subVT[11]=reinterpret_cast<void*>(&Kind);')
a=s.index('auto& block=CAnimManager');b=s.index('bool pass=true',a)
s=s[:a]+'''const auto savedTime=CTimer::GetTimeInMS();const auto savedFocus=CWorld::PlayerInFocus;auto* savedPlayer=CWorld::Players[0].m_pPed;'''+s[b:]
a=s.index('EntryPatch pe(');b=s.index('for(int i=0;',a)
s=s[:a]+'''EntryPatch pe(0x5DF8F0,IsPlayer),pd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
EntryPatch fr(0x601D10,Friends),frd(bit_cast<uintptr_t>(&CPedIntelligence::AreFriends),Friends);
EntryPatch ac(0x681720,Active),acd(bit_cast<uintptr_t>(&CTaskManager::GetActiveTask),Active);
EntryPatch pa(0x55E7F0,Pavement),pad(bit_cast<uintptr_t>(&SurfaceInfos_c::IsPavement),Pavement);
EntryPatch bu(0x4076A0,Busy),bud(bit_cast<uintptr_t>(&CStreaming::IsVeryBusy),Busy);
EntryPatch ri(0x441C10,Riot),rid(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),Riot);
EntryPatch fi(0x600EE0,Find),fid(bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),Find);
EntryPatch ev(0x4AB650,Event),evd(bit_cast<uintptr_t>(&CEventGroup::GetEventOfType),Event);
EntryPatch los(0x56A490,LOS),losd(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),LOS);
EntryPatch gr(0x5F7E80,Group),grd(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),Group);
EntryPatch mem(0x5F6AA0,Members),memd(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembersExcludingLeader),Members);
EntryPatch ta(0x669F60,Target),tad(bit_cast<uintptr_t>(&CTaskComplexWander::ComputeTargetPos),Target);
EntryPatch adde(0x4AB420,AddEvent),addd(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),AddEvent);
EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch cle(0x571A00,Clean),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
'''+s[b:]
s=s.replace('scenario=i%32;subType=i%3==0?0x190:i%3==1?TASK_COMPLEX_TURN_TO_FACE_ENTITY:TASK_SIMPLE_STAND_STILL;', 'scenario=i;subType=TASK_COMPLEX_WANDER;')
s=s.replace('auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);','auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);')
a=s.index('t->m_response=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''intel.fill(0);otherIntel.fill(0);vehicle.fill(0);handling.fill(0);matrix.fill(0);otherMatrix.fill(0);vehicleMatrix.fill(0);group.fill(0);Put(ped.data(),0x47C,intel.data());Put(player.data(),0x47C,otherIntel.data());Put(ped.data(),0x14,matrix.data());Put(player.data(),0x14,otherMatrix.data());Put(vehicle.data(),0x14,vehicleMatrix.data());reinterpret_cast<CMatrix*>(matrix.data())->GetForward()=CVector{0.f,1.f,0.f};reinterpret_cast<CMatrix*>(otherMatrix.data())->GetForward()=CVector{0.f,-1.f,0.f};p->GetPosition()=CVector{0.f,1.f,0.f};q->GetPosition()=CVector{0.f,2.f,0.f};auto*v=reinterpret_cast<CVehicle*>(vehicle.data());v->GetPosition()=CVector{0.f,float(i%7),0.f};v->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());v->m_pHandlingData->m_nMonetaryValue=(i&4)?40000:40001;v->m_fHealth=(i&8)?500.f:501.f;Put(intel.data(),0xE0+4*((i/8)%16),(i&16)?nullptr:vehicle.data());Put(intel.data(),0x130+4*((i/16)%16),player.data());t->m_nMoveState=static_cast<eMoveState>(i%3==0?PEDMOVE_WALK:PEDMOVE_RUN);t->m_nDir=uint8(i%8);t->m_bWanderSensibly=(i&2)!=0;t->m_nMinNextScanTime=0;t->m_LastNode.m_wAreaId=0;t->m_LastNode.m_wNodeId=0;t->m_NextNode.m_wAreaId=0;t->m_NextNode.m_wNodeId=0;p->m_nPedType=std::string(fn.name)=="WillChat"?static_cast<ePedType>(i%32):PED_TYPE_CIVMALE;q->m_nPedType=std::string(fn.name)=="WillChat"?static_cast<ePedType>((i/16)%32):PED_TYPE_CIVMALE;std::memcpy(taskVT.data(),reinterpret_cast<void*>(0x85A200),15*4);taskVT[4]=reinterpret_cast<void*>(&Type);taskVT[11]=reinterpret_cast<void*>(&Kind);Put(task.data(),0,taskVT.data());CWorld::PlayerInFocus=0;CWorld::Players[0].m_pPed=q;*reinterpret_cast<uint32*>(0xB7CB84)=4000;
'''+s[b:]
a=s.index('uintptr_t ret=');b=s.index('\nif(candidate)',a)
s=s[:a]+'''uintptr_t ret=0;if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));else if(std::string(fn.name)=="WillChat")ret=uintptr_t(reinterpret_cast<bool(__stdcall*)(const CPed&,const CPed&)>(address)(*p,*q));else if(std::string(fn.name)=="SetNextMinScanTime")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(address)(t,p);else ret=uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(address)(t,p));
'''+s[b:]
s=s.replace('fn.slot==6?json(ret):Ptr(ret)', 'fn.slot==1?Ptr(ret):json(ret)')
s=s.replace('{"ped",Words(ped.data(),ped.size())}', '{"ped",Words(ped.data(),ped.size())},{"subtask",Words(subtask.data(),subtask.size())}')
a=s.index('}block.IsLoaded=savedLoaded;');b=s.index('h->State(hs)',a)
s=s[:a]+'''}*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;CWorld::PlayerInFocus=savedFocus;CWorld::Players[0].m_pPed=savedPlayer;'''+s[b:]
a=s.index('if(fn.slot==8){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(std::string(fn.name).starts_with("LookFor")){require("nonnull");require("add_event");}'''+s[b:]
s=s.replace('block.IsLoaded==savedLoaded','CTimer::GetTimeInMS()==savedTime&&CWorld::Players[0].m_pPed==savedPlayer&&CWorld::PlayerInFocus==savedFocus')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index("if __name__",a)
files=['Tasks/TaskTypes/TaskComplexWanderStandard','Tasks/TaskTypes/TaskComplexWander','Tasks/TaskTimer','Events/EventSexyVehicle','Events/EventChatPartner','Events/EventAcquaintancePed','Events/EventEditableResponse','PedIntelligence','PedGroups','Entity/Vehicle/Vehicle','Core/Vector']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','wander355').replace('GangJoin355Live','Wander355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','wander355').replace('GangJoinRespond pendientes','WanderStandard pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexWanderStandard: seis pendientes

2.944pares previstos; pendiente de ejecución. Incluye WillChat stdcall, tiempo mínimo de escaneo, SexyCars/ChatPartners/GangMembers con fixtures positivos y negativos, y Clone sensible. Fake peds/inteligencia,16slots scanner con huecos, matrices y vehículo se comparan; eventos se inspeccionan semánticamente y referencias se trazan.

Se aíslan amistad, estado de pavement, streaming/riots, tareas activas, eventos anteriores, visibilidad, grupos y destino de nodo; no se ejecuta navegación ni población real. Eventos temporales conservan contenido definido; no se compara relleno de pila no inicializado. Jugador, foco y tiempo se restauran. Evidencia ../Tasks355Evidence.
''',encoding='utf-8')
