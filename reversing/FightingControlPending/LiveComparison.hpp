#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleFightingControl.h"
#include "TaskSimpleFight.h"
#include "PedStats.h"
#include "../ProbeRuntime.hpp"
namespace FightingControlPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{ {"CTaskSimpleFightingControl","ProcessPed",0x62A0A0,0x86D700,7,1024} };
alignas(16) inline std::array<uint8,sizeof(CPed)> ped;
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelligence;
alignas(16) inline std::array<uint8,sizeof(CPedStat)> stats;
alignas(16) inline std::array<uint8,sizeof(CPed)> target;
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleFightingControl)> control;
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleFight)> fight;
inline std::array<void*,9> fakeVT{};inline CTask* secondary{}; inline int caseId{},randomCalls{},type{};
template<class T> inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline int Rand(){const int r=((caseId*40503u+randomCalls++*17389u)>>2)&32767;trace.push_back({"rand",r});return r;}
inline uint16 Random(){return uint16(Rand());}
inline void __fastcall Give(CPed*,void*){trace.push_back({"give_weapon"});}
inline CTask* __fastcall Get(CTaskManager*,void*,eSecondaryTask s){trace.push_back({"get_secondary",int(s)});return secondary;}
inline void __fastcall Set(CTaskManager*,void*,CTask* t,eSecondaryTask s){trace.push_back({"set_secondary",int(s),t!=nullptr});secondary=t;}
inline void* __cdecl New(size_t n){trace.push_back({"new_task",n});return fight.data();}
inline void* __fastcall Construct(void* t,void*,CEntity* e,int command,uint32 idle){trace.push_back({"construct_fight",e==reinterpret_cast<CEntity*>(target.data()),command,idle});Put(t,0,fakeVT.data());Put<uint32>(t,4,0);Put<int8>(t,0x24,int8(caseId%12));return t;}
inline int __fastcall Type(CTask*,void*){trace.push_back({"get_type",type});return type;}
inline bool __fastcall Abort(CTask*,void*,CPed*,int p,const CEvent* e){trace.push_back({"abort",p,e==nullptr});return false;}
inline bool __fastcall Control(CTaskSimpleFight*,void*,CEntity* e,uint8 command){trace.push_back({"control_fight",e==reinterpret_cast<CEntity*>(target.data()),command});return true;}
inline int16 __fastcall Move(CTaskSimpleFightingControl* t,void*,CPed*){const int16 r=int16(caseId%14-2);trace.push_back({"calc_move",r});t->m_nextAttackTime+=uint32(caseId%3);return r;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline json Bytes(const void* p,size_t n){auto* b=static_cast<const uint8*>(p);return std::vector<uint8>(b,b+n);}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"fighting-control-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"fighting-control-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("fighting-control-pending");Write({{"event","waiting_for_player"}});}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 {std::ifstream filter(dir/"fighting-control-pending-only.txt");std::string only;if(filter&&std::getline(filter,only)&&!only.empty()&&only!="CTaskSimpleFightingControl::ProcessPed"){Write({{"event","error"},{"reason","unknown function filter"},{"function",only}});return;}}
 auto* h=FindHook(fns[0].cls,fns[0].name);if(!h){Write({{"event","error"},{"reason","hook missing"}});return;}
 const bool hooked=h->Hooked(),locked=h->Locked();h->LockState(false);
 std::array<uint8,24> savedConfig;std::memcpy(savedConfig.data(),reinterpret_cast<void*>(0x8D2E4C),24);
 const auto savedTime=CTimer::GetTimeInMS();const float savedStep=CTimer::GetTimeStep();const auto savedCombos=CTaskSimpleFight::m_aComboData;
 fakeVT[4]=reinterpret_cast<void*>(&Type);fakeVT[6]=reinterpret_cast<void*>(&Abort);
 bool pass=true;int failures=0;json coverage=json::object();
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch g(0x5E8AB0,Give),gd(bit_cast<uintptr_t>(&CPed::GiveWeaponAtStartOfFight),Give);
 EntryPatch a(0x681810,Get),ad(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Get);
 EntryPatch s(0x681B60,Set),sd(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),Set);
 EntryPatch n(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New),c(0x61C470,Construct);
 EntryPatch cf(0x61C5E0,Control),cfd(bit_cast<uintptr_t>(&CTaskSimpleFight::ControlFight),Control);
 EntryPatch m(0x624B50,Move),md(bit_cast<uintptr_t>(&CTaskSimpleFightingControl::CalcMoveCommand),Move);
 EntryPatch r(0x821B1E,Rand),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 for(int i=0;i<fns[0].cases;++i){caseId=i;const int scenario=i%16,variation=i/16;json results[2];
  for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=*reinterpret_cast<uintptr_t*>(0x86D700+7*4);const bool switches=(address>=0x400000&&address<0x1000000)!=(candidate!=0);if(!switches){pass=false;}
   ped.fill(0);intelligence.fill(0);stats.fill(0);target.fill(0);control.fill(0xCD);fight.fill(0xCD);trace=json::array();randomCalls=0;
   auto* task=reinterpret_cast<CTaskSimpleFightingControl*>(control.data());Put(ped.data(),0x47C,intelligence.data());Put(ped.data(),0x59C,stats.data());
   Put<uint8>(ped.data(),0x484,uint8(i%3));Put<int32>(ped.data(),0x598,i%4==0?6:4);Put<uint16>(stats.data(),0x30,uint16((i%8)*20));
   Put<uint8>(ped.data(),0x719,i%3==0?40:uint8(i%101));Put<uint8>(ped.data(),0x72D,uint8(i%6));Put<uint8>(ped.data(),0x718,0);Put<int32>(ped.data(),0x5A0,i%2);
   Put<uint8>(target.data(),0x36,scenario==10?2:3);task->m_target=scenario==0?nullptr:reinterpret_cast<CEntity*>(target.data());task->m_bool=scenario==1;task->m_unk3=float(variation%11)*.1f;
   task->m_nextAttackTime=scenario==4?0:scenario==5?2000:3000;task->m_someTime=scenario==8?1000:scenario==9?1:0;
   Put(fight.data(),0,fakeVT.data());Put<int8>(fight.data(),0x24,int8(variation%16-4));type=scenario==3?int(TASK_SIMPLE_PAUSE):int(TASK_SIMPLE_FIGHT);secondary=scenario==2?nullptr:reinterpret_cast<CTask*>(fight.data());
   if(scenario==4||scenario==5){Put<uint8>(ped.data(),0x72D,scenario==4?4:0);Put<int32>(ped.data(),0x5A0,scenario==4?1:0);}
   if(scenario==6||scenario==7)Put<uint8>(ped.data(),0x719,60);
   *reinterpret_cast<uint32*>(0xB7CB84)=2000;*reinterpret_cast<float*>(0xB7CB5C)=float(i%10)*.5f;
   *reinterpret_cast<uint32*>(0x8D2E4C)=variation%7==0?0x80000001u:1000u;*reinterpret_cast<float*>(0x8D2E50)=60000.f;*reinterpret_cast<float*>(0x8D2E54)=scenario==6?400.f:scenario==7?0.f:float(variation%5)*30.f;*reinterpret_cast<int32*>(0x8D2E58)=30;*reinterpret_cast<int32*>(0x8D2E5C)=133;
   if(scenario==9)*reinterpret_cast<float*>(0xB7CB5C)=1.f;
   for(size_t k=0;k<12;++k)CTaskSimpleFight::m_aComboData[k].m_fRanges=float(k)*.25f;
   const auto beforeBlock=task->m_someTime,beforeAttack=task->m_nextAttackTime;const bool hadSecondary=secondary!=nullptr;
   bool ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(address)(task,reinterpret_cast<CPed*>(ped.data()));
   if(candidate){auto hit=[&](const char* name){coverage[name]=coverage.value(name,0)+1;};if(ret)hit("early_return");else if(!hadSecondary)hit("create_fight");else if(type!=int(TASK_SIMPLE_FIGHT))hit("abort_other_task");else if(beforeAttack<=2000)hit("expired_attack");else {if(beforeBlock){hit(task->m_someTime?"block_decrement_remaining":"block_decrement_expired");}else if(target[0x36]==3){hit("block_random_test");hit(task->m_someTime?"block_random_started":"block_random_rejected");}else hit("non_ped_no_block");}for(const auto& e:trace)if(e[0]=="calc_move")hit("calc_move");}
   results[candidate]={{"return",ret},{"task",Bytes(control.data()+8,control.size()-8)},{"fight",Bytes(fight.data()+4,fight.size()-4)},{"ped",Bytes(ped.data(),ped.size())},{"trace",trace},{"switch",switches}};
  }
  const bool equal=results[0]==results[1];failures+=!equal;pass&=equal;Write({{"event","comparison"},{"function","CTaskSimpleFightingControl::ProcessPed"},{"pair",i},{"case",i},{"equal",equal},{"accepted",equal},{"original",results[0]},{"candidate",results[1]}});
 }
 }
 std::memcpy(reinterpret_cast<void*>(0x8D2E4C),savedConfig.data(),24);*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;*reinterpret_cast<float*>(0xB7CB5C)=savedStep;CTaskSimpleFight::m_aComboData=savedCombos;h->State(hooked);h->LockState(locked);
 for(const auto* branch:{"early_return","create_fight","abort_other_task","expired_attack","block_decrement_remaining","block_decrement_expired","block_random_test","block_random_started","block_random_rejected","non_ped_no_block","calc_move"})if(!coverage.value(branch,0)){pass=false;Write({{"event","error"},{"reason","missing branch coverage"},{"branch",branch}});}
 Write({{"event","function_summary"},{"function","CTaskSimpleFightingControl::ProcessPed"},{"cases",1024},{"failures",failures},{"coverage",coverage}});Write({{"event",pass?"complete":"error"},{"pairs",1024},{"state_restored",std::memcmp(reinterpret_cast<void*>(0x8D2E4C),savedConfig.data(),24)==0&&CTimer::GetTimeInMS()==savedTime&&CTimer::GetTimeStep()==savedStep}});
}
}
