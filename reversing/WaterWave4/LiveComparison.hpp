#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1/GfxWave2, enganchado en CGame::Process): WaterWave4.
// Clase CWaterLevel: las 4 funciones que quedaban pendientes en docs/hooks.csv (validated=0):
//   AddWaterLevelQuad (0x6E7EF0), RenderBoatWakes (0x6ED9A0), SplitWaterTriangleAlongYLine (0x6EE5A0) y WaterLevelInitialise (0x6EAE80).
// Original (hook desactivado: codigo del EXE) frente a candidato (hook activo) sobre OBJETOS FALSOS; los colaboradores que dibujan o abren ficheros
// se doblan con trazas (argumentos crudos); los globales reales tocados se guardan al empezar, se restauran tras cada ejecucion y se verifican al
// final (state_restored). Hilos ajenos: ver Mine() en WaterCommon.hpp.
#include "WaterCommon.hpp"
#include "WaterPart.hpp"

namespace WaterWave4Live {
// Tabla (el run_local.py extrae de aqui {clase, nombre, direccion, vt, slot, casos}); es la misma que Wl::fns.
// Tabla: {clase, nombre, direccion EXE, vt, slot, casos}
//   {"CWaterLevel","AddWaterLevelQuad",0x6E7EF0,0,-1,1024},
//   {"CWaterLevel","RenderBoatWakes",0x6ED9A0,0,-1,512},
//   {"CWaterLevel","SplitWaterTriangleAlongYLine",0x6EE5A0,0,-1,1024},
//   {"CWaterLevel","WaterLevelInitialise",0x6EAE80,0,-1,256},

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "waterwave4-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "waterwave4-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("waterwave4"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "waterwave4-only.txt"); if (f) std::getline(f, only); }
    const auto& all = Wl::fns;
    std::vector<Hook*> hooks(all.size());
    for (size_t f = 0; f < all.size(); ++f) if (!(hooks[f] = FindHook(all[f].cls, all[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(all[f].cls) + "::" + all[f].name} }); return; }
    std::vector<bool> enabled(all.size()), locked(all.size());
    for (size_t f = 0; f < all.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    bool layoutOk = true; const json layout = Wl::Layout(layoutOk);
    Write({ {"event", "layout"}, {"ok", layoutOk}, {"fields", layout} });

    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& fn : all) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    const auto globals = Wl::Globals();
    std::vector<std::vector<uint8>> saved; for (auto& g : globals) saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(g.addr), g.size));
    auto RestoreGlobals = [&]() { for (size_t j = 0; j < globals.size(); ++j) std::memcpy(reinterpret_cast<void*>(globals[j].addr), saved[j].data(), saved[j].size()); };
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"layout_ok", layoutOk}, {"fp_control", savedFP} });

    bool passed = layoutOk; int pair = 0;
    if (layoutOk) {
        for (size_t f = 0; f < all.size(); ++f) {
            const auto& fn = all[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) continue;
            for (int i = 0; i < fn.cases; ++i) {
                json original = Wl::Run(f, unsigned(i), false, hooks[f]); RestoreGlobals();
                json candidate = Wl::Run(f, unsigned(i), true, hooks[f]); RestoreGlobals();
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
        }
    }
    for (size_t f = 0; f < all.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= saved[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].addr), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"layout_ok", layoutOk}, {"state_restored", restored} });
}
}
