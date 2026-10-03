#pragma once
#include <fstream>
#include <crtdbg.h>
#include "C_PcSave.h"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"

namespace PcSaveLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << row.dump() << '\n'; log.flush(); }
inline int Report(int type, char* message, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", message ? message : ""}});
    ExitProcess(91);
}
inline bool Select(bool enabled) {
    bool found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        if (category.Name() != "C_PcSave") return;
        for (auto& hook : category.Items()) {
            if (hook->Name() != "GenerateGameFilename") continue;
            category.SetItemEnabled(hook, enabled);
            found = hook->Hooked() == enabled;
        }
    });
    return found;
}
inline void Update() {
    if (done) return;
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    const auto directory = std::filesystem::path(path).parent_path();
    if (!std::filesystem::exists(directory / "pcsave-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(directory / "pcsave-runtime.jsonl", std::ios::trunc);
        Write({{"event", "waiting_for_player"}});
        _CrtSetReportHook(Report);
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    Write({{"event", "player_ready"}, {"startup_candidate_calls", PcSaveProbe::calls}});
    struct Restore {
        std::array<char, MAX_PATH> prefix;
        Restore() { memcpy(prefix.data(), C_PcSave::DefaultPCSaveFileName, prefix.size()); }
        ~Restore() { memcpy(C_PcSave::DefaultPCSaveFileName, prefix.data(), prefix.size()); Select(true); }
    } restore;
    auto entry = reinterpret_cast<void(__thiscall*)(C_PcSave*, int32, char*)>(0x6190A0);
    int pair{};
    for (int length = -1; length < MAX_PATH; ++length) {
        auto& prefix = C_PcSave::DefaultPCSaveFileName;
        if (length >= 0) {
            for (int i = 0; i < length; ++i) prefix[i] = "AZ09 \\:/._-%"[i % 12];
            prefix[length] = 0;
        }
        for (int slot = 0; slot < 8; ++slot) {
            std::array<char, 544> original, candidate;
            original.fill(char(0x5a)); candidate.fill(char(0x5a));
            const auto before = PcSaveProbe::calls;
            if (!Select(false)) { Write({{"event", "error"}, {"reason", "original selection failed"}}); return; }
            entry(&s_PcSaveHelper, slot, original.data() + 16);
            const auto middle = PcSaveProbe::calls;
            if (!Select(true)) { Write({{"event", "error"}, {"reason", "candidate selection failed"}}); return; }
            entry(&s_PcSaveHelper, slot, candidate.data() + 16);
            const auto after = PcSaveProbe::calls;
            const auto expected = std::string(prefix) + std::to_string(slot + 1) + ".b";
            bool guards = true;
            for (size_t i = 0; i < 16; ++i) guards &= candidate[i] == char(0x5a);
            for (size_t i = 16 + expected.size() + 1; i < candidate.size(); ++i) guards &= candidate[i] == char(0x5a);
            const bool equal = original == candidate && guards &&
                !memcmp(candidate.data() + 16, expected.c_str(), expected.size() + 1) &&
                middle == before && after == middle + 1;
            Write({{"event", "comparison"}, {"pair", pair++}, {"slot", slot}, {"prefix_length", length},
                   {"equal", equal}, {"guards", guards}, {"original_candidate_calls", middle - before},
                   {"candidate_calls", after - middle}, {"expected", expected}});
            if (!equal) { Write({{"event", "error"}, {"reason", "comparison mismatch"}}); return; }
        }
    }
    Write({{"event", "complete"}, {"pairs", pair}});
}
}
