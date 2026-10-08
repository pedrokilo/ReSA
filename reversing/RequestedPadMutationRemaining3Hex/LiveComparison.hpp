#pragma once
#include <cfenv>
#include <xmmintrin.h>
#include "Fixtures.hpp"
#include "HexTransport.hpp"
#include "ScratchProof.hpp"
#ifndef REQUESTED_PAD_REMAINING_BANK
#error Builder must select exactly one remaining CPad target bank
#endif
namespace RequestedPadRemaining3Hex {
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
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();std::ofstream log(dir/(std::string("requested-pad-remaining3-hex-")+slugs[bank]+"-runtime.jsonl"));auto write=[&](const json& row){log<<row.dump()<<'\n';};
 std::fenv_t originalFP{};std::fegetenv(&originalFP);const unsigned originalMXCSR=_mm_getcsr();CurrentProof proof;json error;
 write({{"event","start"},{"bank",slugs[bank]},{"function",names[bank]},{"address",addresses[bank]},{"cases",counts[bank]},{"wire_encoding","lowercasehex-v1"}});
 if(uintptr_t(&FrontEndMenuManager.m_PrefsUseVibration)!=0xBA6768||uintptr_t(&CCutsceneMgr::ms_running)!=0xB5F851||uintptr_t(&CTimer::m_snTimeInMilliseconds)!=0xB7CB84){write({{"event","error"},{"reason","typed global aliases mismatch"},{"state_restored",true}});log.flush();return;}
 if(!VerifyCurrent(dir,proof,error)){std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);write({{"event","error"},{"reason",error},{"comparisons",0},{"state_restored",true}});log.flush();return;}
 write({{"event","current_artifact_proof"},{"proof",proof.evidence}});log.flush();
 std::vector<Range> globals;for(auto [at,size]:std::vector<std::pair<uintptr_t,size_t>>{{0xB7CB7C,16},{0xBA6758,33},{0xB5F841,33}})globals.push_back({reinterpret_cast<void*>(at),ReversingProbe::Bytes(reinterpret_cast<void*>(at),size)});
 std::vector<Range> scratch{{reinterpret_cast<void*>(0xB736B0),ReversingProbe::Bytes(reinterpret_cast<void*>(0xB736B0),80)},{proof.scratch-16,ReversingProbe::Bytes(proof.scratch-16,80)}};
 auto* hook=Find(addresses[bank]);if(!hook){write({{"event","error"},{"reason","missing selected hook"},{"state_restored",true}});log.flush();return;}
 const std::array<uintptr_t,3> helperAddresses{0x53EF80,0x53EFF0,0x53F530};
 std::array<Simple*,3> cachedHelpers{};for(size_t i=0;i<helperAddresses.size();++i)cachedHelpers[i]=Find(helperAddresses[i]);
 if(std::any_of(cachedHelpers.begin(),cachedHelpers.end(),[](auto* helper){return !helper;})){write({{"event","error"},{"reason","cached exact helper identity missing; no case run"},{"state_restored",true}});log.flush();return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);bool allSwitches=true,allGuards=true,allInactive=true,allRefs=true,allGlobals=true,allHelpers=true;size_t executed=0;
 // JSON originalrecords only (no duplicate contract/scratch seeds retained). Estimate with array capacity,
 // 192 bytes per map entry and container/string slack; buffer policy max32MiB, not an RSS assertion.
 auto retainedEstimate=[](const json& root){auto estimate=[](auto&& self,const json& value)->size_t {
  if(value.is_array()){const auto& a=value.get_ref<const json::array_t&>();size_t bytes=64+a.capacity()*sizeof(json);for(const auto& item:a)bytes+=self(self,item);return bytes;}
  if(value.is_object()){size_t bytes=64+value.size()*192;for(auto it=value.begin();it!=value.end();++it)bytes+=it.key().capacity()+1+self(self,it.value());return bytes;}
  if(value.is_string())return 64+value.get_ref<const json::string_t&>().capacity()+1;
  return size_t(0);
 };return estimate(estimate,root);};
 struct Leg {size_t seed;json physical;bool switched,guarded,inactive,refs,globalsReadonly,helpersOriginal;};
 auto scratchFor=[](size_t seed){json result=json::array();for(size_t role=0;role<2;++role){std::vector<uint8_t> data(48);for(size_t i=0;i<48;++i)data[i]=uint8_t(seed*73+i*29+role*101);result.push_back(HexBytes(data.data(),data.size()));}return result;};
 auto capture=[&](size_t seed,bool candidate)->Leg {
  const auto fixture=Case(bank,seed);const auto scratchSeeds=scratchFor(seed);bool switched=true,guarded=true,inactive=true,refs=true,globalsReadonly=true,helpersOriginal=true;
   GuardedPad pad(fixture.pad);GuardedController a(fixture.a),b(fixture.b),out(fixture.out);CControllerState* output=fixture.alias==1?&a.value:fixture.alias==2?&b.value:&out.value;
   for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());for(size_t role=0;role<2;++role){const auto& range=scratch[role];std::memcpy(range.ptr,range.data.data(),range.data.size());const auto data=HexToBytes(scratchSeeds[role].get<std::string>());std::memcpy(static_cast<uint8_t*>(range.ptr)+16,data.data(),48);}
   std::memcpy(reinterpret_cast<void*>(0xB7CB84),&fixture.clock,4);*reinterpret_cast<uint8_t*>(0xBA6768)=(fixture.gates&1)!=0;*reinterpret_cast<int8_t*>(0xB5F851)=(fixture.gates&2)!=0;
   json seededGlobals=json::array();for(const auto& range:globals)seededGlobals.push_back(HexBytes(range.ptr,range.data.size()));
   std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);hook->State(candidate);switched&=SmallPendingSupport::ExactSwitch(*hook,candidate);for(size_t helperIndex=0;helperIndex<helperAddresses.size();++helperIndex){if(helperAddresses[helperIndex]==addresses[bank])continue;auto* helper=cachedHelpers[helperIndex];helpersOriginal&=helper&&!helper->Hooked()&&SmallPendingSupport::ExactSwitch(*helper,false);}
   void* entry=candidate?hook->GetHookOurAddress():hook->GetHookGTAAddress();bool returned=true;
   if constexpr(bank==0){returned=reinterpret_cast<CControllerState*(__thiscall*)(CPad*,CControllerState*,const CControllerState*,const CControllerState*)>(entry)(&pad.value,output,&a.value,&b.value)==output;}
   else if constexpr(bank==1){reinterpret_cast<void(__thiscall*)(CPad*,int32_t)>(entry)(&pad.value,fixture.padId);}
   else{reinterpret_cast<void(__thiscall*)(CPad*,int16_t,uint8_t,uint32_t)>(entry)(&pad.value,fixture.duration,fixture.frequency,fixture.delay);}
   const auto postCallFP=FP(); // Capture target FP effects before integer-only transport allocation.
   json currentGlobals=json::array(),currentScratch=json::array();for(const auto& range:globals)currentGlobals.push_back(HexBytes(range.ptr,range.data.size()));for(const auto& range:scratch)currentScratch.push_back(HexBytes(range.ptr,range.data.size()));
   bool guardLeg=PrivateGuards(pad,a,b,out);for(size_t role=0;role<2;++role){const auto bytes=HexToBytes(currentScratch[role].get<std::string>());guardLeg&=std::equal(bytes.begin(),bytes.begin()+16,scratch[role].data.begin())&&std::equal(bytes.begin()+64,bytes.end(),scratch[role].data.begin()+64);}
   bool inactiveLeg=true;for(size_t role=0;role<2;++role){const bool active=bank==0?role==size_t(candidate):bank==1?role==0:false;if(!active)inactiveLeg&=HexToBytes(currentScratch[role].get<std::string>())==[&]{auto bytes=scratch[role].data;const auto data=HexToBytes(scratchSeeds[role].get<std::string>());std::copy(data.begin(),data.end(),bytes.begin()+16);return bytes;}();}
   guarded&=guardLeg;inactive&=inactiveLeg;refs&=returned;globalsReadonly&=currentGlobals==seededGlobals;json state={{"pad",HexBytes(&pad,sizeof(pad))},{"controllers",json::array({HexBytes(&a,sizeof(a)),HexBytes(&b,sizeof(b)),HexBytes(&out,sizeof(out))})},{"globals",currentGlobals},{"return_reference_exact",returned},{"fp",postCallFP}};
   json trace=state;trace["scratch_physical"]=currentScratch;trace["scratch_guards_unchanged"]=guardLeg;trace["inactive_scratch_unchanged"]=inactiveLeg;return Leg{seed,std::move(trace),switched,guarded,inactive,refs,globalsReadonly,helpersOriginal};

 };
 auto contractFor=[](const Leg& leg,bool candidate){const auto& value=leg.physical;json state;for(const char* key:{"pad","controllers","globals","return_reference_exact","fp"})state[key]=value[key];if constexpr(bank==0){state["scratch_active_role"]=value["scratch_physical"][size_t(candidate)].get<std::string>().substr(32,96);}else state["scratch_physical"]=value["scratch_physical"];return state;};
 constexpr size_t chunkSize=1024;
 bool retainedBound=true;size_t maxRetainedEstimate{};
 for(size_t chunkStart=0;chunkStart<counts[bank];chunkStart+=chunkSize){const size_t chunkEnd=std::min(counts[bank],chunkStart+chunkSize);std::vector<Leg> originals;originals.reserve(chunkEnd-chunkStart);
  size_t retainedBytes=originals.capacity()*sizeof(Leg);
  for(size_t seed=chunkStart;seed<chunkEnd;++seed){auto leg=capture(seed,false);retainedBytes+=retainedEstimate(leg.physical);if(retainedBytes>32u*1024u*1024u){retainedBound=false;allSwitches=false;break;}originals.push_back(std::move(leg));}
  maxRetainedEstimate=std::max(maxRetainedEstimate,retainedBytes);if(!retainedBound)break;
  for(size_t seed=chunkStart;seed<chunkEnd;++seed){const auto fixture=Case(bank,seed);auto candidate=capture(seed,true);auto& original=originals[seed-chunkStart];
   const bool mappingExact=originals.size()==chunkEnd-chunkStart&&original.seed==seed&&candidate.seed==seed;
   const bool switched=mappingExact&&original.switched&&candidate.switched,guarded=original.guarded&&candidate.guarded,inactive=original.inactive&&candidate.inactive,refs=original.refs&&candidate.refs,globalsReadonly=original.globalsReadonly&&candidate.globalsReadonly,helpersOriginal=original.helpersOriginal&&candidate.helpersOriginal;
   const json contracts=json::array({contractFor(original,false),contractFor(candidate,true)});const json physical=json::array({original.physical,candidate.physical});const auto scratchSeeds=scratchFor(seed);
  ++executed;allSwitches&=switched;allGuards&=guarded;allInactive&=inactive;allRefs&=refs;allGlobals&=globalsReadonly;allHelpers&=helpersOriginal;
  write({{"event","comparison"},{"function",names[bank]},{"address",addresses[bank]},{"seed",seed},{"domain",fixture.domain},{"alias",fixture.alias},{"clock",fixture.clock},{"deadline",fixture.deadline},{"delay",fixture.delay},{"duration",fixture.duration},{"frequency",fixture.frequency},{"stored_frequency",fixture.storedFrequency},{"gates",fixture.gates},{"pad_id",fixture.padId},{"switch_exact",switched},{"guards_unchanged",guarded},{"inactive_scratch_unchanged",inactive},{"return_reference_exact",refs},{"globals_unchanged",globalsReadonly},{"helper_entries_original",helpersOriginal},{"equal",contracts[0]==contracts[1]},{"original",physical[0]},{"candidate",physical[1]},{"scratch_seed_bytes",scratchSeeds}});if(seed%128==127)log.flush();

  }
 }
 hook->State(enabled);hook->LockState(locked);for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());for(const auto& range:scratch)std::memcpy(range.ptr,range.data.data(),range.data.size());std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);
 bool restored=true;for(const auto& range:globals)restored&=range.data==ReversingProbe::Bytes(range.ptr,range.data.size());for(const auto& range:scratch)restored&=range.data==ReversingProbe::Bytes(range.ptr,range.data.size());std::fenv_t currentFP{};std::fegetenv(&currentFP);const bool fpRestored=_mm_getcsr()==originalMXCSR&&std::memcmp(&originalFP,&currentFP,sizeof(originalFP))==0;restored&=fpRestored;
 write({{"event",restored&&allSwitches&&allGuards&&allInactive&&allRefs&&allGlobals&&allHelpers?"complete":"error"},{"comparisons",executed},{"state_restored",restored},{"floating_environment_restored",fpRestored},{"switch_exact",allSwitches},{"guards_unchanged",allGuards},{"inactive_scratch_unchanged",allInactive},{"return_reference_exact",allRefs},{"globals_unchanged",allGlobals},{"helper_entries_original",allHelpers},{"phased_chunk_pairs",chunkSize},{"phased_original_buffer_estimate_bytes",maxRetainedEstimate},{"phased_original_buffer_bound",retainedBound}});log.flush();
}
}
