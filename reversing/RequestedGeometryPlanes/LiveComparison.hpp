#pragma once
#include <fstream>
#include <cfenv>
#include <float.h>
#include <xmmintrin.h>
#include "PedGeometryAnalyser.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGeometryPlanesLive {
using namespace ReversibleHooks::ReversibleHook;
inline bool done{};
struct Fixture { std::array<uint32,4> prefix; CVector corners[4]; CVector planes[4]; std::array<float,4> dots; std::array<uint32,4> suffix; };
inline void Update(){
    if(done)return;done=true;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();if(!std::filesystem::exists(dir/"requested-geometry-planes-runtime.start"))return;
    std::ofstream log(dir/"requested-geometry-planes-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    const auto flags=SmallPendingSupport::SnapshotHooks();Simple* target{};Simple* normalise{};
    for(const auto&f:flags)if(f.hook->Type()==Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);if(h->GetHookGTAAddress()==reinterpret_cast<void*>(0x5F1670)&&h->Name()=="ComputeEntityBoundingBoxPlanesUncached")target=h;if(h->GetHookGTAAddress()==reinterpret_cast<void*>(0x59C910)&&h->Name()=="Normalise")normalise=h;}
    if(!target||!normalise){write({{"event","error"},{"reason","exact target/native Normalise collaborator missing"}});return;}
    struct Range{const void* address;std::vector<uint8> bytes;};std::vector<Range> entries;
    for(auto*h:{target,normalise})for(auto*address:{h->GetHookGTAAddress(),h->GetHookOurAddress()})entries.push_back({address,ReversingProbe::Bytes(address,52)});
    const auto zeroBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x858B50),4),oneBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x858624),4);
    fenv_t caller{};std::fegetenv(&caller);const uint32 callerMxcsr=_mm_getcsr();unsigned callerControl{};_controlfp_s(&callerControl,0,0);
    unsigned canonicalControl{};_controlfp_s(&canonicalControl,_CW_DEFAULT,_MCW_EM|_MCW_RC|_MCW_PC);_mm_setcsr(0x1F80);std::feclearexcept(FE_ALL_EXCEPT);_controlfp_s(&canonicalControl,0,0);fenv_t canonical{};std::fegetenv(&canonical);
    write({{"event","fixture"},{"caller_control",callerControl},{"canonical_control",canonicalControl},{"caller_mxcsr",callerMxcsr},{"normalise","native59C910 shared by both sides; helper itself not accredited"},{"scope","finite planar and sloped quadrilaterals; output separate or exact corners array alias"}});
    normalise->LockState(false);normalise->State(false);target->LockState(false);bool passed=true;int pair{},failures{},separateFailures{},aliasFailures{};
    for(int seed=0;seed<1024;++seed)for(int alias=0;alias<2;++alias){
        Fixture initial{};initial.prefix.fill(0x13579BDF);initial.suffix.fill(0x2468ACE0);for(auto&v:initial.planes)v=CVector{17.f,-19.f,23.f};initial.dots.fill(29.f);
        const float cx=float(seed%31-15)*.25f,cy=float(seed%29-14)*.125f;
        const float ux=float(seed%17-8)*.125f,uy=float(seed%13-6)*.25f,vx=float(seed%11-5)*.0625f,vy=float(seed%19-9)*.125f;
        const float z=float(seed%23-11)*.125f,dz=seed%4==0?float(seed%7-3)*.25f:0.f;
        initial.corners[0]=CVector{cx-ux-vx,cy-uy-vy,z};initial.corners[1]=CVector{cx+ux-vx,cy+uy-vy,z+dz};initial.corners[2]=CVector{cx+ux+vx,cy+uy+vy,z};initial.corners[3]=CVector{cx-ux+vx,cy-uy+vy,z-dz};
        Fixture sides[2]{initial,initial};json result[2];
        for(int side=0;side<2;++side){
            auto&state=sides[side];auto*out=alias?&state.corners:&state.planes;
            target->State(side!=0);const bool gate=SmallPendingSupport::ExactSwitch(*target,side!=0)&&SmallPendingSupport::ExactSwitch(*normalise,false);
            std::fesetenv(&canonical);_mm_setcsr(0x1F80);
            using Fn=void(__cdecl*)(float,const CVector*,CVector(*)[4],float*);
            reinterpret_cast<Fn>(0x5F1670)(z,state.corners,out,state.dots.data());
            bool untouched=state.prefix==initial.prefix&&state.suffix==initial.suffix;
            untouched&=alias?ReversingProbe::Bytes(state.planes,48)==ReversingProbe::Bytes(initial.planes,48):ReversingProbe::Bytes(state.corners,48)==ReversingProbe::Bytes(initial.corners,48);
            bool zeroZ=true;for(const auto&v:*out)zeroZ&=std::bit_cast<uint32>(v.z)==0;
            unsigned control{};_controlfp_s(&control,0,0);
            result[side]={{"arena",ReversingProbe::Bytes(&state,sizeof(state))},{"normals",ReversingProbe::Bytes(out,48)},{"dots",ReversingProbe::Bytes(state.dots.data(),16)},{"gate",gate},{"outside_output_unchanged",untouched},{"plane_z_pluszero",zeroZ},{"control_unchanged",control==canonicalControl},{"mxcsr_control_unchanged",(_mm_getcsr()&~0x3Fu)==0x1F80u}};
        }
        const bool equal=result[0]==result[1]&&result[0]["gate"]==true&&result[0]["outside_output_unchanged"]==true&&result[0]["plane_z_pluszero"]==true&&result[0]["control_unchanged"]==true&&result[0]["mxcsr_control_unchanged"]==true;
        passed&=equal;failures+=!equal;separateFailures+=!equal&&!alias;aliasFailures+=!equal&&alias;
        write({{"event","comparison"},{"function","CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached"},{"pair",pair++},{"seed",seed},{"alias",alias},{"input_corners",ReversingProbe::Bytes(initial.corners,48)},{"z_bits",std::bit_cast<uint32>(z)},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});
    }
    for(const auto&f:flags)if(f.hook==target||f.hook==normalise){f.hook->LockState(false);f.hook->State(f.enabled);f.hook->LockState(f.locked);}
    std::fesetenv(&caller);_mm_setcsr(callerMxcsr);unsigned restoredControl{};_controlfp_s(&restoredControl,0,0);
    bool entriesRestored=true;for(const auto&r:entries)entriesRestored&=r.bytes==ReversingProbe::Bytes(r.address,r.bytes.size());
    const bool globals=zeroBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x858B50),4)&&oneBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x858624),4);
    const bool fp=restoredControl==callerControl&&_mm_getcsr()==callerMxcsr,restored=globals&&fp&&entriesRestored&&SmallPendingSupport::HooksUnchanged(flags);
    write({{"event","function_summary"},{"function","CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached"},{"cases",pair},{"failures",failures},{"separate_failures",separateFailures},{"alias_failures",aliasFailures}});
    write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"entries_restored",entriesRestored},{"globals_restored",globals},{"fp_environment_restored",fp}});
}
}
