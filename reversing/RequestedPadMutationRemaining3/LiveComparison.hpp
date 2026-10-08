#pragma once
#include <cfenv>
#include <xmmintrin.h>
#include "Fixtures.hpp"
#include "ScratchProof.hpp"
#ifndef REQUESTED_PAD_REMAINING_BANK
#error Builder must select exactly one remaining CPad target bank
#endif
namespace RequestedPadRemaining3 {
inline constexpr size_t bank=REQUESTED_PAD_REMAINING_BANK;
static_assert(bank<3);
inline constexpr const char* names[]{"ReconcileTwoControllersInput","Update","StartShake"};
inline constexpr const char* slugs[]{"reconcile","update","shake"};
inline constexpr uintptr_t addresses[]{0x53F530,0x541C40,0x53F920};
struct Range {void* ptr;std::vector<uint8_t> data;};
struct GuardedController {std::array<uint8_t,16> before{};CControllerState value;std::array<uint8_t,16> after{};explicit GuardedController(const CControllerState& state):value(state){before.fill(0x31);after.fill(0xE7);}};
struct GuardedPad {std::array<uint8_t,16> before{};CPad value;std::array<uint8_t,16> after{};explicit GuardedPad(const CPad& pad):value(pad){before.fill(0x49);after.fill(0xC3);}};
static_assert(sizeof(GuardedController)==80&&offsetof(GuardedController,value)==16&&sizeof(GuardedPad)==340&&offsetof(GuardedPad,value)==16);
inline json FP(){return {{"rounding",std::fegetround()},{"exceptions",std::fetestexcept(FE_ALL_EXCEPT)},{"mxcsr",_mm_getcsr()}};}
inline bool PrivateGuards(const GuardedPad& pad,const GuardedController& a,const GuardedController& b,const GuardedController& out){return std::ranges::all_of(pad.before,[](auto x){return x==0x49;})&&std::ranges::all_of(pad.after,[](auto x){return x==0xC3;})&&std::ranges::all_of(std::array{&a,&b,&out},[](const auto* state){return std::ranges::all_of(state->before,[](auto x){return x==0x31;})&&std::ranges::all_of(state->after,[](auto x){return x==0xE7;});});}
inline void Update(){
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();std::ofstream log(dir/(std::string("requested-pad-remaining3-")+slugs[bank]+"-runtime.jsonl"));auto write=[&](const json& row){log<<row.dump()<<'\n';};
 std::fenv_t originalFP{};std::fegetenv(&originalFP);const unsigned originalMXCSR=_mm_getcsr();CurrentProof proof;json error;
 write({{"event","start"},{"bank",slugs[bank]},{"function",names[bank]},{"address",addresses[bank]},{"cases",counts[bank]}});
 if(uintptr_t(&FrontEndMenuManager.m_PrefsUseVibration)!=0xBA6768||uintptr_t(&CCutsceneMgr::ms_running)!=0xB5F851||uintptr_t(&CTimer::m_snTimeInMilliseconds)!=0xB7CB84){write({{"event","error"},{"reason","typed global aliases mismatch"},{"state_restored",true}});log.flush();return;}
 if(!VerifyCurrent(dir,proof,error)){std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);write({{"event","error"},{"reason",error},{"comparisons",0},{"state_restored",true}});log.flush();return;}
 write({{"event","current_artifact_proof"},{"proof",proof.evidence}});log.flush();
 std::vector<Range> globals;for(auto [at,size]:std::vector<std::pair<uintptr_t,size_t>>{{0xB7CB7C,16},{0xBA6758,33},{0xB5F841,33}})globals.push_back({reinterpret_cast<void*>(at),ReversingProbe::Bytes(reinterpret_cast<void*>(at),size)});
 std::vector<Range> scratch{{reinterpret_cast<void*>(0xB736B0),ReversingProbe::Bytes(reinterpret_cast<void*>(0xB736B0),80)},{proof.scratch-16,ReversingProbe::Bytes(proof.scratch-16,80)}};
 auto* hook=Find(addresses[bank]);if(!hook){write({{"event","error"},{"reason","missing selected hook"},{"state_restored",true}});log.flush();return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);bool allSwitches=true,allGuards=true,allInactive=true,allRefs=true,allGlobals=true,allHelpers=true;size_t executed=0;
 for(size_t seed=0;seed<counts[bank];++seed){const auto fixture=Case(bank,seed);json physical=json::array(),contracts=json::array();bool switched=true,guarded=true,inactive=true,refs=true,globalsReadonly=true,helpersOriginal=true;json scratchSeeds=json::array();
  for(size_t role=0;role<2;++role){std::vector<uint8_t> data(48);for(size_t i=0;i<48;++i)data[i]=uint8_t(seed*73+i*29+role*101);scratchSeeds.push_back(data);}
  for(bool candidate:{false,true}){
   GuardedPad pad(fixture.pad);GuardedController a(fixture.a),b(fixture.b),out(fixture.out);CControllerState* output=fixture.alias==1?&a.value:fixture.alias==2?&b.value:&out.value;
   for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());for(size_t role=0;role<2;++role){const auto& range=scratch[role];std::memcpy(range.ptr,range.data.data(),range.data.size());const auto data=scratchSeeds[role].get<std::vector<uint8_t>>();std::memcpy(static_cast<uint8_t*>(range.ptr)+16,data.data(),48);}
   std::memcpy(reinterpret_cast<void*>(0xB7CB84),&fixture.clock,4);*reinterpret_cast<uint8_t*>(0xBA6768)=(fixture.gates&1)!=0;*reinterpret_cast<int8_t*>(0xB5F851)=(fixture.gates&2)!=0;
   json seededGlobals=json::array();for(const auto& range:globals)seededGlobals.push_back(ReversingProbe::Bytes(range.ptr,range.data.size()));
   std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);hook->State(candidate);switched&=SmallPendingSupport::ExactSwitch(*hook,candidate);for(uintptr_t helperAddress:{uintptr_t(0x53EF80),uintptr_t(0x53EFF0),uintptr_t(0x53F530)}){if(helperAddress==addresses[bank])continue;auto* helper=Find(helperAddress);helpersOriginal&=helper&&!helper->Hooked()&&SmallPendingSupport::ExactSwitch(*helper,false);}
   void* entry=candidate?hook->GetHookOurAddress():hook->GetHookGTAAddress();bool returned=true;
   if constexpr(bank==0){returned=reinterpret_cast<CControllerState*(__thiscall*)(CPad*,CControllerState*,const CControllerState*,const CControllerState*)>(entry)(&pad.value,output,&a.value,&b.value)==output;}
   else if constexpr(bank==1){reinterpret_cast<void(__thiscall*)(CPad*,int32_t)>(entry)(&pad.value,fixture.padId);}
   else{reinterpret_cast<void(__thiscall*)(CPad*,int16_t,uint8_t,uint32_t)>(entry)(&pad.value,fixture.duration,fixture.frequency,fixture.delay);}
   json currentGlobals=json::array(),currentScratch=json::array();for(const auto& range:globals)currentGlobals.push_back(ReversingProbe::Bytes(range.ptr,range.data.size()));for(const auto& range:scratch)currentScratch.push_back(ReversingProbe::Bytes(range.ptr,range.data.size()));
   bool guardLeg=PrivateGuards(pad,a,b,out);for(size_t role=0;role<2;++role){const auto bytes=currentScratch[role].get<std::vector<uint8_t>>();guardLeg&=std::equal(bytes.begin(),bytes.begin()+16,scratch[role].data.begin())&&std::equal(bytes.begin()+64,bytes.end(),scratch[role].data.begin()+64);}
   bool inactiveLeg=true;for(size_t role=0;role<2;++role){const bool active=bank==0?role==size_t(candidate):bank==1?role==0:false;if(!active)inactiveLeg&=currentScratch[role].get<std::vector<uint8_t>>()==[&]{auto bytes=scratch[role].data;const auto data=scratchSeeds[role].get<std::vector<uint8_t>>();std::copy(data.begin(),data.end(),bytes.begin()+16);return bytes;}();}
   guarded&=guardLeg;inactive&=inactiveLeg;refs&=returned;globalsReadonly&=currentGlobals==seededGlobals;json state={{"pad",ReversingProbe::Bytes(&pad,sizeof(pad))},{"controllers",json::array({ReversingProbe::Bytes(&a,sizeof(a)),ReversingProbe::Bytes(&b,sizeof(b)),ReversingProbe::Bytes(&out,sizeof(out))})},{"globals",currentGlobals},{"return_reference_exact",returned},{"fp",FP()}};
   json trace=state;trace["scratch_physical"]=currentScratch;trace["scratch_guards_unchanged"]=guardLeg;trace["inactive_scratch_unchanged"]=inactiveLeg;physical.push_back(trace);
   if constexpr(bank==0){const auto bytes=currentScratch[size_t(candidate)].get<std::vector<uint8_t>>();state["scratch_active_role"]=std::vector<uint8_t>(bytes.begin()+16,bytes.begin()+64);}else state["scratch_physical"]=currentScratch;contracts.push_back(state);
  }
  ++executed;allSwitches&=switched;allGuards&=guarded;allInactive&=inactive;allRefs&=refs;allGlobals&=globalsReadonly;allHelpers&=helpersOriginal;
  write({{"event","comparison"},{"function",names[bank]},{"address",addresses[bank]},{"seed",seed},{"domain",fixture.domain},{"alias",fixture.alias},{"clock",fixture.clock},{"deadline",fixture.deadline},{"delay",fixture.delay},{"duration",fixture.duration},{"frequency",fixture.frequency},{"stored_frequency",fixture.storedFrequency},{"gates",fixture.gates},{"pad_id",fixture.padId},{"switch_exact",switched},{"guards_unchanged",guarded},{"inactive_scratch_unchanged",inactive},{"return_reference_exact",refs},{"globals_unchanged",globalsReadonly},{"helper_entries_original",helpersOriginal},{"equal",contracts[0]==contracts[1]},{"original",physical[0]},{"candidate",physical[1]},{"scratch_seed_bytes",scratchSeeds}});if(seed%128==127)log.flush();
 }
 hook->State(enabled);hook->LockState(locked);for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());for(const auto& range:scratch)std::memcpy(range.ptr,range.data.data(),range.data.size());std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);
 bool restored=true;for(const auto& range:globals)restored&=range.data==ReversingProbe::Bytes(range.ptr,range.data.size());for(const auto& range:scratch)restored&=range.data==ReversingProbe::Bytes(range.ptr,range.data.size());std::fenv_t currentFP{};std::fegetenv(&currentFP);const bool fpRestored=_mm_getcsr()==originalMXCSR&&std::memcmp(&originalFP,&currentFP,sizeof(originalFP))==0;restored&=fpRestored;
 write({{"event",restored&&allSwitches&&allGuards&&allInactive&&allRefs&&allGlobals&&allHelpers?"complete":"error"},{"comparisons",executed},{"state_restored",restored},{"floating_environment_restored",fpRestored},{"switch_exact",allSwitches},{"guards_unchanged",allGuards},{"inactive_scratch_unchanged",allInactive},{"return_reference_exact",allRefs},{"globals_unchanged",allGlobals},{"helper_entries_original",allHelpers}});log.flush();
}
}
