#pragma once
#include <fstream>
#include "Audio/Entities/AEFrontendAudioEntity.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedFrontendRadioGetterLive {
inline uint32 CallRawAL(uintptr_t target,CAEFrontendAudioEntity* self) {
    uint32 result{};
    __asm {
        mov ecx,self
        call target
        movzx eax,al
        mov result,eax
    }
    return result;
}
inline void Update() {
    using namespace ReversibleHooks::ReversibleHook;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-frontend-radio-getter.jsonl");
    auto write=[&](const json& value){log<<value.dump()<<'\n';log.flush();};
    const auto flags=SmallPendingSupport::SnapshotHooks();Simple* hook{};
    for(const auto& state:flags)if(state.hook->Type()==Base::HookType::Simple){auto* s=static_cast<Simple*>(state.hook);if(reinterpret_cast<uintptr_t>(s->GetHookGTAAddress())==0x4DD480)hook=s;}
    if(!hook){write({{"event","error"},{"reason","exact getter hook missing"}});return;}
    auto timer=reinterpret_cast<uint8*>(0xB7CB40);const auto timerSaved=ReversingProbe::Bytes(timer,18);
    const auto nativeSaved=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),candidateSaved=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    alignas(CAEFrontendAudioEntity)std::array<uint8,sizeof(CAEFrontendAudioEntity)+32> arena{};arena.fill(0xCD);
    auto* object=::new(arena.data()+16)CAEFrontendAudioEntity{};
    const auto inert=arena;bool passed=true;int failures{};
    write({{"event","start"},{"cases",2048},{"function","CAEFrontendAudioEntity::IsRadioTuneSoundActive"},{"address",0x4DD480}});
    hook->LockState(false);
    for(int seed=0;seed<2048;++seed){const int bits=seed%16;const bool user=bits&1,code=bits&2,normal=bits&4,paused=bits&8;const uint32 expected=(user||code)?paused:normal;json legs[2];
        for(int side=0;side<2;++side){arena=inert;for(size_t i=0;i<16;++i){arena[i]=uint8(seed*31+i);arena[arena.size()-16+i]=uint8(seed*47+i);}auto* bytes=reinterpret_cast<uint8*>(object);
            // ABI-proven int16 m_IsInUse fields. Restrict valid active states to 0/1.
            const int16 a=normal,b=paused;std::memcpy(bytes+0x124,&a,2);std::memcpy(bytes+0x1CC,&b,2);
            timer[8]=code;timer[9]=user;const auto before=arena;const auto globals=ReversingProbe::Bytes(timer,18);
            hook->State(side!=0);const bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            const uint32 returned=CallRawAL(0x4DD480,object);
            legs[side]={{"raw_al",returned},{"expected_al",expected},{"switch_exact",switched},{"arena_unchanged",arena==before},{"globals_unchanged",globals==ReversingProbe::Bytes(timer,18)},{"arena",arena},{"globals",globals}};
        }
        const bool accepted=legs[0]==legs[1]&&legs[0]["raw_al"]==expected&&legs[0]["switch_exact"]==true&&legs[0]["arena_unchanged"]==true&&legs[0]["globals_unchanged"]==true;passed&=accepted;failures+=!accepted;
        write({{"event","comparison"},{"function","CAEFrontendAudioEntity::IsRadioTuneSoundActive"},{"pair",seed},{"case",seed},{"equal",legs[0]==legs[1]},{"accepted",accepted},{"original",legs[0]},{"candidate",legs[1]}});
    }
    std::memcpy(timer,timerSaved.data(),timerSaved.size());arena=inert;
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    const bool restored=SmallPendingSupport::HooksUnchanged(flags)&&nativeSaved==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&candidateSaved==ReversingProbe::Bytes(hook->GetHookOurAddress(),52)&&timerSaved==ReversingProbe::Bytes(timer,18)&&arena==inert;
    // Inert placement instance owns no resources; no destructor integration claimed.
    write({{"event",passed&&restored?"complete":"error"},{"pairs",2048},{"failures",failures},{"state_restored",restored},{"passed",passed&&restored}});
}
}
