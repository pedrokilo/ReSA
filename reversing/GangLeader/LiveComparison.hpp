#pragma once
#include "TaskComplexGangLeader.h"
#include "../ProbeSupport.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <fstream>

namespace GangLeaderLive {
__declspec(noinline) inline eTaskType VirtualCall(const CTask* task) { return task->GetTaskType(); }
inline void Update() {
    static bool done{};
    if (done) return;
    done = true;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "gang-leader-runtime.start")) return;
    std::ofstream log(dir / "gang-leader-runtime.jsonl");
    auto write = [&](const json& row) { log << row.dump() << '\n'; log.flush(); };
    using Hook = ReversibleHooks::ReversibleHook::Base;
    Hook* hook{};
    std::vector<std::tuple<Hook*, bool, bool>> others;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat) {
        for (auto& h : cat.Items()) {
            if (cat.Name() == "CTaskComplexGangLeader" && h->Name() == "GetTaskType") hook = h.get();
            else others.emplace_back(h.get(), h->Hooked(), h->Locked());
        }
    });
    if (!hook) { write({{"event", "error"}, {"reason", "missing hook"}}); return; }
    const bool enabled = hook->Hooked(), locked = hook->Locked();
    write({{"event", "hook"}, {"enabled", enabled}, {"locked", locked}});
    hook->LockState(false);
    bool passed = true;
    int pairs{};
    {
        CTaskComplexGangLeader original{nullptr}, candidate{nullptr};
        auto** ourTable = *reinterpret_cast<void***>(&candidate);
        auto** gtaTable = reinterpret_cast<void**>(0x86F8FC);
        // Exercise objects carrying either of the two real class vtables.
        *reinterpret_cast<void***>(&original) = gtaTable;
        for (int i = 0; i < 8; ++i) {
            original.m_scanTimer.m_nStartTime = candidate.m_scanTimer.m_nStartTime = i * 12345;
            for (int route = 0; route < 3; ++route) {
                auto call = [&](CTaskComplexGangLeader* task) {
                    using Fn = eTaskType(__thiscall*)(const CTaskComplexGangLeader*);
                    if (route == 0) return reinterpret_cast<Fn>(0x65DF20)(task);
                    if (route == 1) return reinterpret_cast<Fn>(gtaTable[4])(task);
                    return VirtualCall(task);
                };
                const auto beforeA = ReversingProbe::Bytes(&original, sizeof(original));
                const auto beforeB = ReversingProbe::Bytes(&candidate, sizeof(candidate));
                hook->State(false);
                const std::array<uint8, 6> expected{0xB8, 0xB1, 0x04, 0, 0, 0xC3};
                const bool originalBytes = memcmp(reinterpret_cast<void*>(0x65DF20), expected.data(), 6) == 0;
                const bool originalTables = gtaTable[4] == reinterpret_cast<void*>(0x65DF20) && ourTable[4] == gtaTable[4];
                const auto ca = GangLeaderProbe::calls;
                const auto a = call(&original);
                const auto cb = GangLeaderProbe::calls;
                hook->State(true);
                const bool candidateTables = gtaTable[4] != reinterpret_cast<void*>(0x65DF20) && ourTable[4] == gtaTable[4];
                const auto b = call(&candidate);
                const auto cc = GangLeaderProbe::calls;
                const bool unchanged = beforeA == ReversingProbe::Bytes(&original, sizeof(original)) && beforeB == ReversingProbe::Bytes(&candidate, sizeof(candidate));
                const bool equal = a == b && int(a) == 1201 && ca == cb && cc == cb + 1 && unchanged && originalBytes && originalTables && candidateTables;
                passed &= equal;
                write({{"event", "comparison"}, {"case", i}, {"route", route}, {"original", int(a)}, {"candidate", int(b)}, {"original_candidate_calls", cb-ca}, {"candidate_calls", cc-cb}, {"original_bytes", originalBytes}, {"original_tables", originalTables}, {"candidate_tables", candidateTables}, {"inputs_unchanged", unchanged}, {"equal", equal}});
                ++pairs;
            }
        }
        *reinterpret_cast<void***>(&original) = ourTable;
    }
    hook->State(enabled);
    hook->LockState(locked);
    const bool othersUnchanged = std::ranges::all_of(others, [](auto& item) { auto [h,e,l] = item; return h->Hooked() == e && h->Locked() == l; });
    const bool restored = hook->Hooked() == enabled && hook->Locked() == locked && othersUnchanged;
    write({{"event", passed && restored ? "complete" : "error"}, {"pairs", pairs}, {"state_restored", restored}, {"other_hooks_unchanged", othersUnchanged}});
}
}
