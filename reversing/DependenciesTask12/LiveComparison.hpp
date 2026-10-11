#pragma once
#include <fstream>
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Tasks/TaskTypes/TaskSimpleSlideToCoord.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendStaticAssociation.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Animation/AnimBlendNode.h"
#include "Animation/AnimBlendAssocGroup.h"
#include "Animation/AnimBlock.h"
#include "Pools/TaskPool.h"
#include "Reference.h"
#include "References.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesTask12Live {
inline bool done{};inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address;int reserved,slot,count;};
inline constexpr Fn fns[]{{"CTaskSimpleFight","Destructor",0x61C530,0,-1,2048}};
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
struct ABI {uint32 eax,ecx,edx;bool stack,nv,seh;};
inline ABI Gate(uintptr_t entry,void* object,uint32 seed){uint32 a{},c{},d{},sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};
 __asm {
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  mov eax,seed
  mov ecx,object
  mov edx,0C0DE5921h
  call entry
  mov a,eax
  mov c,ecx
  mov d,edx
  mov sp1,esp
  mov b1,ebx
  mov s1,esi
  mov d1,edi
  mov p1,ebp
  mov eax,fs:[0]
  mov h1,eax
 }
 return {a,c,d,sp0==sp1,b0==b1&&s0==s1&&d0==d1&&p0==p1,h0==h1};}


inline void Put32(std::vector<uint8>& b,unsigned at,uint32 v){std::memcpy(b.data()+at,&v,4);}
inline uintptr_t RegisteredBody(){uintptr_t out{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())if(h->Name()=="Destructor"&&h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* s=static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());if(uintptr_t(s->GetHookGTAAddress())==0x61C530)out=uintptr_t(s->GetHookOurAddress());}});return out;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciestask12-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciestask12-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciestask12");}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;const auto fp=CaptureFP();
 const auto body=RegisteredBody();if(!body){Write({{"event","error"},{"reason","Production destructor registration required"},{"pairs",0},{"state_restored",true}});RestoreFP(fp);return;}
 OriginalDependencies499::HookRoute route("CTaskSimpleFight","Destructor",0x61C530,reinterpret_cast<void(__cdecl*)()>(body));
 auto*& globalPool=*reinterpret_cast<CTaskPool**>(0xB744A8);auto* oldPool=globalPool;
 if(!oldPool){route.Restore();RestoreFP(fp);Write({{"event","error"},{"reason","Original task pool missing"},{"pairs",0},{"state_restored",true}});RestoreFP(fp);return;}
 const auto oldPoolHeader=ReversingProbe::Bytes(oldPool,20);void* oldStorage{};void* oldBitmap{};std::memcpy(&oldStorage,oldPoolHeader.data(),4);std::memcpy(&oldBitmap,oldPoolHeader.data()+4,4);const auto capacity=oldPool->GetSize();
 auto* groups=*reinterpret_cast<CAnimBlendAssocGroup**>(0xB4EA34);const auto groupCount=*reinterpret_cast<uint32*>(0xB4EA28);
 if(!groups||!groupCount||groupCount>200||!groups[0].m_Anims||groups[0].m_GroupID!=ANIM_GROUP_DEFAULT||groups[0].m_IdOffset>3||3-groups[0].m_IdOffset>=groups[0].m_NumAnims){route.Restore();RestoreFP(fp);Write({{"event","error"},{"reason","Actual animation catalog unavailable"},{"pairs",0},{"state_restored",true}});RestoreFP(fp);return;}
 auto* animation=plugin::CallAndReturn<CAnimBlendStaticAssociation*,0x4D3A60,int,int>(0,3);
 const bool defaultIdle=animation==groups[0].m_Anims+(3-groups[0].m_IdOffset)&&static_cast<AnimationId>(animation->m_AnimId)==ANIM_ID_IDLE&&static_cast<AssocGroupId>(animation->m_AnimGroupId)==ANIM_GROUP_DEFAULT;
 const auto blockIndex=animation&&animation->m_BlendHier?animation->m_BlendHier->m_nAnimBlockId:-1;
 auto* blocks=reinterpret_cast<CAnimBlock*>(0xB5D4A0);
 if(!oldStorage||!oldBitmap||!capacity||capacity>4096||!groups||!groupCount||groupCount>200||!animation||!defaultIdle||!animation->m_BlendSeqs||(animation->m_Flags&ANIMATION_REFERENCE_BLOCK)||!animation->m_BlendHier||!animation->m_NumBlendNodes||animation->m_NumBlendNodes>256||blockIndex<0||blockIndex>=180||!blocks[blockIndex].IsLoaded||blocks[blockIndex].RefCnt<0||blocks[blockIndex].RefCnt>32760||groups[0].m_AnimBlock!=&blocks[blockIndex]){
  route.Restore();RestoreFP(fp);Write({{"event","error"},{"reason","Real loaded animation/private-pool domain unavailable"},{"pairs",0},{"state_restored",true}});RestoreFP(fp);return;
 }
 const auto oldTaskStorage=ReversingProbe::Bytes(oldStorage,capacity*0x80);const auto oldTaskBitmap=ReversingProbe::Bytes(oldBitmap,capacity);
 const auto oldPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));auto* playerHead=player->m_pReferences;const auto oldRefs=ReversingProbe::Bytes(&CReferences::aRefs,sizeof(CReferences::aRefs));auto* oldHead=CReferences::pEmptyList;
 const auto oldBlocks=ReversingProbe::Bytes(blocks,180*0x20);const auto oldGroups=ReversingProbe::Bytes(groups,groupCount*sizeof(CAnimBlendAssocGroup));
 const auto oldDefinition=ReversingProbe::Bytes(animation,sizeof(*animation));const auto oldHierarchy=ReversingProbe::Bytes(animation->m_BlendHier,sizeof(CAnimBlendHierarchy));const auto oldSequences=ReversingProbe::Bytes(animation->m_BlendSeqs,animation->m_NumBlendNodes*4);
 alignas(16) std::array<uint8,16+4*0x80+16> storage{};alignas(16) std::array<uint8,36> bitmap{};alignas(16) std::array<uint8,16+16*sizeof(CReference)+16> refArena{};
 CTaskPool ownPool(4,storage.data()+16,bitmap.data()+16);const auto ownHeader=ReversingProbe::Bytes(&ownPool,sizeof(ownPool));storage.fill(0xA5);bitmap.fill(0xD6);refArena.fill(0xB7);const auto cleanStorage=storage;const auto cleanBitmap=bitmap;const auto cleanRefs=refArena;auto* refs=reinterpret_cast<CReference*>(refArena.data()+16);
 std::vector<Helper> helpers;for(auto at:{0x61C470u,0x61A390u,0x61A3A0u,0x571A00u,0x571B70u,0x61A5A0u,0x61A5B0u,0x4CF080u,0x4CEEC0u,0x4CE9F0u,0x4CECF0u,0x4CEBC0u,0x4CEBE0u,0x4D3FB0u,0x4D3FD0u,0x4D3A60u,0x82119Au,0x8214BDu,0x72F4C0u,0x72F4F0u})helpers.emplace_back(at);
 auto helpersOK=[&]{for(auto& h:helpers)if(!h.Check())return false;return true;};
 auto originalPoolIntact=[&]{return oldPoolHeader==ReversingProbe::Bytes(oldPool,20)&&oldTaskStorage==ReversingProbe::Bytes(oldStorage,capacity*0x80)&&oldTaskBitmap==ReversingProbe::Bytes(oldBitmap,capacity);};
 auto animationsIntact=[&]{return *reinterpret_cast<CAnimBlendAssocGroup**>(0xB4EA34)==groups&&*reinterpret_cast<uint32*>(0xB4EA28)==groupCount&&oldGroups==ReversingProbe::Bytes(groups,oldGroups.size())&&oldDefinition==ReversingProbe::Bytes(animation,sizeof(*animation))&&oldHierarchy==ReversingProbe::Bytes(animation->m_BlendHier,sizeof(CAnimBlendHierarchy))&&oldSequences==ReversingProbe::Bytes(animation->m_BlendSeqs,oldSequences.size());};
 auto originalRefsIntact=[&]{return oldRefs==ReversingProbe::Bytes(&CReferences::aRefs,sizeof(CReferences::aRefs));};
 auto restore=[&]{player->m_pReferences=playerHead;CReferences::pEmptyList=oldHead;globalPool=oldPool;RestoreFP(fp);};
 auto restored=[&]{return globalPool==oldPool&&originalPoolIntact()&&originalRefsIntact()&&player->m_pReferences==playerHead&&CReferences::pEmptyList==oldHead&&oldPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&oldBlocks==ReversingProbe::Bytes(blocks,180*0x20)&&animationsIntact();};
 auto guards=[&]{return std::equal(storage.begin(),storage.begin()+16,cleanStorage.begin())&&std::equal(storage.end()-16,storage.end(),cleanStorage.end()-16)&&std::equal(bitmap.begin(),bitmap.begin()+16,cleanBitmap.begin())&&std::equal(bitmap.end()-16,bitmap.end(),cleanBitmap.end()-16)&&std::equal(refArena.begin(),refArena.begin()+16,cleanRefs.begin())&&std::equal(refArena.end()-16,refArena.end(),cleanRefs.end()-16);};
 auto partition=[&]{std::array<bool,16> seen{};unsigned total=0;auto walk=[&](CReference* p,bool live){unsigned count=0;while(p){auto delta=intptr_t(p)-intptr_t(refs);if(delta<0||delta>=16*8||delta%8||++count>16||seen[delta/8])return false;seen[delta/8]=true;++total;if(live&&(!p->m_ppEntity||*p->m_ppEntity!=player))return false;p=p->m_pNext;}return true;};return walk(player->m_pReferences,true)&&walk(CReferences::pEmptyList,false)&&total==16;};
 json hs=json::array();for(auto& h:helpers)hs.push_back(h.Metadata());RestoreFP(fp);
 Write({{"event","begin"},{"registered_hook",route.Registered()},{"helper_routes",hs},{"helper_routes_ok",helpersOK()},{"real_animation",true},{"default_idle_identity",defaultIdle},{"animation_nodes",animation->m_NumBlendNodes},{"animation_block",blockIndex},{"original_task_pool_capacity",capacity},{"original_task_storage_bytes",oldTaskStorage.size()},{"original_task_bitmap_bytes",oldTaskBitmap.size()},{"private_task_capacity",4},{"private_reference_capacity",16},{"scope","Complete Fight destructor; actual loaded static animation and shared collaborators; normal SEH path only"}});RestoreFP(fp);
 bool passed=route.Registered()&&helpersOK()&&restored();int failures=0;
 for(unsigned scenario=0;scenario<2048;++scenario){
  auto invoke=[&](bool candidate){restore();storage=cleanStorage;bitmap=cleanBitmap;refArena=cleanRefs;std::memcpy(&ownPool,ownHeader.data(),20);for(unsigned i=0;i<4;++i)bitmap[16+i]=uint8(0x80|((scenario+i)&127));for(unsigned i=0;i<16;++i){refs[i].m_pNext=i<15?refs+i+1:nullptr;refs[i].m_ppEntity=nullptr;}
   globalPool=&ownPool;player->m_pReferences=nullptr;CReferences::pEmptyList=refs;
   const bool target=(scenario&1)!=0;const auto mask=(scenario/2)%4;const auto refMode=(scenario/8)%4;const bool release=refMode>=2;const auto seed=scenario/32;
   auto* task=plugin::CallAndReturn<CTaskSimpleFight*,0x61A5A0,uint32>(sizeof(CTaskSimpleFight));bool taskConstructed=false;
   std::array<CAnimBlendAssociation*,2> assocs{};std::array<std::vector<uint8>,2> beforeAssoc,beforeNodes;
   auto fail=[&](const char* why){
    if(taskConstructed)plugin::CallMethod<0x61C530,CTaskSimpleFight*>(task); // RH is OFF between invocations; original cleanup of constructor-owned references.
    for(auto* assoc:assocs)if(assoc){plugin::CallMethod<0x4CECF0,CAnimBlendAssociation*>(assoc);plugin::Call<0x8214BD,void*>(assoc);}
    if(task)plugin::Call<0x61A5B0,void*>(task);
    const bool balanced=ownPool.GetNoOfUsedSpaces()==0&&player->m_pReferences==nullptr&&partition()&&oldBlocks==ReversingProbe::Bytes(blocks,180*0x20);
    storage=cleanStorage;bitmap=cleanBitmap;refArena=cleanRefs;std::memcpy(&ownPool,ownHeader.data(),20);restore();
    return json{{"error",why},{"stack_restored",false},{"nonvolatile_preserved",false},{"seh_restored",false},{"route_ok",false},{"helper_route_ok",helpersOK()},{"fixture_contract",false},{"guards_preserved",false},{"resources_balanced",balanced},{"original_memory_intact",originalPoolIntact()&&originalRefsIntact()&&animationsIntact()},{"state_restored",restored()},{"floating_state_restored",CaptureFP()==fp}};
   };
   if(!task)return fail("Actual task allocation returned NULL");
   std::memset(task,uint8(0x30+seed),sizeof(*task));plugin::CallMethod<0x61C470,CTaskSimpleFight*,CEntity*,int32,uint32>(task,target?player:nullptr,int32(seed),1000+seed);taskConstructed=true;
   for(unsigned i=0;i<2;++i)if(mask&(1u<<i)){auto* raw=plugin::CallAndReturn<void*,0x82119A,uint32>(sizeof(CAnimBlendAssociation));if(!raw)return fail("Actual association allocation returned NULL");std::memset(raw,uint8(0x80+seed),sizeof(CAnimBlendAssociation));auto* assoc=static_cast<CAnimBlendAssociation*>(raw);plugin::CallMethod<0x4CF080,CAnimBlendAssociation*,CAnimBlendStaticAssociation*>(assoc,animation);assocs[i]=assoc;if(!assoc->m_BlendNodes||assoc->m_NumBlendNodes!=animation->m_NumBlendNodes||assoc->m_BlendHier!=animation->m_BlendHier)return fail("Actual association constructor domain failed");assoc->SetFinishCallback(reinterpret_cast<void(*)(CAnimBlendAssociation*,void*)>(0x4D1390),task);for(unsigned n=0;n<assoc->m_NumBlendNodes;++n){const uint32 theta=0x3F800000|seed,inv=0x3F000000|seed;std::memcpy(&assoc->m_BlendNodes[n].m_Theta,&theta,4);std::memcpy(&assoc->m_BlendNodes[n].m_InvSinTheta,&inv,4);}
    assocs[i]=assoc;beforeAssoc[i]=ReversingProbe::Bytes(assoc,sizeof(*assoc));beforeNodes[i]=ReversingProbe::Bytes(assoc->m_BlendNodes,assoc->m_NumBlendNodes*sizeof(CAnimBlendNode));}
   task->m_pAnim=assocs[0];task->m_pIdleAnim=assocs[1];task->m_bAnimsReferenced=refMode!=0;task->m_nRequiredAnimGroup=release?ANIM_GROUP_DEFAULT:AssocGroupId(33);if(release)plugin::Call<0x4D3FB0,int32>(blockIndex);
   const auto inputTask=ReversingProbe::Bytes(task,sizeof(*task));const auto beforeBlocks=ReversingProbe::Bytes(blocks,180*0x20);bool helperOK=helpersOK(),oldOK=originalPoolIntact()&&originalRefsIntact()&&animationsIntact();bool routeOK=false;
   RestoreFP(fp);const auto abi=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x61C530)==candidate;return Gate(0x61C530,task,0xA5000000|scenario);});
   helperOK&=helpersOK();oldOK&=originalPoolIntact()&&originalRefsIntact()&&animationsIntact();const auto actualTask=ReversingProbe::Bytes(task,sizeof(*task));auto expectedTask=inputTask;Put32(expectedTask,0,0x86D48C);if(release)expectedTask[10]=0;
   auto expectedBlocks=beforeBlocks;if(release){int16 n{};std::memcpy(&n,expectedBlocks.data()+blockIndex*32+18,2);--n;std::memcpy(expectedBlocks.data()+blockIndex*32+18,&n,2);}
   bool contract=actualTask==expectedTask&&expectedBlocks==ReversingProbe::Bytes(blocks,180*0x20)&&player->m_pReferences==nullptr&&partition();auto normalizedTask=actualTask;Put32(normalizedTask,28,assocs[0]?1:0);Put32(normalizedTask,32,assocs[1]?2:0);
   json associationStates=json::array();for(unsigned i=0;i<2;++i)if(assocs[i]){const auto actual=ReversingProbe::Bytes(assocs[i],sizeof(CAnimBlendAssociation));auto expected=beforeAssoc[i];Put32(expected,48,2);Put32(expected,52,0x4D1390);Put32(expected,56,0);const auto nodes=ReversingProbe::Bytes(assocs[i]->m_BlendNodes,assocs[i]->m_NumBlendNodes*sizeof(CAnimBlendNode));contract&=actual==expected&&nodes==beforeNodes[i];auto normalized=actual;Put32(normalized,16,1);auto normalizedNodes=nodes;for(unsigned n=0;n<assocs[i]->m_NumBlendNodes;++n)Put32(normalizedNodes,n*24+20,1);associationStates.push_back({{"slot",i},{"bytes",normalized},{"nodes",normalizedNodes},{"callback_type",2},{"callback",0x4D1390},{"callback_data_null",assocs[i]->m_pCallbackData==nullptr}});}
   const bool guardOK=guards();for(auto* assoc:assocs)if(assoc){plugin::CallMethod<0x4CECF0,CAnimBlendAssociation*>(assoc);plugin::Call<0x8214BD,void*>(assoc);}
   plugin::Call<0x61A5B0,void*>(task);const bool resources=ownPool.GetNoOfUsedSpaces()==0&&player->m_pReferences==nullptr&&partition()&&oldBlocks==ReversingProbe::Bytes(blocks,180*0x20)&&animationsIntact();helperOK&=helpersOK();oldOK&=originalPoolIntact()&&originalRefsIntact();storage=cleanStorage;bitmap=cleanBitmap;refArena=cleanRefs;std::memcpy(&ownPool,ownHeader.data(),20);restore();
   return json{{"target",target},{"association_mask",mask},{"reference_mode",refMode},{"seed",seed},{"task_bytes",normalizedTask},{"associations",associationStates},{"animation_reference_consumed",release},{"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nv},{"seh_restored",abi.seh},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"fixture_contract",contract},{"guards_preserved",guardOK},{"resources_balanced",resources},{"original_memory_intact",oldOK},{"state_restored",restored()},{"floating_state_restored",CaptureFP()==fp}};
  };
  const auto original=invoke(false);const auto candidate=invoke(true);bool ok=original==candidate;for(auto k:{"stack_restored","nonvolatile_preserved","seh_restored","route_ok","helper_route_ok","fixture_contract","guards_preserved","resources_balanced","original_memory_intact","state_restored","floating_state_restored"})ok&=original[k]==true&&candidate[k]==true;passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"function","CTaskSimpleFight::Destructor"},{"pair",scenario},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});RestoreFP(fp);
 }
 Write({{"event","function_summary"},{"function","CTaskSimpleFight::Destructor"},{"cases",2048},{"failures",failures}});restore();const bool finalMemory=restored(),finalHelpers=helpersOK();const bool finalRoute=route.Restore();const bool final=finalMemory&&finalHelpers&&finalRoute;RestoreFP(fp);Write({{"event",passed&&final?"complete":"error"},{"pairs",2048},{"state_restored",final},{"helper_route_ok",helpersOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
