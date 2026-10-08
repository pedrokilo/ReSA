#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskGangHasslePed.h"
#include "TaskComplexGangLeader.h"
#include "AnimManager.h"
#include "../ProbeRuntime.hpp"
namespace GangHasslePendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskGangHasslePed","CreateFirstSubTask",0x664380,0x86FA00,9,512},
{"CTaskGangHasslePed","CreateNextSubTask",0x6642C0,0x86FA00,8,512},
{"CTaskGangHasslePed","ControlSubTask",0x65FFE0,0x86FA00,10,1024}};
alignas(16)inline std::array<uint8,sizeof(CPed)> ped,target;
alignas(16)inline std::array<uint8,sizeof(CPedIntelligence)> intelligence;
alignas(16)inline std::array<uint8,sizeof(CTaskGangHasslePed)> task;
alignas(16)inline std::array<uint8,128> subtask;
alignas(16)inline std::array<std::array<uint8,256>,8> arena;
inline std::array<size_t,8> sizes{};inline int allocCount{},caseId{},rngCalls{},subType{},randomChoice{};inline bool shouldLoad{},playing{},haveSecondary{};
inline std::array<void*,11> subVT{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;auto in=[&](const void* b,size_t n){return p>=uintptr_t(b)&&p<uintptr_t(b)+n;};if(in(ped.data(),ped.size()))return json::array({"ped",p-uintptr_t(ped.data())});if(in(target.data(),target.size()))return json::array({"target",p-uintptr_t(target.data())});if(in(task.data(),task.size()))return json::array({"task",p-uintptr_t(task.data())});if(in(subtask.data(),subtask.size()))return json::array({"subtask",p-uintptr_t(subtask.data())});if(in(intelligence.data(),intelligence.size()))return json::array({"intelligence",p-uintptr_t(intelligence.data())});for(int k=0;k<8;++k)if(in(arena[k].data(),256))return json::array({"allocation",k,p-uintptr_t(arena[k].data())});return p;}
inline json Words(const void* p,size_t n,bool vptr=false){json j=json::array();auto* w=static_cast<const uint32*>(p);for(size_t i=vptr?1:0;i<n/4;++i)j.push_back(Ptr(w[i]));return j;}
inline void* __cdecl New(size_t n){trace.push_back({"allocate",n});if(allocCount>=8||n>256)return nullptr;sizes[allocCount]=n;return arena[allocCount++].data();}
inline int __cdecl Rand(){int r;if(randomChoice>=0&&rngCalls==0)r=(randomChoice*32768+199)/200;else r=int((caseId*40503u+rngCalls*17389u)>>2)&32767;++rngCalls;trace.push_back({"rand",r});return r;}
inline uint16 Random(){return uint16(Rand());}
inline int __fastcall Type(void*,void*){trace.push_back({"subtask_type",subType});return subType;}
inline void __fastcall Ref(CEntity* e,void*,CEntity** slot){trace.push_back({"register_reference",Ptr(uintptr_t(e)),Ptr(uintptr_t(slot))});}
inline bool __cdecl Should(){trace.push_back({"should_load",shouldLoad});return shouldLoad;}
inline int __cdecl Block(const char* name){trace.push_back({"anim_block",std::string(name)});return 0;}
inline void __cdecl Add(int b){trace.push_back({"add_anim_ref",b});}
inline void __cdecl Remove(int b){trace.push_back({"remove_anim_ref",b});}
inline void __cdecl Request(int m,int flags){trace.push_back({"request_model",m,flags});}
inline bool __fastcall Playing(CPed*,void*){trace.push_back({"playing_signal",playing});return playing;}
inline CTask* __fastcall Secondary(CTaskManager*,void*,eSecondaryTask k){trace.push_back({"get_secondary",int(k),haveSecondary});return haveSecondary?reinterpret_cast<CTask*>(subtask.data()):nullptr;}
inline void __fastcall Set(CTaskManager*,void*,CTask* t,eSecondaryTask k){trace.push_back({"set_secondary",Ptr(uintptr_t(t)),int(k)});}
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c){trace.push_back({"say",Ptr(uintptr_t(p)),ctx,delay,std::bit_cast<uint32>(prob),a,b,c});return 0;}
inline void __cdecl Abuse(CPed* p,CPed* q){trace.push_back({"abuse",Ptr(uintptr_t(p)),Ptr(uintptr_t(q))});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"gang-hassle-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"gang-hassle-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("gang-hassle-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::string only;{std::ifstream f(dir/"gang-hassle-pending-only.txt");if(f)std::getline(f,only);}
 subVT[4]=reinterpret_cast<void*>(&Type);const uint32 savedTime=CTimer::GetTimeInMS();std::array<uint8,32> savedBlock;std::memcpy(savedBlock.data(),reinterpret_cast<void*>(0xB5D4A0),32);
 bool pass=true,selected=false;int total=0;
 for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}const bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
  {
   using ReversingProbe::EntryPatch;using std::bit_cast;
   EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
   EntryPatch re(0x821B1E,Rand),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
   EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
   EntryPatch sle(0x65E7F0,Should),sld(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),Should);
   EntryPatch ble(0x4D3990,Block),bld(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
   EntryPatch ade(0x4D3FB0,Add),add(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
   EntryPatch rme(0x4D3FD0,Remove),rmd(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Remove);
   EntryPatch rqe(0x4087E0,Request),rqd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
   EntryPatch pe(0x5E0460,Playing),pd(bit_cast<uintptr_t>(&CPed::IsPlayingHandSignal),Playing);
   EntryPatch se(0x681810,Secondary),sd(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
   EntryPatch ste(0x681B60,Set),stdp(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),Set);
   EntryPatch spe(0x5EFFE0,Say),spd(bit_cast<uintptr_t>(&CPed::Say),Say);
   EntryPatch abe(0x65E860,Abuse),abd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::DoGangAbuseSpeech),Abuse);
   for(int i=0;i<fn.cases;++i){caseId=i;const int scenario=i%32;json result[2];
    for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=*reinterpret_cast<uintptr_t*>(fn.vt+fn.slot*4);const bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
     ped.fill(0);target.fill(0);intelligence.fill(0);task.fill(0xCD);subtask.fill(0xCD);for(auto& a:arena)a.fill(0xCD);sizes.fill(0);allocCount=0;rngCalls=0;trace=json::array();
     auto* t=reinterpret_cast<CTaskGangHasslePed*>(task.data());Put<uintptr_t>(task.data(),4,0);Put(task.data(),8,reinterpret_cast<CTask*>(subtask.data()));Put(subtask.data(),0,subVT.data());Put(ped.data(),0x47C,intelligence.data());
     t->m_Ped=scenario==0?nullptr:reinterpret_cast<CPed*>(target.data());t->dword10=scenario%4;t->m_RndMin=100+i%73;t->m_RndMax=700+i%113;t->m_nTime=scenario<8?900:1500;t->m_nSomeRandomShit=200;t->m_bFirstSubTaskInitialised=scenario!=1;t->m_bRefreshTime=scenario==2;t->m_bAnimBlockRefAdded=(scenario&4)!=0;
     shouldLoad=(scenario&8)!=0;playing=scenario==21;haveSecondary=scenario==22;subType=scenario==3?int(TASK_COMPLEX_KILL_PED_ON_FOOT):int(TASK_COMPLEX_SEEK_ENTITY);randomChoice=fn.slot==10?(scenario==24?51:scenario==25?100:30):-1;
     *reinterpret_cast<uint32*>(0xB7CB84)=1200;*reinterpret_cast<uint8*>(0xB5D4B0)=uint8((scenario&16)!=0);
     auto* ret=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,reinterpret_cast<CPed*>(ped.data()));
     if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};if(fn.slot==9)hit(t->m_Ped?"create_seek":"null_target");else if(fn.slot==8)hit(subType==int(TASK_COMPLEX_KILL_PED_ON_FOOT)?"kill_finished":"next_seek");else {if(!t->m_Ped)hit("null_target");if(t->m_bRefreshTime==false&&scenario==2)hit("refresh_time");if(!ret)hit("end");if(allocCount&&sizes[0]==0x38)hit("create_kill");for(const auto& e:trace){if(e[0]=="request_model")hit("request_anim");if(e[0]=="add_anim_ref")hit("add_ref");if(e[0]=="remove_anim_ref")hit("remove_ref");if(e[0]=="set_secondary")hit("secondary_anim");if(e[0]=="say")hit("eyeing");if(e[0]=="abuse")hit("abuse");}}}
     json allocations=json::array();for(int k=0;k<allocCount;++k)allocations.push_back({{"size",sizes[k]},{"bytes",Words(arena[k].data(),sizes[k],true)}});
     result[candidate]={{"return",Ptr(uintptr_t(ret))},{"task",Words(task.data(),task.size(),true)},{"allocations",allocations},{"trace",trace},{"switch",switches}};
    }
    const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
   }
  }
  *reinterpret_cast<uint32*>(0xB7CB84)=savedTime;std::memcpy(reinterpret_cast<void*>(0xB5D4A0),savedBlock.data(),32);h->State(hs);h->LockState(hl);
  auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
  if(fn.slot==9){require("create_seek");require("null_target");}else if(fn.slot==8){require("kill_finished");require("next_seek");}else{for(const auto* n:{"null_target","refresh_time","end","create_kill","request_anim","add_ref","remove_ref","secondary_anim","eyeing","abuse"})require(n);}
  Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
 }
 Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",CTimer::GetTimeInMS()==savedTime&&std::memcmp(reinterpret_cast<void*>(0xB5D4A0),savedBlock.data(),32)==0}});
}
}
