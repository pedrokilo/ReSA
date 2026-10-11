#pragma once
#include <fstream>
#include <float.h>
#include "Stats.h"
#include "AudioEngine.h"
#include "AERadioTrackManager.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperHookRoute.hpp"
namespace DependenciesStatsRadio7Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) {ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
struct Fn {const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,2> fns{{
    {"CStats","PopulateFavoriteRadioStationList",0x558EC0,0,-1,2048},
    {"CStats","FindMostFavoriteRadioStation",0x558FA0,0,-1,2048},
}};
static_assert(sizeof(CAERadioTrackManager)==0x370&&offsetof(CAERadioTrackManager,m_aListenTimes)==0x18);
static_assert(sizeof(CAudioEngine)==0x1FD8);
inline constexpr uintptr_t statsBegin=0xB78E48,engineBegin=0xB6BC90,managerBegin=0x8CB6F8;
inline constexpr size_t statsSize=16+14*4+16;
struct ABIResult {bool stack,registers;uint32 returned;};
inline ABIResult CallRaw(uintptr_t address) {
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    uint32 returned{};
    __asm {
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov eax,address
        call eax
        mov returned,eax
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter,ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter,returned};
}
inline int32 Pattern(unsigned v) {
    const std::array<uint32,8> values{0,1,0xFFFFFFFFu,0x80000000u,0x7FFFFFFFu,0x40000000u,0xC0000000u,7};
    if(v<8)return std::bit_cast<int32>(values[v]);
    return std::bit_cast<int32>((v*0x9E3779B9u)^0xA55A5AA5u);
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesstatsradio7-runtime.start"))return;
    if(!log.is_open()) {log.open(dir/"dependenciesstatsradio7-runtime.jsonl");ReversingProbe::InstallFailureCapture("dependenciesstatsradio7");Write({{"event","waiting_for_player"}});}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const std::array<uintptr_t,2> symbols{uintptr_t(&CStats::PopulateFavoriteRadioStationList),uintptr_t(&CStats::FindMostFavoriteRadioStation)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> finalEntries;
    for(unsigned i=0;i<2;++i)for(auto at:{fns[i].address,symbols[i]})finalEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute populateRoute("CStats",fns[0].name,0x558EC0,&CStats::PopulateFavoriteRadioStationList);
    OriginalDependencies499::HookRoute mostRoute("CStats",fns[1].name,0x558FA0,&CStats::FindMostFavoriteRadioStation);
    std::array<OriginalDependencies499::HookRoute*,2> routes{&populateRoute,&mostRoute};
    HelperHookRoute engineHelper("CAudioEngine","GetRadioStationListenTimes",0x507020,FunctionToVoidPtr(&CAudioEngine::GetRadioStationListenTimes));
    HelperHookRoute managerHelper("CAERadioTrackManager","GetRadioStationListenTimes",0x4E83E0,FunctionToVoidPtr(&CAERadioTrackManager::GetRadioStationListenTimes));
    auto helperOK=[&] {return engineHelper.Check()&&managerHelper.Check();};
    const auto savedStats=ReversingProbe::Bytes(reinterpret_cast<void*>(statsBegin),statsSize);
    const auto savedEngine=ReversingProbe::Bytes(reinterpret_cast<void*>(engineBegin),sizeof(CAudioEngine));
    const auto savedManager=ReversingProbe::Bytes(reinterpret_cast<void*>(managerBegin),sizeof(CAERadioTrackManager));
    const unsigned savedFP=_controlfp(0,0);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> inactiveEntries;
    for(unsigned i=0;i<2;++i)for(auto at:{fns[i].address,symbols[i],routes[i]->Body()})inactiveEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    auto restore=[&] {std::memcpy(reinterpret_cast<void*>(statsBegin),savedStats.data(),statsSize);
        std::memcpy(reinterpret_cast<void*>(engineBegin),savedEngine.data(),savedEngine.size());std::memcpy(reinterpret_cast<void*>(managerBegin),savedManager.data(),savedManager.size());
        _controlfp(savedFP,_MCW_PC|_MCW_RC);};
    auto restored=[&] {bool ok=helperOK()&&savedStats==ReversingProbe::Bytes(reinterpret_cast<void*>(statsBegin),statsSize)
        &&savedEngine==ReversingProbe::Bytes(reinterpret_cast<void*>(engineBegin),savedEngine.size())
        &&savedManager==ReversingProbe::Bytes(reinterpret_cast<void*>(managerBegin),savedManager.size())&&_controlfp(0,0)==savedFP;
        for(auto& [at,bytes]:inactiveEntries)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);return ok;};
    const bool registered=populateRoute.Registered()&&mostRoute.Registered()&&engineHelper.Registered()&&managerHelper.Registered();
    Write({{"event","begin"},{"functions",2},{"pairs",4096},{"registered_hook",registered},{"collaborators_stubbed",false},
        {"helper_route_ok",helperOK()},{"helpers",json::array({engineHelper.Metadata("CAudioEngine","GetRadioStationListenTimes"),managerHelper.Metadata("CAERadioTrackManager","GetRadioStationListenTimes")})}});
    bool passed=registered&&helperOK(),allRestored=true;unsigned pair=0;
    if(passed)for(unsigned f=0;f<2;++f) {
        unsigned failures=0;const std::string label=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<2048;++fixture,++pair) {
            const unsigned profile=fixture/128,value=fixture%128;
            auto run=[&](bool candidate) {
                restore();std::array<int32,14> input{};
                if(f==0) {
                    if(profile>=1&&profile<=14)input[profile-1]=Pattern(value);
                    if(profile==15)for(unsigned i=0;i<14;++i)input[i]=Pattern(value+i);
                    std::memcpy(reinterpret_cast<void*>(managerBegin+0x18),input.data(),56);
                    std::memset(reinterpret_cast<void*>(statsBegin+16),(value&1)?0xA5:0x5A,56);
                } else {
                    input.fill(-1);input[0]=0x7FFFFFFF;input[13]=0x7FFFFFFF;
                    if(profile>=1&&profile<=12)input[profile]=Pattern(value);
                    if(profile==13)for(unsigned i=1;i<13;++i)input[i]=Pattern(value);
                    if(profile==14) {unsigned first=value%11+1;input[first]=input[first+1]=std::abs(int(value))+1;}
                    if(profile==15)for(unsigned i=1;i<13;++i)input[i]=Pattern(value+i);
                    std::memcpy(reinterpret_cast<void*>(statsBegin+16),input.data(),56);
                }
                const auto beforeStats=ReversingProbe::Bytes(reinterpret_cast<void*>(statsBegin),statsSize);
                const auto beforeEngine=ReversingProbe::Bytes(reinterpret_cast<void*>(engineBegin),savedEngine.size());
                const auto beforeManager=ReversingProbe::Bytes(reinterpret_cast<void*>(managerBegin),savedManager.size());
                bool routeOK=false,helpersValid=false;ABIResult abi{};
                routes[f]->Invoke(candidate,[&] {routeOK=ReversingProbe::JumpsToDll(fns[f].address)==candidate;helpersValid=helperOK();
                    if(helpersValid)abi=CallRaw(fns[f].address);helpersValid&=helperOK();return true;});
                const auto afterStats=ReversingProbe::Bytes(reinterpret_cast<void*>(statsBegin),statsSize);
                const auto afterEngine=ReversingProbe::Bytes(reinterpret_cast<void*>(engineBegin),savedEngine.size());
                const auto afterManager=ReversingProbe::Bytes(reinterpret_cast<void*>(managerBegin),savedManager.size());
                uint32 expected{};auto expectedStats=beforeStats;
                if(f==0) {expected=std::ranges::all_of(input,[](int32 v){return v==0;});std::memcpy(expectedStats.data()+16,input.data(),56);}
                else {int32 maximum=0;expected=1;for(unsigned i=1;i<13;++i)if(input[i]>maximum) {maximum=input[i];expected=i;}}
                const uint32 resultReturn=f==0?abi.returned&0xFF:abi.returned;
                json result{{"route_ok",routeOK},{"helper_route_ok",helpersValid},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                    {"protected_state_unchanged",beforeEngine==afterEngine&&beforeManager==afterManager},{"expected_effects_correct",afterStats==expectedStats&&resultReturn==expected},
                    {"fpu_unchanged",_controlfp(0,0)==savedFP},{"profile",profile},{"value",value},{"input",input},{"stats_raw",afterStats},{"return_bits",resultReturn}};
                restore();Write({{"event","raw_abi_return"},{"pair",pair},{"candidate",candidate},{"eax",abi.returned},{"defined_mask",f==0?0xFFu:0xFFFFFFFFu}});return result;
            };
            const auto original=run(false),candidate=run(true);const bool equal=original==candidate,stateOK=restored();
            bool accepted=equal&&stateOK;for(auto key:{"route_ok","helper_route_ok","stack_restored","nonvolatile_registers_preserved","protected_state_unchanged","expected_effects_correct","fpu_unchanged"})accepted&=original[key]==true&&candidate[key]==true;
            failures+=!accepted;passed&=accepted;allRestored&=stateOK;
            Write({{"event","comparison"},{"function",label},{"pair",pair},{"case",fixture},{"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",label},{"cases",2048},{"failures",failures}});
    }
    restore();allRestored&=restored();for(auto* r:routes)allRestored&=r->Restore();allRestored&=engineHelper.Restore();allRestored&=managerHelper.Restore();
    for(auto& [at,bytes]:finalEntries)allRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&allRestored?"complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
