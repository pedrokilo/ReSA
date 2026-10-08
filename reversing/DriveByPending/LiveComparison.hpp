#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleGangDriveBy.h"
#include "Events/EventDamage.h"
#include "../ProbeRuntime.hpp"
namespace DriveByPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskSimpleGangDriveBy","Clone",0x6236D0,0x86D944,1,512},
{"CTaskSimpleGangDriveBy","MakeAbortable",0x62D290,0x86D944,6,1024},
{"CTaskSimpleGangDriveBy","ProcessPed",0x62D3B0,0x86D944,7,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped;
alignas(16) inline std::array<uint8,0xA20> vehicle;
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> data;
alignas(16) inline std::array<uint8,sizeof(CPlayerInfo)> playerInfo;
alignas(16) inline std::array<uint8,sizeof(CPad)> pad;
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleGangDriveBy)> task;
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;
alignas(16) inline std::array<uint8,sizeof(CAnimBlendHierarchy)> hierarchy;
alignas(16) inline std::array<uint8,sizeof(CWeaponInfo)> weaponInfo;
alignas(16) inline std::array<uint8,sizeof(CEventDamage)> event;
alignas(16) inline std::array<uint8,256> arena;
inline std::array<void*,16> eventVT{},taskVT{};
inline int caseId{},scenario{},eventType{},rngCalls{};inline bool player{},setup{},lean{};inline size_t allocSize{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+x.size())return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(vehicle) P(data) P(playerInfo) P(pad) P(task) P(assoc) P(hierarchy) P(weaponInfo) P(event) P(arena)
#undef P
return p;}
inline json Words(const void* p,size_t n,bool vptr=false){json j=json::array();for(size_t i=vptr?1:0;i<n/4;++i)j.push_back(Ptr(static_cast<const uint32*>(p)[i]));return j;}
inline void* __cdecl New(size_t n){trace.push_back({"allocate",n});allocSize=n;return n<=arena.size()?arena.data():nullptr;}
inline void __fastcall Ref(CEntity* p,void*,CEntity** slot){trace.push_back({"reference",Ptr(uintptr_t(p)),Ptr(uintptr_t(slot))});}
inline int __fastcall Type(const CEvent*,void*){trace.push_back({"event_type",eventType});return eventType;}
inline bool __fastcall Player(const CPed*,void*){trace.push_back({"is_player",player});return player;}
inline eWeaponSkill __fastcall Skill(CPed*,void*){trace.push_back({"skill"});return eWeaponSkill::STD;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"weapon_info",int(type),int(skill)});return reinterpret_cast<CWeaponInfo*>(weaponInfo.data());}
inline bool __fastcall Lean(CVehicle*,void*,CPed*){trace.push_back({"lean",lean});return lean;}
inline void __fastcall Window(CVehicle*,void*,uint8 id){trace.push_back({"window",id});}
inline void __fastcall Model(CPed*,void*,int id){trace.push_back({"model",id});}
inline CPlayerInfo* __fastcall PI(CPlayerPed*,void*){trace.push_back({"player_info"});return reinterpret_cast<CPlayerInfo*>(playerInfo.data());}
inline CPad* __fastcall Pad(CPlayerPed*,void*){trace.push_back({"pad"});return reinterpret_cast<CPad*>(pad.data());}
inline int __fastcall Weapon(const CPad*,void*,CPed*){trace.push_back({"pad_weapon",scenario&1});return scenario&1;}
inline void __fastcall WeaponUpdate(CWeapon*,void*,CPed*){trace.push_back({"weapon_update"});}
inline bool __fastcall Setup(CTaskSimpleGangDriveBy*,void*,CPed*){trace.push_back({"setup",setup});return setup;}
inline void __fastcall AI(CTaskSimpleGangDriveBy*,void*,CPed*){trace.push_back({"ai"});}
inline void __fastcall Input(CTaskSimpleGangDriveBy*,void*,CPed*){trace.push_back({"input"});}
inline void __fastcall Fire(CTaskSimpleGangDriveBy*,void*,CPed*){trace.push_back({"fire"});}
inline void __fastcall Aim(CTaskSimpleGangDriveBy*,void*,CPed*){trace.push_back({"aim"});}
inline void __fastcall Time(CAnimBlendAssociation* a,void*,float t){trace.push_back({"set_time",std::bit_cast<uint32>(t)});a->m_CurrentTime=t;}
inline void __fastcall Callback(CAnimBlendAssociation*,void*,void(*)(CAnimBlendAssociation*,void*),void* p){trace.push_back({"delete_callback",Ptr(uintptr_t(p))});}
inline int __cdecl Rand(){const int r=int((caseId*15731u+(rngCalls++)*14543u)&32767);trace.push_back({"rand",r});return r;}
inline uint16 Random(){return uint16(Rand());}
inline int16 __fastcall Say(CPed*,void*,int ctx,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",ctx,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"drive-by-pending-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"drive-by-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("drive-by-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"drive-by-pending-only.txt");if(f)std::getline(f,only);}eventVT[1]=reinterpret_cast<void*>(&Type);
const auto savedTime=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();bool pass=true,selected=false;int total=0;
for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
EntryPatch re(0x571B70,Ref),rd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch pe(0x5DF8F0,Player),pd(bit_cast<uintptr_t>(&CPed::IsPlayer),Player);
EntryPatch ske(0x5E6580,Skill),skd(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),Skill);
EntryPatch ie(0x743C60,Info),id(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
EntryPatch le(0x6D5CF0,Lean),ld(bit_cast<uintptr_t>(&CVehicle::CanPedLeanOut),Lean);
EntryPatch we(0x6D3080,Window),wd(bit_cast<uintptr_t>(&CVehicle::SetWindowOpenFlag),Window);
EntryPatch me(0x5E5ED0,Model),md(bit_cast<uintptr_t>(&CPed::AddWeaponModel),Model);
EntryPatch pie(0x609FF0,PI),pid(bit_cast<uintptr_t>(&CPlayerPed::GetPlayerInfoForThisPlayerPed),PI);
EntryPatch pade(0x609560,Pad),padd(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),Pad);
EntryPatch pwe(0x540180,Weapon),pwd(bit_cast<uintptr_t>(&CPad::GetWeapon),Weapon);
EntryPatch wue(0x73DB40,WeaponUpdate),wud(bit_cast<uintptr_t>(&CWeapon::Update),WeaponUpdate);
EntryPatch su(0x627B20,Setup),ai(0x627600,AI),inp(0x621960,Input),fire(0x627CC0,Fire),aim(0x628350,Aim);
EntryPatch te(0x4CEA80,Time),td(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),Time);
EntryPatch ce(0x4CEBC0,Callback),cd(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback),Callback);
EntryPatch rnge(0x821B1E,Rand),rngd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch se(0x5EFFE0,Say),sd(bit_cast<uintptr_t>(&CPed::Say),Say);
for(int i=0;i<fn.cases;++i){caseId=i;scenario=i%64;json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);const bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
ped.fill(0);vehicle.fill(0);data.fill(0);playerInfo.fill(0);pad.fill(0);task.fill(0xCD);assoc.fill(0);hierarchy.fill(0);weaponInfo.fill(0);event.fill(0);arena.fill(0xCD);allocSize=0;rngCalls=0;trace=json::array();
auto* t=reinterpret_cast<CTaskSimpleGangDriveBy*>(task.data());auto* p=reinterpret_cast<CPed*>(ped.data());auto* a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());auto* v=reinterpret_cast<CVehicle*>(vehicle.data());auto* wi=reinterpret_cast<CWeaponInfo*>(weaponInfo.data());
std::memcpy(taskVT.data(),reinterpret_cast<void*>(fn.vt),9*4);Put(task.data(),0,taskVT.data());Put<uint32>(task.data(),4,0);Put(event.data(),0,eventVT.data());Put<uint8>(event.data(),0x40,uint8((scenario&8)!=0));Put<uint8>(event.data(),0xC,uint8((scenario&16)!=0));
p->m_pVehicle=v;p->bInVehicle=scenario!=4;p->m_fHealth=scenario==5?0.5f:100.f;Put(ped.data(),0x480,(scenario&4)?data.data():nullptr);p->m_pWeaponObject=(scenario&8)?reinterpret_cast<RpClump*>(1):nullptr;
p->GetActiveWeapon().m_Type=WEAPON_PISTOL;p->GetActiveWeapon().m_State=eWeaponState(scenario%3);
player=(scenario&32)!=0;lean=scenario!=2;setup=(scenario&1)!=0;eventType=(scenario&2)?int(EVENT_DAMAGE):int(EVENT_NONE);
v->m_nVehicleType=eVehicleType(scenario%4==1?9:0);v->m_nVehicleSubType=eVehicleType(scenario%4==2?2:0);v->m_pDriver=p;v->m_apPassengers[scenario%3]=p;Put<int32>(vehicle.data(),0x640,scenario%4==1?2:1);v->m_vecMoveSpeed=CVector{scenario%3==0?1.f:scenario%3==1?.2f:0.f,0.f,0.f};
wi->m_nWeaponFire=eWeaponFire(scenario==1?0:1);wi->m_nAmmoClip=uint16(10+i%60);wi->m_nModelId1=346;
t->m_bIsFinished=scenario==3;t->m_bAnimsReferenced=false;t->m_bSeatRHS=(scenario&1)!=0;t->m_bInRangeToShoot=(scenario&4)!=0;t->m_bInWeaponRange=(scenario&8)!=0;t->m_bReachedAbortRange=false;t->m_bFromScriptCommand=(scenario&16)!=0;t->m_nNextCommand=char((i/64)%8);t->m_nLastCommand=char((i/8)%8);t->m_nBurstShots=char(i%5-1);t->m_nDrivebyStyle=eDrivebyStyle(i%9);t->m_nFrequencyPercentage=int8(i%101);t->m_nFakeShootDirn=-1;t->m_nAttackTimer=int16(scenario%3==0?-1:scenario%3==1?0:500);t->m_nLOSCheckTime=(scenario&2)?100:0;t->m_nLOSBlocked=true;t->m_fAbortRange=15.5f;t->m_nRequiredAnimID=200;t->m_nRequiredAnimGroup=AssocGroupId((scenario&16)?72:0);t->m_pAnimAssoc=(setup||scenario%3)?a:nullptr;t->m_pWeaponInfo=(scenario&4)?wi:nullptr;t->m_pTargetEntity=(scenario&8)?p:nullptr;t->m_vecCoords=CVector{float(i%7),float(i%13),.25f};
a->m_BlendHier=reinterpret_cast<CAnimBlendHierarchy*>(hierarchy.data());a->m_BlendHier->m_fTotalTime=2.f;a->m_Flags=uint16((scenario&2)!=0);a->m_BlendAmount=(scenario&4)?1.f:0.f;a->m_BlendDelta=float(scenario%3-1)*2.f;a->m_AnimId=AnimationId((scenario&8)?200:201);a->m_TimeStep=.5f;a->m_CurrentTime=StaticRef<float>(0x8D2E7C)+float(scenario%3)*.25f;
*reinterpret_cast<uint32*>(0xB7CB84)=1200;CTimer::SetTimeStep(float(i%5)+.25f);
uintptr_t ret{};if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));else if(fn.slot==6)ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,eAbortPriority,const CEvent*)>(address)(t,p,eAbortPriority(i%4),(scenario&1)?reinterpret_cast<CEvent*>(event.data()):nullptr);else ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(address)(t,p);
if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(ret?"return_true":"return_false");if(fn.slot==1)hit("clone");for(const auto& e:trace)if(e[0].is_string())hit(e[0].get<std::string>().c_str());}
result[candidate]={{"return",fn.slot==1?Ptr(ret):json(ret)},{"task",Words(task.data(),task.size(),true)},{"ped",Words(ped.data(),ped.size(),true)},{"vehicle",Words(vehicle.data(),vehicle.size(),true)},{"data",Words(data.data(),data.size())},{"info",Words(playerInfo.data(),playerInfo.size())},{"anim",Words(assoc.data(),assoc.size(),true)},{"allocation",allocSize?Words(arena.data(),allocSize,true):json(nullptr)},{"trace",trace},{"switch",switches}};
}const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}
*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;CTimer::SetTimeStep(savedStep);h->State(hs);h->LockState(hl);auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
if(fn.slot==1){require("clone");require("reference");}else if(fn.slot==6){for(const auto* n:{"return_true","return_false","event_type","setup","delete_callback","player_info"})require(n);}else{for(const auto* n:{"return_true","return_false","window","model","ai","input","setup","fire","aim","set_time","weapon_update","say"})require(n);}
Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",CTimer::GetTimeInMS()==savedTime&&CTimer::GetTimeStep()==savedStep}});
}
}
