#pragma once
#include <fstream>
#include <map>
#include <cfenv>
#include <xmmintrin.h>
#include <Interior/InteriorGroup_c.h>
#include <Animation/AnimManager.h>
#include <Streaming.h>
#include <ProjectileInfo.h>
#include <Entity/Object/Projectile.h>
#include <Entity/Object/CutsceneObject.h>
#include <Pools/ObjectPool.h>
#include <Pools/Pools.h>
#include <Fx/FxSystem.h>
#include <Radar.h>
#include <World.h>
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedAnimProjectilePair {
using Simple=ReversibleHooks::ReversibleHook::Simple;
static_assert(sizeof(CObjectPool)==0x14);
static_assert(sizeof(CProjectileInfo)==0x24 && sizeof(CProjectile)==0x17C && sizeof(CCutsceneObject)==0x19C);
static_assert(sizeof(InteriorGroup_c)==0xBC && offsetof(InteriorGroup_c,m_animBlockReferenced)==0xBA);
inline json trace;inline bool collaboratorsValid{};inline int animIndex{},expectedGroup{},expectedSlot{},expectedRef{};
inline CProjectileInfo* expectedInfo{};inline CProjectile* expectedProjectile{};inline FxSystem_c* expectedFx{};
inline int __cdecl Lookup(const char* name){const char* names[]{"int_house","int_shop","int_office"};const bool valid=std::string_view(name)==names[expectedGroup];collaboratorsValid&=valid;trace.push_back({{"event","anim_index"},{"name",name},{"valid",valid},{"index",animIndex}});return animIndex;}
inline void __cdecl AddRef(int index){const bool valid=index==animIndex;collaboratorsValid&=valid;trace.push_back({{"event","anim_ref"},{"index",index},{"valid",valid}});}
inline void __cdecl Request(int model,int flags){const bool valid=model==animIndex+25575&&flags==8;collaboratorsValid&=valid;trace.push_back({{"event","request_model"},{"model",model},{"flags",flags},{"valid",valid}});}
inline void __fastcall Kill(FxSystem_c* fx,void*){const bool valid=fx==expectedFx&&!expectedInfo->m_bActive;collaboratorsValid&=valid;trace.push_back({{"event","fx_kill"},{"slot",expectedSlot},{"valid",valid}});}
inline void __cdecl Radar(eBlipType type,int handle){const bool valid=int(type)==3&&handle==expectedRef&&!expectedInfo->m_bActive&&expectedInfo->m_pFxSystem==nullptr;collaboratorsValid&=valid;trace.push_back({{"event","radar_clear"},{"slot",expectedSlot},{"type",int(type)},{"handle",handle},{"valid",valid}});}
inline void __cdecl Remove(CEntity* entity){const bool valid=entity==expectedProjectile;collaboratorsValid&=valid;trace.push_back({{"event","world_remove"},{"slot",expectedSlot},{"valid",valid}});}
inline void* __fastcall ScalarDelete(CProjectile* object,void*,unsigned flags){const bool valid=object==expectedProjectile&&flags==1;collaboratorsValid&=valid;trace.push_back({{"event","scalar_delete"},{"slot",expectedSlot},{"flags",flags},{"valid",valid}});return object;}
inline Simple* Find(uintptr_t address){Simple* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items())if(holder->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* s=static_cast<Simple*>(holder.get());if(uintptr_t(s->GetHookGTAAddress())==address)result=s;}});return result;}
template<class T>inline void Put(void* target,size_t offset,T value){std::memcpy(static_cast<uint8*>(target)+offset,&value,sizeof(value));}
inline void Update(){
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);std::ofstream log(std::filesystem::path(exe).parent_path()/"anim-projectile-pair-runtime.jsonl");auto write=[&](const json& row){log<<row.dump()<<'\n';};
 const auto hookFlags=SmallPendingSupport::SnapshotHooks();auto* animHook=Find(0x5950D0);auto* projectileHook=Find(0x739860);
 if(!animHook||!projectileHook){write({{"event","error"},{"reason","missing target"}});return;}
 std::fenv_t fp{};std::fegetenv(&fp);const auto mxcsr=_mm_getcsr();
 const auto infoBefore=ReversingProbe::Bytes(gaProjectileInfo.data(),sizeof(gaProjectileInfo)),pointersBefore=ReversingProbe::Bytes(CProjectileInfo::ms_apProjectile.data(),sizeof(CProjectileInfo::ms_apProjectile));
 const auto poolPointerBefore=*reinterpret_cast<CObjectPool**>(0xB7449C);
 std::map<uintptr_t,void*> patches{{0x4D3990,reinterpret_cast<void*>(&Lookup)},{uintptr_t(static_cast<int(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),reinterpret_cast<void*>(&Lookup)},{0x4D3FB0,reinterpret_cast<void*>(&AddRef)},{uintptr_t(&CAnimManager::AddAnimBlockRef),reinterpret_cast<void*>(&AddRef)},{0x4087E0,reinterpret_cast<void*>(&Request)},{uintptr_t(&CStreaming::RequestModel),reinterpret_cast<void*>(&Request)},{0x4AA3F0,reinterpret_cast<void*>(&Kill)}};
 // Pointer-to-member Kill has a compiler ABI representation; derive its entry separately, never cast it as a data address.

 std::map<uintptr_t,std::vector<uint8>> patchedBefore;
 auto addEntry=[&](uintptr_t address,void* replacement){patches[address]=replacement;};
 // Candidate member address follows the same explicit representation extraction used by other probe banks.
 const auto killMember=&FxSystem_c::Kill;uintptr_t killCandidate{};static_assert(sizeof(killMember)==sizeof(killCandidate));std::memcpy(&killCandidate,&killMember,sizeof(killCandidate));addEntry(killCandidate,reinterpret_cast<void*>(&Kill));
 addEntry(0x587C60,reinterpret_cast<void*>(&Radar));addEntry(uintptr_t(static_cast<void(*)(eBlipType,int)>(&CRadar::ClearBlipForEntity)),reinterpret_cast<void*>(&Radar));addEntry(0x563280,reinterpret_cast<void*>(&Remove));addEntry(uintptr_t(&CWorld::Remove),reinterpret_cast<void*>(&Remove));
 for(const auto& [address,replacement]:patches)patchedBefore[address]=ReversingProbe::Bytes(reinterpret_cast<void*>(address),52);
 for(const auto& state:hookFlags)if(state.hook->Name()!="NOTSA_WinMain"){state.hook->LockState(false);state.hook->State(false);}
 std::vector<std::pair<uintptr_t,uint8>> loadStates;for(int i:{0,1,127,179}){auto a=uintptr_t(0xB5D4B0+32*i);loadStates.emplace_back(a,*reinterpret_cast<uint8*>(a));}
 size_t pair{},differences{};bool valid=true;
 {
 std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> owned;for(const auto& [address,replacement]:patches)owned.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,replacement));
 auto helperGate=[&](){return std::ranges::all_of(owned,[](const auto& patch){return std::memcmp(reinterpret_cast<void*>(patch->address),patch->jump.data(),patch->jump.size())==0;});};
 for(int group=0;group<3;++group)for(int referenced:{0,1,-1})for(int index:{0,1,127,179})for(int loaded=0;loaded<5;++loaded)for(int poison:{0,85,170,255}){
  animIndex=index;expectedGroup=group;alignas(InteriorGroup_c) std::array<uint8,0xBC> original{},arena{},model{};original.fill(uint8(poison));original[0xE]=uint8(group);original[0xBA]=uint8(referenced);model=original;json modelTrace=json::array();
  if(!referenced){modelTrace.push_back({{"event","anim_index"},{"name",std::array<const char*,3>{"int_house","int_shop","int_office"}[group]},{"valid",true},{"index",index}});if(loaded==1){model[0xBA]=1;modelTrace.push_back({{"event","anim_ref"},{"index",index},{"valid",true}});}else modelTrace.push_back({{"event","request_model"},{"model",index+25575},{"flags",8},{"valid",true}});}
  json results[2];for(int route=0;route<2;++route){arena=original;*reinterpret_cast<uint8*>(0xB5D4B0+32*index)=uint8(loaded);trace=json::array();collaboratorsValid=helperGate();std::fesetenv(&fp);_mm_setcsr(mxcsr);animHook->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*animHook,route!=0);reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(0x5950D0)(reinterpret_cast<InteriorGroup_c*>(arena.data()));results[route]={{"bytes",ReversingProbe::Bytes(arena.data(),arena.size())},{"trace",trace},{"gate",gate},{"collaborators_valid",collaboratorsValid}};animHook->State(false);}
  const bool equal=results[0]==results[1]&&results[0]["bytes"]==ReversingProbe::Bytes(model.data(),model.size())&&results[0]["trace"]==modelTrace&&results[0]["gate"]==true&&results[0]["collaborators_valid"]==true;differences+=!equal;write({{"event","comparison"},{"function","InteriorGroup_c::ReferenceAnims"},{"pair",pair++},{"group",group},{"referenced",referenced},{"index",index},{"loaded",loaded},{"poison",poison},{"original",results[0]},{"candidate",results[1]},{"model_bytes",ReversingProbe::Bytes(model.data(),model.size())},{"model_trace",modelTrace},{"equal",equal}});
 }
 alignas(16) std::array<std::array<uint8,sizeof(CCutsceneObject)>,32> storage{};std::array<uint8,32> states{};CObjectPool privatePool(32,storage.data(),states.data());*reinterpret_cast<CObjectPool**>(0xB7449C)=&privatePool;
 std::array<void*,1> fakeVtable{reinterpret_cast<void*>(&ScalarDelete)};alignas(16) std::array<uint8,0x48> matrix{};alignas(16) std::array<uint8,sizeof(FxSystem_c)> fx{};
 constexpr int weapons[]{0,15,16,17,18,19,20,21,55};const std::array<CVector,13> points{{{0,0,0},{-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1},{-2,0,0},{2,0,0},{0,-2,0},{0,2,0},{0,0,-2},{0,0,2}}};
 for(int slot=0;slot<32;++slot)for(int weapon:weapons)for(bool active:{false,true})for(bool matrixOn:{false,true})for(bool destroy:{false,true})for(bool fxOn:{false,true})for(int position=0;position<13;++position){
  expectedSlot=slot;expectedRef=(slot<<8)|((slot%127)+1);expectedProjectile=reinterpret_cast<CProjectile*>(storage[slot].data());expectedInfo=&gaProjectileInfo[slot];expectedFx=reinterpret_cast<FxSystem_c*>(fx.data());const bool inside=position<7;const bool hit=active&&weapon>=16&&weapon<=20&&inside;json expectedTrace=json::array();if(hit&&destroy){if(fxOn)expectedTrace.push_back({{"event","fx_kill"},{"slot",slot},{"valid",true}});expectedTrace.push_back({{"event","radar_clear"},{"slot",slot},{"type",3},{"handle",expectedRef},{"valid",true}});expectedTrace.push_back({{"event","world_remove"},{"slot",slot},{"valid",true}});expectedTrace.push_back({{"event","scalar_delete"},{"slot",slot},{"flags",1},{"valid",true}});}
  json results[2];for(int route=0;route<2;++route){storage.fill({});states.fill(0x80);states[slot]=uint8((slot%127)+1);matrix.fill(0);Put(matrix.data(),0x30,points[position]);Put(storage[slot].data(),0,fakeVtable.data());Put(storage[slot].data(),4,points[position]);Put(storage[slot].data(),0x14,matrixOn?matrix.data():nullptr);std::memset(gaProjectileInfo.data(),0,sizeof(gaProjectileInfo));CProjectileInfo::ms_apProjectile.fill(nullptr);expectedInfo->m_nWeaponType=weapon;expectedInfo->m_bActive=active;expectedInfo->m_pFxSystem=fxOn?expectedFx:nullptr;CProjectileInfo::ms_apProjectile[slot]=expectedProjectile;
   const auto storageBefore=storage;const auto matrixBefore=matrix;const auto statesBefore=states;const auto poolBefore=ReversingProbe::Bytes(&privatePool,sizeof(privatePool));trace=json::array();collaboratorsValid=helperGate();std::fesetenv(&fp);_mm_setcsr(mxcsr);projectileHook->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*projectileHook,route!=0);const bool result=reinterpret_cast<bool(__cdecl*)(float,float,float,float,float,float,bool)>(0x739860)(-1,1,-1,1,-1,1,destroy);const bool privateUnchanged=storage==storageBefore&&matrix==matrixBefore&&states==statesBefore&&poolBefore==ReversingProbe::Bytes(&privatePool,sizeof(privatePool));results[route]={{"return",result},{"info_bytes",ReversingProbe::Bytes(gaProjectileInfo.data(),sizeof(gaProjectileInfo))},{"pointer_bytes",ReversingProbe::Bytes(CProjectileInfo::ms_apProjectile.data(),sizeof(CProjectileInfo::ms_apProjectile))},{"projectile_bytes",ReversingProbe::Bytes(storage[slot].data(),storage[slot].size())},{"matrix_bytes",ReversingProbe::Bytes(matrix.data(),matrix.size())},{"states",states},{"pool_bytes",poolBefore},{"trace",trace},{"gate",gate},{"collaborators_valid",collaboratorsValid},{"private_unchanged",privateUnchanged}};projectileHook->State(false);
  }
  std::array<CProjectileInfo,32> model{};model[slot].m_nWeaponType=weapon;model[slot].m_bActive=active&&!(hit&&destroy);model[slot].m_pFxSystem=fxOn&&!(hit&&destroy)?expectedFx:nullptr;
  const bool equal=results[0]==results[1]&&results[0]["return"]==hit&&results[0]["info_bytes"]==ReversingProbe::Bytes(model.data(),sizeof(model))&&results[0]["trace"]==expectedTrace&&results[0]["gate"]==true&&results[0]["collaborators_valid"]==true&&results[0]["private_unchanged"]==true;differences+=!equal;write({{"event","comparison"},{"function","CProjectileInfo::IsProjectileInRange"},{"pair",pair++},{"slot",slot},{"weapon",weapon},{"active",active},{"matrix",matrixOn},{"destroy",destroy},{"fx",fxOn},{"position",position},{"original",results[0]},{"candidate",results[1]},{"model_info_bytes",ReversingProbe::Bytes(model.data(),sizeof(model))},{"model_trace",expectedTrace},{"model_return",hit},{"equal",equal}});if(pair%128==0)log.flush();ReversingProbe::PumpMessages();
 }
 states.fill(0x80);*reinterpret_cast<CObjectPool**>(0xB7449C)=poolPointerBefore;
 }
 for(const auto& entry:loadStates){*reinterpret_cast<uint8*>(entry.first)=entry.second;}std::memcpy(gaProjectileInfo.data(),infoBefore.data(),infoBefore.size());std::memcpy(CProjectileInfo::ms_apProjectile.data(),pointersBefore.data(),pointersBefore.size());*reinterpret_cast<CObjectPool**>(0xB7449C)=poolPointerBefore;std::fesetenv(&fp);_mm_setcsr(mxcsr);
 for(const auto& state:hookFlags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
 bool restored=SmallPendingSupport::HooksUnchanged(hookFlags)&&infoBefore==ReversingProbe::Bytes(gaProjectileInfo.data(),sizeof(gaProjectileInfo))&&pointersBefore==ReversingProbe::Bytes(CProjectileInfo::ms_apProjectile.data(),sizeof(CProjectileInfo::ms_apProjectile))&&*reinterpret_cast<CObjectPool**>(0xB7449C)==poolPointerBefore;
 for(const auto& [address,bytes]:patchedBefore)restored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(address),bytes.size());for(const auto& entry:loadStates){restored&=*reinterpret_cast<uint8*>(entry.first)==entry.second;}std::fenv_t after{};std::fegetenv(&after);restored&=std::memcmp(&fp,&after,sizeof(fp))==0&&_mm_getcsr()==mxcsr;
 write({{"event","complete"},{"pairs",pair},{"differences",differences},{"state_restored",restored},{"scope","finite private projectile/animation fields and traced isolated collaborators; no real object lifecycle"}});log.flush();
}
}
