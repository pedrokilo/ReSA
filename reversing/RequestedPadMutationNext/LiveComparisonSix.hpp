#pragma once
#include <cfenv>
#include <xmmintrin.h>
#include <fstream>
#include "FixturesSix.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPadMutationSix {
using Simple=ReversibleHooks::ReversibleHook::Simple;
struct Target {const char* name;uintptr_t address;};
inline constexpr Target targets[]{{"StopShaking",0x53FB50},{"SetTouched",0x53F200},{"ClearMouseHistory",0x541BD0},{"ClearKeyBoardHistory",0x53F1E0},{"Clear",0x541A70},{"Initialise",0x541D90}};
struct Range {void* ptr;std::vector<uint8_t> data;};
inline Simple* Find(uintptr_t address){Simple* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* simple=static_cast<Simple*>(h.get());if(uintptr_t(simple->GetHookGTAAddress())==address)found=simple;}});return found;}
inline void Call(size_t target,void* entry,CPad* pad,const Fixture& fixture){
 switch(target){
  case 0:reinterpret_cast<void(__thiscall*)(CPad*,int16_t)>(entry)(pad,fixture.stopArgument);break;
  case 1:reinterpret_cast<void(__thiscall*)(CPad*)>(entry)(pad);break;
  case 2:case 3:case 5:reinterpret_cast<void(__cdecl*)()>(entry)();break;
  case 4:reinterpret_cast<void(__thiscall*)(CPad*,bool,bool)>(entry)(pad,fixture.clearDisabled,fixture.resetPhase);break;
 }
}
inline void Update(){
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-pad-mutation-six-runtime.jsonl");
 auto write=[&](const json& row){log<<row.dump()<<'\n';};
 std::fenv_t originalFP{};std::fegetenv(&originalFP);const unsigned originalMXCSR=_mm_getcsr();
 std::vector<Range> saved;for(const auto& r:globalRanges)saved.push_back({reinterpret_cast<void*>(r.address),ReversingProbe::Bytes(reinterpret_cast<void*>(r.address),r.size)});
 write({{"event","start"},{"functions",6},{"comparisons",5888},{"global_ranges",std::size(globalRanges)}});
 size_t executed=0;bool switches=true;bool missing=false;
 for(size_t index=0;index<std::size(targets);++index){const auto& target=targets[index];auto* hook=Find(target.address);
  if(!hook){missing=true;write({{"event","missing_hook"},{"function",target.name}});break;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  for(size_t seed=0;seed<Counts[index];++seed){const auto fixture=Case(index,seed);json outcomes=json::array();bool switched=true;
   for(bool candidate:{false,true}){
    auto pad=CPad::Pads[0];std::memcpy(&pad,fixture.pad.data(),sizeof(pad));
    for(size_t role=0;role<std::size(globalRanges);++role)std::memcpy(reinterpret_cast<void*>(globalRanges[role].address),fixture.globals[role].data(),globalRanges[role].size);
    std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);
    hook->State(candidate);switched&=SmallPendingSupport::ExactSwitch(*hook,candidate);
    Call(index,candidate?hook->GetHookOurAddress():hook->GetHookGTAAddress(),&pad,fixture);
    json globals=json::array();for(const auto& r:globalRanges)globals.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(r.address),r.size));
    // IP/data pointers in fenv_t are diagnostic execution locations, not normalized output; compare control/status semantics.
    outcomes.push_back({{"pad",ReversingProbe::Bytes(&pad,sizeof(pad))},{"globals",globals},{"rounding",std::fegetround()},{"exceptions",std::fetestexcept(FE_ALL_EXCEPT)},{"mxcsr",_mm_getcsr()}});
   }
   ++executed;switches&=switched;
   write({{"event","comparison"},{"function",target.name},{"address",target.address},{"seed",seed},{"clock",fixture.clock},{"stop_argument",fixture.stopArgument},{"clear_disabled",fixture.clearDisabled},{"reset_phase",fixture.resetPhase},{"switch_exact",switched},{"equal",outcomes[0]==outcomes[1]},{"original",outcomes[0]},{"candidate",outcomes[1]}});
   if(seed%128==127)log.flush();
  }
  hook->State(enabled);hook->LockState(locked);
 }
 for(const auto& range:saved)std::memcpy(range.ptr,range.data.data(),range.data.size());std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);
 bool globalsRestored=true;for(const auto& range:saved)globalsRestored&=range.data==ReversingProbe::Bytes(range.ptr,range.data.size());
 std::fenv_t restoredFP{};std::fegetenv(&restoredFP);const bool fpRestored=_mm_getcsr()==originalMXCSR&&std::memcmp(&originalFP,&restoredFP,sizeof(originalFP))==0;
 const bool complete=!missing&&executed==5888&&switches&&globalsRestored&&fpRestored;
 write({{"event",complete?"complete":"error"},{"comparisons",executed},{"functions",6},{"state_restored",globalsRestored&&fpRestored},{"globals_restored",globalsRestored},{"floating_environment_restored",fpRestored},{"switch_exact",switches}});log.flush();
}
}
