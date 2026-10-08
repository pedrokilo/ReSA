#pragma once
#include "../RequestedParallelReplay/WanderType/LiveComparison.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPreflightWanderLive {
inline void Update() {
    using namespace ReversibleHooks::ReversibleHook;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();
    std::ofstream log(dir/"preflight-wander-harness.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    write({{"event","preflight_entered"},{"graphics_initialized",false}});
    const auto flags=SmallPendingSupport::SnapshotHooks();
    std::vector<RequestedWanderTypeLive::Range> entries;
    for(const auto& state:flags) {
        void* gta{};void* candidate{};
        if(state.hook->Type()==Base::HookType::Simple){auto* hook=static_cast<Simple*>(state.hook);gta=hook->GetHookGTAAddress();candidate=hook->GetHookOurAddress();}
        else if(state.hook->Type()==Base::HookType::Virtual){auto* hook=static_cast<Virtual*>(state.hook);gta=hook->GetHookGTAAddress();candidate=hook->GetHookOurAddress();}
        if(gta)entries.push_back({gta,ReversingProbe::Bytes(gta,52)});
        if(candidate)entries.push_back({candidate,ReversingProbe::Bytes(candidate,52)});
    }
    for(const auto& state:flags) {
        // Keep the entry currently executing active; all other hooks use original code.
        if(state.hook->Name()!="NOTSA_WinMain") {
            state.hook->LockState(false);state.hook->State(false);
        }
    }
    write({{"event","before_getter_bank"},{"hooks_snapshotted",flags.size()}});
    RequestedWanderTypeLive::Update();
    write({{"event","getter_bank_returned"}});
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    bool entriesRestored=true;
    json changed=json::array();
    for(const auto& range:entries)if(range.bytes!=ReversingProbe::Bytes(range.address,range.bytes.size())){entriesRestored=false;changed.push_back(uintptr_t(range.address));}
    const bool flagsRestored=SmallPendingSupport::HooksUnchanged(flags);
    write({{"event",entriesRestored&&flagsRestored?"complete":"error"},{"state_restored",entriesRestored&&flagsRestored},{"entries_restored",entriesRestored},{"flags_restored",flagsRestored},{"snapshot_ranges",entries.size()},{"changed_ranges",changed}});
}
}
