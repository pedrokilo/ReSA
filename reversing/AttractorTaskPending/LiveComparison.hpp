#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexGoToAttractor.h"
#include "PedAttractorManager.h"
#include "Attractors/PedAttractor.h"
#include "PedPlacement.h"
#include "../ProbeRuntime.hpp"
namespace AttractorTaskPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskComplexGoToAttractor","Clone",0x66D130,0x86FF3C,1,256},
{"CTaskComplexGoToAttractor","CreateNextSubTask",0x66B6C0,0x86FF3C,8,256},
{"CTaskComplexGoToAttractor","CreateFirstSubTask",0x670420,0x86FF3C,9,512}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped;
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matrix;
alignas(16) inline std::array<uint8,sizeof(CTaskComplexGoToAttractor)> task;
alignas(16) inline std::array<uint8,sizeof(CPedAttractor)> attractor;
alignas(16) inline std::array<std::array<uint8,256>,8> arena;
inline std::array<size_t,8> sizes{};inline int allocCount{},caseId{};inline bool headIsPed{},correctZ{};inline int attractorType{};
inline std::array<void*,8> attractorVT{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;auto in=[&](const void* b,size_t n)->bool{return p>=uintptr_t(b)&&p<uintptr_t(b)+n;};if(in(ped.data(),ped.size()))return json::array({"ped",p-uintptr_t(ped.data())});if(in(matrix.data(),matrix.size()))return json::array({"matrix",p-uintptr_t(matrix.data())});if(in(task.data(),task.size()))return json::array({"task",p-uintptr_t(task.data())});if(in(attractor.data(),attractor.size()))return json::array({"attractor",p-uintptr_t(attractor.data())});for(int i=0;i<8;++i)if(in(arena[i].data(),arena[i].size()))return json::array({"allocation",i,p-uintptr_t(arena[i].data())});return p;}
inline json Words(const void* p,size_t n,bool vptr=false){json j=json::array();auto* w=static_cast<const uint32*>(p);for(size_t i=vptr?1:0;i<n/4;++i)j.push_back(Ptr(w[i]));return j;}
inline void* __cdecl New(size_t n){trace.push_back({"allocate",n});if(allocCount>=8||n>256)return nullptr;sizes[allocCount]=n;return arena[allocCount++].data();}
inline int __fastcall Type(void*,void*){trace.push_back({"attractor_type",attractorType});return attractorType;}
inline CPedAttractorManager* __cdecl Manager(){trace.push_back({"manager"});return reinterpret_cast<CPedAttractorManager*>(0x12340000);}
inline bool __fastcall Arrival(CPedAttractorManager*,void*,CPed* p,CPedAttractor* a){trace.push_back({"arrival",Ptr(uintptr_t(p)),Ptr(uintptr_t(a))});return caseId%2==0;}
inline CPed* __fastcall Head(const CPedAttractor*,void*){trace.push_back({"head_of_queue",headIsPed});return headIsPed?reinterpret_cast<CPed*>(ped.data()):nullptr;}
inline bool __cdecl ZExe(CVector* p){trace.push_back({"find_z",std::bit_cast<uint32>(p->z),correctZ});if(correctZ)p->z=7.25f;return correctZ;}
inline std::pair<CVector,bool> __cdecl ZDll(CVector p){ZExe(&p);return {p,correctZ};}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"attractor-task-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"attractor-task-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("attractor-task-pending");Write({{"event","waiting_for_player"}});}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::string only;{std::ifstream f(dir/"attractor-task-pending-only.txt");if(f)std::getline(f,only);}
 attractorVT[0]=reinterpret_cast<void*>(&Type);
 bool pass=true;int total=0;bool selected=false;
 for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;
  auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",full}});return;}
  const bool savedHook=h->Hooked(),savedLock=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
  {
   using ReversingProbe::EntryPatch;using std::bit_cast;
   EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
   EntryPatch me(0x5EE190,Manager),md(bit_cast<uintptr_t>(&GetPedAttractorManager),Manager);
   EntryPatch ae(0x5EFE20,Arrival),ad(bit_cast<uintptr_t>(static_cast<bool(CPedAttractorManager::*)(CPed*,CPedAttractor*)>(&CPedAttractorManager::BroadcastArrival)),Arrival);
   EntryPatch he(0x5EB590,Head),hd(bit_cast<uintptr_t>(&CPedAttractor::GetHeadOfQueue),Head);
   EntryPatch ze(0x616920,ZExe),zd(bit_cast<uintptr_t>(&CPedPlacement::FindZCoorForPed),ZDll);
   for(int i=0;i<fn.cases;++i){caseId=i;json result[2];
    for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const auto address=*reinterpret_cast<uintptr_t*>(fn.vt+fn.slot*4);const bool switches=(address<0x400000||address>=0x1000000)==(candidate!=0);if(!switches)pass=false;
     ped.fill(0);matrix.fill(0);task.fill(0xCD);attractor.fill(0);for(auto& a:arena)a.fill(0xCD);sizes.fill(0);allocCount=0;trace=json::array();
     auto* t=reinterpret_cast<CTaskComplexGoToAttractor*>(task.data());Put(attractor.data(),0,attractorVT.data());Put<uintptr_t>(task.data(),4,0);Put<uintptr_t>(task.data(),8,0);
     t->m_Attractor=reinterpret_cast<CPedAttractor*>(attractor.data());t->m_vecAttrPosn=CVector{float(i%37)-18.f,float(i%29)*.25f,float(i%19)-9.f};t->m_fAttrHeading=float(i%17)*.3f;t->m_fAttrTime=float(i%23)*13.f;t->m_nQueueNumber=i%11-2;t->m_MoveState=eMoveState(i%8);
     auto* p=reinterpret_cast<CPed*>(ped.data());p->bUseAttractorInstantly=(i&1)!=0;headIsPed=(i&2)!=0;correctZ=(i&4)!=0;attractorType=i%7;
     if(i&8)Put(ped.data(),0x14,matrix.data());
     CTask* ret{};if(fn.slot==1)ret=reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t);else ret=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p);
     if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};if(fn.slot==1)hit("clone");else if(fn.slot==8){hit(p->bUseAttractorInstantly?"instant_retained":"instant_cleared_or_absent");}else {hit((i&1)?"teleport":"sequence");if(i&1)hit((i&8)?"matrix_position":"placement_position");if(attractorType==4&&!(i&1))hit("shelter_run");}}
     json allocations=json::array();for(int k=0;k<allocCount;++k)allocations.push_back({{"size",sizes[k]},{"bytes",Words(arena[k].data(),sizes[k],true)}});
     result[candidate]={{"return",Ptr(uintptr_t(ret))},{"task",Words(task.data(),task.size(),true)},{"ped",Words(ped.data(),ped.size())},{"matrix",Words(matrix.data(),matrix.size())},{"allocations",allocations},{"trace",trace},{"switch",switches}};
    }
    const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
   }
  }
  h->State(savedHook);h->LockState(savedLock);
  auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"reason","missing coverage"},{"branch",n},{"function",full}});}};
  if(fn.slot==1)require("clone");else if(fn.slot==8){require("instant_retained");require("instant_cleared_or_absent");}else{for(const auto* n:{"teleport","sequence","matrix_position","placement_position","shelter_run"})require(n);}
  Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
 }
 Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",true}});
}
}
