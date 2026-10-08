#pragma once
#include <fstream>
#include <cfenv>
#include <float.h>
#include <xmmintrin.h>
#include <memory>
#include "NativeNormaliseBytes.hpp"
#include "PedGeometryAnalyser.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGeometryPlanesV2Live {
using namespace ReversibleHooks::ReversibleHook;
inline bool done{};
inline int nativeAdapterCalls{};inline json adapterTrace;
inline void __fastcall NativeAdapter(CVector*self,void*){
    const auto before=ReversingProbe::Bytes(self,12);const int index=nativeAdapterCalls++;
    reinterpret_cast<void(__thiscall*)(CVector*)>(0x59C910)(self);
    adapterTrace.push_back({{"index",index},{"self_address",reinterpret_cast<uintptr_t>(self)},{"input_bytes",before},{"output_bytes",ReversingProbe::Bytes(self,12)}});
}
inline uintptr_t ResolveJump(uintptr_t address){for(int n=0;n<8;++n){if(*reinterpret_cast<const uint8*>(address)!=0xE9)return address;address=address+5+*reinterpret_cast<const int32*>(address+1);}return 0;}
inline bool NativeUnchanged(){return !std::memcmp(reinterpret_cast<void*>(0x59C910),GeometryNativeNormaliseBytes.data(),GeometryNativeNormaliseBytes.size());}
struct Fixture { std::array<uint32,4> prefix; CVector corners[4]; CVector planes[4]; std::array<float,4> dots; std::array<uint32,4> suffix; };
inline void Update(){
    if(done)return;done=true;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();if(!std::filesystem::exists(dir/"requested-geometry-planes-v2-runtime.start"))return;
    std::ofstream log(dir/"requested-geometry-planes-v2-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    const auto flags=SmallPendingSupport::SnapshotHooks();Simple* target{};
    for(const auto&f:flags)if(f.hook->Type()==Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);if(h->GetHookGTAAddress()==reinterpret_cast<void*>(0x5F1670)&&h->Name()=="ComputeEntityBoundingBoxPlanesUncached")target=h;}
    if(!target){write({{"event","error"},{"reason","exact target identity missing"}});return;}
    const uintptr_t normaliseEntry=std::bit_cast<uintptr_t>(&CVector::Normalise),normaliseBody=ResolveJump(normaliseEntry);
    // Resolve candidate body with its original ILT entry active before checking CALLsite.
    const bool targetEnabled=target->Hooked(),targetLocked=target->Locked();
    const auto targetGtaBefore=ReversingProbe::Bytes(target->GetHookGTAAddress(),52),targetDllBefore=ReversingProbe::Bytes(target->GetHookOurAddress(),52);
    target->LockState(false);target->State(true);const uintptr_t planeBody=ResolveJump(reinterpret_cast<uintptr_t>(target->GetHookOurAddress()));
    const std::array<uint8,4> callPrefix{0x8D,0x4D,0xBC,0xE8};
    const bool prefix=planeBody&&!std::memcmp(reinterpret_cast<void*>(planeBody+0x77),callPrefix.data(),4);
    const uintptr_t normaliseCall=prefix?planeBody+0x7A:0;
    const uintptr_t callTarget=prefix?normaliseCall+5+*reinterpret_cast<const int32*>(normaliseCall+1):0;
    const bool callProof=prefix&&normaliseBody&&ResolveJump(callTarget)==normaliseBody&&NativeUnchanged();
    target->State(targetEnabled);target->LockState(targetLocked);
    if(!callProof){write({{"event","error"},{"reason","exact compiled Normalise CALLsite/native readonly proof failed; no helper patch applied"},{"state_restored",SmallPendingSupport::HooksUnchanged(flags)&&targetGtaBefore==ReversingProbe::Bytes(target->GetHookGTAAddress(),52)&&targetDllBefore==ReversingProbe::Bytes(target->GetHookOurAddress(),52)}});return;}
    std::vector<uintptr_t> manualHelpers{normaliseEntry};if(normaliseBody!=normaliseEntry)manualHelpers.push_back(normaliseBody);if(std::find(manualHelpers.begin(),manualHelpers.end(),callTarget)==manualHelpers.end())manualHelpers.push_back(callTarget);
    write({{"event","helper_proof"},{"normalise_entry",normaliseEntry},{"normalise_resolved_body",normaliseBody},{"plane_body",planeBody},{"normalise_callsite",normaliseCall},{"normalise_call_target",callTarget},{"callsite_proof",callProof},{"native_readonly",NativeUnchanged()},{"manual_helpers",manualHelpers},{"registered_Vector_hooks_activated",false}});
    struct Range{const void* address;std::vector<uint8> bytes;};std::vector<Range> entries;
    for(auto*address:{target->GetHookGTAAddress(),target->GetHookOurAddress()})entries.push_back({address,ReversingProbe::Bytes(address,52)});
    for(const auto address:manualHelpers)entries.push_back({reinterpret_cast<void*>(address),ReversingProbe::Bytes(reinterpret_cast<void*>(address),52)});
    const auto zeroBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x858B50),4),oneBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x858624),4);
    fenv_t caller{};std::fegetenv(&caller);const uint32 callerMxcsr=_mm_getcsr();unsigned callerControl{};_controlfp_s(&callerControl,0,0);
    unsigned canonicalControl{};_controlfp_s(&canonicalControl,_CW_DEFAULT,_MCW_EM|_MCW_RC|_MCW_PC);_mm_setcsr(0x1F80);std::feclearexcept(FE_ALL_EXCEPT);_controlfp_s(&canonicalControl,0,0);fenv_t canonical{};std::fegetenv(&canonical);
    write({{"event","fixture"},{"caller_control",callerControl},{"canonical_control",canonicalControl},{"caller_mxcsr",callerMxcsr},{"normalise","native59C910 shared by both sides; helper itself not accredited"},{"scope","finite planar and sloped quadrilaterals; output separate or exact corners array alias"}});
    target->LockState(false);bool passed=true;int pair{},failures{},separateFailures{},aliasFailures{};
    {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;for(auto address:manualHelpers)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,&NativeAdapter));
    auto manualGate=[&](){return std::all_of(patches.begin(),patches.end(),[](const auto&p){return !std::memcmp(reinterpret_cast<void*>(p->address),p->jump.data(),5);});};
    for(int seed=0;seed<1024;++seed)for(int alias=0;alias<2;++alias){
        Fixture initial{};initial.prefix.fill(0x13579BDF);initial.suffix.fill(0x2468ACE0);for(auto&v:initial.planes)v=CVector{17.f,-19.f,23.f};initial.dots.fill(29.f);
        const float cx=float(seed%31-15)*.25f,cy=float(seed%29-14)*.125f;
        const float ux=float(seed%17-8)*.125f,uy=float(seed%13-6)*.25f,vx=float(seed%11-5)*.0625f,vy=float(seed%19-9)*.125f;
        const float z=float(seed%23-11)*.125f,dz=seed%4==0?float(seed%7-3)*.25f:0.f;
        initial.corners[0]=CVector{cx-ux-vx,cy-uy-vy,z};initial.corners[1]=CVector{cx+ux-vx,cy+uy-vy,z+dz};initial.corners[2]=CVector{cx+ux+vx,cy+uy+vy,z};initial.corners[3]=CVector{cx-ux+vx,cy-uy+vy,z-dz};
        Fixture sides[2]{initial,initial};json result[2],adapterTraces[2];int adapterCounts[2]{};
        for(int side=0;side<2;++side){
            auto&state=sides[side];auto*out=alias?&state.corners:&state.planes;
            target->State(side!=0);const bool gate=SmallPendingSupport::ExactSwitch(*target,side!=0)&&NativeUnchanged()&&manualGate();
            std::fesetenv(&canonical);_mm_setcsr(0x1F80);nativeAdapterCalls=0;adapterTrace=json::array();
            using Fn=void(__cdecl*)(float,const CVector*,CVector(*)[4],float*);
            reinterpret_cast<Fn>(0x5F1670)(z,state.corners,out,state.dots.data());
            adapterTraces[side]=adapterTrace;adapterCounts[side]=nativeAdapterCalls;
            const bool adapterGate=nativeAdapterCalls==(side?4:0)&&NativeUnchanged()&&manualGate();
            bool untouched=state.prefix==initial.prefix&&state.suffix==initial.suffix;
            untouched&=alias?ReversingProbe::Bytes(state.planes,48)==ReversingProbe::Bytes(initial.planes,48):ReversingProbe::Bytes(state.corners,48)==ReversingProbe::Bytes(initial.corners,48);
            bool zeroZ=true;for(const auto&v:*out)zeroZ&=std::bit_cast<uint32>(v.z)==0;
            unsigned control{};_controlfp_s(&control,0,0);
            result[side]={{"arena",ReversingProbe::Bytes(&state,sizeof(state))},{"normals",ReversingProbe::Bytes(out,48)},{"dots",ReversingProbe::Bytes(state.dots.data(),16)},{"gate",gate},{"adapter_contract",adapterGate},{"outside_output_unchanged",untouched},{"plane_z_pluszero",zeroZ},{"control_unchanged",control==canonicalControl},{"mxcsr_control_unchanged",(_mm_getcsr()&~0x3Fu)==0x1F80u}};
        }
        const bool equal=result[0]==result[1]&&result[0]["gate"]==true&&result[0]["adapter_contract"]==true&&result[0]["outside_output_unchanged"]==true&&result[0]["plane_z_pluszero"]==true&&result[0]["control_unchanged"]==true&&result[0]["mxcsr_control_unchanged"]==true;
        passed&=equal;failures+=!equal;separateFailures+=!equal&&!alias;aliasFailures+=!equal&&alias;
        write({{"event","comparison"},{"function","CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached"},{"pair",pair++},{"seed",seed},{"alias",alias},{"original_adapter_calls",adapterCounts[0]},{"candidate_adapter_calls",adapterCounts[1]},{"original_adapter_trace",adapterTraces[0]},{"candidate_adapter_trace",adapterTraces[1]},{"input_corners",ReversingProbe::Bytes(initial.corners,48)},{"z_bits",std::bit_cast<uint32>(z)},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
    }
    } // Manual helper patches restore before equality checks of saved entry ranges.
    for(const auto&f:flags)if(f.hook==target){f.hook->LockState(false);f.hook->State(f.enabled);f.hook->LockState(f.locked);}
    std::fesetenv(&caller);_mm_setcsr(callerMxcsr);unsigned restoredControl{};_controlfp_s(&restoredControl,0,0);
    bool entriesRestored=true;for(const auto&r:entries)entriesRestored&=r.bytes==ReversingProbe::Bytes(r.address,r.bytes.size());
    const bool globals=zeroBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x858B50),4)&&oneBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x858624),4);
    const bool fp=restoredControl==callerControl&&_mm_getcsr()==callerMxcsr,restored=NativeUnchanged()&&globals&&fp&&entriesRestored&&SmallPendingSupport::HooksUnchanged(flags);
    write({{"event","function_summary"},{"function","CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached"},{"cases",pair},{"failures",failures},{"separate_failures",separateFailures},{"alias_failures",aliasFailures}});
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"entries_restored",entriesRestored},{"globals_restored",globals},{"manual_helper_entries_restored",entriesRestored},{"native_readonly_unchanged",NativeUnchanged()},{"fp_environment_restored",fp}});
}
}
