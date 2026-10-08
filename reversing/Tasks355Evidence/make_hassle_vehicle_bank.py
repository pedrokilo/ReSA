from pathlib import Path
p=Path('reversing/HassleVehicle355');p.mkdir(exist_ok=True)
s=Path('reversing/Boat355/LiveComparison.hpp').read_text().replace('Boat355Live','HassleVehicle355Live').replace('boat355','hassle-vehicle355').replace('TaskComplexKillPedFromBoat','TaskGangHassleVehicle').replace('#include "TaskComplexKillPedOnFoot.h"','#include "TaskComplexGangLeader.h"\n#include "Ragdoll/IKChainManager.h"')
a=s.index('inline Fn fns[]');b=s.index('alignas(16)',a)
s=s[:a]+'''inline Fn fns[]{
{"CTaskGangHassleVehicle","GetTargetHeading",0x65FDD0,0,-1,512},
{"CTaskGangHassleVehicle","CalcTargetOffset",0x6641A0,0,-1,512},
{"CTaskGangHassleVehicle","Clone",0x65FC00,0x86F9D4,1,256},
{"CTaskGangHassleVehicle","CreateNextSubTask",0x65FC80,0x86F9D4,8,1024},
{"CTaskGangHassleVehicle","CreateFirstSubTask",0x664BA0,0x86F9D4,9,1024},
{"CTaskGangHassleVehicle","ControlSubTask",0x6637C0,0x86F9D4,10,1024}};
'''+s[b:]
s=s.replace('inline std::array<void*,11> subVT{};', 'alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix,pedMatrix;alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel;alignas(16) inline std::array<uint8,sizeof(CBaseModelInfo)> model;alignas(16) inline std::array<uint8,sizeof(CColModel)> col;alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;inline std::array<void*,11> subVT{};inline int mode;')
s=s.replace('P(vehicle)','P(vehicle) P(matrix) P(pedMatrix) P(intel) P(model) P(col) P(assoc)')
a=s.index('inline CWanted* __cdecl Wanted');b=s.index('inline ReversibleHooks::',a)
s=s[:a]+'''inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=mode!=1&&(scenario&1)!=0;trace.push_back({"abort",priority,b});return b;}
inline void __fastcall Ref(CEntity* p,void*,CEntity**){trace.push_back({"reference",Ptr(uintptr_t(p))});}
inline void __fastcall Clean(CEntity* p,void*,CEntity**){trace.push_back({"cleanup",Ptr(uintptr_t(p))});}
inline bool __cdecl Should(){bool b=mode!=15&&(scenario&2)!=0;trace.push_back({"should",b});return b;}
inline int __cdecl Block(const char* name){trace.push_back({"block",name});return 0;}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Remove(int index){trace.push_back({"remove_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
inline int __cdecl Random(){int r=(scenario*701)%32768;trace.push_back({"rand",r});return r;}
inline int __cdecl Range(int low,int high){int r=low+(scenario%(high-low));if(low==0&&high==100)r=(scenario&4)?61:60;trace.push_back({"range",low,high,r});return r;}
inline int __fastcall Spare(const CVehicle*,void*){int r=scenario%8==7?-1:scenario%6;trace.push_back({"spare",r});return r;}
inline void __fastcall SetHassle(CVehicle*,void*,int pos,bool enable){trace.push_back({"reserve",pos,enable});}
inline void __fastcall Stats(CPed*,void*,int stats){trace.push_back({"stats",stats});}
inline CTask* __fastcall Next(CTaskGangHassleVehicle*,void*,CPed*){trace.push_back({"next"});return reinterpret_cast<CTask*>(arena.data());}
inline bool __fastcall Visible(CEntity*,void*){bool b=(scenario&8)!=0;trace.push_back({"visible",b});return b;}
inline bool __stdcall Looking(CPed*){bool b=(scenario&16)!=0;trace.push_back({"looking",b});return b;}
inline void __fastcall Look(IKChainManager_c*,void*,const char* label,CPed*,CEntity* target,int time,int bone,CVector*,bool torso,float speed,int blend,int priority,bool force){trace.push_back({"look",label,Ptr(uintptr_t(target)),time,bone,torso,std::bit_cast<uint32>(speed),blend,priority,force});}
inline CTask* __fastcall Secondary(CTaskManager*,void*,int slot){trace.push_back({"secondary",slot});return mode>=8&&mode<=11?reinterpret_cast<CTask*>(player.data()):nullptr;}
inline void __fastcall SetSecondary(CTaskManager*,void*,CTask* t,int slot){trace.push_back({"set_secondary",Ptr(uintptr_t(t)),slot});}
inline int16 __fastcall Say(CPed*,void*,int context,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",context,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline bool __fastcall HandSignal(CPed*,void*){bool b=(scenario&32)!=0;trace.push_back({"hand_signal",b});return b;}
inline float __fastcall Heading(CPlaceable*,void*){trace.push_back({"heading"});return 0.f;}
inline CAnimBlendAssociation* __cdecl Association(RpClump*,int anim){trace.push_back({"association",anim});return mode>=9&&mode<=11&&anim==0x128+(mode-9)?reinterpret_cast<CAnimBlendAssociation*>(assoc.data()):nullptr;}
inline void __fastcall Force(CPhysical*,void*,CVector a,CVector b){trace.push_back({"force",std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z)});}
inline void __fastcall FightSetup(CTaskSimpleFight*,void*,float r){trace.push_back({"fight_setup",std::bit_cast<uint32>(r)});}
inline int __cdecl Collision(const CMatrix&,CColModel&,const CMatrix&,CColModel&,std::array<CColPoint,32>&,CColPoint*,float*,bool){trace.push_back({"collision"});return (scenario&1)!=0?1:0;}
inline void __fastcall FightHit(CTaskSimpleFight*,void*,CPed*,CVehicle*,CVector*,CVector*,int16 a,int8 b){trace.push_back({"fight_hit",a,b});}
inline void __fastcall Audio(CAEVehicleAudioEntity*,void*,int event,float volume){trace.push_back({"audio",event,std::bit_cast<uint32>(volume)});}
'''+s[b:]
s=s.replace('bool pass=true,selected=false;int total=0;', 'const auto savedTime=CTimer::GetTimeInMS();auto*savedModel=CModelInfo::GetModelInfo(100);auto& block=CAnimManager::GetAnimBlocks()[0];const auto savedLoaded=block.IsLoaded;bool pass=true,selected=false;int total=0;')
s=s.replace('auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);','auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);')
a=s.index('EntryPatch pe(');b=s.index('for(int i=',a)
s=s[:a]+'''EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch cle(0x571A00,Clean),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
EntryPatch she(0x65E7F0,Should),shd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),Should);
EntryPatch be(0x4D3990,Block),bd(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch rme(0x4D3FD0,Remove),rmd(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Remove);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
EntryPatch re(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch rge(0x407180,Range),rgd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CGeneral::GetRandomIntInRange)),Range);
EntryPatch spe(0x6D3AE0,Spare),spd(bit_cast<uintptr_t>(&CVehicle::GetSpareHasslePosId),Spare);
EntryPatch ste(0x6D3B30,SetHassle),std_(bit_cast<uintptr_t>(&CVehicle::SetHasslePosId),SetHassle);
EntryPatch sate(0x5E0360,Stats),satd(bit_cast<uintptr_t>(&CPed::SetPedStats),Stats);
EntryPatch ve(0x536BC0,Visible),vd(bit_cast<uintptr_t>(&CEntity::IsVisible),Visible);
EntryPatch le(0x6181A0,Looking),ld(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),Looking);
EntryPatch loe(0x618970,Look),lod(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),Look);
EntryPatch sce(0x681810,Secondary),scd(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
EntryPatch sse(0x681B60,SetSecondary),ssd(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),SetSecondary);
EntryPatch sae(0x5EFFE0,Say),sad(bit_cast<uintptr_t>(&CPed::Say),Say);
EntryPatch hse(0x5E0460,HandSignal),hsd(bit_cast<uintptr_t>(&CPed::IsPlayingHandSignal),HandSignal);
EntryPatch he(0x441DB0,Heading),hd(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
EntryPatch ase(0x4D68B0,Association),asd(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),Association);
EntryPatch fe(0x542A50,Force),fd(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),Force);
EntryPatch fse(0x61D5F0,FightSetup),fhe(0x61D0B0,FightHit);
EntryPatch ce(0x4185C0,Collision),cd(bit_cast<uintptr_t>(&CCollision::ProcessColModels),Collision);
EntryPatch aue(0x4F6420,Audio),aud(bit_cast<uintptr_t>(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),Audio);
std::optional<EntryPatch> nextE,nextD;if(fn.slot==9||fn.slot==10){nextE.emplace(0x65FC80,Next);nextD.emplace(bit_cast<uintptr_t>(&CTaskGangHassleVehicle::CreateNextSubTask),Next);}
'''+s[b:]
s=s.replace('scenario=i%128;subType=i%3==0?0x3E8:i%3==1?0xCB:12345;', 'scenario=i%64;mode=(i/64)%16;const int types[]{0x38F,0x4BC,0x39D,0x38A,1000,12345};subType=fn.slot==10?(mode==13?12345:0x39D):types[(i/16)%6];')
a=s.index('t->m_Ped=');b=s.index('\nuintptr_t ret=',a)
s=s[:a]+'''vehicle.fill(0);matrix.fill(0);pedMatrix.fill(0);intel.fill(0);model.fill(0);col.fill(0);assoc.fill(0);auto*v=reinterpret_cast<CVehicle*>(vehicle.data());Put(vehicle.data(),0x14,matrix.data());Put(ped.data(),0x14,pedMatrix.data());auto*m=reinterpret_cast<CMatrix*>(matrix.data());auto*pm=reinterpret_cast<CMatrix*>(pedMatrix.data());m->GetRight()=CVector{1.f,0.f,0.f};m->GetForward()=CVector{0.f,1.f,0.f};m->GetUp()=CVector{0.f,0.f,1.f};m->GetPosition()=CVector{0.f,0.f,0.f};pm->GetRight()=m->GetRight();pm->GetForward()=m->GetForward();pm->GetUp()=m->GetUp();pm->GetPosition()=CVector{0.f,0.f,0.f};p->m_pIntelligence=reinterpret_cast<CPedIntelligence*>(intel.data());p->m_nMoveState=static_cast<eMoveState>(i%8);v->m_nModelIndex=100;Put(model.data(),0x14,col.data());CModelInfo::SetModelInfo(100,reinterpret_cast<CBaseModelInfo*>(model.data()));auto&box=reinterpret_cast<CColModel*>(col.data())->m_boundBox;box.m_vecMin=CVector{-1.f,-2.f,-.5f};box.m_vecMax=CVector{(i&128)?4.f:1.f,(i&256)?7.f:2.f,1.f};t->m_Vehicle=(fn.slot==10?(mode<2):bool(i&512))?nullptr:v;t->m_nHasslePosId=i%8-1;t->byte18=uint8(mode==2||bool(i&1));t->dword1C=2.f;t->m_fOffsetX=float(i%5)/4.f;t->m_vecPosn=CVector{1.f,-.5f,0.f};t->m_bRemoveAnim=mode==15||bool(i&2);t->m_b31=uint8(mode==5||mode==12||mode==13?0:mode==6?1:2);t->m_pEntity=mode==3?static_cast<CEntity*>(q):nullptr;t->m_nTime=3000;t->dword3C=mode==2?0:100000;t->byte40=mode==2;t->byte41=uint8(i&32);v->m_pDriver=mode==3?nullptr:q;v->m_fHealth=mode==4?249.f:float(249+i%3);if(fn.slot!=10)v->m_pDriver=(i&2)?q:nullptr;v->m_fMass=1000.f;p->bInVehicle=(i&4)!=0;p->m_pVehicle=(i&8)?v:nullptr;Put<float>(subtask.data(),0x50,mode==12?1.f:float(i%9));block.IsLoaded=(i&4)!=0;auto*a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());a->m_CurrentTime=(i&8)?.8f:.4f;a->m_TimeStep=.4f;CTimer::m_snTimeInMilliseconds=4000;
'''+s[b:]
a=s.index('uintptr_t ret=');b=s.index('\nif(candidate)',a)
s=s[:a]+'''uintptr_t ret=0;json value=nullptr;if(std::string(fn.name)=="GetTargetHeading"){float r=reinterpret_cast<float(__thiscall*)(void*,CPed*)>(address)(t,p);value=std::bit_cast<uint32>(r);}else if(std::string(fn.name)=="CalcTargetOffset")reinterpret_cast<void(__thiscall*)(void*)>(address)(t);else if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));else ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));
'''+s[b:]
s=s.replace('{"task",Words(task.data(),task.size())}', '{"value",value},{"vehicle",Words(vehicle.data(),vehicle.size())},{"subtask",Words(subtask.data(),128)},{"task",Words(task.data(),task.size())}')
a=s.index('if(fn.slot==10){require(');b=s.index('Write({{"event","function_summary"}',a)
s=s[:a]+'''if(fn.slot==9){require("spare");require("reserve");require("stats");require("next");}if(fn.slot==10){require("say");require("set_secondary");require("look");require("force");require("collision");require("fight_hit");require("audio");require("new");}'''+s[b:]
s=s.replace('Write({{"event",pass&&selected?', 'CTimer::m_snTimeInMilliseconds=savedTime;CModelInfo::SetModelInfo(100,savedModel);block.IsLoaded=savedLoaded;Write({{"event",pass&&selected?')
(p/'LiveComparison.hpp').write_text(s)
s=Path('reversing/GangJoin355/build_probe.py').read_text();a=s.index('SOURCES=');b=s.index('if __name__',a)
files=['Tasks/TaskTypes/TaskGangHassleVehicle','Tasks/TaskTypes/TaskGangHasslePed','Tasks/TaskTypes/TaskComplexTrackEntity','Tasks/TaskTypes/TaskComplexSmartFleeEntity','Tasks/TaskTypes/TaskComplexLeaveCar','Tasks/TaskTypes/TaskComplexGangLeader','Tasks/TaskTypes/TaskComplexKillPedOnFoot','Tasks/TaskTypes/TaskSimpleShakeFist','Tasks/TaskTypes/TaskComplexPlayHandSignalAnim','Tasks/TaskTypes/TaskSimpleRunAnim','Tasks/TaskTypes/TaskSimpleFight','Core/Vector','Core/Matrix','General','Entity/Physical','Entity/Entity','Entity/Ped/Ped','Tasks/TaskManager','Collision/Collision','Audio/Entities/AEVehicleAudioEntity','Ragdoll/IKChainManager']
s=s[:a]+'SOURCES='+repr(['source/game_sa/'+x+ext for x in files for ext in ('.cpp','.h')])+'\n'+s[b:];s=s.replace('gang-join355','hassle-vehicle355').replace('GangJoin355Live','HassleVehicle355Live');(p/'build_probe.py').write_text(s)
s=Path('reversing/GangJoin355/run_local.py').read_text().replace('gang-join355','hassle-vehicle355').replace('GangJoinRespond pendientes','GangHassleVehicle pendientes');(p/'run_local.py').write_text(s)
(p/'README.md').write_text('''# CTaskGangHassleVehicle: seis pendientes

Banco preparado de 4.352 pares: Heading/Offset 512, Clone 256, Next/First/Control 1024 cada una. Pendiente de compilación y ejecución. Control cubre reserva y referencia, carga de gangs, temporizador, amenaza al conductor, giro, voz, gestos secundarios, señal de mano y golpe al vehículo. m_b31 es byte con estados0..3, no bool. Next conserva reserva0x58 de TrackEntity.

Se comparan tarea,ped,vehículo,subtarea,asignaciones completas salvo vptr,trazas y heading bit a bit. CalcTargetOffset se ejecuta real; First conserva el modelo de colisión ficticio. El banco restaura modelo,reloj y carga del bloque. Visibilidad, IK,voz, fuerzas y colisión se aíslan por entrada EXE/DLL; la construcción local de Fight es real, sus helpers externos están aislados. No valida daño físico ni animación reales. Reference/Cleanup comparan entidad: la dirección de la ranura de referencias temporales en pila no es estable entre EXE y DLL.
''')
