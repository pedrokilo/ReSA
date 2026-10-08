#pragma once
#include <cfenv>
#include <xmmintrin.h>
#include <fstream>
#include "Fixtures.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPadContext9 {
using Simple=ReversibleHooks::ReversibleHook::Simple;
enum class ReturnKind {Short,Bool,Int};
struct Target {const char* name;uintptr_t address;ReturnKind kind;bool gun;};
inline constexpr Target targets[]{
    {"AimWeaponLeftRight",0x541040,ReturnKind::Short,false},{"AimWeaponUpDown",0x5410C0,ReturnKind::Short,false},
    {"GetPedWalkLeftRight-ped",0x540DC0,ReturnKind::Short,false},{"GetPedWalkUpDown-ped",0x540E20,ReturnKind::Short,false},
    {"LookAroundLeftRight",0x540BD0,ReturnKind::Short,false},{"LookAroundUpDown",0x540CC0,ReturnKind::Short,false},
    {"GetDisplayVitalStats",0x5408B0,ReturnKind::Short,true},{"GetWeapon",0x540180,ReturnKind::Int,true},{"WeaponJustDown",0x540250,ReturnKind::Bool,true}
};
struct Range {void* ptr;std::vector<uint8_t> data;};
inline Simple* Find(uintptr_t address){Simple* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* simple=static_cast<Simple*>(h.get());if(uintptr_t(simple->GetHookGTAAddress())==address)found=simple;}});return found;}
template<class R>inline R Call(void* entry,CPad* pad,CPed* actor){return reinterpret_cast<R(__thiscall*)(CPad*,CPed*)>(entry)(pad,actor);}
inline uint32_t CallBits(const Target& target,void* entry,CPad* pad,CPed* actor){
    switch(target.kind){case ReturnKind::Short:return uint16_t(Call<int16_t>(entry,pad,actor));case ReturnKind::Bool:return Call<bool>(entry,pad,actor)?1u:0u;case ReturnKind::Int:return uint32_t(Call<int32_t>(entry,pad,actor));}return 0;
}
inline void Update(){
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-pad-context9-runtime.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';};
    std::fenv_t originalFP{};std::fegetenv(&originalFP);const unsigned originalMXCSR=_mm_getcsr();
    std::vector<Range> globals;
    for(auto [address,size]:std::vector<std::pair<uintptr_t,size_t>>{{0xB73402,2},{0xB6EC2E,1},{0x8CD782,1}})globals.push_back({reinterpret_cast<void*>(address),ReversingProbe::Bytes(reinterpret_cast<void*>(address),size)});
    const auto spatial=SpatialFixtures(),gun=GunFixtures();write({{"event","start"},{"functions",9},{"spatial_cases",spatial.size()},{"gun_cases",gun.size()}});
    for(const auto& target:targets){
        auto* hook=Find(target.address);if(!hook){write({{"event","error"},{"reason","missing hook"},{"function",target.name}});break;}
        const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
        const auto& fixtures=target.gun?gun:spatial;
        for(size_t seed=0;seed<fixtures.size();++seed){const auto& fixture=fixtures[seed];json outcomes=json::array();bool switched=true,immutable=true;
            for(bool candidate:{false,true}){
                ActorLayout actor(fixture.actor,fixture.gun);auto pad=fixture.pad;
                const auto pedBefore=actor.ped.bytes;const auto otherPedBefore=actor.otherPed.bytes;const auto vehicleBefore=actor.vehicle.bytes;const auto intelligenceBefore=actor.intelligence.bytes;
                const auto primaryBefore=ReversingProbe::Bytes(&actor.primary,sizeof(actor.primary)),secondaryBefore=ReversingProbe::Bytes(&actor.secondary,sizeof(actor.secondary));
                *reinterpret_cast<bool*>(0xB6EC2E)=(fixture.flags&1)!=0;*reinterpret_cast<bool*>(0xB73402)=(fixture.flags&2)!=0;
                *reinterpret_cast<bool*>(0xB73403)=(fixture.flags&4)!=0;*reinterpret_cast<bool*>(0x8CD782)=(fixture.flags&8)!=0;
                std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);
                hook->State(candidate);switched&=SmallPendingSupport::ExactSwitch(*hook,candidate);
                const uint32_t bits=CallBits(target,candidate?hook->GetHookOurAddress():hook->GetHookGTAAddress(),&pad,actor.actor);
                immutable &= pedBefore==actor.ped.bytes&&otherPedBefore==actor.otherPed.bytes&&vehicleBefore==actor.vehicle.bytes&&intelligenceBefore==actor.intelligence.bytes&&primaryBefore==ReversingProbe::Bytes(&actor.primary,sizeof(actor.primary))&&secondaryBefore==ReversingProbe::Bytes(&actor.secondary,sizeof(actor.secondary));
                json globalsAfter=json::array();for(const auto& range:globals)globalsAfter.push_back(ReversingProbe::Bytes(range.ptr,range.data.size()));
                outcomes.push_back({{"bits",bits},{"pad",ReversingProbe::Bytes(&pad,sizeof(pad))},{"globals",globalsAfter}});
            }
            json row={{"event","comparison"},{"function",target.name},{"address",target.address},{"seed",seed},{"actor_state",int(fixture.actor)},{"gun_state",int(fixture.gun)},{"global_flags",fixture.flags},{"switch_exact",switched},{"actor_task_immutable",immutable},{"equal",outcomes[0]==outcomes[1]},{"original_bits",outcomes[0]["bits"]},{"candidate_bits",outcomes[1]["bits"]}};
            if(outcomes[0]!=outcomes[1]){row["original"]=outcomes[0];row["candidate"]=outcomes[1];}write(row);if(seed%128==127)log.flush();
        }
        hook->State(enabled);hook->LockState(locked);
    }
    for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());std::fesetenv(&originalFP);_mm_setcsr(originalMXCSR);
    bool restored=true;for(const auto& range:globals)restored &= range.data==ReversingProbe::Bytes(range.ptr,range.data.size());
    std::fenv_t restoredFP{};std::fegetenv(&restoredFP);const bool floatingRestored=_mm_getcsr()==originalMXCSR&&std::memcmp(&originalFP,&restoredFP,sizeof(originalFP))==0;restored &= floatingRestored;
    write({{"event",restored?"complete":"error"},{"state_restored",restored},{"floating_environment_restored",floatingRestored},{"functions",9},{"comparisons",6*spatial.size()+3*gun.size()}});log.flush();
}
}
