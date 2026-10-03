#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MenuSystem.h"
#include "Probe.hpp"
namespace MenuLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& value) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << value.dump() << '\n'; log.flush(); }
inline int Report(int type, char* message, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", message ? message : ""}}); ExitProcess(91);
}
using Palette = std::array<CRGBA, 128>;
struct Guarded {
    std::array<uint8, 32> prefix;
    CMenuSystem::Menu menu;
    std::array<uint8, 32> suffix;
};
inline std::vector<uint8> Bytes(const void* p, size_t n) {
    auto* bytes = static_cast<const uint8*>(p); return {bytes, bytes + n};
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "menusystem-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "menusystem-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    auto previousReport = _CrtSetReportHook(Report);
    auto& pointers = StaticRef<std::array<CMenuSystem::Menu*, 2>>(0xBA82D8);
    auto& flags = StaticRef<std::array<uint8, 4>>(0xBA82E0);
    const auto savedPointers = pointers; const auto savedFlags = flags;
    Palette savedPalette; std::memcpy(savedPalette.data(), CVehicleModelInfo::ms_vehicleColourTable, sizeof(savedPalette));
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() == "CMenuSystem") for (auto& h : c.Items()) if (h->Name() == "FillGridWithCarColours") hook = h.get();
    });
    if (!hook) { Write({{"event", "error"}, {"reason", "missing hook"}}); _CrtSetReportHook(previousReport); return; }
    const bool enabled = hook->Hooked(), locked = hook->Locked(); hook->LockState(false);
    std::vector<Palette> palettes{savedPalette};
    for (int mode = 0; mode < 5; ++mode) {
        Palette palette;
        for (int i = 0; i < 128; ++i) {
            if (mode == 0) palette[i] = {0, 0, 0, uint8(i)};
            if (mode == 1) palette[i] = {255, 255, 255, uint8(i)};
            if (mode == 2) palette[i] = {uint8(i * 2), uint8(i * 2), uint8(i * 2), uint8(i)};
            if (mode == 3) palette[i] = {uint8((i % 8) * 32), uint8(((i / 8) % 8) * 32), uint8((i / 64) * 128), uint8(i)};
            if (mode == 4) palette[i] = {uint8(i % 2 ? 14 : 128), uint8(i % 2 ? 14 : 128), uint8(i % 2 ? 14 : 128), uint8(i)};
        }
        palettes.push_back(palette);
    }
    for (int channel = 0; channel < 3; ++channel) for (int base : {0, 14, 15, 128, 240, 255}) for (int delta : {-16, -15, -14, 0, 14, 15, 16}) {
        Palette palette; palette.fill(CRGBA{uint8(base), uint8(base), uint8(base), 73});
        reinterpret_cast<uint8*>(&palette[1])[channel] = uint8(std::clamp(base + delta, 0, 255));
        palettes.push_back(palette);
    }
    uint32 random = 0xC0FFEE;
    for (int seed = 0; seed < 64; ++seed) {
        Palette palette;
        for (auto& c : palette) for (size_t channel = 0; channel < 4; ++channel) {
            random = random * 1664525u + 1013904223u;
            reinterpret_cast<uint8*>(&c)[channel] = uint8(random >> 24);
        }
        palettes.push_back(palette);
    }
    Write({{"event", "player_ready"}, {"palettes", palettes.size()}, {"independent_inputs", true}});
    bool passed = true; int pair{};
    auto entry = reinterpret_cast<void(__cdecl*)(MenuId)>(0x5820E0);
    for (size_t p = 0; p < palettes.size() && passed; ++p) {
        Write({{"event", "palette"}, {"id", p}, {"rgba", Bytes(palettes[p].data(), sizeof(Palette))}});
        for (MenuId id = 0; id < 2 && passed; ++id) for (int poison : {0, 165}) {
            Guarded a, b, other;
            std::memset(&a, poison, sizeof(a)); std::memcpy(&b, &a, sizeof(a)); std::memset(&other, 90, sizeof(other));
            const auto before = Bytes(&a, sizeof(a)), otherBefore = Bytes(&other, sizeof(other));
            const bool independent = &a.menu != &b.menu && &a.menu != &other.menu && &b.menu != &other.menu;
            flags = {uint8(id == 0), uint8(id == 1), id, 1}; const auto inputFlags = flags;
            pointers[1 - id] = &other.menu; pointers[id] = &a.menu;
            std::memcpy(CVehicleModelInfo::ms_vehicleColourTable, palettes[p].data(), sizeof(Palette));
            hook->State(false); const auto start = MenuProbe::calls; entry(id); const auto middle = MenuProbe::calls;
            const bool originalReadOnly = flags == inputFlags && pointers[id] == &a.menu && pointers[1-id] == &other.menu && Bytes(&other, sizeof(other)) == otherBefore && std::memcmp(CVehicleModelInfo::ms_vehicleColourTable, palettes[p].data(), sizeof(Palette)) == 0;
            pointers[id] = &b.menu; hook->State(true); entry(id); const auto end = MenuProbe::calls;
            auto original = Bytes(&a, sizeof(a)), candidate = Bytes(&b, sizeof(b));
            bool bounded = true;
            for (size_t i = 0; i < before.size(); ++i) if (i < offsetof(Guarded, menu) || i >= offsetof(Guarded, menu) + 64) bounded &= original[i] == before[i] && candidate[i] == before[i];
            const bool readOnly = originalReadOnly && flags == inputFlags && pointers[id] == &b.menu && pointers[1-id] == &other.menu && Bytes(&other, sizeof(other)) == otherBefore && std::memcmp(CVehicleModelInfo::ms_vehicleColourTable, palettes[p].data(), sizeof(Palette)) == 0;
            passed = original == candidate && bounded && readOnly && independent && middle == start && end == middle + 1;
            Write({{"event", "comparison"}, {"pair", pair++}, {"palette", p}, {"menu", id}, {"poison", poison}, {"original", Bytes(a.menu.m_anUsedCarColors, 64)}, {"candidate", Bytes(b.menu.m_anUsedCarColors, 64)}, {"full_storage_equal", original == candidate}, {"bounded_writes", bounded}, {"inputs_unchanged", readOnly}, {"independent_inputs", independent}, {"original_candidate_calls", middle-start}, {"candidate_calls", end-middle}, {"equal", passed}});
        }
    }
    pointers = savedPointers; flags = savedFlags; std::memcpy(CVehicleModelInfo::ms_vehicleColourTable, savedPalette.data(), sizeof(Palette));
    hook->State(enabled); hook->LockState(locked);
    const bool restored = pointers == savedPointers && flags == savedFlags && std::memcmp(CVehicleModelInfo::ms_vehicleColourTable, savedPalette.data(), sizeof(Palette)) == 0 && hook->Hooked() == enabled && hook->Locked() == locked;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored}});
    _CrtSetReportHook(previousReport);
}
}
