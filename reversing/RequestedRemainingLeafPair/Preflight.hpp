#pragma once
#include "LiveComparison.hpp"
#include <map>
#include "VTableRanges.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPreflightRemainingLeafLive {
struct Range {const void* ptr;std::vector<uint8> data;};
inline void Update() {
    using namespace ReversibleHooks::ReversibleHook;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();
    std::ofstream log(dir/"preflight-remaining-leaf-pair-harness.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    write({{"event","preflight_entered"},{"graphics_initialized",false}});
    const auto flags=SmallPendingSupport::SnapshotHooks();
    const auto timerBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB7C),12);
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
    RequestedRemainingLeafPair::Update();
    write({{"event","task_bank_returned"}});
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    bool entriesRestored=true;
    json changed=json::array();
    for(const auto& range:entries)if(range.data!=ReversingProbe::Bytes(range.ptr,range.data.size())){entriesRestored=false;changed.push_back(uintptr_t(range.ptr));}
    const bool timerRestored=timerBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB7C),12);
    const bool flagsRestored=SmallPendingSupport::HooksUnchanged(flags);
    write({{"event",entriesRestored&&flagsRestored&&timerRestored?"complete":"error"},{"state_restored",entriesRestored&&flagsRestored&&timerRestored},{"entries_restored",entriesRestored},{"flags_restored",flagsRestored},{"timer_restored",timerRestored},{"tables_restored",entriesRestored},{"snapshot_ranges",entries.size()},{"changed_ranges",changed}});
}
}
