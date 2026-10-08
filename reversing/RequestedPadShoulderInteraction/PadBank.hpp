#pragma once
#include <Pad.h>
#include <Timer.h>
#include <fstream>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPadShoulderInteractionLive {
using Simple=ReversibleHooks::ReversibleHook::Simple;
struct Range {void* ptr;std::vector<uint8_t> data;};
struct Fixture {CPad pad;int mode,truth,pressure;bool disabled;};
inline std::vector<Fixture> Fixtures(){
    std::vector<Fixture> out;
    for(int mode=0;mode<4;++mode)for(bool disabled:{false,true})for(int pressure:{0,1,128,255})for(int truth=0;truth<16;++truth){
        auto pad=CPad::Pads[0];std::memset(&pad,0,sizeof(pad));pad.Mode=mode;pad.DisablePlayerControls=disabled?1:0;
        pad.NewState.LeftShoulder2=(truth&1)?pressure:0;pad.OldState.LeftShoulder2=(truth&2)?pressure:0;
        pad.NewState.RightShoulder2=(truth&4)?pressure:0;pad.OldState.RightShoulder2=(truth&8)?pressure:0;
        out.push_back({pad,mode,truth,pressure,disabled});
    }
    return out;
}
inline void Update(){
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-pad-shoulder-runtime.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    Simple* hook{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* s=static_cast<Simple*>(h.get());if(uintptr_t(s->GetHookGTAAddress())==0x53FE10)hook=s;}});
    if(!hook){write({{"event","error"},{"reason","GetLookRight exact hook missing"}});return;}
    const auto fixtures=Fixtures();const bool enabled=hook->Hooked(),locked=hook->Locked();
    const auto gta=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),candidate=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    std::vector<Range> globals;
    for(auto [address,size]:std::vector<std::pair<uintptr_t,size_t>>{{0xB73458,sizeof(CPad::Pads)},{0xB736F0,14},{0xB7CB84,4}})globals.push_back({reinterpret_cast<void*>(address),ReversingProbe::Bytes(reinterpret_cast<void*>(address),size)});
    hook->LockState(false);write({{"event","start"},{"function","GetLookRight"},{"address",0x53FE10},{"cases",fixtures.size()}});
    for(size_t seed=0;seed<fixtures.size();++seed){const auto& f=fixtures[seed];json results=json::array();bool switched=true,globalsUnchanged=true;
        for(bool useCandidate:{false,true}){
            auto pad=f.pad;hook->State(useCandidate);switched&=SmallPendingSupport::ExactSwitch(*hook,useCandidate);
            auto* entry=useCandidate?hook->GetHookOurAddress():hook->GetHookGTAAddress();
            const bool value=reinterpret_cast<bool(__thiscall*)(CPad*)>(entry)(&pad);
            for(const auto& range:globals)globalsUnchanged &= range.data==ReversingProbe::Bytes(range.ptr,range.data.size());
            results.push_back({{"return_al_bool",value},{"pad",ReversingProbe::Bytes(&pad,sizeof(pad))}});
        }
        write({{"event","comparison"},{"function","GetLookRight"},{"address",0x53FE10},{"seed",seed},{"mode",f.mode},{"disabled",f.disabled},{"pressure",f.pressure},{"truth_mask",f.truth},{"switch_exact",switched},{"globals_unchanged",globalsUnchanged},{"equal",results[0]==results[1]},{"original",results[0]},{"candidate",results[1]}});
    }
    hook->State(enabled);hook->LockState(locked);
    for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());
    bool restored=hook->Hooked()==enabled&&hook->Locked()==locked&&gta==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&candidate==ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    for(const auto& range:globals)restored &= range.data==ReversingProbe::Bytes(range.ptr,range.data.size());
    write({{"event",restored?"complete":"error"},{"state_restored",restored},{"cases",fixtures.size()}});
}
}
