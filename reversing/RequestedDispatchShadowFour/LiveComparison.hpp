#pragma once
#include <bit>
#include <cfenv>
#include <float.h>
#include <xmmintrin.h>
#include <fstream>
#include <map>
#include "CustomBuildingRenderer.h"
#include "CustomBuildingPipeline.h"
#include "CustomBuildingDNPipeline.h"
#include "RealTimeShadowManager.h"
#include "RealTimeShadow.h"
#include "Entity/Physical.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
#include <reversiblehooks/ReversibleHook/Virtual.h>
namespace RequestedDispatchShadowFour {
using Simple=ReversibleHooks::ReversibleHook::Simple;
static_assert(sizeof(CRealTimeShadowManager)==0x54);
static_assert(sizeof(CRealTimeShadow)==0x4C);
static_assert(sizeof(CPhysical)==0x138);
static_assert(std::is_same_v<decltype(&CCustomBuildingPipeline::CreatePipe),bool(__cdecl*)()>);
static_assert(std::is_same_v<decltype(&CCustomBuildingDNPipeline::CreatePipe),bool(__cdecl*)()>);
static_assert(std::is_same_v<decltype(&CCustomBuildingDNPipeline::ExtraVertColourPluginAttach),bool(__cdecl*)()>);
inline json trace;
inline bool normalStatus{},dnStatus{},pluginStatus{};
inline std::array<uint8,128> payload{};
inline bool identities=true;
inline json Call(int role,bool status){return {{"role",role},{"status",status},{"token",uint32(payload[64+role])}};}
inline void Side(int role){payload[role]=uint8(payload[role]+role+1);}
inline int32 __cdecl NormalExe(){trace.push_back(Call(0,normalStatus));Side(0);return normalStatus?1:0;}
inline bool __cdecl NormalDll(){trace.push_back(Call(0,normalStatus));Side(0);return normalStatus;}
inline int32 __cdecl DNExe(){trace.push_back(Call(1,dnStatus));Side(1);return dnStatus?1:0;}
inline bool __cdecl DNDll(){trace.push_back(Call(1,dnStatus));Side(1);return dnStatus;}
inline void __cdecl DestroyNormal(){trace.push_back(Call(2,false));Side(2);}
inline void __cdecl DestroyDN(){trace.push_back(Call(3,false));Side(3);}
inline int32 __cdecl PluginExe(){trace.push_back(Call(4,pluginStatus));Side(4);return pluginStatus?1:0;}
inline bool __cdecl PluginDll(){trace.push_back(Call(4,pluginStatus));Side(4);return pluginStatus;}
// Raw AL is the target's bool ABI. Do not widen, truth-normalize or compare EAX residue.
__declspec(noinline) inline uint32 RawAL(uintptr_t fn){
 uint32 result;
 __asm {
  mov eax, fn
  call eax
  movzx eax, al
  mov result, eax
 }
 return result;
}
template<class T> struct Guarded {alignas(T) std::array<uint8,16> before;alignas(T) std::array<uint8,sizeof(T)> data;std::array<uint8,16> after;void Fill(uint8 v){before.fill(0xA5);data.fill(v);after.fill(0x5A);}T* Get(){return reinterpret_cast<T*>(data.data());}bool Guards()const{return std::ranges::all_of(before,[](uint8 b){return b==0xA5;})&&std::ranges::all_of(after,[](uint8 b){return b==0x5A;});}};
inline void Update(){
 static bool done{};if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dispatch-shadow-four-runtime.start"))return;done=true;
 std::ofstream log(dir/"dispatch-shadow-four-runtime.jsonl");auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
 const auto flags=SmallPendingSupport::SnapshotHooks();std::map<std::string,Simple*> targets;std::vector<std::pair<void*,std::vector<uint8>>> entries;
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items()){auto* base=holder.get();void* gta{};void* ours{};using namespace ReversibleHooks::ReversibleHook;
  if(base->Type()==Base::HookType::Simple){auto* h=static_cast<Simple*>(base);gta=h->GetHookGTAAddress();ours=h->GetHookOurAddress();targets[std::string(cat.Name())+"::"+h->Name()]=h;}
  else if(base->Type()==Base::HookType::Virtual){auto* h=static_cast<Virtual*>(base);gta=h->GetHookGTAAddress();ours=h->GetHookOurAddress();}if(gta)entries.emplace_back(gta,ReversingProbe::Bytes(gta,52));if(ours)entries.emplace_back(ours,ReversingProbe::Bytes(ours,52));}});
 const std::array<const char*,4> keys{"CCustomBuildingRenderer::Initialise","CCustomBuildingRenderer::Shutdown","CCustomBuildingRenderer::PluginAttach","CRealTimeShadowManager::ReturnRealTimeShadow"};const std::array<uintptr_t,4> addresses{0x5D7EC0,0x5D7EE0,0x5D7EF0,0x705B30};
 for(size_t n=0;n<4;++n)if(!targets.contains(keys[n])||uintptr_t(targets.at(keys[n])->GetHookGTAAddress())!=addresses[n]){write({{"event","error"},{"reason","missing or wrong target"},{"function",keys[n]}});return;}
 std::vector<std::pair<uintptr_t,std::vector<uint8>>> globals;for(uintptr_t at:{uintptr_t(0xC02C1C),uintptr_t(0xC02C68),uintptr_t(0x8D12BC)})globals.emplace_back(at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),4));
 const auto initialPayload=payload;std::fenv_t fpBefore{};const bool gotFp=std::fegetenv(&fpBefore)==0;uint16 statusBefore{};__asm fnstsw statusBefore
 unsigned controlBefore{};_controlfp_s(&controlBefore,0,0);const auto mxcsrBefore=_mm_getcsr();
 std::vector<std::pair<uintptr_t,void*>> patches{
 {0x5D7D90,reinterpret_cast<void*>(&NormalExe)},{std::bit_cast<uintptr_t>(&CCustomBuildingPipeline::CreatePipe),reinterpret_cast<void*>(&NormalDll)},
 {0x5D7100,reinterpret_cast<void*>(&DNExe)},{std::bit_cast<uintptr_t>(&CCustomBuildingDNPipeline::CreatePipe),reinterpret_cast<void*>(&DNDll)},
 {0x5D7380,reinterpret_cast<void*>(&DestroyNormal)},{std::bit_cast<uintptr_t>(&CCustomBuildingPipeline::DestroyPipe),reinterpret_cast<void*>(&DestroyNormal)},
 {0x5D5FA0,reinterpret_cast<void*>(&DestroyDN)},{std::bit_cast<uintptr_t>(&CCustomBuildingDNPipeline::DestroyPipe),reinterpret_cast<void*>(&DestroyDN)},
 {0x5D72E0,reinterpret_cast<void*>(&PluginExe)},{std::bit_cast<uintptr_t>(&CCustomBuildingDNPipeline::ExtraVertColourPluginAttach),reinterpret_cast<void*>(&PluginDll)}};
 std::vector<std::pair<uintptr_t,std::vector<uint8>>> patchBytes;for(const auto& [at,fn]:patches)patchBytes.emplace_back(at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),52));
 for(const auto& state:flags)if(state.hook->Name()!="NOTSA_WinMain"){state.hook->LockState(false);state.hook->State(false);}
 bool passed=true;size_t pairs=0,differences=0;
 {
  std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> owned;for(const auto& [at,fn]:patches)owned.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fn));
  for(int target=0;target<3;++target)for(int i=0;i<1024;++i){normalStatus=i&1;dnStatus=i&2;pluginStatus=i&1;for(size_t k=0;k<payload.size();++k)payload[k]=uint8((i/(target==0?4:target==2?2:1))*17+k*29+(i/(target==0?1024:target==2?512:256))*(k%5));const auto before=payload;auto model=before;json calls=json::array();uint32 expected=0;
   auto step=[&](int role,bool status){calls.push_back(Call(role,status));model[role]=uint8(model[role]+role+1);};
   if(target==0){step(0,normalStatus);if(normalStatus){step(1,dnStatus);expected=dnStatus?1:0;}}
   else if(target==1){step(2,false);step(3,false);}else{step(4,pluginStatus);expected=pluginStatus?1:0;}
   json results[2];for(int route=0;route<2;++route){payload=before;trace=json::array();auto* h=targets.at(keys[target]);h->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*h,route!=0);uint32 value=0;if(target==1)reinterpret_cast<void(__cdecl*)()>(addresses[target])();else value=RawAL(addresses[target]);results[route]={{"gate",gate},{"raw_al",value},{"payload",payload},{"trace",trace}};h->State(false);}
   const bool equal=results[0]==results[1]&&results[0]["gate"]==true&&results[0]["raw_al"]==expected&&results[0]["payload"]==json(model)&&results[0]["trace"]==calls;passed&=equal;differences+=!equal;
   write({{"event","comparison"},{"function",keys[target]},{"pair",pairs++},{"case",i},{"equal",equal},{"normal_status",normalStatus},{"dn_status",dnStatus},{"plugin_status",pluginStatus},{"initial_payload",before},{"model_payload",model},{"model_trace",calls},{"model_al",expected},{"original",results[0]},{"candidate",results[1]}});
  }
  Guarded<CRealTimeShadowManager> manager;Guarded<CRealTimeShadow> shadow;Guarded<CPhysical> physical;
  for(int i=0;i<2048;++i){const bool initialized=i&1,shadowNull=!initialized&&(i&2),ownerNull=!initialized&&(i&4);const uint8 poison=uint8((i>>3)*37);manager.Fill(poison);shadow.Fill(uint8(poison^0x99));physical.Fill(uint8(poison^0x66));auto* m=manager.Get();auto* s=shadow.Get();auto* p=physical.Get();m->m_bInitialised=char(initialized);s->m_pOwner=ownerNull?nullptr:p;p->m_pShadowData=(i&8)?nullptr:s;
   if(uintptr_t(&m->m_bInitialised)-uintptr_t(m)!=0||uintptr_t(&s->m_pOwner)-uintptr_t(s)!=0||uintptr_t(&p->m_pShadowData)-uintptr_t(p)!=0x134){write({{"event","error"},{"reason","shadow layout"}});passed=false;break;}
   const auto mi=manager.data;const auto si=shadow.data;const auto pi=physical.data;auto ms=si;auto mp=pi;if(initialized){std::fill(ms.begin(),ms.begin()+4,0);std::fill(mp.begin()+0x134,mp.begin()+0x138,0);}json results[2];
   for(int route=0;route<2;++route){manager.data=mi;shadow.data=si;physical.data=pi;trace=json::array();auto* h=targets.at(keys[3]);h->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*h,route!=0);reinterpret_cast<void(__thiscall*)(CRealTimeShadowManager*,CRealTimeShadow*)>(addresses[3])(m,shadowNull?nullptr:s);results[route]={{"gate",gate},{"manager",manager.data},{"shadow",shadow.data},{"physical",physical.data},{"trace",trace},{"guards",manager.Guards()&&shadow.Guards()&&physical.Guards()}};h->State(false);}
   const bool equal=results[0]==results[1]&&results[0]["gate"]==true&&results[0]["guards"]==true&&results[0]["manager"]==json(mi)&&results[0]["shadow"]==json(ms)&&results[0]["physical"]==json(mp)&&results[0]["trace"]==json::array();passed&=equal;differences+=!equal;
   write({{"event","comparison"},{"function",keys[3]},{"pair",pairs++},{"case",i},{"manager_address",uintptr_t(m)},{"shadow_address",uintptr_t(s)},{"physical_address",uintptr_t(p)},{"initialized",initialized},{"shadow_null",shadowNull},{"owner_null",ownerNull},{"poison",poison},{"equal",equal},{"initial_manager",mi},{"initial_shadow",si},{"initial_physical",pi},{"model_manager",mi},{"model_shadow",ms},{"model_physical",mp},{"original",results[0]},{"candidate",results[1]}});
  }
 }
 const bool globalsUnchanged=std::ranges::all_of(globals,[](const auto& r){return r.second==ReversingProbe::Bytes(reinterpret_cast<void*>(r.first),r.second.size());});for(const auto& r:globals)std::memcpy(reinterpret_cast<void*>(r.first),r.second.data(),r.second.size());
 const bool globalsRestored=std::ranges::all_of(globals,[](const auto& r){return r.second==ReversingProbe::Bytes(reinterpret_cast<void*>(r.first),r.second.size());});
 payload=initialPayload;for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
 const bool entriesRestored=std::ranges::all_of(entries,[](const auto& r){return r.second==ReversingProbe::Bytes(r.first,r.second.size());});const bool patchesRestored=std::ranges::all_of(patchBytes,[](const auto& r){return r.second==ReversingProbe::Bytes(reinterpret_cast<void*>(r.first),r.second.size());});const bool flagsRestored=SmallPendingSupport::HooksUnchanged(flags),payloadRestored=payload==initialPayload;
 const bool setFp=std::fesetenv(&fpBefore)==0;_mm_setcsr(mxcsrBefore);uint16 statusAfter{};__asm fnstsw statusAfter
 unsigned controlAfter{};_controlfp_s(&controlAfter,0,0);const bool fpRestored=gotFp&&setFp&&statusAfter==statusBefore&&controlAfter==controlBefore&&_mm_getcsr()==mxcsrBefore;
 const bool restored=entriesRestored&&patchesRestored&&flagsRestored&&payloadRestored&&fpRestored&&globalsRestored;passed&=globalsUnchanged;
 write({{"event",passed&&restored&&pairs==5120?"complete":"error"},{"pairs",pairs},{"differences",differences},{"state_restored",restored},{"entries_restored",entriesRestored},{"collaborator_entries_restored",patchesRestored},{"flags_restored",flagsRestored},{"globals_unchanged",globalsUnchanged},{"globals_restored",globalsRestored},{"payload_restored",payloadRestored},{"fp_restored",fpRestored},{"fp_status_before",statusBefore},{"fp_status_after",statusAfter},{"fp_control_before",controlBefore},{"fp_control_after",controlAfter},{"mxcsr_before",mxcsrBefore},{"mxcsr_after",_mm_getcsr()},{"scope","CPU pregraphics; canonical bool collaborator statuses; private byte-representation fixtures; no pipeline/plugin helper implementation credit"}});
 trace=nullptr;
}
}
