#pragma once
#include <fstream>
#include "PedGroups.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesPedGroups1Live {
inline bool done{};
inline std::ofstream log;
struct Fn { const char* cls; const char* name; uintptr_t address,vtable; int slot,cases; };
inline constexpr Fn fns[]{{"CPedGroups","GetPedsGroup",0x5F7E80,0,-1,1024}};
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir/"depspedgroups1-runtime.start")) { done=true; return; }
    if (!log.is_open()) { log.open(dir/"depspedgroups1-runtime.jsonl"); Write({{"event","waiting_for_player"}}); ReversingProbe::InstallFailureCapture("depspedgroups1"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done=true;
    OriginalDependencies499::HookRoute routeControl("CPedGroups","GetPedsGroup",0x5F7E80,&CPedGroups::GetPedsGroup);
    const auto savedActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,sizeof(CPedGroups::ms_activeGroups));
    const auto savedGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
    const auto savedEntry=ReversingProbe::Bytes(reinterpret_cast<void*>(0x5F7E80),8);
    bool passed=!ReversingProbe::JumpsToDll(0x5F7E80);
    Write({{"event","begin"},{"known",passed},{"registered_hook",routeControl.Registered()},{"candidate_route","rh_or_temporary_entry_patch"},{"original_route","unpatched_exe_entry"},{"layout",sizeof(CPedGroup)==0x2D4}});
    unsigned failures=0;
    for (unsigned i=0; i<1024; ++i) {
        const unsigned mask=i&255, mode=i>>8, group=(mask>>3)&7, member=mask&7;
        const auto* needle=reinterpret_cast<const CPed*>(0x12345000);
        if (mode==0) needle=nullptr;
        auto run=[&](bool candidate) {
            // EXE only compares pointer values: tokens are never dereferenced.
            std::memset(&CPedGroups::ms_groups,0xA7,sizeof(CPedGroups::ms_groups));
            for(unsigned g=0;g<8;++g) {
                CPedGroups::ms_activeGroups[g]=(mask&(1u<<g)) ? char((i&1)?0x80:1) : 0;
                for(unsigned m=0;m<8;++m) {
                    const uintptr_t token=(m==member && (mode==3 || g==group) && mode!=1) ? uintptr_t(needle) : 0;
                    std::memcpy(reinterpret_cast<uint8*>(&CPedGroups::ms_groups[g])+0xC+m*4,&token,4);
                }
            }
            const auto beforeActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,sizeof(CPedGroups::ms_activeGroups));
            const auto beforeGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
            CPedGroup* ret{};
            bool route{};
            ret=routeControl.Invoke(candidate,[&] {
                route=ReversingProbe::JumpsToDll(0x5F7E80)==candidate;
                return reinterpret_cast<CPedGroup*(__cdecl*)(const CPed*)>(0x5F7E80)(needle);
            });
            return json{{"return",uintptr_t(ret)},{"route_ok",route},{"active_unchanged",beforeActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,sizeof(CPedGroups::ms_activeGroups))},{"groups_unchanged",beforeGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups))}};
        };
        const auto original=run(false), candidate=run(true);
        const bool equal=passed && original==candidate && original["route_ok"]==true && original["active_unchanged"]==true && original["groups_unchanged"]==true;
        failures+=!equal;
        Write({{"event","comparison"},{"pair",i},{"function","CPedGroups::GetPedsGroup"},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate},{"fixture",{{"mask",mask},{"mode",mode},{"group",group},{"member",member}}}});
    }
    std::memcpy(&CPedGroups::ms_activeGroups,savedActive.data(),savedActive.size());
    std::memcpy(&CPedGroups::ms_groups,savedGroups.data(),savedGroups.size());
    const bool restored=savedActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,savedActive.size()) && savedGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,savedGroups.size()) && savedEntry==ReversingProbe::Bytes(reinterpret_cast<void*>(0x5F7E80),8) && routeControl.Restore();
    Write({{"event","function_summary"},{"function","CPedGroups::GetPedsGroup"},{"failures",failures},{"cases",1024}});
    Write({{"event",passed && !failures && restored ? "complete":"error"},{"pairs",1024},{"state_restored",restored}});
}
}
