#pragma once
#include <fstream>
#include "Stats.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesStats1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush(); }
struct RegisterResult { float value; uint32 ecx, edx; bool stack; };
inline RegisterResult CallEntry(uintptr_t entry, int abilitySelector, uint32 ecxSeed, uint32 edxSeed) {
    float returned;
    uint32 ecxAfter, edxAfter, stackBefore, stackAfter;
    __asm {
        mov stackBefore,esp
        push abilitySelector
        mov ecx,ecxSeed
        mov edx,edxSeed
        call entry
        mov ecxAfter,ecx
        mov edxAfter,edx
        fstp returned
        add esp,4
        mov stackAfter,esp
    }
    return {returned,ecxAfter,edxAfter,stackBefore==stackAfter};
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesstats1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesstats1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesstats1");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    constexpr uintptr_t address=0x559AF0;
    OriginalDependencies499::HookRoute route("CStats","GetFatAndMuscleModifier",address,&CStats::GetFatAndMuscleModifier);
    Write({{"event","begin"},{"registered_hook",route.Registered()}});
    const auto originalCode=ReversingProbe::Bytes(reinterpret_cast<void*>(address),5);
    const auto originalFloats=CStats::StatTypesFloat;
    const auto originalInts=CStats::StatTypesInt;
    auto* settings=reinterpret_cast<float*>(0x8CDE58);
    std::array<float,27> originalSettings;
    memcpy(originalSettings.data(),settings,sizeof(originalSettings));
    const auto candidate=route.Body();
    if(candidate>=0x400000 && candidate<0x1000000) {Write({{"event","error"},{"reason","candidate redirects to EXE"}});return;}
    const uint32 savedControl=_controlfp(0,0);
    auto restore=[&] {
        CStats::StatTypesFloat=originalFloats;CStats::StatTypesInt=originalInts;
        memcpy(settings,originalSettings.data(),sizeof(originalSettings));
        _controlfp(savedControl,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_DN);
    };
    bool passed=true;
    int failures=0;
    for(int scenario=0;scenario<4096;++scenario) {
        auto invoke=[&](bool hooked) {
            restore();
            const int mods[]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,-1};
            const auto mod=static_cast<eStatModAbilities>(mods[scenario%18]);
            uint32 seed=uint32(scenario)*2654435761u+0x137AFF01;
            auto random=[&]{seed=seed*1664525u+1013904223u;return seed;};
            const float edge[]{0.f,-0.f,399.99997f,400.f,400.00003f,799.99994f,800.f,800.00006f,1000.f,-100.f,
                std::bit_cast<float>(0x7FC01234u),std::bit_cast<float>(0x7F800000u),std::bit_cast<float>(0xFF800000u)};
            for(auto& value:CStats::StatTypesFloat)value=float(random()%2001);
            for(auto& value:CStats::StatTypesInt)value=int32(random()%3001)-1000;
            CStats::StatTypesFloat[21]=edge[(scenario/18)%13];
            CStats::StatTypesFloat[22]=edge[(scenario/234)%13];
            CStats::StatTypesFloat[23]=float(random()%1601)-200.f;
            CStats::StatTypesFloat[24]=float(random()%1601);
            if(scenario&2048)for(int i=0;i<27;++i)settings[i]=float(int(random()%2001)-1000)*.01f;
            const auto floatBefore=CStats::StatTypesFloat;
            const auto intBefore=CStats::StatTypesInt;
            std::array<float,27> settingsBefore;memcpy(settingsBefore.data(),settings,sizeof(settingsBefore));
            _controlfp(_PC_24,_MCW_PC);
            const uint32 ecxSeed=0x137AFF01u^uint32(scenario)*1664525u;
            const uint32 edxSeed=0xC0DE5921u^uint32(scenario)*1013904223u;
            bool routeOk{};
            const auto result=route.Invoke(hooked,[&] {
                routeOk=ReversingProbe::JumpsToDll(address)==hooked;
                return CallEntry(address,int(mod),ecxSeed,edxSeed);
            });
            const bool unchanged=memcmp(floatBefore.data(),CStats::StatTypesFloat.data(),sizeof(floatBefore))==0 &&
                memcmp(intBefore.data(),CStats::StatTypesInt.data(),sizeof(intBefore))==0 &&
                memcmp(settingsBefore.data(),settings,sizeof(settingsBefore))==0;
            const json value={{"return_bits",std::bit_cast<uint32>(result.value)},{"globals_unchanged",unchanged},{"route_ok",routeOk},{"ecx",result.ecx},{"edx",result.edx},{"registers_preserved",result.ecx==ecxSeed&&result.edx==edxSeed},{"stack_restored",result.stack}};
            restore();return value;
        };
        const auto original=invoke(false),rebuilt=invoke(true);
        const bool ok=original==rebuilt&&original["route_ok"]==true&&original["globals_unchanged"]==true&&original["registers_preserved"]==true&&original["stack_restored"]==true;
        passed&=ok;failures+=!ok;
        Write({{"event","comparison"},{"function","CStats::GetFatAndMuscleModifier"},{"pair",scenario},{"case",scenario},
            {"original",original},{"candidate",rebuilt},{"equal",ok},{"accepted",ok}});
    }
    restore();
    const bool offStateRestored=memcmp(originalFloats.data(),CStats::StatTypesFloat.data(),sizeof(originalFloats))==0 &&
        memcmp(originalInts.data(),CStats::StatTypesInt.data(),sizeof(originalInts))==0 &&
        memcmp(originalSettings.data(),settings,sizeof(originalSettings))==0 &&
        originalCode==ReversingProbe::Bytes(reinterpret_cast<void*>(address),5)&&_controlfp(0,0)==savedControl;
    const bool hookRestored=route.Restore();
    const bool restored=offStateRestored&&hookRestored;
    Write({{"event","function_summary"},{"function","CStats::GetFatAndMuscleModifier"},{"cases",4096},{"failures",failures}});
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",4096},{"state_restored",restored}});
}
}
