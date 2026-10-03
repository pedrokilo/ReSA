#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
namespace BaseModelLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", msg ? msg : ""}}); ExitProcess(91);
}
struct Result { uint32 eax, stackBefore, stackAfter, calls; };
__declspec(noinline) inline Result Invoke(const CBaseModelInfo* model, uint32 seed) {
    uint32 value, before, after;
    const auto count = BaseModelProbe::calls;
    __asm {
        mov before, esp
        mov ecx, model
        mov eax, seed
        mov edx, 04212C0h
        call edx
        mov value, eax
        mov after, esp
    }
    return {value, before, after, BaseModelProbe::calls - count};
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "basemodelinfo-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "basemodelinfo-runtime.jsonl"); _CrtSetReportHook(Report); }
    auto* player = FindPlayerPed();
    if (!player || !player->GetRpClump() || CCutsceneMgr::IsRunning()) return;
    done = true;
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() == "CBaseModelInfo") for (auto& h : c.Items()) if (h->Name() == "SwaysInWind") hook = h.get();
    });
    if (!hook) { Write({{"event", "error"}, {"reason", "missing hook"}}); return; }
    const bool saved = hook->Hooked(), locked = hook->Locked(); hook->LockState(false);
    // Copy a live model into independent storage; the function only reads offset 0x12.
    auto* source = CModelInfo::GetModelInfo(player->m_nModelIndex);
    std::array<uint8, 32> sourceBefore; std::memcpy(sourceBefore.data(), source, 32);
    struct alignas(4) Input { std::array<uint8, 32> bytes; };
    std::vector<Input> a(65536), b(65536);
    std::vector<Result> original(65536);
    for (unsigned f = 0; f < 65536; ++f) {
        a[f].bytes = sourceBefore;
        const auto flags = uint16(f); std::memcpy(a[f].bytes.data() + 0x12, &flags, 2);
        b[f] = a[f];
    }
    Write({{"event", "player_ready"}, {"independent_inputs", a.data() != b.data()}, {"initial_hook", saved}});
    hook->State(false);
    for (unsigned f = 0; f < 65536; ++f) original[f] = Invoke(reinterpret_cast<CBaseModelInfo*>(&a[f]), 0xA5A50000u | f);
    hook->State(true);
    bool passed = true;
    for (unsigned f = 0; f < 65536; ++f) {
        const auto candidate = Invoke(reinterpret_cast<CBaseModelInfo*>(&b[f]), 0x5A5A0000u | f);
        auto expected = sourceBefore; const auto flags = uint16(f); std::memcpy(expected.data() + 0x12, &flags, 2);
        const auto& o = original[f];
        const bool unchanged = a[f].bytes == expected && b[f].bytes == expected;
        const bool stack = o.stackBefore == o.stackAfter && candidate.stackBefore == candidate.stackAfter;
        const bool equal = o.eax == candidate.eax && candidate.eax <= 1 && o.calls == 0 && candidate.calls == 1 && unchanged && stack;
        passed &= equal;
        Write({{"event", "comparison"}, {"flags", f}, {"original", o.eax}, {"candidate", candidate.eax}, {"original_candidate_calls", o.calls}, {"candidate_calls", candidate.calls}, {"input_unchanged", unchanged}, {"stack_balanced", stack}, {"equal", equal}});
    }
    hook->State(saved); hook->LockState(locked);
    const bool restored = hook->Hooked() == saved && hook->Locked() == locked && std::memcmp(source, sourceBefore.data(), 32) == 0;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs", 65536}, {"state_restored", restored}});
}
}
