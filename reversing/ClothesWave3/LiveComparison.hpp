#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1/GfxWave2, enganchado en CGame::Process): ClothesWave3.
// Funcion: CClothesBuilder::CreateSkinnedClump (0x5A69D0). Original (hook desactivado) frente a candidato (hook activo) con
// TODOS los colaboradores externos doblados con trazas (ver ClothesPart.hpp). Los globales reales tocados se guardan al empezar,
// se restauran tras cada ejecucion y se verifican al final (state_restored). Hilos ajenos: ver Mine() en WaveCommon.hpp.
#include "WaveCommon.hpp"
#include "ClothesPart.hpp"

namespace ClothesWave3Live {
// Tabla (el run_local.py extrae de aqui {clase, nombre, direccion, vt, slot, casos}).
//   {"CClothesBuilder","CreateSkinnedClump",0x5A69D0,0,-1,256},
inline const std::array<Fn, 1> table{{ {"CClothesBuilder","CreateSkinnedClump",0x5A69D0,0,-1,256} }};

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "clotheswave3-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "clotheswave3-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("clotheswave3"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "clotheswave3-only.txt"); if (f) std::getline(f, only); }
    const auto& fn = table[0]; const std::string full = std::string(fn.cls) + "::" + fn.name;
    Hook* hook = FindHook(fn.cls, fn.name);
    if (!hook) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", full} }); return; }
    const bool enabled = hook->Hooked(), locked = hook->Locked(); hook->LockState(false);

    auto stubs = Cl::Stubs();
    bool known = true; json siteInfo = json::array();
    for (auto& s : stubs) {
        const bool ok = s.exeAddr >= 0x401000 && s.exeAddr < 0x1000000 && s.dll >= 0x1000000 && Body(s.dll) >= 0x1000000; known &= ok;
        siteInfo.push_back({ {"name", s.name}, {"exe", s.exeAddr}, {"dll", s.dll}, {"body", Body(s.dll)}, {"ok", ok} });
    }
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& s : stubs) for (auto a : { s.exeAddr, s.dll, Body(s.dll) }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    entryBefore.push_back({ 0x8214BD, ReversingProbe::Bytes(reinterpret_cast<void*>(0x8214BD), 8) });
    auto globals = Cl::Globals();
    std::vector<std::vector<uint8>> saved; for (auto& g : globals) saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(g.addr), g.size));
    auto RestoreGlobals = [&]() { for (size_t j = 0; j < globals.size(); ++j) std::memcpy(reinterpret_cast<void*>(globals[j].addr), saved[j].data(), saved[j].size()); };
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP} });

    bool passed = known; int pair = 0;
    if (known && (only.empty() || only == full)) {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        // Regla 11: todos los Body() ANTES de crear ningun EntryPatch.
        std::vector<uintptr_t> bodies; for (auto& s : stubs) bodies.push_back(Body(s.dll));
        for (size_t k = 0; k < stubs.size(); ++k) {
            const auto& s = stubs[k];
            patches.push_back(std::make_unique<EntryPatch>(s.exeAddr, reinterpret_cast<void*>(s.stub)));
            patches.push_back(std::make_unique<EntryPatch>(s.dll, reinterpret_cast<void*>(s.stub)));
            if (bodies[k] != s.dll) patches.push_back(std::make_unique<EntryPatch>(bodies[k], reinterpret_cast<void*>(s.stub)));
        }
        int failures = 0;
        for (int i = 0; i < fn.cases; ++i) {
            json original = Cl::Run(0, unsigned(i), false, hook); RestoreGlobals();
            json candidate = Cl::Run(0, unsigned(i), true, hook); RestoreGlobals();
            const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
            const std::string od = original.dump(), cd = candidate.dump();
            passed &= ok; failures += !ok;
            if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                            {"original", {{"trace", original["trace"]}, {"ret", original.value("ret", json(nullptr))}, {"state_fnv", Fnv(od.data(), od.size())}}},
                            {"candidate", {{"trace", candidate["trace"]}, {"ret", candidate.value("ret", json(nullptr))}, {"state_fnv", Fnv(cd.data(), cd.size())}}},
                            {"events", original["trace"].size()} });
            else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                        {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
        }
        Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
        patches.clear();
    }
    hook->State(enabled); hook->LockState(locked);
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= saved[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].addr), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored} });
}
}
