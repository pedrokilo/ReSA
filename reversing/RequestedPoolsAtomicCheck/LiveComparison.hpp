#pragma once
#include <fstream>
#include <cfenv>
#include <xmmintrin.h>
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPoolsAtomicCheckLive {
inline json Call(uintptr_t target,uint32 seed) {
    uint32 returned{},beforeESP{},afterESP{};uint32 before[4]{},after[4]{};
    __asm {
        lea edx,before
        mov [edx],ebx
        mov [edx+4],esi
        mov [edx+8],edi
        mov [edx+12],ebp
        mov beforeESP,esp
        mov eax,seed
        call target
        mov returned,eax
        mov afterESP,esp
        lea edx,after
        mov [edx],ebx
        mov [edx+4],esi
        mov [edx+8],edi
        mov [edx+12],ebp
    }
    const int round=std::fegetround(),exceptions=std::fetestexcept(FE_ALL_EXCEPT);const uint32 mxcsr=_mm_getcsr();
    return {{"returned",returned},{"stack_balanced",beforeESP==afterESP},{"callee_saved",std::memcmp(before,after,sizeof(before))==0},{"fp",{{"rounding",round},{"exceptions",exceptions},{"mxcsr",mxcsr}}}};
}
inline void Update() {
    using namespace ReversibleHooks::ReversibleHook;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-pools-atomic-check-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    const auto flags=SmallPendingSupport::SnapshotHooks();Simple* hook{};
    for(const auto& s:flags)if(s.hook->Type()==Base::HookType::Simple){auto*h=static_cast<Simple*>(s.hook);if(h->GetHookGTAAddress()==reinterpret_cast<void*>(0x550170)&&h->Name()=="CheckBuildingAtomics")hook=h;}
    if(!hook){write({{"event","error"},{"reason","exact constant getter hook missing"}});return;}
    const auto native=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),candidate=ReversingProbe::Bytes(hook->GetHookOurAddress(),52),pools=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68);
    std::fenv_t initialFP{};std::fegetenv(&initialFP);const auto initialMXCSR=_mm_getcsr();bool passed=true;int failures{};hook->LockState(false);
    write({{"event","start"},{"cases",256},{"function","CPools::CheckBuildingAtomics"},{"address",0x550170}});
    write({{"event","fixture"},{"pool_globals",pools},{"fp",{{"rounding",std::fegetround()},{"exceptions",std::fetestexcept(FE_ALL_EXCEPT)},{"mxcsr",initialMXCSR}}}});
    for(uint32 seed=0;seed<256;++seed){json legs[2];std::array<uint8,64> guard{};for(size_t i=0;i<guard.size();++i)guard[i]=uint8(seed*47+i*29);const auto before=guard;
        const uint32 eaxSeed=seed*0x9E3779B9u;
        for(int side=0;side<2;++side){std::fesetenv(&initialFP);_mm_setcsr(initialMXCSR);hook->State(side!=0);const bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            legs[side]=Call(0x550170,eaxSeed);legs[side]["switch_exact"]=switched;legs[side]["guard"]=guard;legs[side]["guard_unchanged"]=guard==before;legs[side]["pool_globals"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68);legs[side]["globals_unchanged"]=legs[side]["pool_globals"]==pools;
        }
        const bool accepted=legs[0]==legs[1]&&legs[0]["returned"]==0xFFFFFFFFu&&legs[0]["switch_exact"]==true&&legs[0]["stack_balanced"]==true&&legs[0]["callee_saved"]==true&&legs[0]["guard_unchanged"]==true&&legs[0]["globals_unchanged"]==true;passed&=accepted;failures+=!accepted;
        write({{"event","comparison"},{"function","CPools::CheckBuildingAtomics"},{"pair",seed},{"case",seed},{"eax_seed",eaxSeed},{"equal",legs[0]==legs[1]},{"accepted",accepted},{"original",legs[0]},{"candidate",legs[1]}});
    }
    std::fesetenv(&initialFP);_mm_setcsr(initialMXCSR);
    for(const auto& s:flags){s.hook->LockState(false);s.hook->State(s.enabled);s.hook->LockState(s.locked);}
    std::fenv_t finalFP{};std::fegetenv(&finalFP);const bool fpRestored=std::memcmp(&initialFP,&finalFP,sizeof(initialFP))==0&&initialMXCSR==_mm_getcsr();
    const bool restored=fpRestored&&SmallPendingSupport::HooksUnchanged(flags)&&native==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&candidate==ReversingProbe::Bytes(hook->GetHookOurAddress(),52)&&pools==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68);
    write({{"event",passed&&restored?"complete":"error"},{"pairs",256},{"failures",failures},{"state_restored",restored},{"floating_environment_restored",fpRestored},{"passed",passed&&restored}});
}
}
