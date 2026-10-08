#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskSimpleStandStill.h"
#include "Cover.h"
#include "CoverPoint.h"
#include "TaskComplexFallAndGetUp.h"
#include "CarEnterExit.h"
#include "../ProbeRuntime.hpp"
namespace ArmedNextPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskComplexKillPedOnFootArmed","CreateNextSubTask",0x62C190,0x86D918,8,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped,target;
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> data;
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted;
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel;
alignas(16) inline std::array<uint8,sizeof(CTaskComplexKillPedOnFootArmed)> task;
alignas(16) inline std::array<uint8,128> subtask,falltask;
alignas(16) inline std::array<uint8,256> arena;
alignas(16) inline std::array<uint8,sizeof(CWeaponInfo)> weaponInfo;
alignas(16) inline std::array<uint8,sizeof(CCoverPoint)> cover;
inline CMatrix pedMatrix;inline std::array<uintptr_t,9> subVT{};
inline int rngCalls{},losCalls{};inline bool lineClear{},haveGroup{};
inline int scenario{},caseId{},subType{};inline bool aborted{},falling{},foundFall{},foundLeave{},locks{},roomDriver{},roomPassenger{},player{},closest{},alive{},melee{};inline size_t allocSize{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+x.size())return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(target) P(vehicle) P(data) P(wanted) P(intel) P(task) P(subtask) P(falltask) P(arena) P(weaponInfo) P(cover)
#undef P
return p;}
inline json Words(const void* p,size_t n,bool vptr=false){json j=json::array();for(size_t i=vptr?1:0;i<n/4;++i)j.push_back(Ptr(static_cast<const uint32*>(p)[i]));return j;}
inline void* __cdecl New(size_t n){trace.push_back({"allocate",n});allocSize=n;return n<=arena.size()?arena.data():nullptr;}
inline void __fastcall Ref(CEntity* p,void*,CEntity** slot){trace.push_back({"reference",Ptr(uintptr_t(p)),Ptr(uintptr_t(slot))});}
inline int __fastcall Type(CTask*,void*){trace.push_back({"task_type",subType});return subType;}
inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){trace.push_back({"abort",priority,aborted});return aborted;}
inline bool __fastcall Stand(CTaskSimpleStandStill*,void*,CPed*){trace.push_back({"stand_process"});return false;}
inline eWeaponSkill __fastcall Skill(CPed*,void*){trace.push_back({"skill"});return eWeaponSkill::STD;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"weapon_info",int(type),int(skill)});return reinterpret_cast<CWeaponInfo*>(weaponInfo.data());}
inline bool __fastcall Line(CTaskComplexKillPedOnFootArmed*,void*,CPed*){trace.push_back({"line",lineClear});return lineClear;}
inline CPedGroup* __cdecl Group(const CPed*){trace.push_back({"group",haveGroup});return haveGroup?reinterpret_cast<CPedGroup*>(1):nullptr;}
inline bool __cdecl LOS(const CVector& a,const CVector& b,bool c,bool d,bool e,bool f,bool g,bool h,bool j){const bool clear=(scenario&1)?losCalls++==(scenario%3):(++losCalls,true);trace.push_back({"los",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),c,d,e,f,g,h,j,clear});return clear;}
inline int __cdecl Rand(){const int r=int((caseId*15731u+1u+(caseId/32u)+(caseId/64u)+(rngCalls++)*14543u)&32767);trace.push_back({"rand",r});return r;}
inline uint16 Random(){return uint16(Rand());}
inline bool __fastcall Player(const CPed* p,void*){trace.push_back({"player",Ptr(uintptr_t(p)),player});return player;}
inline bool __fastcall Alive(const CPed*,void*){trace.push_back({"alive",alive});return alive;}
inline bool __fastcall Locks(const CVehicle*,void*,const CPed*){trace.push_back({"locks",locks});return locks;}
inline bool __fastcall Driver(const CVehicle*,void*,const CPed*){const bool b=(scenario&1)!=0;trace.push_back({"driver",b});return b;}
inline CTask* __fastcall Find(const CTaskManager*,void*,eTaskType type){const bool yes=type==TASK_COMPLEX_FALL_AND_GET_UP?foundFall:foundLeave;trace.push_back({"find",int(type),yes});return yes?reinterpret_cast<CTask*>(type==TASK_COMPLEX_FALL_AND_GET_UP?falltask.data():subtask.data()):nullptr;}
inline bool __fastcall Falling(const CTaskComplexFallAndGetUp*,void*){trace.push_back({"falling",falling});return falling;}
inline void __fastcall Down(CTaskComplexFallAndGetUp*,void*,int time){trace.push_back({"down",time});}
inline bool __cdecl Room(const CVehicle*,int32 door,const CVector*){const bool r=door==10?roomDriver:roomPassenger;trace.push_back({"room",door,r});return r;}
inline bool __fastcall Upside(const CVehicle*,void*){const bool b=scenario==25;trace.push_back({"upside",b});return b;}
inline bool __fastcall Side(const CVehicle*,void*){const bool b=scenario==26;trace.push_back({"side",b});return b;}
inline bool __fastcall Melee(const CWeapon*,void*){trace.push_back({"melee",melee});return melee;}
inline bool __fastcall Has(CPed*,void*,eWeaponType type){const bool b=(scenario&16)!=0;trace.push_back({"has_weapon",int(type),b});return b;}
inline void __fastcall Set(CPed*,void*,eWeaponType type){trace.push_back({"set_weapon",int(type)});}
inline CWanted* __cdecl Wanted(int id){trace.push_back({"wanted",id});return reinterpret_cast<CWanted*>(wanted.data());}
inline bool __fastcall Closest(const CWanted*,void*,CPed*,int n){trace.push_back({"closest",n,closest});return closest;}
inline int16 __fastcall Say(CPed*,void*,int ctx,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",ctx,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline CTask* __fastcall Sub(CTaskComplexKillPedOnFootArmed*,void*,eTaskType type,CPed*){trace.push_back({"subtask",int(type)});Put<uint32>(arena.data(),4,uint32(type));return reinterpret_cast<CTask*>(arena.data());}
inline void __fastcall Release(CPed* p,void*){trace.push_back({"release_cover"});p->m_pCoverPoint=nullptr;}
inline bool __cdecl CoverValid(CCoverPoint*,CVector pos){const bool valid=(scenario&2)==0;trace.push_back({"cover_valid",valid});return valid;}
inline CCoverPoint* __cdecl Reserve(CPed*,const CVector& pos,bool attack){const bool available=(scenario&4)==0;trace.push_back({"reserve",attack,available});return available?reinterpret_cast<CCoverPoint*>(cover.data()):nullptr;}
inline bool __cdecl Coordinates(const CCoverPoint&,CPed* p,const CVector&,CVector& out){const bool ok=(scenario&8)==0;out=p->GetPosition()+CVector{.25f,.25f,0.f};trace.push_back({"cover_coordinates",ok});return ok;}
inline void __fastcall Duck(CPedIntelligence*,void*,uint16 ms){trace.push_back({"duck_secondary",ms});}
inline void __fastcall ClearDuck(CPedIntelligence*,void*){trace.push_back({"clear_duck"});}
inline CTask* __fastcall Secondary(CTaskManager*,void*,eSecondaryTask id){const bool present=(scenario&16)!=0;trace.push_back({"secondary",int(id),present});return present?reinterpret_cast<CTask*>(subtask.data()):nullptr;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"armed-next-pending-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"armed-next-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("armed-next-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"armed-next-pending-only.txt");if(f)std::getline(f,only);}
const auto savedTime=CTimer::GetTimeInMS();bool pass=true,selected=false;int total=0;
for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
EntryPatch re(0x571B70,Ref),rd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch pe(0x5DF8F0,Player),pd(bit_cast<uintptr_t>(&CPed::IsPlayer),Player);
EntryPatch stpe(0x62F370,Stand),stpd(bit_cast<uintptr_t>(&CTaskSimpleStandStill::ProcessPed),Stand),stpv(reinterpret_cast<uintptr_t*>(0x86DD2C)[7],Stand);
EntryPatch ske(0x5E6580,Skill),skd(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),Skill);
EntryPatch ie(0x743C60,Info),id(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
EntryPatch lie(0x621500,Line),lid(bit_cast<uintptr_t>(&CTaskComplexKillPedOnFootArmed::LineOfSightClearForAttack),Line);
EntryPatch ge(0x5F7E80,Group),gd(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),Group);
EntryPatch lose(0x56A490,LOS),losd(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),LOS);
EntryPatch rnge(0x821B1E,Rand),rngd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch we(0x56E230,Wanted),wd(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
EntryPatch se(0x5EFFE0,Say),sd(bit_cast<uintptr_t>(&CPed::Say),Say);
EntryPatch sube(0x626FC0,Sub),subd(bit_cast<uintptr_t>(&CTaskComplexKillPedOnFootArmed::CreateSubTask),Sub);
EntryPatch mee(0x73B1C0,Melee),med(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),Melee);
EntryPatch rele(0x5E0270,Release),reld(bit_cast<uintptr_t>(&CPed::ReleaseCoverPoint),Release);
EntryPatch cve(0x698DD0,CoverValid),cvd(bit_cast<uintptr_t>(&CCover::DoesCoverPointStillProvideCover),CoverValid);
EntryPatch rese(0x6992B0,Reserve),resd(bit_cast<uintptr_t>(&CCover::FindAndReserveCoverPoint),Reserve);
EntryPatch coe(0x699570,Coordinates),cod(bit_cast<uintptr_t>(&CCover::FindCoordinatesCoverPoint),Coordinates);
EntryPatch due(0x601230,Duck),dud(bit_cast<uintptr_t>(&CPedIntelligence::SetTaskDuckSecondary),Duck);
EntryPatch cle(0x601390,ClearDuck),cld(bit_cast<uintptr_t>(&CPedIntelligence::ClearTaskDuckSecondary),ClearDuck);
EntryPatch see(0x681810,Secondary),sed(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
subVT.fill(0);subVT[4]=uintptr_t(Type);
const int types[]{TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_DUCK,TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_THROW_CTRL,TASK_COMPLEX_SEEK_ENTITY,900,12345};
for(int i=0;i<fn.cases;++i){caseId=i;scenario=i%32;json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];const bool switches=(address!=fn.address)==(candidate!=0);pass&=switches;
ped.fill(0);target.fill(0);vehicle.fill(0);data.fill(0);wanted.fill(0);intel.fill(0);task.fill(0xCD);subtask.fill(0);falltask.fill(0);arena.fill(0xCD);weaponInfo.fill(0);allocSize=0;rngCalls=losCalls=0;trace=json::array();
auto* t=reinterpret_cast<CTaskComplexKillPedOnFootArmed*>(task.data());auto* p=reinterpret_cast<CPed*>(ped.data());auto* q=reinterpret_cast<CPed*>(target.data());auto* wi=reinterpret_cast<CWeaponInfo*>(weaponInfo.data());
Put<uint32>(task.data(),4,0);Put<uintptr_t>(task.data(),8,uintptr_t(subtask.data()));Put<uintptr_t>(subtask.data(),0,uintptr_t(subVT.data()));
t->m_target=scenario==31?nullptr:q;t->m_aimImmediate=(scenario&1)!=0;t->m_competence=int8(scenario%3);t->m_needToMoveInCloserTime=(scenario&1)?1000:0;t->m_lastStrafeTime=17;t->m_bStrafeBack=false;t->m_lastAttackTime=29;
pedMatrix.SetUnity();pedMatrix.GetPosition()=CVector{0.f,0.f,0.f};Put(ped.data(),0x14,&pedMatrix);Put(ped.data(),0x47C,reinterpret_cast<CPedIntelligence*>(intel.data()));
p->GetActiveWeapon().m_Type=WEAPON_PISTOL;q->GetActiveWeapon().m_Type=WEAPON_PISTOL;q->GetPosition()=CVector{scenario==0?.5f:i/256==0?8.5f:.5f+float(i/256)*15.f,0.f,0.f};wi->m_fTargetRange=40.f;wi->flags.bThrow=(scenario&4)!=0;
p->bStayInSamePlace=scenario==1&&(i/256)%2;p->bKindaStayInSamePlace=scenario==2;p->bNotAllowedToDuck=(scenario&16)!=0;p->bIsDucking=true;cover.fill(0);cover[1]=uint8((scenario/8)%3);p->m_pCoverPoint=(scenario&1)?reinterpret_cast<CCoverPoint*>(cover.data()):nullptr;subtask[0x39]=uint8((scenario&2)!=0);
player=(scenario&8)!=0;lineClear=(scenario&16)!=0;haveGroup=(scenario&1)!=0;melee=(scenario&4)!=0;*reinterpret_cast<uint32*>(0xB7CB84)=12000;subType=types[(i/32)%8];
CTask* ret=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p);
if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(ret?"nonnull":"null");hit(std::to_string(subType).c_str());if(allocSize==0x30)hit("cover_goto");if(allocSize==0x28)hit("cover_duck");if(allocSize==0x3C)hit("cover_gun");for(const auto& e:trace)if(e[0].is_string())hit(e[0].get<std::string>().c_str());}

result[candidate]={{"return",Ptr(uintptr_t(ret))},{"task",Words(task.data(),task.size(),true)},{"target",Words(target.data(),target.size(),true)},{"vehicle",Words(vehicle.data(),vehicle.size(),true)},{"allocation",allocSize?Words(arena.data(),allocSize,true):json(nullptr)},{"trace",trace},{"switch",switches}};
}const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}
*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;h->State(hs);h->LockState(hl);auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
for(const auto* n:{"nonnull","null","allocate","reference","line","rand","say","cover_valid","cover_coordinates","reserve","release_cover","duck_secondary","clear_duck","secondary","los","cover_goto","cover_duck","cover_gun"})require(n);
Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",CTimer::GetTimeInMS()==savedTime}});
}
}
