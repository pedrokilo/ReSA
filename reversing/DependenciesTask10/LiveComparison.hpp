#pragma once
#include <fstream>
#include "PedGroups.h"
#include "References.h"
#include "Reference.h"
#include "Entity/Ped/PlayerPed.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Pools/TaskPool.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesTask10Live {
inline bool done{};inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address;int reserved,slot,count;};
inline constexpr Fn fns[]{
 {"CPedGroups","AddGroup",0x5FB800,0,-1,2048},
 {"CPedGroups","Init",0x5FB8C0,0,-1,2048},
 {"CPedGroups","CleanUpForShutDown",0x5FB930,0,-1,2048}
};
inline uintptr_t Resolve(uintptr_t p){for(int i=0;i<16&&*reinterpret_cast<uint8*>(p)==0xE9;++i)p+=5+*reinterpret_cast<int32*>(p+1);return p;}
inline HMODULE ModuleOf(uintptr_t p){HMODULE m{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(p),&m);return m;}
inline void ModuleMarker(){}
struct Helper {
 uintptr_t entry,body;HMODULE module;std::vector<uint8> entryBytes,bodyBytes;
 ReversibleHooks::ReversibleHook::Base* hook{};bool enabled{},locked{};
 explicit Helper(uintptr_t at):entry(at),body(Resolve(at)),module(ModuleOf(body)),entryBytes(ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)),bodyBytes(ReversingProbe::Bytes(reinterpret_cast<void*>(body),64)){
  ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple&&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==at){hook=h.get();enabled=hook->Hooked();locked=hook->Locked();}});
 }
 bool Check()const{return body==Resolve(entry)&&ModuleOf(body)==module&&(module==GetModuleHandleA(nullptr)||module==ModuleOf(uintptr_t(&ModuleMarker)))
  &&entryBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(entry),16)&&bodyBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(body),64)
  &&(!hook||(hook->Hooked()==enabled&&hook->Locked()==locked));}
 json Metadata()const{return {{"entry",entry},{"body",body},{"module",uintptr_t(module)},{"entry_bytes",entryBytes},{"body_bytes",bodyBytes},{"registered",hook!=nullptr},{"enabled",enabled},{"locked",locked},{"route_ok",Check()}};}
};
struct Fpu {std::array<uint8,108> state;uint32 mxcsr;bool operator==(const Fpu&)const=default;};
inline Fpu CaptureFP(){Fpu f{};auto* p=f.state.data();auto* m=&f.mxcsr;__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]} __asm {mov eax,m} __asm {stmxcsr [eax]}return f;}
inline void RestoreFP(const Fpu& f){const auto* p=f.state.data();const auto m=f.mxcsr;__asm {mov eax,p} __asm {frstor [eax]} __asm {ldmxcsr m}}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct ABI {uint32 eax;bool stack,nv,seh;};
inline ABI Gate(uintptr_t entry){uint32 a{},sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};
 __asm {
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  mov eax,0A5123456h
  mov ecx,137AFF01h
  mov edx,0C0DE5921h
  call entry
  mov a,eax
  mov sp1,esp
  mov b1,ebx
  mov s1,esi
  mov d1,edi
  mov p1,ebp
  mov eax,fs:[0]
  mov h1,eax
 }
 return {a,sp0==sp1,b0==b1&&s0==s1&&d0==d1&&p0==p1,h0==h1};}
inline void Put(void* p,uintptr_t v){memcpy(p,&v,4);}
inline CPed*& Member(unsigned g,unsigned m){return *reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(&CPedGroups::ms_groups[g])+0xC+m*4);}
inline CTask*& TaskAt(unsigned g,unsigned pair){constexpr unsigned bases[]{0xC,0xAC,0x14C,0x1EC};return *reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(&CPedGroups::ms_groups[g])+0x30+bases[pair/8]+20*(pair%8)+4);}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciestask10-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciestask10-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciestask10");}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
 const auto fp=CaptureFP();
 std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,3> routes;
 routes[0]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","AddGroup",0x5FB800,&CPedGroups::AddGroup);
 routes[1]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","Init",0x5FB8C0,&CPedGroups::Init);
 routes[2]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","CleanUpForShutDown",0x5FB930,&CPedGroups::CleanUpForShutDown);
 const auto savedGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
 const auto savedActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8);
 const auto savedIndices=ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16);
 const auto oldRefs=ReversingProbe::Bytes(&CReferences::aRefs,sizeof(CReferences::aRefs));
 auto* oldRefHead=CReferences::pEmptyList;auto* playerHead=player->m_pReferences;
 auto*& globalPool=*reinterpret_cast<CTaskPool**>(0xB744A8);auto* oldPool=globalPool;
 if(!oldPool){RestoreFP(fp);Write({{"event","error"},{"reason","missing real task pool"}});return;}
 const auto oldPoolBytes=ReversingProbe::Bytes(oldPool,sizeof(CTaskPool));
 static_assert(sizeof(CTaskPool)==0x14&&sizeof(CTaskSimpleSlideToCoord)==0x80);
 void* oldTaskStorage{};void* oldTaskBitmap{};
 memcpy(&oldTaskStorage,oldPoolBytes.data(),4);memcpy(&oldTaskBitmap,oldPoolBytes.data()+4,4);
 const auto oldTaskCapacity=oldPool->GetSize();
 if(!oldTaskStorage||!oldTaskBitmap||oldTaskCapacity==0||oldTaskCapacity>4096){RestoreFP(fp);Write({{"event","error"},{"reason","invalid original task-pool fixture"}});return;}
 const auto oldTaskStorageBytes=ReversingProbe::Bytes(oldTaskStorage,oldTaskCapacity*sizeof(CTaskSimpleSlideToCoord));
 const auto oldTaskBitmapBytes=ReversingProbe::Bytes(oldTaskBitmap,oldTaskCapacity);
 auto originalPoolIntact=[&]{return oldPoolBytes==ReversingProbe::Bytes(oldPool,sizeof(CTaskPool))
  &&memcmp(oldTaskStorage,oldTaskStorageBytes.data(),oldTaskStorageBytes.size())==0
  &&memcmp(oldTaskBitmap,oldTaskBitmapBytes.data(),oldTaskBitmapBytes.size())==0;};
 const auto oldPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
 const auto beforeGroupsGuard=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC09910),16);
 const auto afterGroupsGuard=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFC0),16);
 alignas(16) std::array<uint8,16+16*sizeof(CTaskSimpleSlideToCoord)+16> storage{};
 alignas(16) std::array<uint8,48> bitmap{};
 alignas(16) std::array<uint8,16+64*sizeof(CReference)+16> referenceArena{};
 auto* refs=reinterpret_cast<CReference*>(referenceArena.data()+16);
 CTaskPool ownPool(16,storage.data()+16,bitmap.data()+16);
 const auto ownPoolHeader=ReversingProbe::Bytes(&ownPool,sizeof(ownPool));
 storage.fill(0xA5);bitmap.fill(0xD6);referenceArena.fill(0xB7);
 const auto cleanStorage=storage;const auto cleanBitmap=bitmap;const auto cleanReferenceArena=referenceArena;
 std::vector<Helper> helpers;
 for(auto at:{0x5FC150u,0x5F7250u,0x5F80D0u,0x5F7350u,0x5E95B0u,0x571A00u,0x571B70u,0x5DF8F0u,0x61A5A0u,0x61A5B0u,0x61A500u,0x62F310u})helpers.emplace_back(at);
 auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
 auto originalRefsIntact=[&]{return oldRefs==ReversingProbe::Bytes(&CReferences::aRefs,sizeof(CReferences::aRefs));};
 auto groupGuards=[&]{return beforeGroupsGuard==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC09910),16)&&afterGroupsGuard==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFC0),16);};
 auto arenaGuards=[&]{return std::equal(storage.begin(),storage.begin()+16,cleanStorage.begin())&&std::equal(storage.end()-16,storage.end(),cleanStorage.end()-16)
  &&std::equal(bitmap.begin(),bitmap.begin()+16,cleanBitmap.begin())&&std::equal(bitmap.end()-16,bitmap.end(),cleanBitmap.end()-16)
  &&std::equal(referenceArena.begin(),referenceArena.begin()+16,cleanReferenceArena.begin())&&std::equal(referenceArena.end()-16,referenceArena.end(),cleanReferenceArena.end()-16);};
 auto restore=[&]{memcpy(&CPedGroups::ms_groups,savedGroups.data(),savedGroups.size());memcpy(&CPedGroups::ms_activeGroups,savedActive.data(),8);memcpy(&CPedGroups::ScriptReferenceIndex,savedIndices.data(),16);
  player->m_pReferences=playerHead;CReferences::pEmptyList=oldRefHead;globalPool=oldPool;RestoreFP(fp);};
 auto restored=[&]{return savedGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,savedGroups.size())&&savedActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8)
  &&savedIndices==ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16)&&originalRefsIntact()&&CReferences::pEmptyList==oldRefHead&&player->m_pReferences==playerHead
  &&globalPool==oldPool&&originalPoolIntact()&&oldPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&groupGuards()&&CaptureFP()==fp;};
 auto referencePartition=[&]{std::array<bool,64> seen{};unsigned used=0,free=0;
  auto walk=[&](CReference* at,bool live){unsigned n=0;while(at){const auto delta=intptr_t(at)-intptr_t(refs);if(delta<0||delta>=64*8||delta%8||++n>64)return false;const auto index=unsigned(delta/8);if(seen[index])return false;seen[index]=true;
    if(live&&(!at->m_ppEntity||*at->m_ppEntity!=player))return false;at=at->m_pNext;}if(live)used=n;else free=n;return true;};
  return walk(player->m_pReferences,true)&&walk(CReferences::pEmptyList,false)&&used+free==64;};
 bool ready=oldPool&&player->IsPlayer()&&helpersOK()&&sizeof(CTaskPool)==20;for(auto& r:routes)ready&=r->Registered()&&ModuleOf(r->Body())==ModuleOf(uintptr_t(&ModuleMarker));
 json helperRows=json::array();for(const auto& h:helpers)helperRows.push_back(h.Metadata());
 RestoreFP(fp);Write({{"event","begin"},{"registered_hook",ready},{"helper_routes_ok",helpersOK()},{"helper_routes",helperRows},{"actor_real_player",player->IsPlayer()},
  {"original_task_pool_capacity",oldTaskCapacity},{"original_task_storage_bytes",oldTaskStorageBytes.size()},{"original_task_bitmap_bytes",oldTaskBitmapBytes.size()},
  {"original_task_pool_intact",originalPoolIntact()},{"private_task_pool_capacity",16},{"private_reference_capacity",64},{"scope","real shared callbacks; no transitive independence claim"}});
 int pair=0;bool passed=ready;
 for(unsigned fn=0;fn<3&&ready;++fn){int failures=0;
  for(unsigned scenario=0;scenario<2048;++scenario){
   auto invoke=[&](bool candidate){
    bool oldPoolOK=originalPoolIntact();
    restore();storage=cleanStorage;bitmap=cleanBitmap;referenceArena=cleanReferenceArena;memcpy(&ownPool,ownPoolHeader.data(),ownPoolHeader.size());
    for(unsigned i=0;i<16;++i)bitmap[16+i]=uint8(0x80|((scenario+i)&0x7F));
    for(unsigned i=0;i<64;++i){refs[i].m_pNext=i<63?&refs[i+1]:nullptr;refs[i].m_ppEntity=nullptr;}
    globalPool=&ownPool;CReferences::pEmptyList=refs;player->m_pReferences=nullptr;
    const unsigned mask=scenario&255,mode=scenario/256%4;const bool hasTasks=(scenario&1024)!=0;
    std::memset(&CPedGroups::ms_groups,0xA7,sizeof(CPedGroups::ms_groups));
    unsigned acquiredTasks=0;std::vector<Helper> virtualRoutes;json taskRoutes=json::array();
    for(unsigned g=0;g<8;++g){auto* group=&CPedGroups::ms_groups[g];auto* bytes=reinterpret_cast<uint8*>(group);
     Put(bytes+0x34,0);Put(bytes+0x38,0);reinterpret_cast<void(__thiscall*)(void*)>(0x5FC150)(group);
     CPedGroups::ms_activeGroups[g]=(mask&(1u<<g))?char(hasTasks?0x80:1):0;CPedGroups::ScriptReferenceIndex[g]=uint16(scenario*17+g);
     for(unsigned m=0;m<8;++m)if(mode==3||(mode==1&&m==(scenario+g)%8)||(mode==2&&m%2==g%2)){auto& slot=Member(g,m);slot=player;CEntity::SafeRegisterRef(slot);}
     if(hasTasks){void* raw=reinterpret_cast<void*(__cdecl*)(size_t)>(0x61A5A0)(sizeof(CTaskSimpleStandStill));if(!raw)std::abort();
      auto* task=reinterpret_cast<CTaskSimpleStandStill*(__thiscall*)(void*,int32,bool,bool,float)>(0x62F310)(raw,int32(scenario),true,false,8.f);
      if(!ownPool.IsPtrFromPool(task)||ownPool.IsFreeSlotAtIndex(ownPool.GetIndex(task)))std::abort();
      const auto virtualEntry=*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(task));virtualRoutes.emplace_back(virtualEntry);taskRoutes.push_back(virtualRoutes.back().Metadata());
      TaskAt(g,(scenario/8+g)%32)=task;++acquiredTasks;
     }
    }
    const auto inputGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
    const auto inputActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8);const auto inputIndices=ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16);
    auto expectedGroups=inputGroups,expectedActive=inputActive,expectedIndices=inputIndices;int selected=-1;
    if(fn==0){for(unsigned g=0;g<8;++g)if(!(mask&(1u<<g))){selected=int(g);break;}if(selected>=0)expectedActive[selected]=1;}
    if(fn==1){for(unsigned g=0;g<8;++g){expectedIndices[g*2]=1;expectedIndices[g*2+1]=0;if(mask&(1u<<g))expectedActive[g]=0;}}
    unsigned expectedRemaining=acquiredTasks;
    for(unsigned g=0;g<8;++g){bool flushed=fn==2||(fn==1&&(mask&(1u<<g)))||(fn==0&&int(g)==selected);if(!flushed)continue;
     for(unsigned m=0;m<8;++m)std::memset(expectedGroups.data()+g*0x2D4+0xC+m*4,0,4);
     constexpr unsigned bases[]{0xC,0xAC,0x14C,0x1EC};for(auto b:bases)for(unsigned m=0;m<8;++m){auto* p=expectedGroups.data()+g*0x2D4+0x30+b+m*20;std::memset(p,0,8);std::memset(p+8,0xFF,4);}
     for(auto o:{0x34u,0x38u,0x2C0u,0x2C4u})std::memset(expectedGroups.data()+g*0x2D4+o,0,4);
     for(auto o:{0x2C8u,0x2CCu})std::memset(expectedGroups.data()+g*0x2D4+o,0xFF,4);
     expectedGroups[g*0x2D4+0x2D0]=0;if(hasTasks)--expectedRemaining;
    }
    bool helperOK=helpersOK(),refOK=referencePartition()&&originalRefsIntact(),routeOK=false;
    oldPoolOK&=originalPoolIntact();
    for(const auto& v:virtualRoutes)helperOK&=v.Check();
    const auto abi=routes[fn]->Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;return Gate(fns[fn].address);});
    helperOK&=helpersOK();for(const auto& v:virtualRoutes)helperOK&=v.Check();oldPoolOK&=originalPoolIntact();
    refOK&=referencePartition()&&originalRefsIntact();
    const auto actualGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));const auto actualActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8);const auto actualIndices=ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16);
    const bool contract=actualGroups==expectedGroups&&actualActive==expectedActive&&actualIndices==expectedIndices&&(fn!=0||int32(abi.eax)==selected)&&ownPool.GetNoOfUsedSpaces()==expectedRemaining;
    const bool guards=arenaGuards()&&groupGuards();const auto taskStates=std::vector<uint8>(bitmap.begin()+16,bitmap.begin()+32);
    for(unsigned g=0;g<8;++g){for(unsigned m=0;m<8;++m)CEntity::SafeCleanUpRef(Member(g,m));for(unsigned t=0;t<32;++t){auto*& p=TaskAt(g,t);if(p){delete p;p=nullptr;}}}
    const bool resources=ownPool.GetNoOfUsedSpaces()==0&&player->m_pReferences==nullptr&&referencePartition()&&originalRefsIntact();
    oldPoolOK&=originalPoolIntact();
    storage=cleanStorage;bitmap=cleanBitmap;referenceArena=cleanReferenceArena;memcpy(&ownPool,ownPoolHeader.data(),ownPoolHeader.size());restore();
    return json{{"active_mask",mask},{"member_mode",mode},{"has_tasks",hasTasks},{"selected",selected},{"return",fn==0?int32(abi.eax):0},
     {"groups",actualGroups},{"active",actualActive},{"indices",actualIndices},{"task_states",taskStates},{"task_virtual_routes",taskRoutes},{"tasks_acquired",acquiredTasks},{"tasks_remaining",expectedRemaining},
     {"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nv},{"seh_restored",abi.seh},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"references_valid",refOK},
     {"fixture_contract",contract},{"guards_preserved",guards},{"resources_balanced",resources},{"original_task_pool_intact",oldPoolOK&&originalPoolIntact()},{"state_restored",restored()},{"floating_state_restored",CaptureFP()==fp},
     {"private_arenas_restored",storage==cleanStorage&&bitmap==cleanBitmap&&referenceArena==cleanReferenceArena&&ownPoolHeader==ReversingProbe::Bytes(&ownPool,sizeof(ownPool))}};
   };
   const auto original=invoke(false);const auto candidate=invoke(true);bool ok=original==candidate;
   for(auto k:{"stack_restored","nonvolatile_preserved","seh_restored","route_ok","helper_route_ok","references_valid","fixture_contract","guards_preserved","resources_balanced","original_task_pool_intact","state_restored","floating_state_restored","private_arenas_restored"})ok&=original[k]==true&&candidate[k]==true;
   passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function",std::string("CPedGroups::")+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});RestoreFP(fp);
  }
  Write({{"event","function_summary"},{"function",std::string("CPedGroups::")+fns[fn].name},{"cases",2048},{"failures",failures}});RestoreFP(fp);
 }
 restore();bool final=restored()&&helpersOK();for(auto& r:routes){const bool one=r->Restore();final&=one;}RestoreFP(fp);
 Write({{"event",passed&&final?"complete":"error"},{"pairs",pair},{"state_restored",final},{"original_task_pool_intact",originalPoolIntact()},{"helper_route_ok",helpersOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
