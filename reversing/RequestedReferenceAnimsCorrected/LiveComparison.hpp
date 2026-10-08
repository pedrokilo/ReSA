#pragma once
#include <fstream>
#include <map>
#include <cfenv>
#include <xmmintrin.h>
#include <Interior/InteriorGroup_c.h>
#include <Animation/AnimManager.h>
#include <Streaming.h>
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedReferenceAnimsCorrected {
using Simple=ReversibleHooks::ReversibleHook::Simple;
static_assert(sizeof(InteriorGroup_c)==0xBC && offsetof(InteriorGroup_c,m_animBlockReferenced)==0xBA);
inline json trace;inline bool collaboratorsValid{};inline int animIndex{},expectedGroup{};

inline int __cdecl Lookup(const char* name){const char* names[]{"int_house","int_shop","int_office"};const bool valid=std::string_view(name)==names[expectedGroup];collaboratorsValid&=valid;trace.push_back({{"event","anim_index"},{"name",name},{"valid",valid},{"index",animIndex}});return animIndex;}
inline void __cdecl AddRef(int index){const bool valid=index==animIndex;collaboratorsValid&=valid;trace.push_back({{"event","anim_ref"},{"index",index},{"valid",valid}});}
inline void __cdecl Request(int model,int flags){const bool valid=model==animIndex+25575&&flags==8;collaboratorsValid&=valid;trace.push_back({{"event","request_model"},{"model",model},{"flags",flags},{"valid",valid}});}
inline Simple* Find(uintptr_t address){Simple* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items())if(holder->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* s=static_cast<Simple*>(holder.get());if(uintptr_t(s->GetHookGTAAddress())==address)result=s;}});return result;}
template<class T>inline void Put(void* target,size_t offset,T value){std::memcpy(static_cast<uint8*>(target)+offset,&value,sizeof(value));}
inline void Update(){
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);std::ofstream log(std::filesystem::path(exe).parent_path()/"reference-anims-corrected-runtime.jsonl");auto write=[&](const json& row){log<<row.dump()<<'\n';};
 const auto flags=SmallPendingSupport::SnapshotHooks();auto* animHook=Find(0x5950D0);if(!animHook){write({{"event","error"},{"reason","missing hook"}});return;}
 std::fenv_t fp{};std::fegetenv(&fp);const auto mxcsr=_mm_getcsr();
 std::map<uintptr_t,void*> patches{{0x4D3990,reinterpret_cast<void*>(&Lookup)},{uintptr_t(static_cast<int(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),reinterpret_cast<void*>(&Lookup)},{0x4D3FB0,reinterpret_cast<void*>(&AddRef)},{uintptr_t(&CAnimManager::AddAnimBlockRef),reinterpret_cast<void*>(&AddRef)},{0x4087E0,reinterpret_cast<void*>(&Request)},{uintptr_t(&CStreaming::RequestModel),reinterpret_cast<void*>(&Request)}};
 std::map<uintptr_t,std::vector<uint8>> patchBefore;for(const auto& [address,replacement]:patches)patchBefore[address]=ReversingProbe::Bytes(reinterpret_cast<void*>(address),52);
 for(const auto& state:flags)if(state.hook->Name()!="NOTSA_WinMain"){state.hook->LockState(false);state.hook->State(false);}
 std::vector<std::pair<uintptr_t,uint8>> loadStates;for(int index:{0,1,127,179}){auto at=uintptr_t(0xB5D4B0+32*index);loadStates.emplace_back(at,*reinterpret_cast<uint8*>(at));}
 size_t pair{},differences{};
 {
 std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> owned;for(const auto& [address,replacement]:patches)owned.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,replacement));
 auto helperGate=[&](){return std::ranges::all_of(owned,[](const auto& patch){return std::memcmp(reinterpret_cast<void*>(patch->address),patch->jump.data(),5)==0;});};
 for(int group=0;group<3;++group)for(int referenced:{0,1,-1})for(int index:{0,1,127,179})for(int loaded=0;loaded<5;++loaded)for(int poison:{0,85,170,255}){
  animIndex=index;expectedGroup=group;alignas(InteriorGroup_c) std::array<uint8,0xBC> original{},arena{},model{};original.fill(uint8(poison));original[0xE]=uint8(group);original[0xBA]=uint8(referenced);model=original;json modelTrace=json::array();
  if(!referenced){modelTrace.push_back({{"event","anim_index"},{"name",std::array<const char*,3>{"int_house","int_shop","int_office"}[group]},{"valid",true},{"index",index}});if(loaded==1){model[0xBA]=1;modelTrace.push_back({{"event","anim_ref"},{"index",index},{"valid",true}});}else modelTrace.push_back({{"event","request_model"},{"model",index+25575},{"flags",8},{"valid",true}});}
  json results[2];for(int route=0;route<2;++route){arena=original;*reinterpret_cast<uint8*>(0xB5D4B0+32*index)=uint8(loaded);trace=json::array();collaboratorsValid=helperGate();std::fesetenv(&fp);_mm_setcsr(mxcsr);animHook->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*animHook,route!=0);reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(0x5950D0)(reinterpret_cast<InteriorGroup_c*>(arena.data()));results[route]={{"bytes",ReversingProbe::Bytes(arena.data(),arena.size())},{"trace",trace},{"gate",gate},{"collaborators_valid",collaboratorsValid}};animHook->State(false);}
  const bool equal=results[0]==results[1]&&results[0]["bytes"]==ReversingProbe::Bytes(model.data(),model.size())&&results[0]["trace"]==modelTrace&&results[0]["gate"]==true&&results[0]["collaborators_valid"]==true;differences+=!equal;write({{"event","comparison"},{"function","InteriorGroup_c::ReferenceAnims"},{"pair",pair++},{"group",group},{"referenced",referenced},{"index",index},{"loaded",loaded},{"poison",poison},{"original",results[0]},{"candidate",results[1]},{"model_bytes",ReversingProbe::Bytes(model.data(),model.size())},{"model_trace",modelTrace},{"equal",equal}});
 }

 }
 for(const auto& entry:loadStates)*reinterpret_cast<uint8*>(entry.first)=entry.second;
 std::fesetenv(&fp);_mm_setcsr(mxcsr);for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
 bool restored=SmallPendingSupport::HooksUnchanged(flags);for(const auto& [address,bytes]:patchBefore)restored &= bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(address),bytes.size());for(const auto& entry:loadStates)restored &= *reinterpret_cast<uint8*>(entry.first)==entry.second;
 std::fenv_t after{};std::fegetenv(&after);restored &= std::memcmp(&fp,&after,sizeof(fp))==0&&_mm_getcsr()==mxcsr;
 write({{"event","complete"},{"pairs",pair},{"differences",differences},{"state_restored",restored},{"scope","720 identical original-reference fixtures; raw byte flag0..4, isolated index/ref/request traces, no streaming/model lifecycle accreditation"}});log.flush();
}
}
