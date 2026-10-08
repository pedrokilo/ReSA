#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexArrestPed.h"
#include "TaskComplexFallAndGetUp.h"
#include "CarEnterExit.h"
#include "../ProbeRuntime.hpp"
namespace ArrestPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskComplexArrestPed","CreateSubTask",0x68CF80,0,-1,512},
{"CTaskComplexArrestPed","CreateNextSubTask",0x690220,0x8709A8,8,1024},
{"CTaskComplexArrestPed","ControlSubTask",0x68D350,0x8709A8,10,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped,target;
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> data;
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted;
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel;
alignas(16) inline std::array<uint8,sizeof(CTaskComplexArrestPed)> task;
alignas(16) inline std::array<uint8,128> subtask,falltask;
alignas(16) inline std::array<uint8,256> arena;
inline std::array<void*,11> subVT{},taskVT{};
inline int scenario{},caseId{},subType{};inline bool aborted{},falling{},foundFall{},foundLeave{},locks{},roomDriver{},roomPassenger{},player{},closest{},alive{},melee{};inline size_t allocSize{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+x.size())return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(target) P(vehicle) P(data) P(wanted) P(intel) P(task) P(subtask) P(falltask) P(arena)
#undef P
return p;}
inline json Words(const void* p,size_t n,bool vptr=false){json j=json::array();for(size_t i=vptr?1:0;i<n/4;++i)j.push_back(Ptr(static_cast<const uint32*>(p)[i]));return j;}
inline void* __cdecl New(size_t n){trace.push_back({"allocate",n});allocSize=n;return n<=arena.size()?arena.data():nullptr;}
inline void __fastcall Ref(CEntity* p,void*,CEntity** slot){trace.push_back({"reference",Ptr(uintptr_t(p)),Ptr(uintptr_t(slot))});}
inline int __fastcall Type(CTask*,void*){trace.push_back({"task_type",subType});return subType;}
inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){trace.push_back({"abort",priority,aborted});return aborted;}
inline CTask* __fastcall First(CTaskComplexArrestPed*,void*,CPed*){trace.push_back({"first"});return reinterpret_cast<CTask*>(arena.data());}
inline CTask* __fastcall Sub(CTaskComplexArrestPed*,void*,eTaskType type,CPed*){trace.push_back({"create_sub",int(type)});return type==TASK_FINISHED?nullptr:reinterpret_cast<CTask*>(arena.data());}
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
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"arrest-pending-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"arrest-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("arrest-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"arrest-pending-only.txt");if(f)std::getline(f,only);}subVT[4]=reinterpret_cast<void*>(&Type);subVT[6]=reinterpret_cast<void*>(&Abort);
const auto savedTime=CTimer::GetTimeInMS();bool pass=true,selected=false;int total=0;
for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
EntryPatch re(0x571B70,Ref),rd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch pe(0x5DF8F0,Player),pd(bit_cast<uintptr_t>(&CPed::IsPlayer),Player);
EntryPatch ae(0x5E0170,Alive),ad(bit_cast<uintptr_t>(&CPed::IsAlive),Alive);
EntryPatch le(0x6D1E20,Locks),ld(bit_cast<uintptr_t>(&CVehicle::CanPedOpenLocks),Locks);
EntryPatch dre(0x6D1C40,Driver),drd(bit_cast<uintptr_t>(static_cast<bool(CVehicle::*)(const CPed*)const>(&CVehicle::IsDriver)),Driver);
EntryPatch fe(0x681740,Find),fd(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),Find);
EntryPatch fle(0x6787D0,Falling),fld(bit_cast<uintptr_t>(&CTaskComplexFallAndGetUp::IsFalling),Falling);
EntryPatch de(0x6787A0,Down),dd(bit_cast<uintptr_t>(&CTaskComplexFallAndGetUp::SetDownTime),Down);
EntryPatch rme(0x6504C0,Room),rmd(bit_cast<uintptr_t>(&CCarEnterExit::IsRoomForPedToLeaveCar),Room);
EntryPatch ue(0x6D1D90,Upside),ud(bit_cast<uintptr_t>(&CVehicle::IsUpsideDown),Upside);
EntryPatch sie(0x6D1DD0,Side),sid(bit_cast<uintptr_t>(&CVehicle::IsOnItsSide),Side);
EntryPatch mle(0x73B1C0,Melee),mld(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),Melee);
EntryPatch he(0x5DF300,Has),hd(bit_cast<uintptr_t>(&CPed::DoWeHaveWeaponAvailable),Has);
EntryPatch ste(0x5E6280,Set),stdp(bit_cast<uintptr_t>(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),Set);
EntryPatch we(0x56E230,Wanted),wd(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
EntryPatch cle(0x5627D0,Closest),cld(bit_cast<uintptr_t>(&CWanted::IsClosestCop),Closest);
EntryPatch se(0x5EFFE0,Say),sd(bit_cast<uintptr_t>(&CPed::Say),Say);
std::optional<EntryPatch> sube,subd;if(fn.slot!=-1){sube.emplace(0x68CF80,Sub);subd.emplace(bit_cast<uintptr_t>(&CTaskComplexArrestPed::CreateSubTask),Sub);}
const int types[]{TASK_SIMPLE_ARREST_PED,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_DESTROY_CAR,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_DRAG_PED_FROM_CAR,TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR,TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR,TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR,TASK_FINISHED,12345};
for(int i=0;i<fn.cases;++i){caseId=i;scenario=i%32;subType=types[(i/32)%10];json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);const bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
ped.fill(0);target.fill(0);vehicle.fill(0);data.fill(0);wanted.fill(0);intel.fill(0);task.fill(0xCD);subtask.fill(0);falltask.fill(0);arena.fill(0xCD);allocSize=0;trace=json::array();
auto* t=reinterpret_cast<CTaskComplexArrestPed*>(task.data());auto* p=reinterpret_cast<CPed*>(ped.data());auto* q=reinterpret_cast<CPed*>(target.data());auto* v=reinterpret_cast<CVehicle*>(vehicle.data());
std::memcpy(taskVT.data(),reinterpret_cast<void*>(0x8709A8),11*4);taskVT[9]=reinterpret_cast<void*>(&First);Put(task.data(),0,taskVT.data());Put<uint32>(task.data(),4,0);Put(task.data(),8,subtask.data());Put(subtask.data(),0,subVT.data());Put(target.data(),0x47C,intel.data());Put(target.data(),0x480,data.data());Put(data.data(),0,wanted.data());
t->m_PedToArrest=fn.slot<0||scenario!=0?q:nullptr;t->m_bSubTaskNeedsToBeCreated=scenario==1;t->m_Vehicle=(scenario&16)?v:nullptr;q->bInVehicle=(scenario&2)!=0;q->bIsBeingArrested=(scenario%8)==4;q->m_fHealth=scenario==2?0.f:100.f;q->m_pVehicle=v;q->GetPosition()=CVector{float(scenario%5),0.f,scenario==3?3.f:0.f};p->GetPosition()=CVector{0.f,0.f,0.f};p->m_nPedType=(scenario&8)?PED_TYPE_CIVMALE:PED_TYPE_COP;
v->m_pDriver=q;v->m_nVehicleType=eVehicleType(scenario%4==1?9:scenario%4==2?5:0);v->m_nVehicleSubType=eVehicleType(scenario%4==3?2:0);
Put<uint8>(subtask.data(),0x10,uint8((scenario&8)?2:0));Put<uint8>(subtask.data(),0x48,uint8((scenario&16)?4:0));aborted=(scenario&1)!=0;foundFall=(scenario&8)!=0;foundLeave=(scenario&16)!=0;falling=(scenario&4)!=0;locks=(scenario&4)!=0;roomDriver=(scenario&8)!=0;roomPassenger=(scenario&16)!=0;player=(scenario&16)!=0;closest=(scenario&1)!=0;alive=scenario!=7;melee=(scenario&16)!=0;reinterpret_cast<CWanted*>(wanted.data())->m_NumCopsInPursuit=uint8(scenario%3);*reinterpret_cast<uint32*>(0xB7CB84)=1200;
CTask* ret=fn.slot<0?reinterpret_cast<CTask*(__thiscall*)(void*,eTaskType,CPed*)>(address)(t,eTaskType(types[i%10]),p):reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p);
if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(ret?"nonnull":"null");if(fn.slot<0)hit(std::to_string(types[i%10]).c_str());for(const auto& e:trace)if(e[0].is_string())hit(e[0].get<std::string>().c_str());}
result[candidate]={{"return",Ptr(uintptr_t(ret))},{"task",Words(task.data(),task.size(),true)},{"target",Words(target.data(),target.size(),true)},{"vehicle",Words(vehicle.data(),vehicle.size(),true)},{"allocation",allocSize?Words(arena.data(),allocSize,true):json(nullptr)},{"trace",trace},{"switch",switches}};
}const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}
*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;h->State(hs);h->LockState(hl);auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
if(fn.slot<0){for(const auto* n:{"nonnull","null","allocate","reference","driver"})require(n);}else if(fn.slot==8){for(const auto* n:{"create_sub","task_type","falling","down","locks","alive","player"})require(n);}else{for(const auto* n:{"create_sub","abort","first","falling","room","say","closest","set_weapon"})require(n);}
Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",CTimer::GetTimeInMS()==savedTime}});
}
}
