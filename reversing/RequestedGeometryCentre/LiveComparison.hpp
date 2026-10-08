#pragma once
#include <fstream>
#include <cfenv>
#include <float.h>
#include <xmmintrin.h>
#include "PedGeometryAnalyser.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGeometryCentreLive {
using namespace ReversibleHooks::ReversibleHook;
inline bool done{};
struct Fixture {
    std::array<uint32,4> prefix;
    std::array<CVector,4> corners;
    CVector separate;
    std::array<uint32,4> suffix;
};
inline uint32 FiniteBits(uint32 seed) {
    constexpr uint32 edges[]{0,0x80000000,1,0x80000001,0x007FFFFF,0x807FFFFF,0x00800000,0x80800000,0x3F800000,0xBF800000,0x3F7FFFFF,0x3F800001,0x4B000000,0xCB000000,0x4B000001,0xCB000001,0x3EAAAAAB,0xBEAAAAAB,0x5F000000,0xDF000000,0x34000000,0xB4000000,0x33800000,0xB3800000};
    if(seed<24)return edges[seed];
    uint32 x=seed*1664525u+1013904223u;x^=x>>13;
    const uint32 exponent=1u+(x>>24)%189u;
    return (x&0x807FFFFFu)|(exponent<<23);
}
inline void Update() {
    if(done)return;done=true;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();
    if(!std::filesystem::exists(dir/"requested-geometry-centre-runtime.start"))return;
    std::ofstream log(dir/"requested-geometry-centre-runtime.jsonl");
    auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    const auto hooks=SmallPendingSupport::SnapshotHooks();Simple* hook{};
    for(const auto&s:hooks)if(s.hook->Type()==Base::HookType::Simple){auto*p=static_cast<Simple*>(s.hook);if(p->GetHookGTAAddress()==reinterpret_cast<void*>(0x5F1600)&&p->Name()=="ComputeEntityBoundingBoxCentreUncached")hook=p;}
    if(!hook){write({{"event","error"},{"reason","missing exact typed identity"}});return;}
    const auto originalEntry=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),candidateEntry=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    const bool enabled=hook->Hooked(),locked=hook->Locked();
    const auto constantBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x858C84),4);
    if(*reinterpret_cast<const uint32*>(0x858C84)!=0x3E800000){write({{"event","error"},{"reason","original multiplier is not canonical .25"}});return;}
    // Canonical, masked nearest environment is a fixture; preserve caller's environment.
    fenv_t caller{};std::fegetenv(&caller);const uint32 callerMxcsr=_mm_getcsr();
    unsigned callerControl{};_controlfp_s(&callerControl,0,0);
    unsigned canonicalControl{};_controlfp_s(&canonicalControl,_CW_DEFAULT,_MCW_EM|_MCW_RC|_MCW_PC);
    _mm_setcsr(0x1F80);std::feclearexcept(FE_ALL_EXCEPT);
    fenv_t canonical{};std::fegetenv(&canonical);
    write({{"event","fixture"},{"caller_control",callerControl},{"canonical_control",canonicalControl},{"caller_mxcsr",callerMxcsr},{"canonical_mxcsr",_mm_getcsr()},{"scope","finite corners; nearest masked FP; aliases only complete CVector elements"}});
    hook->LockState(false);bool passed=true;int failures{},pair{};
    for(int seed=0;seed<1024;++seed)for(int alias=-1;alias<4;++alias){
        Fixture initial{};initial.prefix.fill(0x13579BDF);initial.suffix.fill(0x2468ACE0);
        std::array<uint32,12> bits{};
        for(int i=0;i<12;++i){bits[i]=FiniteBits(seed<24?uint32((seed+i)%24):uint32(seed*19+i*71));std::memcpy(reinterpret_cast<uint8*>(initial.corners.data())+4*i,&bits[i],4);}
        initial.separate=CVector{17.25f,-33.5f,41.f};const uint32 zBits=FiniteBits(uint32(seed%24));const float z=std::bit_cast<float>(zBits);
        Fixture sides[2]{initial,initial};json result[2];
        for(int side=0;side<2;++side){
            auto& state=sides[side];auto& output=alias<0?state.separate:state.corners[alias];
            hook->State(side!=0);const bool gate=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            std::fesetenv(&canonical);_mm_setcsr(0x1F80);
            using Fn=void(__cdecl*)(float,const CVector*,CVector&);
            reinterpret_cast<Fn>(0x5F1600)(z,state.corners.data(),output);
            bool untouched=state.prefix==initial.prefix&&state.suffix==initial.suffix;
            for(int i=0;i<4;++i)if(i!=alias)untouched&=ReversingProbe::Bytes(&state.corners[i],12)==ReversingProbe::Bytes(&initial.corners[i],12);
            if(alias>=0)untouched&=ReversingProbe::Bytes(&state.separate,12)==ReversingProbe::Bytes(&initial.separate,12);
            const bool outputZ=std::bit_cast<uint32>(output.z)==zBits;
            unsigned control{};_controlfp_s(&control,0,0);
            result[side]={{"arena",ReversingProbe::Bytes(&state,sizeof(state))},{"output",ReversingProbe::Bytes(&output,12)},{"gate",gate},{"outside_output_unchanged",untouched},{"output_z_exact",outputZ},{"control_unchanged",control==canonicalControl},{"mxcsr_control_unchanged",(_mm_getcsr()&~0x3Fu)==0x1F80u}};
        }
        const bool equal=result[0]==result[1]&&result[0]["gate"]==true&&result[0]["outside_output_unchanged"]==true&&result[0]["output_z_exact"]==true&&result[0]["control_unchanged"]==true&&result[0]["mxcsr_control_unchanged"]==true;
        passed&=equal;failures+=!equal;
        write({{"event","comparison"},{"function","CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached"},{"pair",pair++},{"seed",seed},{"alias",alias},{"input_bits",bits},{"z_bits",zBits},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
    }
    hook->State(enabled);hook->LockState(locked);
    std::fesetenv(&caller);_mm_setcsr(callerMxcsr);
    unsigned restoredControl{};_controlfp_s(&restoredControl,0,0);
    const bool restored=SmallPendingSupport::HooksUnchanged(hooks)&&originalEntry==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&candidateEntry==ReversingProbe::Bytes(hook->GetHookOurAddress(),52)&&constantBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x858C84),4)&&restoredControl==callerControl&&_mm_getcsr()==callerMxcsr;
    write({{"event","function_summary"},{"function","CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached"},{"cases",pair},{"failures",failures}});
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"fp_environment_restored",restoredControl==callerControl&&_mm_getcsr()==callerMxcsr}});
}
}
