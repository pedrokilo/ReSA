#pragma once
#include <bit>
#include <map>
#include <new>
using std::bit_cast;
#include "../RequestedAdditionalLifetimes/LiveComparison.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
#include "../ProbeRuntime.hpp"
namespace RequestedPreflightAdditionalLive {
inline void Update(){
    using namespace ReversibleHooks::ReversibleHook;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);const auto dir=std::filesystem::path(executable).parent_path();
    std::ofstream log(dir/"preflight-additional-harness.jsonl");const auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    write({{"event","preflight_entered"},{"graphics_initialized",false},{"bank","additional-lifetime"}});
    const auto flags=SmallPendingSupport::SnapshotHooks();
    std::vector<std::pair<void*,std::vector<uint8>>> entries,tables;
    for(const auto& state:flags){void* gta{};void* candidate{};
        if(state.hook->Type()==Base::HookType::Simple){auto* hook=static_cast<Simple*>(state.hook);gta=hook->GetHookGTAAddress();candidate=hook->GetHookOurAddress();}
        else if(state.hook->Type()==Base::HookType::Virtual){auto* hook=static_cast<Virtual*>(state.hook);gta=hook->GetHookGTAAddress();candidate=hook->GetHookOurAddress();}
        if(gta)entries.emplace_back(gta,ReversingProbe::Bytes(gta,52));if(candidate)entries.emplace_back(candidate,ReversingProbe::Bytes(candidate,52));
    }
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    const auto captureTable=[&](void* address,size_t count){if(address)tables.emplace_back(address,ReversingProbe::Bytes(address,count*4));};
    captureTable(reinterpret_cast<void*>(GetProcAddress(module,"??_7CTaskSimpleFightingControl@@6B@")),9);
    captureTable(reinterpret_cast<void*>(GetProcAddress(module,"??_7CTaskComplexProstituteSolicit@@6B@")),11);
    captureTable(reinterpret_cast<void*>(0x86D700),9);captureTable(reinterpret_cast<void*>(0x86FB88),11);
    const auto assertionEntry=reinterpret_cast<void*>(&_wassert);
    const auto assertionBytes=ReversingProbe::Bytes(assertionEntry,52);
    // EntryPatch owns exactly its saved five-byte jump span. Preserve the wider
    // diagnostic snapshot too: adjacent changes remain a failure until explained.
    const auto assertionOwnedBefore=ReversingProbe::Bytes(assertionEntry,5);
    MEMORY_BASIC_INFORMATION assertionMemory{};
    const auto assertionQuery=VirtualQuery(assertionEntry,&assertionMemory,sizeof(assertionMemory));
    HMODULE assertionModule{};
    const bool assertionModuleFound=GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(assertionEntry),&assertionModule)!=0;
    char assertionModulePath[MAX_PATH]{};
    if(assertionModuleFound)GetModuleFileNameA(assertionModule,assertionModulePath,MAX_PATH);
    const auto byteHex=[](const std::vector<uint8>& bytes){
        constexpr char digits[]="0123456789abcdef";std::string out;out.reserve(bytes.size()*2);
        for(auto byte:bytes){out+=digits[byte>>4];out+=digits[byte&15];}return out;
    };
    write({{"event","crt_assertion_snapshot"},{"address",reinterpret_cast<uintptr_t>(assertionEntry)},{"owned_span_bytes",5},{"diagnostic_span_bytes",52},{"before_hex52",byteHex(assertionBytes)},{"module_found",assertionModuleFound},{"module_path",assertionModulePath},{"module_base",reinterpret_cast<uintptr_t>(assertionModule)},{"virtual_query_bytes",assertionQuery},{"region_base",reinterpret_cast<uintptr_t>(assertionMemory.BaseAddress)},{"allocation_base",reinterpret_cast<uintptr_t>(assertionMemory.AllocationBase)},{"region_size",assertionMemory.RegionSize},{"protection",assertionMemory.Protect},{"memory_type",assertionMemory.Type},{"memory_state",assertionMemory.State}});
    for(const auto& state:flags)if(state.hook->Name()!="NOTSA_WinMain"){state.hook->LockState(false);state.hook->State(false);}
    const auto oldLog=ReversingProbe::failureLog;ReversingProbe::failureLog=dir/"additional-lifetime-runtime.jsonl";
    const auto oldMode=_set_error_mode(_REPORT_ERRMODE);
    const auto oldAssertMode=_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_REPORT_MODE);
    const auto oldErrorMode=_CrtSetReportMode(_CRT_ERROR,_CRTDBG_REPORT_MODE);
    _set_error_mode(_OUT_TO_STDERR);
    _CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);
    _CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);
    const auto oldRtc=_RTC_SetErrorFuncW(ReversingProbe::RuntimeFailure);
    const auto oldException=SetUnhandledExceptionFilter(ReversingProbe::ExceptionFailure);
    const auto vectored=AddVectoredExceptionHandler(1,ReversingProbe::FirstChanceAV);
    {
        ReversingProbe::EntryPatch assertion{reinterpret_cast<uintptr_t>(assertionEntry),ReversingProbe::AssertFailure};
        write({{"event","before_lifetime_bank"},{"hooks_snapshotted",flags.size()},{"vrt_tables",tables.size()}});
        RequestedAdditionalLifetimeLive::Update();
        write({{"event","lifetime_bank_returned"}});
    }
    const bool vectoredRemoved=vectored&&RemoveVectoredExceptionHandler(vectored)!=0;
    SetUnhandledExceptionFilter(oldException);_RTC_SetErrorFuncW(oldRtc);
    _CrtSetReportMode(_CRT_ASSERT,oldAssertMode);_CrtSetReportMode(_CRT_ERROR,oldErrorMode);_set_error_mode(oldMode);ReversingProbe::failureLog=oldLog;
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    const bool entriesRestored=std::ranges::all_of(entries,[](const auto& item){return item.second==ReversingProbe::Bytes(item.first,item.second.size());});
    const bool tablesRestored=tables.size()==4&&std::ranges::all_of(tables,[](const auto& item){return item.second==ReversingProbe::Bytes(item.first,item.second.size());});
    const bool flagsRestored=SmallPendingSupport::HooksUnchanged(flags);
    const auto assertionAfter=ReversingProbe::Bytes(assertionEntry,52);
    const auto assertionOwnedAfter=ReversingProbe::Bytes(assertionEntry,5);
    write({{"event","crt_assertion_after"},{"after_hex52",byteHex(assertionAfter)},{"owned_before_hex5",byteHex(assertionOwnedBefore)},{"owned_after_hex5",byteHex(assertionOwnedAfter)},{"owned_entry_restored",assertionOwnedBefore==assertionOwnedAfter}});
    const auto modeAfter=_set_error_mode(_REPORT_ERRMODE);
    const auto assertModeAfter=_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_REPORT_MODE);
    const auto errorModeAfter=_CrtSetReportMode(_CRT_ERROR,_CRTDBG_REPORT_MODE);
    const bool crtRestored=assertionBytes==assertionAfter&&modeAfter==oldMode&&assertModeAfter==oldAssertMode&&errorModeAfter==oldErrorMode&&vectoredRemoved;
    json assertionOffsets=json::array();for(size_t index=0;index<assertionBytes.size();++index)if(assertionBytes[index]!=assertionAfter[index])assertionOffsets.push_back(index);
    const bool restored=entriesRestored&&tablesRestored&&flagsRestored&&crtRestored;
    write({{"event",restored?"complete":"error"},{"state_restored",restored},{"entries_restored",entriesRestored},{"tables_restored",tablesRestored},{"flags_restored",flagsRestored},{"crt_restored",crtRestored},{"crt_assertion_restored",assertionBytes==assertionAfter},{"crt_assertion_changed_offsets",assertionOffsets},{"crt_error_mode_before",oldMode},{"crt_error_mode_after",modeAfter},{"crt_assert_report_before",oldAssertMode},{"crt_assert_report_after",assertModeAfter},{"crt_error_report_before",oldErrorMode},{"crt_error_report_after",errorModeAfter},{"vectored_handler_removed",vectoredRemoved},{"snapshot_ranges",entries.size()},{"return_to_winmain",true}});
}
}
