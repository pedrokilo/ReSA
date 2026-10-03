#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace GeneralLive {
inline bool done{};inline std::ofstream log;inline unsigned randomValue{},randomCalls{};
inline int __cdecl Random(){++randomCalls;return int(randomValue);}
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int Report(int t,char* msg,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});log.flush();ExitProcess(91);}
inline uint32 Run(float lo,float hi,std::array<uint8,10>& extended){
    randomCalls=0;float value;uint8 raw[10];
    __asm {
        push hi
        push lo
        mov eax, 0x41BD90
        call eax
        add esp, 8
        fstp tbyte ptr raw
        fld tbyte ptr raw
        fstp value
    }
    std::memcpy(extended.data(),raw,10);return std::bit_cast<uint32>(value);
}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"general-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"general-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CGeneral")for(auto& h:c.Items())if(h->Name()=="GetRandomNumberInRange<float>-")hook=h.get();});
    if(!hook){Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;}
    auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    Write({{"event","hook_state"},{"enabled",enabled},{"locked",locked},{"calls_before_bank",GeneralProbeCalls}});
    constexpr std::array<std::array<float,2>,16> bounds{{
        {0.f,1.f},{0.f,-1.f},{-100.f,100.f},{100.f,-100.f},
        {0.f,0.f},{1.f,1.f},{-0.f,0.f},{0.f,-0.f},
        {0.00001f,-0.00003f},{-123.456f,789.012f},{1.f,1.00000011920928955078125f},
        {1e20f,-1e20f},{1e-30f,-1e-30f},{-5000.f,-1.f},{5000.f,0.f},{-3.402823466e38f,3.402823466e38f}
    }};
    const auto fpControl=_controlfp(0,0);
    bool passed=true;int pairs=0;uint32 maxUlp{};int different{};
    {
        ReversingProbe::EntryPatch exeRng(0x821B1E,Random),dllRng(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
        for(int i=0;i<65536&&passed;++i){
            auto ends=bounds[i/4096];randomValue=i%4096==0?0:i%4096==1?32767:unsigned(i*7919)&32767;
            std::array<uint8,10> extended{};
            const auto run=[&]{return Run(ends[0],ends[1],extended);};
            hook->State(false);auto a=GeneralProbeCalls;auto original=run();auto originalExtended=extended;auto originalRng=randomCalls;auto b=GeneralProbeCalls;
            hook->State(true);auto candidate=run();auto candidateRng=randomCalls;auto c=GeneralProbeCalls;
            const auto ordered=[](uint32 bits){return bits&0x80000000u?~bits:bits|0x80000000u;};
            auto x=ordered(original),y=ordered(candidate);auto ulp=x>y?x-y:y-x;maxUlp=std::max(maxUlp,ulp);different+=original!=candidate;
            passed=originalExtended==extended&&ulp==0&&a==b&&c==b+1&&originalRng==1&&candidateRng==1;
            Write({{"event","comparison"},{"case",i},{"bounds",ends},{"rng",randomValue},{"original_bits",original},{"candidate_bits",candidate},{"original_extended",originalExtended},{"candidate_extended",extended},{"ulp",ulp},{"equal",original==candidate},{"passed",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b},{"original_rng_calls",originalRng},{"candidate_rng_calls",candidateRng}});
            ++pairs;
        }
    }
    hook->State(enabled);hook->LockState(locked);
    bool restored=fpControl==_controlfp(0,0)&&playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&hook->Hooked()==enabled&&hook->Locked()==locked;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"different",different},{"max_ulp",maxUlp},{"fp_control",fpControl},{"state_restored",restored}});log.flush();_CrtSetReportHook(report);
}
}
