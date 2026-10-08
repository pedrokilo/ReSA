#pragma once
#include "LiveComparison.hpp"
#include <map>
#include <cfenv>
#include <xmmintrin.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "VTableRanges.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPreflightPoolsSixGettersLive {
struct Range {const void* ptr;std::vector<uint8> data;};
inline void Update() {
    using namespace ReversibleHooks::ReversibleHook;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();
    std::ofstream log(dir/"preflight-pools-six-getters-v2-harness.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    write({{"event","preflight_entered"},{"graphics_initialized",false}});
    const auto flags=SmallPendingSupport::SnapshotHooks();
    const auto timerBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB40),96);
    std::fenv_t initialFP{};std::fegetenv(&initialFP);const auto initialMXCSR=_mm_getcsr();
    std::vector<Range> entries;
    for(const auto& state:flags) {
        void* gta{};void* candidate{};
        if(state.hook->Type()==Base::HookType::Simple){auto* hook=static_cast<Simple*>(state.hook);gta=hook->GetHookGTAAddress();candidate=hook->GetHookOurAddress();}
        else if(state.hook->Type()==Base::HookType::Virtual){auto* hook=static_cast<Virtual*>(state.hook);gta=hook->GetHookGTAAddress();candidate=hook->GetHookOurAddress();}
        if(gta)entries.push_back({gta,ReversingProbe::Bytes(gta,52)});
        if(candidate)entries.push_back({candidate,ReversingProbe::Bytes(candidate,52)});
    }
    bool tablesKnown=true;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){
        if(!std::ranges::any_of(cat.Items(),[](const auto& item){return item->Type()==Base::HookType::Virtual;}))return;
        const auto found=vtableRanges.find(std::string(cat.Name()));
        if(found==vtableRanges.end()){tablesKnown=false;write({{"event","unknown_vtable_category"},{"category",cat.Name()}});return;}
        const auto [gta,count]=found->second;auto* native=ReversibleHooks::detail::GetVTableAddress(cat.Name());
        entries.push_back({reinterpret_cast<void*>(gta),ReversingProbe::Bytes(reinterpret_cast<void*>(gta),count*sizeof(void*))});
        entries.push_back({native,ReversingProbe::Bytes(native,count*sizeof(void*))});
    });
    if(!tablesKnown){write({{"event","error"},{"reason","unknown virtual class metadata"},{"state_restored",true}});return;}
    for(const auto& state:flags) {
        // Keep the entry currently executing active; all other hooks use original code.
        if(state.hook->Name()!="NOTSA_WinMain") {
            state.hook->LockState(false);state.hook->State(false);
        }
    }
    write({{"event","before_task_bank"},{"hooks_snapshotted",flags.size()}});
    const auto bankMarker=dir/"requested-pools-six-getters-v2-runtime.start";
    {std::ofstream marker(bankMarker);marker<<"pregraphics task bank\n";}
    RequestedPoolsSixGettersV2Live::Update();
    std::filesystem::remove(bankMarker);
    write({{"event","task_bank_returned"}});
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    std::fenv_t bankFP{};std::fegetenv(&bankFP);const auto bankMXCSR=_mm_getcsr();
    const bool bankFPUnchanged=std::memcmp(&initialFP,&bankFP,sizeof(initialFP))==0&&initialMXCSR==bankMXCSR;
    std::fesetenv(&initialFP);_mm_setcsr(initialMXCSR);
    std::fenv_t finalFP{};std::fegetenv(&finalFP);
    const bool fpRestored=std::memcmp(&initialFP,&finalFP,sizeof(initialFP))==0&&initialMXCSR==_mm_getcsr();
    const bool timerRestored=timerBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB40),96);
    bool entriesRestored=true;
    json changed=json::array();
    for(const auto& range:entries)if(range.data!=ReversingProbe::Bytes(range.ptr,range.data.size())){entriesRestored=false;changed.push_back(uintptr_t(range.ptr));}
    const bool flagsRestored=SmallPendingSupport::HooksUnchanged(flags);
    write({{"event",entriesRestored&&flagsRestored&&fpRestored&&timerRestored?"complete":"error"},{"state_restored",entriesRestored&&flagsRestored&&fpRestored&&timerRestored},{"entries_restored",entriesRestored},{"flags_restored",flagsRestored},{"floating_environment_restored",fpRestored},{"bank_fp_unchanged",bankFPUnchanged},{"timer_unchanged",timerRestored},{"initial_fenv",ReversingProbe::Bytes(&initialFP,sizeof(initialFP))},{"bank_fenv",ReversingProbe::Bytes(&bankFP,sizeof(bankFP))},{"initial_mxcsr",initialMXCSR},{"bank_mxcsr",bankMXCSR},{"snapshot_ranges",entries.size()},{"changed_ranges",changed}});
}
}
