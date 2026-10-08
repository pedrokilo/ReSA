from pathlib import Path
p=Path('reversing/GangFollower355');p.mkdir(exist_ok=True)
s=Path('reversing/Boat355/LiveComparison.hpp').read_text().replace('Boat355Live','GangFollower355Live').replace('boat355','gang-follower355').replace('TaskComplexKillPedFromBoat','TaskComplexGangFollower').replace('#include "TaskComplexKillPedOnFoot.h"','#include "TaskComplexGangLeader.h"\n#include "TaskSimpleGoToPoint.h"\n#include "TaskSimpleHoldEntity.h"\n#include "EventPassObject.h"\n#include "Fx/FxManager.h"\n#include "Ragdoll/IKChainManager.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskComplexGangFollower","Constructor",0x65EAA0,0,-1,256},
{"CTaskComplexGangFollower","Destructor",0x65EBB0,0,-2,256},
{"CTaskComplexGangFollower","Clone",0x65ECB0,0x86F938,1,256},
{"CTaskComplexGangFollower","MakeAbortable",0x65EC30,0x86F938,6,512},
{"CTaskComplexGangFollower","CreateNextSubTask",0x665E00,0x86F938,8,1024},
{"CTaskComplexGangFollower","CreateFirstSubTask",0x666160,0x86F938,9,1024},
{"CTaskComplexGangFollower","ControlSubTask",0x662A10,0x86F938,10,1024}};
'''+s[b:]
s=s.replace('inline std::array<uint8,128> arena,subtask;', 'inline std::array<uint8,256> arena,subtask;')
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CPed)> receiver;alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group;alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix,pedMatrix;alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel,leaderIntel,receiverIntel;alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;alignas(16) inline std::array<uint8,sizeof(CObject)> held;alignas(16) inline std::array<uint8,128> gotoTask;alignas(16) inline std::array<uint8,sizeof(RpClump)> clump;alignas(16) inline std::array<uint8,sizeof(RwFrame)> frame;alignas(16) inline std::array<uint8,sizeof(FxSystem_c)> fx;inline CVector constructorPos;inline int mode;inline std::array<void*,11> subVT{};')
s=s.replace('P(vehicle)','P(vehicle) P(receiver) P(group) P(matrix) P(pedMatrix) P(intel) P(leaderIntel) P(receiverIntel) P(assoc) P(held) P(gotoTask) P(clump) P(frame) P(fx)')
s=s.replace('return reinterpret_cast<CPed*>(player.data());}', 'return reinterpret_cast<CPed*>((scenario&2)?player.data():receiver.data());}')
a=s.index('inline CWanted* __cdecl Wanted');b=s.index('inline ReversibleHooks::',a)
s=s[:a]+'''inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=(scenario&1)!=0;trace.push_back({"abort",priority,b});return b;}
inline void __fastcall Ref(CEntity* p,void*,CEntity**){trace.push_back({"reference",Ptr(uintptr_t(p))});}
inline void __fastcall Clean(CEntity* p,void*,CEntity**){trace.push_back({"cleanup",Ptr(uintptr_t(p))});}
inline bool __cdecl Should(){bool b=mode!=7&&(scenario&2)!=0;trace.push_back({"should",b});return b;}
inline int __cdecl Block(const char* name){trace.push_back({"block",name});return 0;}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Remove(int index){trace.push_back({"remove_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
inline int __cdecl Random(){int r=(scenario*701)%32768;trace.push_back({"rand",r});return r;}
inline int __cdecl Range(int low,int high){int r=low+scenario%(high-low);if(low==0&&high==500)r=mode==10?51+scenario%5:mode==11?100:mode==13?200:201;if(low==0&&high==100)r=mode==9?96:mode==14?50:0;if(low==0&&high==30)r=20;if(low==0&&high==2000)r=500;trace.push_back({"range",low,high,r});return r;}
inline int __fastcall Count(CPedGroupMembership*,void*){trace.push_back({"count"});return 5;}
inline CPed* __fastcall Member(CPedGroupMembership*,void*,int index){trace.push_back({"member",index});return reinterpret_cast<CPed*>(index%2?ped.data():receiver.data());}
inline CPed* __fastcall Leader(CPedGroupMembership*,void*){trace.push_back({"leader"});return reinterpret_cast<CPed*>(player.data());}
inline void __fastcall RemoveMember(CPedGroupMembership*,void*,CPed*){trace.push_back({"remove_member"});}
inline void __fastcall SetPrimary(CTaskManager*,void*,CTask* t,int slot,bool extra){trace.push_back({"set_primary",Ptr(uintptr_t(t)),slot,extra});}
inline void __fastcall SetSecondary(CTaskManager*,void*,CTask* t,int slot){trace.push_back({"set_secondary",Ptr(uintptr_t(t)),slot});}
inline CTask* __fastcall Secondary(CTaskManager*,void*,int slot){trace.push_back({"secondary",slot});return (scenario&4)?reinterpret_cast<CTask*>(gotoTask.data()):nullptr;}
inline int16 __fastcall Say(CPed*,void*,int context,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",context,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline bool __fastcall Visible(CEntity*,void*){bool b=mode>=8||bool(scenario&8);trace.push_back({"visible",b});return b;}
inline bool __stdcall Looking(CPed*){bool b=(scenario&16)!=0;trace.push_back({"looking",b});return b;}
inline void __fastcall Look(IKChainManager_c*,void*,const char* label,CPed*,CEntity* target,int time,int bone,CVector*,bool torso,float speed,int blend,int priority,bool force){trace.push_back({"look",label,Ptr(uintptr_t(target)),time,bone,torso,std::bit_cast<uint32>(speed),blend,priority,force});}
inline void __fastcall AbortLook(IKChainManager_c*,void*,CPed*,uint32 time){trace.push_back({"abort_look",time});}
inline CAnimBlendAssociation* __cdecl Association(RpClump*,int anim){trace.push_back({"association",anim});if(anim==0&&mode>=3&&mode<=4)return reinterpret_cast<CAnimBlendAssociation*>(assoc.data());if(mode==12&&(anim==300||anim==302))return reinterpret_cast<CAnimBlendAssociation*>(assoc.data());return nullptr;}
inline int __fastcall Move(CPedIntelligence*,void*){int r=mode>=3&&mode<=4?4:1;trace.push_back({"move",r});return r;}
inline void __fastcall MoveSpeed(CPed*,void*,CAnimBlendAssociation* a){trace.push_back({"move_speed"});a->m_Speed=(scenario&32)?.9f:1.1f;}
inline CTask* __fastcall Find(CTaskManager*,void*,int type){trace.push_back({"find",type});if(type==900&&mode>=3&&mode<=4)return reinterpret_cast<CTask*>(gotoTask.data());if(type==0x133&&mode==14)return reinterpret_cast<CTask*>(gotoTask.data());return nullptr;}
inline void __fastcall UpdatePoint(CTaskSimpleGoToPoint* t,void*,const CVector& target,float radius,bool dont){trace.push_back({"update_point",std::bit_cast<uint32>(target.x),std::bit_cast<uint32>(target.y),std::bit_cast<uint32>(target.z),std::bit_cast<uint32>(radius),dont});t->m_vecTargetPoint=target;t->m_fRadius=radius;}
inline bool __fastcall Out(CTaskTimer*,void*){trace.push_back({"out"});return (scenario&1)!=0;}
inline FxSystem_c* __fastcall CreateFx(FxManager_c*,void*,const char* name,const CVector&,RwMatrix*,bool){trace.push_back({"fx",name});return reinterpret_cast<FxSystem_c*>(fx.data());}
inline void __fastcall AttachFx(FxSystem_c*,void*,CEntity*,int bone){trace.push_back({"fx_attach",bone});}
inline void __fastcall KillFx(FxSystem_c*,void*){trace.push_back({"fx_play_kill"});}
inline bool __fastcall HandSignal(CPed*,void*){bool b=(scenario&32)!=0;trace.push_back({"hand_signal",b});return b;}
inline CEntity* __fastcall Held(CPed* p,void*){trace.push_back({"held",Ptr(uintptr_t(p))});return p==reinterpret_cast<CPed*>(ped.data())&&mode>=12?reinterpret_cast<CEntity*>(held.data()):nullptr;}
inline CPed* __cdecl Pass(CPed*,CPedGroup*){trace.push_back({"pass_target"});return reinterpret_cast<CPed*>(receiver.data());}
inline CEvent* __fastcall EventAdd(CEventGroup*,void*,CEvent* e,bool){auto*p=static_cast<CEventPassObject*>(e);trace.push_back({"pass_event",Ptr(uintptr_t(p->m_giver)),p->m_dontPassObject});return nullptr;}
inline int __cdecl Ambient(CPed*,CEntity*){trace.push_back({"ambient"});return 0x117;}
inline void __fastcall HoldAnim(CTaskSimpleHoldEntity*,void*,int anim,int group){trace.push_back({"hold_anim",anim,group});}
'''+s[b:]
s=s.replace('bool pass=true,selected=false;int total=0;', 'const auto savedTime=CTimer::GetTimeInMS();auto& block=CAnimManager::GetAnimBlocks()[0];const auto savedLoaded=block.IsLoaded;bool pass=true,selected=false;int total=0;')
s=s.replace('auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);','auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);')
a=s.index('EntryPatch we(');b=s.index('for(int i=',a)
s=s[:a]+'''EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch cle(0x571A00,Clean),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
EntryPatch she(0x65E7F0,Should),shd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),Should);
EntryPatch be(0x4D3990,Block),bd(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch rme(0x4D3FD0,Remove),rmd(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Remove);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
EntryPatch re(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch rge(0x407180,Range),rgd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CGeneral::GetRandomIntInRange)),Range);
EntryPatch ce(0x5F6A50,Count),cd(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembers),Count);
EntryPatch me(0x5F69B0,Member),md(bit_cast<uintptr_t>(&CPedGroupMembership::GetMember),Member);
EntryPatch mle(0x5F69A0,Leader),mld(bit_cast<uintptr_t>(&CPedGroupMembership::GetLeader),Leader);
EntryPatch mree(0x5FB210,RemoveMember),mred(bit_cast<uintptr_t>(static_cast<void(CPedGroupMembership::*)(CPed*)>(&CPedGroupMembership::RemoveMember)),RemoveMember);
EntryPatch pre(0x681AF0,SetPrimary),prd(bit_cast<uintptr_t>(&CTaskManager::SetTask),SetPrimary);
EntryPatch sse(0x681B60,SetSecondary),ssd(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),SetSecondary);
EntryPatch sce(0x681810,Secondary),scd(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
EntryPatch sae(0x5EFFE0,Say),sad(bit_cast<uintptr_t>(&CPed::Say),Say);
EntryPatch ve(0x536BC0,Visible),vd(bit_cast<uintptr_t>(&CEntity::IsVisible),Visible);
EntryPatch le(0x6181A0,Looking),ld(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),Looking);
EntryPatch loe(0x618970,Look),lod(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),Look);
EntryPatch ale(0x618280,AbortLook),ald(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLook);
EntryPatch ase(0x4D68B0,Association),asd(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),Association);
EntryPatch movee(0x601D70,Move),moved(bit_cast<uintptr_t>(&CPedIntelligence::GetMoveStateFromGoToTask),Move);
EntryPatch mse(0x5DEC10,MoveSpeed),msd(bit_cast<uintptr_t>(&CPed::SetMoveAnimSpeed),MoveSpeed);
EntryPatch fte(0x681740,Find),ftd(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),Find);
EntryPatch upe(0x645700,UpdatePoint),upd(bit_cast<uintptr_t>(&CTaskSimpleGoToPoint::UpdatePoint),UpdatePoint);
EntryPatch oe(0x420E30,Out),od(bit_cast<uintptr_t>(&CTaskTimer::IsOutOfTime),Out);
EntryPatch fxe(0x4A9BE0,CreateFx),fxd(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),CreateFx);
EntryPatch fae(0x4AA400,AttachFx),fad(bit_cast<uintptr_t>(&FxSystem_c::AttachToBone),AttachFx);
EntryPatch fke(0x4AA3D0,KillFx),fkd(bit_cast<uintptr_t>(&FxSystem_c::PlayAndKill),KillFx);
EntryPatch hse(0x5E0460,HandSignal),hsd(bit_cast<uintptr_t>(&CPed::IsPlayingHandSignal),HandSignal);
EntryPatch helde(0x5E02E0,Held),heldd(bit_cast<uintptr_t>(&CPed::GetEntityThatThisPedIsHolding),Held);
EntryPatch pae(0x65EA50,Pass),pad(bit_cast<uintptr_t>(&CTaskComplexGangLeader::TryToPassObject),Pass);
EntryPatch eve(0x4AB420,EventAdd),evd(bit_cast<uintptr_t>(&CEventGroup::Add),EventAdd);
EntryPatch ame(0x65E730,Ambient),amd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::GetRandomGangAmbientAnim),Ambient);
EntryPatch hae(0x691720,HoldAnim),had(bit_cast<uintptr_t>(&CTaskSimpleHoldEntity::PlayAnim),HoldAnim);
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;', 'scenario=i%64;mode=(i/64)%16;const int types[]{0x4BA,0xCB,0x1AA,0x2C5,0x38B,12345};subType=fn.slot==10?(mode==1?0x2C5:mode>=2&&mode<=5?0x38B:0xCB):types[(i/16)%6];')
a=s.index('t->m_Ped=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''vehicle.fill(0);matrix.fill(0);pedMatrix.fill(0);intel.fill(0);leaderIntel.fill(0);receiverIntel.fill(0);group.fill(0);receiver.fill(0);assoc.fill(0);held.fill(0);gotoTask.fill(0);clump.fill(0);frame.fill(0);fx.fill(0);auto*r=reinterpret_cast<CPed*>(receiver.data());p->m_pIntelligence=reinterpret_cast<CPedIntelligence*>(intel.data());q->m_pIntelligence=reinterpret_cast<CPedIntelligence*>(leaderIntel.data());r->m_pIntelligence=reinterpret_cast<CPedIntelligence*>(receiverIntel.data());Put(ped.data(),0x14,pedMatrix.data());Put(player.data(),0x14,matrix.data());auto*m=reinterpret_cast<CMatrix*>(matrix.data());auto*pm=reinterpret_cast<CMatrix*>(pedMatrix.data());m->GetRight()=CVector{1.f,0.f,0.f};m->GetForward()=CVector{0.f,1.f,0.f};m->GetUp()=CVector{0.f,0.f,1.f};m->GetPosition()=CVector{0.f,mode==4?10.f:float(i%3),0.f};pm->GetRight()=m->GetRight();pm->GetForward()=m->GetForward();pm->GetUp()=m->GetUp();pm->GetPosition()=CVector{0.f,-1.f,0.f};auto*v=reinterpret_cast<CVehicle*>(vehicle.data());p->bInVehicle=(i&4)!=0;p->m_pVehicle=(i&8)?v:nullptr;q->m_pVehicle=(i&16)?v:nullptr;q->m_nMoveState=mode>=3&&mode<=4?PEDMOVE_WALK:static_cast<eMoveState>(i%8);p->m_nMoveState=PEDMOVE_STILL;Put<int32>(intel.data(),0x274,mode==5?9:(i&32)?31:0);t->m_PedGroup=reinterpret_cast<CPedGroup*>(group.data());t->m_PedGroup->m_bIsMissionGroup=(scenario&4)!=0;t->m_Leader=(fn.slot==10?mode==0:bool(i&128))?nullptr:q;t->m_PedPosn=CVector{0.f,0.f,0.f};t->dword20=CVector{.25f,-.5f,0.f};t->dword2C=t->dword20;t->dword38=3.f;t->byte3C=uint8(i%8);t->m_Flags=uint8(fn.slot==10?(mode>=2&&mode<=5?4:mode==6?0:mode==7?1:1):i%32);t->dword40=3000;t->dword44=2700;t->byte48=uint8(mode==8);t->byte49=0;auto*a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());a->m_Speed=1.f;a->m_CurrentTime=.4f;reinterpret_cast<CEntity*>(held.data())->m_nModelIndex=int16(eModelID(ModelIndices::MI_GANG_SMOKE));Put<float>(gotoTask.data(),0x18,1.f);Put<uint8>(clump.data(),0,2);Put(clump.data(),4,frame.data());p->m_pRwObject=reinterpret_cast<RwObject*>(clump.data());if(fn.slot==-2)t->m_pSubTask=nullptr;if(fn.slot==6&&(i&4))t->m_pSubTask=nullptr;block.IsLoaded=(i&4)!=0;constructorPos=CVector{.25f,-.5f,1.f};CTimer::m_snTimeInMilliseconds=4000;
'''+s[b:]
a=s.index('uintptr_t ret=');b=s.index('\nif(candidate)',a)
s=s[:a]+'''uintptr_t ret=0;if(fn.slot==-1)ret=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPedGroup*,CPed*,uint8,CVector,float)>(address)(t,reinterpret_cast<CPedGroup*>(group.data()),(i&1)?q:nullptr,uint8(i%8),constructorPos,float(i%6)));else if(fn.slot==-2)reinterpret_cast<void(__thiscall*)(void*)>(address)(t);else if(fn.slot==6)ret=uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(address)(t,p,i%3,nullptr));else if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));else ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));
'''+s[b:]
s=s.replace('fn.slot==4?json(ret):Ptr(ret)', 'fn.slot==6?json(ret):Ptr(ret)')
s=s.replace('{"task",Words(task.data(),task.size())}', '{"association",Words(assoc.data(),assoc.size())},{"goto",Words(gotoTask.data(),128)},{"subtask",Words(subtask.data(),128)},{"task",Words(task.data(),task.size())}')
a=s.index('if(fn.slot==10){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==8){require("set_primary");require("remove_member");require("member");require("new");}if(fn.slot==10){require("move_speed");require("update_point");require("fx");require("fx_attach");require("look");require("set_secondary");require("pass_event");require("hold_anim");}'''+s[b:]
s=s.replace('Write({{"event",pass&&selected?', 'CTimer::m_snTimeInMilliseconds=savedTime;block.IsLoaded=savedLoaded;Write({{"event",pass&&selected?')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index('if __name__',a)
files=['Tasks/TaskTypes/TaskComplexGangFollower','Tasks/TaskTypes/TaskComplexFollowLeaderInFormation','Tasks/TaskTypes/TaskComplexSignalAtPed','Tasks/TaskTypes/TaskSimpleStandStill','Tasks/TaskTypes/TaskSimplePause','Tasks/TaskTypes/TaskSimpleCarDrive','Tasks/TaskTypes/TaskComplexLeaveCar','Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord','Tasks/TaskTypes/TaskComplexWanderGang','Tasks/TaskTypes/TaskComplexWander','Tasks/TaskTypes/TaskSimpleGoToPoint','Tasks/TaskTypes/TaskComplexGangLeader','Tasks/TaskTypes/TaskComplexPlayHandSignalAnim','Tasks/TaskTypes/TaskSimpleRunAnim','Tasks/TaskTypes/TaskSimpleHoldEntity','Tasks/TaskTypes/TaskComplexPassObject','Events/EventPassObject','Tasks/TaskTimer','PedIntelligence','PedGroupMembership','Tasks/TaskManager','Core/Vector','Core/Matrix','General','Entity/Ped/Ped','Fx/FxManager','Fx/FxSystem','Ragdoll/IKChainManager','Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset']
paths=['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')]+['source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h']
s=s[:a]+'SOURCES='+repr(paths)+'\n'+s[b:];s=s.replace('gang-join355','gang-follower355').replace('GangJoin355Live','GangFollower355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','gang-follower355').replace('GangJoinRespond pendientes','GangFollower pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskComplexGangFollower: siete pendientes

Banco preparado de 4.352 pares: Constructor/Destructor/Clone 256, Abort512, Next/First/Control1024. Pendiente de compilación y ejecución. CalculateOffsetPosition también se reconstruyó como dependencia nativa; sigue sin hook catalogado. Compara tarea,ped,subtarea,asociación de animación,punto de marcha y asignaciones completas salvo vptr. Destructor compara estado/trazas, no EAX que el cuerpo original no define.

Fixtures positivos para crear señales, pausa, búsqueda de líder, coche, abandonar grupo, velocidad/seguimiento, exhale, IK, gestos y pasar objeto. Streaming, movimiento,grupo,FX y eventos externos se aíslan por pares EXE/DLL; Offset calcula realmente con las tablas del EXE. Reloj y estado del bloque se restauran. References comparan entidad, no dirección temporal de ranuras en pila. No valida efectos visuales ni desplazamiento real.
''')
