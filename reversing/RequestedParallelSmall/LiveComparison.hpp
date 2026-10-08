#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedDamageResponseCalculator.h"
#include "../ProbeSupport.hpp"
#include "HookSnapshots.hpp"
namespace RequestedParallelSmallLive {
inline bool done{};
inline void Update() {
    if (done) return;
    char executable[MAX_PATH]{};
    GetModuleFileNameA(nullptr, executable, MAX_PATH);
    const auto dir = std::filesystem::path(executable).parent_path();
    if (!std::filesystem::exists(dir / "small-pending-runtime.start")) { done = true; return; }
    done = true;
    std::ofstream log(dir / "small-pending-runtime.jsonl");
    auto write = [&](const json& row) { log << row.dump() << '\n'; log.flush(); };
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        if (category.Name() == "CPedDamageResponseCalculator")
            for (auto& item : category.Items()) if (item->Name() == "Constructor") hook = item.get();
    });
    if (!hook) { write({{"event", "error"}, {"reason", "missing Constructor hook"}}); return; }
    constexpr uintptr_t address = 0x4AD3F0;
    const auto allHooks = SmallPendingSupport::SnapshotHooks();
    auto& simple = *static_cast<ReversibleHooks::ReversibleHook::Simple*>(hook);
    const bool enabled = hook->Hooked(), locked = hook->Locked();
    const auto originalEntry = ReversingProbe::Bytes(reinterpret_cast<void*>(address), 52);
    const auto candidateEntry = ReversingProbe::Bytes(simple.GetHookOurAddress(), 52);
    hook->LockState(false);
    bool passed = true;
    int failures{};
    alignas(CPedDamageResponseCalculator) std::array<uint8, sizeof(CPedDamageResponseCalculator)> storage{};
    constexpr uint32 damageBits[]{0, 0x80000000, 0x3F800000, 0x7F800000, 0xFF800000, 0x7FC12345, 1, 0x7F7FFFFF};
    for (int i = 0; i < 1024; ++i) {
        json results[2];
        for (int side = 0; side < 2; ++side) {
            storage.fill(uint8(0xA0 + i % 31));
            auto* self = reinterpret_cast<CPedDamageResponseCalculator*>(storage.data());
            auto* entity = i % 3 ? reinterpret_cast<CEntity*>(uintptr_t(0x12340000 + i * 16)) : nullptr;
            const auto body = static_cast<ePedPieceTypes>((i / 8) % 11);
            const auto weapon = static_cast<eWeaponType>((i / 88) % 60);
            hook->State(side != 0);
            const bool switched = SmallPendingSupport::ExactSwitch(simple, side != 0);
            auto* result = reinterpret_cast<CPedDamageResponseCalculator*(__thiscall*)(CPedDamageResponseCalculator*, CEntity*, float, eWeaponType, ePedPieceTypes, bool)>(address)(self, entity, std::bit_cast<float>(damageBits[i % 8]), weapon, body, (i & 1) != 0);
            results[side] = {{"bytes", ReversingProbe::Bytes(storage.data(), storage.size())}, {"returned_this", result == self}, {"switched", switched}};
        }
        const bool equal = results[0] == results[1] && results[0]["switched"] == true && results[0]["returned_this"] == true;
        passed &= equal;
        failures += !equal;
        write({{"event", "comparison"}, {"function", "CPedDamageResponseCalculator::Constructor"}, {"pair", i}, {"case", i}, {"equal", equal}, {"accepted", equal}, {"original", results[0]}, {"candidate", results[1]}});
    }
    hook->State(enabled);
    hook->LockState(locked);
    const bool restored = SmallPendingSupport::HooksUnchanged(allHooks) && originalEntry == ReversingProbe::Bytes(reinterpret_cast<void*>(address), 52) && candidateEntry == ReversingProbe::Bytes(simple.GetHookOurAddress(), 52);
    write({{"event", "function_summary"}, {"function", "CPedDamageResponseCalculator::Constructor"}, {"cases", 1024}, {"failures", failures}});
    write({{"event", passed && restored ? "complete" : "error"}, {"pairs", 1024}, {"state_restored", restored}, {"hook_bytes_restored", restored}});
}
}
