#pragma once
#include <fstream>
#include "PedGroups.h"
#include "References.h"
#include "Reference.h"
#include "Entity/Ped/PlayerPed.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Pools/TaskPool.h"
#include "Pools/PedPool.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesGroupTasks14Live {
inline bool done{};inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address;int reserved,slot,count;};
inline constexpr Fn fns[]{
 {"CPedGroupIntelligence","ComputeScriptCommandTasks",0x5F7800,0,-1,2048}
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
inline std::function<bool()> pumpStateCheck;inline bool pumpStateOK=true;inline bool writeFPActive{};inline Fpu writeFP{};
struct PumpObserverGuard{~PumpObserverGuard(){pumpStateCheck={};writeFPActive=false;}};
inline void Write(const json& r){const auto enteringFP=CaptureFP();const auto& baseline=writeFPActive?writeFP:enteringFP;RestoreFP(baseline);
 if(pumpStateCheck)pumpStateOK&=pumpStateCheck();RestoreFP(baseline);ReversingProbe::PumpMessages();RestoreFP(baseline);if(pumpStateCheck)pumpStateOK&=pumpStateCheck();RestoreFP(baseline);
 auto record=r;if(record.value("event","")=="complete"&&!pumpStateOK){record["event"]="error";record["state_restored"]=false;}if(pumpStateCheck)record["pump_state_intact"]=pumpStateOK;log<<record.dump()<<'\n';log.flush();RestoreFP(baseline);
}
struct ABI {uint32 eax;bool stack,nv,seh;};
inline ABI Gate(uintptr_t entry,void* object){uint32 a{},sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};
 __asm {
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  mov eax,0A5123456h
  mov ecx,object
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
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesgrouptasks14-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesgrouptasks14-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesgrouptasks14");}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;
 auto* pedPool=GetPedPool();CPed* other{};if(pedPool)for(unsigned i=0;i<unsigned(pedPool->GetSize());++i){auto* p=pedPool->GetAt(i);if(p&&p!=player&&!p->IsPlayer()&&p->GetIntelligence()&&p->GetAreaCode()==AREA_CODE_NORMAL_WORLD){other=p;break;}}
 static uint64 waitingSince{};if(!other||player->GetAreaCode()!=AREA_CODE_NORMAL_WORLD){if(!waitingSince)waitingSince=GetTickCount64();if(GetTickCount64()-waitingSince>60000){done=true;Write({{"event","error"},{"reason","no naturally loaded second exterior actor"},{"pairs",0},{"state_restored",true}});}return;}done=true;
 const auto fp=CaptureFP();writeFP=fp;writeFPActive=true;pumpStateOK=true;PumpObserverGuard pumpGuard;
 OriginalDependencies499::HookRoute route("CPedGroupIntelligence","ComputeScriptCommandTasks",0x5F7800,FunctionToVoidPtr(&CPedGroupIntelligence::ComputeScriptCommandTasks));
 const auto savedGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
 const auto savedActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8);
 const auto savedIndices=ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16);
 const auto oldRefs=ReversingProbe::Bytes(&CReferences::aRefs,sizeof(CReferences::aRefs));
 auto* oldRefHead=CReferences::pEmptyList;auto* playerHead=player->m_pReferences;auto* otherHead=other->m_pReferences;static_assert(sizeof(CCopPed)==0x7C4);const auto oldOther=ReversingProbe::Bytes(other,0x7C4);
 const auto pedPoolHeader=ReversingProbe::Bytes(pedPool,sizeof(CPedPool));const auto pedPoolCapacity=pedPool->GetSize();const auto otherIndex=pedPool->GetIndex(other);
 void* pedBitmap{};memcpy(&pedBitmap,pedPoolHeader.data()+4,4);const auto pedBitmapSaved=ReversingProbe::Bytes(pedBitmap,pedPoolCapacity);
 const auto otherBefore=otherIndex>0?ReversingProbe::Bytes(reinterpret_cast<uint8*>(other)-16,16):std::vector<uint8>{};
 const auto otherAfter=uint32(otherIndex)+1<pedPoolCapacity?ReversingProbe::Bytes(reinterpret_cast<uint8*>(other)+0x7C4,16):std::vector<uint8>{};
 auto pedSlotGuards=[&]{return pedPoolHeader==ReversingProbe::Bytes(pedPool,sizeof(CPedPool))&&pedBitmapSaved==ReversingProbe::Bytes(pedBitmap,pedPoolCapacity)
  &&(otherBefore.empty()||otherBefore==ReversingProbe::Bytes(reinterpret_cast<uint8*>(other)-16,16))&&(otherAfter.empty()||otherAfter==ReversingProbe::Bytes(reinterpret_cast<uint8*>(other)+0x7C4,16));};auto* otherIntelligence=other->GetIntelligence();const auto oldOtherIntelligence=ReversingProbe::Bytes(otherIntelligence,sizeof(CPedIntelligence));
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
 for(auto at:{0x5FC150u,0x5FC190u,0x5F7250u,0x5F6AE0u,0x5FB160u,0x5F80D0u,0x5F7350u,0x5E95B0u,0x571A00u,0x571B70u,0x61A5A0u,0x61A5B0u,0x61A500u,0x62F310u})helpers.emplace_back(at);
 auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
 auto originalRefsIntact=[&]{return oldRefs==ReversingProbe::Bytes(&CReferences::aRefs,sizeof(CReferences::aRefs));};
 auto groupGuards=[&]{return beforeGroupsGuard==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC09910),16)&&afterGroupsGuard==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFC0),16);};
 auto arenaGuards=[&]{return std::equal(storage.begin(),storage.begin()+16,cleanStorage.begin())&&std::equal(storage.end()-16,storage.end(),cleanStorage.end()-16)
  &&std::equal(bitmap.begin(),bitmap.begin()+16,cleanBitmap.begin())&&std::equal(bitmap.end()-16,bitmap.end(),cleanBitmap.end()-16)
  &&std::equal(referenceArena.begin(),referenceArena.begin()+16,cleanReferenceArena.begin())&&std::equal(referenceArena.end()-16,referenceArena.end(),cleanReferenceArena.end()-16);};
 auto originalGroupsIntact=[&]{return savedGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,savedGroups.size())&&savedActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8)&&savedIndices==ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16);};
 auto restore=[&]{memcpy(&CPedGroups::ms_groups,savedGroups.data(),savedGroups.size());memcpy(&CPedGroups::ms_activeGroups,savedActive.data(),8);memcpy(&CPedGroups::ScriptReferenceIndex,savedIndices.data(),16);
  player->m_pReferences=playerHead;other->m_pReferences=otherHead;CReferences::pEmptyList=oldRefHead;globalPool=oldPool;RestoreFP(fp);};
 auto restored=[&]{return savedGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,savedGroups.size())&&savedActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,8)
  &&savedIndices==ReversingProbe::Bytes(&CPedGroups::ScriptReferenceIndex,16)&&originalRefsIntact()&&CReferences::pEmptyList==oldRefHead&&player->m_pReferences==playerHead
  &&pedSlotGuards()&&other->m_pReferences==otherHead&&oldOther==ReversingProbe::Bytes(other,0x7C4)&&oldOtherIntelligence==ReversingProbe::Bytes(otherIntelligence,sizeof(CPedIntelligence))&&globalPool==oldPool&&originalPoolIntact()&&oldPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&groupGuards()&&CaptureFP()==fp;};
 auto referencePartition=[&]{std::array<bool,64> seen{};unsigned used=0,free=0;
  auto walk=[&](CReference* at,bool live){unsigned n=0;while(at){const auto delta=intptr_t(at)-intptr_t(refs);if(delta<0||delta>=64*8||delta%8||++n>64)return false;const auto index=unsigned(delta/8);if(seen[index])return false;seen[index]=true;
    if(live&&(!at->m_ppEntity||(*at->m_ppEntity!=player&&*at->m_ppEntity!=other)))return false;at=at->m_pNext;}if(live)used+=n;else free=n;return true;};
  return walk(player->m_pReferences,true)&&walk(other->m_pReferences,true)&&walk(CReferences::pEmptyList,false)&&used+free==64;};
 alignas(16) std::array<uint8,16+sizeof(CPedGroup)+16> groupArena{};
 groupArena.fill(0x93);const auto cleanGroupArena=groupArena;
 auto* group=reinterpret_cast<CPedGroup*>(groupArena.data()+16);
 auto* intelligence=reinterpret_cast<CPedGroupIntelligence*>(groupArena.data()+16+0x30);
 auto* script=groupArena.data()+16+0x30+0x14C;
 auto groupArenaGuards=[&]{return std::equal(groupArena.begin(),groupArena.begin()+16,cleanGroupArena.begin())&&std::equal(groupArena.end()-16,groupArena.end(),cleanGroupArena.end()-16);};
 pumpStateCheck=[&]{return restored()&&helpersOK();};
 bool ready=player->IsPlayer()&&helpersOK()&&route.Registered()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker))&&restored();
 json helperRows=json::array();for(const auto& h:helpers)helperRows.push_back(h.Metadata());
 RestoreFP(fp);Write({{"event","begin"},{"registered_hook",ready},{"helper_routes_ok",helpersOK()},{"helper_routes",helperRows},{"actor_real_player",player->IsPlayer()},{"second_actor_real",other!=player&&pedPool->IsPtrFromPool(other)},{"actor_addresses",{uintptr_t(player),uintptr_t(other)}},{"fixture_configuration","FastLoader.SaveGameToLoad=0"},{"ped_pool_slot_stride",0x7C4},{"second_actor_slot_index",otherIndex},{"ped_pool_capacity",pedPoolCapacity},{"ped_slot_canary_bytes",otherBefore.size()+otherAfter.size()},{"npc_intelligence_bytes",oldOtherIntelligence.size()},
  {"original_task_pool_capacity",oldTaskCapacity},{"original_task_storage_bytes",oldTaskStorageBytes.size()},{"original_task_bitmap_bytes",oldTaskBitmapBytes.size()},
  {"original_task_pool_intact",originalPoolIntact()},{"private_task_pool_capacity",16},{"private_reference_capacity",64},{"scope","real shared callbacks; no transitive independence claim"}});
 int pair=0;bool passed=ready;int failures=0;
 if(!ready){restore();const bool memory=restored();const bool rh=route.Restore();Write({{"event","error"},{"reason","preflight"},{"pairs",0},{"state_restored",memory&&rh}});RestoreFP(fp);return;}
 for(unsigned scenario=0;scenario<2048;++scenario){
  auto invoke=[&](bool candidate){
   bool oldPoolOK=originalPoolIntact();bool oldGroupsOK=originalGroupsIntact();restore();storage=cleanStorage;bitmap=cleanBitmap;referenceArena=cleanReferenceArena;groupArena=cleanGroupArena;
   memcpy(&ownPool,ownPoolHeader.data(),ownPoolHeader.size());for(unsigned i=0;i<16;++i)bitmap[16+i]=uint8(0x80|((scenario+i)&0x7F));
   for(unsigned i=0;i<64;++i){refs[i].m_pNext=i<63?&refs[i+1]:nullptr;refs[i].m_ppEntity=nullptr;}
   globalPool=&ownPool;CReferences::pEmptyList=refs;player->m_pReferences=nullptr;other->m_pReferences=nullptr;
   memset(group,0,sizeof(CPedGroup));reinterpret_cast<void(__thiscall*)(void*)>(0x5FC150)(group);
   const bool multi=scenario>=1024;const unsigned n=multi?scenario-1024:scenario;
   const int memberIndex=multi?-2:int(n%9)-1;const unsigned oldIndex=multi?n%8:(n/9)%8,mode=multi?(n/8)%8:(n/72)%4;
   std::array<CPed*,8> members{},oldPeds{};std::array<CTask*,8> oldTasks{};
   if(!multi){if(memberIndex>=0)members[memberIndex]=player;if(mode)oldPeds[oldIndex]=player;}
   else {const unsigned j=(oldIndex+3)%8;oldPeds[oldIndex]=player;oldPeds[j]=other;
    switch(mode){
     case 0:members[oldIndex]=player;members[j]=other;break;
     case 1:members[oldIndex]=other;members[j]=player;break;
     case 2:members[(oldIndex+1)%8]=player;members[(j+1)%8]=other;break;
     case 3:members[j]=other;break;
     case 4:members[oldIndex]=player;break;
     case 5:break;
     case 6:members[oldIndex]=player;members[(j+1)%8]=other;break;
     case 7:members[(oldIndex+4)%8]=player;members[(j+4)%8]=other;break;
    }
   }
   for(unsigned i=0;i<8;++i)if(members[i]){auto& slot=*reinterpret_cast<CPed**>(groupArena.data()+16+0xC+i*4);slot=members[i];CEntity::SafeRegisterRef(slot);}
   std::vector<Helper> virtualRoutes;json taskRoutes=json::array();bool allocationOK=true;unsigned acquired=0;
   for(unsigned i=0;i<8;++i){bool needs=oldPeds[i]&&(multi?(!((mode==6&&oldPeds[i]==other)||(mode==7&&oldPeds[i]==player))):mode>=2);if(!needs)continue;
    void* raw=reinterpret_cast<void*(__cdecl*)(size_t)>(0x61A5A0)(sizeof(CTaskSimpleStandStill));
    if(raw){auto* task=reinterpret_cast<CTaskSimpleStandStill*(__thiscall*)(void*,int32,bool,bool,float)>(0x62F310)(raw,int32(scenario*17+i),true,false,8.f);oldTasks[i]=task;++acquired;
     const auto entry=*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(task));virtualRoutes.emplace_back(entry);taskRoutes.push_back(virtualRoutes.back().Metadata());
     allocationOK&=ownPool.IsPtrFromPool(task)&&!ownPool.IsFreeSlotAtIndex(ownPool.GetIndex(task));
    }else allocationOK=false;
   }
   for(unsigned i=0;i<8;++i){auto* p=script+20*i;Put(p,uintptr_t(oldPeds[i]));Put(p+4,uintptr_t(oldTasks[i]));Put(p+8,uintptr_t(int32((scenario+i)%7)-1));Put(p+12,scenario*17+i);p[16]=uint8((scenario+i)&1);}
   std::array<std::vector<uint8>,8> inputTasks{};for(unsigned i=0;i<8;++i)if(oldTasks[i])inputTasks[i]=ReversingProbe::Bytes(oldTasks[i],sizeof(CTaskSimpleStandStill));
   const auto input=ReversingProbe::Bytes(group,sizeof(CPedGroup));auto expected=input;
   unsigned expectedRetained=0;std::array<CTask*,8> expectedTasks{};
   for(unsigned i=0;i<8;++i){const auto member=uintptr_t(members[i]);uintptr_t selectedTask=0;
    for(unsigned j=0;j<8;++j){uintptr_t oldPed{},oldTask{};memcpy(&oldPed,input.data()+0x30+0x14C+20*j,4);memcpy(&oldTask,input.data()+0x30+0x150+20*j,4);
     if(oldPed==member){selectedTask=oldTask;break;}}
    Put(expected.data()+0x30+0x14C+20*i,member);Put(expected.data()+0x30+0x150+20*i,selectedTask);expectedRetained+=selectedTask!=0;expectedTasks[i]=reinterpret_cast<CTask*>(selectedTask);
   }
   bool helperOK=helpersOK(),refOK=referencePartition()&&originalRefsIntact(),routeOK=false;ABI abi{};
   for(const auto& h:virtualRoutes)helperOK&=h.Check();oldPoolOK&=originalPoolIntact();
   if(allocationOK)abi=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x5F7800)==candidate;return Gate(0x5F7800,intelligence);});
   helperOK&=helpersOK();for(const auto& h:virtualRoutes)helperOK&=h.Check();oldPoolOK&=originalPoolIntact();oldGroupsOK&=originalGroupsIntact();refOK&=referencePartition()&&originalRefsIntact();
   const auto actual=ReversingProbe::Bytes(group,sizeof(CPedGroup));const auto taskStates=std::vector<uint8>(bitmap.begin()+16,bitmap.begin()+32);
   const bool contract=allocationOK&&actual==expected&&ownPool.GetNoOfUsedSpaces()==unsigned(expectedRetained);
   bool taskPayload=true;json observedTasks=json::array();
   for(unsigned i=0;i<8;++i)if(expectedTasks[i]){auto* task=expectedTasks[i];auto it=std::find(oldTasks.begin(),oldTasks.end(),task);const unsigned oldSlot=unsigned(it-oldTasks.begin());const auto bytes=ReversingProbe::Bytes(task,sizeof(CTaskSimpleStandStill));taskPayload&=oldSlot<8&&bytes==inputTasks[oldSlot]&&!ownPool.IsFreeSlotAtIndex(ownPool.GetIndex(task));observedTasks.push_back({{"slot",i},{"old_slot",oldSlot},{"bytes",bytes}});}
   const auto actualTaskStorage=ReversingProbe::Bytes(storage.data()+16,16*0x80);
   const bool guards=arenaGuards()&&groupGuards()&&groupArenaGuards();
   for(unsigned i=0;i<8;++i){CTask* remaining{};memcpy(&remaining,script+20*i+4,4);if(remaining)delete remaining;Put(script+20*i,0);Put(script+20*i+4,0);}
   for(unsigned i=0;i<8;++i){auto& slot=*reinterpret_cast<CPed**>(groupArena.data()+16+0xC+i*4);CEntity::SafeCleanUpRef(slot);slot=nullptr;}
   reinterpret_cast<void(__thiscall*)(void*)>(0x5FC190)(group);
   const bool resources=ownPool.GetNoOfUsedSpaces()==0&&player->m_pReferences==nullptr&&other->m_pReferences==nullptr&&referencePartition()&&originalRefsIntact();oldPoolOK&=originalPoolIntact();
   oldGroupsOK&=originalGroupsIntact()&&oldOtherIntelligence==ReversingProbe::Bytes(otherIntelligence,sizeof(CPedIntelligence));const bool postCleanupGuards=arenaGuards()&&groupGuards()&&groupArenaGuards();
   storage=cleanStorage;bitmap=cleanBitmap;referenceArena=cleanReferenceArena;groupArena=cleanGroupArena;memcpy(&ownPool,ownPoolHeader.data(),ownPoolHeader.size());restore();
   return json{{"multi_actor",multi},{"member_index",memberIndex},{"old_index",oldIndex},{"mode",mode},{"input_group",input},{"group",actual},{"task_states",taskStates},{"task_payloads",observedTasks},{"input_task_payloads",inputTasks},{"task_storage",actualTaskStorage},{"task_virtual_routes",taskRoutes},{"tasks_acquired",acquired},{"tasks_remaining",unsigned(expectedRetained)},
    {"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nv},{"seh_restored",abi.seh},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"references_valid",refOK},{"fixture_contract",contract&&taskPayload},{"guards_preserved",guards&&postCleanupGuards},{"resources_balanced",resources},{"original_task_pool_intact",oldPoolOK&&originalPoolIntact()},{"original_groups_intact",oldGroupsOK&&originalGroupsIntact()},{"state_restored",restored()},{"floating_state_restored",CaptureFP()==fp},{"private_arenas_restored",storage==cleanStorage&&bitmap==cleanBitmap&&referenceArena==cleanReferenceArena&&groupArena==cleanGroupArena&&ownPoolHeader==ReversingProbe::Bytes(&ownPool,sizeof(ownPool))}};
  };
  const auto original=invoke(false);const auto candidate=invoke(true);bool ok=original==candidate;
  for(auto k:{"stack_restored","nonvolatile_preserved","seh_restored","route_ok","helper_route_ok","references_valid","fixture_contract","guards_preserved","resources_balanced","original_task_pool_intact","state_restored","floating_state_restored","private_arenas_restored","original_groups_intact"})ok&=original[k]==true&&candidate[k]==true;
  passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function","CPedGroupIntelligence::ComputeScriptCommandTasks"},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});RestoreFP(fp);
 }
 Write({{"event","function_summary"},{"function","CPedGroupIntelligence::ComputeScriptCommandTasks"},{"cases",2048},{"failures",failures}});RestoreFP(fp);
 restore();const bool memory=restored()&&helpersOK();const bool rh=route.Restore();const bool final=memory&&rh&&pumpStateOK;RestoreFP(fp);
 Write({{"event",passed&&final?"complete":"error"},{"pairs",pair},{"state_restored",final},{"original_task_pool_intact",originalPoolIntact()},{"helper_route_ok",helpersOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
