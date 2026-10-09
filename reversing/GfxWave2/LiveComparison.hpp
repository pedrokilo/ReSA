#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1/GeoFinal1, enganchado en CGame::Process): GfxWave2.
// Clases: CClothesBuilder, CShadows, CWaterLevel (funciones pendientes de docs/hooks.csv con validated=0).
// Original (hook desactivado) frente a candidato (hook activo) sobre OBJETOS FALSOS; las funciones que dibujan se doblan con
// trazas (RwIm3D*/RwRaster*... con argumentos crudos) sin dibujar. Los globales reales tocados se guardan al empezar, se
// restauran tras cada ejecucion y se verifican al final (state_restored). Hilos ajenos: ver Mine() en GfxCommon.hpp.
#include "GfxCommon.hpp"
#include "ClothesPart.hpp"
#include "ShadowsPart.hpp"
#include "WaterPart.hpp"

namespace GfxWave2Live {
// Tabla (el run_local.py extrae de aqui {clase, nombre, direccion, vt, slot, casos}); es la concatenacion de las tablas de las partes.
// Tabla: {clase, nombre, direccion EXE, vt, slot, casos}
// Cl
//   {"CClothesBuilder","AddWeightToBoneVertex",0x5A4840,0,-1,512},
//   {"CClothesBuilder","BlendTextures-Dst-Src",0x5A5820,0,-1,256},
//   {"CClothesBuilder","BlendTextures-Dst-Src1-Src2",0x5A59C0,0,-1,256},
//   {"CClothesBuilder","BlendTextures-Dst-Src1-Src2-Tat",0x5A5BC0,0,-1,256},
//   {"CClothesBuilder","BuildBoneIndexConversionTable",0x5A56E0,0,-1,256},
//   {"CClothesBuilder","CopyTexture",0x5A5730,0,-1,128},
//   {"CClothesBuilder","DestroySkinArrays",0x5A56C0,0,-1,64},
//   {"CClothesBuilder","GetAtomicWithName",0x5A4810,0,-1,512},
//   {"CClothesBuilder","GetTextureFromTxdAndLoadNextTxd",0x5A5F70,0,-1,512},
//   {"CClothesBuilder","LoadCdDirectory",0x5A4190,0,-1,64},
//   {"CClothesBuilder","PlaceTextureOnTopOfTexture",0x5A57B0,0,-1,256},
//   {"CClothesBuilder","RequestGeometry",0x5A41C0,0,-1,256},
//   {"CClothesBuilder","RequestTexture",0x5A4220,0,-1,256},
//   {"CClothesBuilder","StoreBoneArray",0x5A48B0,0,-1,512},
// Sh
//   {"CShadows","Init",0x706CD0,0,-1,64},
//   {"CShadows","Shutdown",0x706ED0,0,-1,64},
//   {"CShadows","TidyUpShadows",0x707770,0,-1,64},
//   {"CShadows","StoreShadowToBeRendered-Texture",0x707390,0,-1,512},
//   {"CShadows","StoreShadowToBeRendered-Type",0x707930,0,-1,384},
//   {"CShadows","SetRenderModeForShadowType",0x707460,0,-1,96},
//   {"CShadows","RemoveOilInArea",0x7074F0,0,-1,384},
//   {"CShadows","GunShotSetsOilOnFire",0x707550,0,-1,384},
//   {"CShadows","PrintDebugPoly",0x7076B0,0,-1,64},
//   {"CShadows","CalcPedShadowValues",0x7076C0,0,-1,512},
//   {"CShadows","AffectColourWithLighting",0x707850,0,-1,768},
//   {"CShadows","StoreShadowForPedObject",0x707B40,0,-1,512},
//   {"CShadows","StoreRealTimeShadow",0x707CA0,0,-1,512},
//   {"CShadows","UpdateStaticShadows",0x707F40,0,-1,256},
//   {"CShadows","RenderExtraPlayerShadows",0x707FA0,0,-1,384},
//   {"CShadows","RenderStaticShadows",0x708300,0,-1,192},
//   {"CShadows","RenderIndicatorShadow",0x70CCB0,0,-1,256},
//   {"CShadows","StoreCarLightShadow",0x70C500,0,-1,512},
//   {"CShadows","StoreShadowForPole",0x70C750,0,-1,384},
// Wl
//   {"CWaterLevel","Shutdown",0x6E59E0,0,-1,64},
//   {"CWaterLevel","AddWaterLevelVertex",0x6E5A40,0,-1,1024},
//   {"CWaterLevel","AddWaterLevelTriangle",0x6E7D40,0,-1,1024},
//   {"CWaterLevel","AddWaterLevelQuad",0x6E7EF0,0,-1,1024},
//   {"CWaterLevel","BlockHit",0x6E6CA0,0,-1,512},
//   {"CWaterLevel","CalculateWavesOnlyForCoordinate",0x6E6EF0,0,-1,1024},
//   {"CWaterLevel","GetWaterLevel-",0x6EB690,0,-1,1024},
//   {"CWaterLevel","MarkQuadsAndPolysToBeRendered",0x6E5810,0,-1,1024},
//   {"CWaterLevel","PreRenderWater",0x6EB710,0,-1,512},
//   {"CWaterLevel","RenderFlatWaterRectangle",0x6EBEC0,0,-1,1024},
//   {"CWaterLevel","RenderFlatWaterRectangle_OneLayer",0x6E9940,0,-1,1024},
//   {"CWaterLevel","RenderFlatWaterTriangle",0x6EE080,0,-1,512},
//   {"CWaterLevel","RenderFlatWaterTriangle_OneLayer",0x6E8ED0,0,-1,1024},
//   {"CWaterLevel","RenderWakeSegment",0x6EA260,0,-1,1024},
//   {"CWaterLevel","RenderWaterRectangle",0x6EC5D0,0,-1,1024},
//   {"CWaterLevel","RenderWaterTriangle",0x6EE240,0,-1,1024},
//   {"CWaterLevel","ScanThroughBlocks",0x6E6D10,0,-1,256},
//   {"CWaterLevel","SetCameraRange",0x6E9C80,0,-1,512},
//   {"CWaterLevel","SetUpWaterFog",0x6EA9F0,0,-1,1024},
//   {"CWaterLevel","SplitWaterRectangleAlongXLine",0x6E73A0,0,-1,512},
//   {"CWaterLevel","SplitWaterRectangleAlongYLine",0x6ED6D0,0,-1,512},
//   {"CWaterLevel","SplitWaterTriangleAlongXLine",0x6ECF00,0,-1,512},

inline json RunCase(size_t f, unsigned i, bool cand, Hook* hook) {
    if (f < Cl::fns.size()) return Cl::Run(f, i, cand, hook);
    f -= Cl::fns.size();
    if (f < Sh::fns.size()) return Sh::Run(f, i, cand, hook);
    return Wl::Run(f - Sh::fns.size(), i, cand, hook);
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "gfxwave2-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "gfxwave2-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("gfxwave2"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "gfxwave2-only.txt"); if (f) std::getline(f, only); }
    std::vector<Fn> all; for (auto& x : Cl::fns) all.push_back(x); for (auto& x : Sh::fns) all.push_back(x); for (auto& x : Wl::fns) all.push_back(x);
    std::vector<Hook*> hooks(all.size());
    for (size_t f = 0; f < all.size(); ++f) if (!(hooks[f] = FindHook(all[f].cls, all[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(all[f].cls) + "::" + all[f].name} }); return; }
    std::vector<bool> enabled(all.size()), locked(all.size());
    for (size_t f = 0; f < all.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    // colaboradores simulados (de las tres partes); los que son funciones probadas se excluyen para no parchearlas
    // Los parches de cada parte solo estan activos mientras corren las funciones de esa parte (los colaboradores de una parte
    // pueden coincidir con los de otra sin conflicto).
    struct PartRef { size_t first, count; std::vector<Stub> stubs; };
    const std::array<PartRef, 3> parts{{ {0, Cl::fns.size(), Cl::Stubs()}, {Cl::fns.size(), Sh::fns.size(), Sh::Stubs()}, {Cl::fns.size() + Sh::fns.size(), Wl::fns.size(), Wl::Stubs()} }};
    std::vector<Stub> stubs; for (auto& p : parts) for (auto& s : p.stubs) stubs.push_back(s);
    bool known = true; json siteInfo = json::array();
    for (auto& s : stubs) {
        const bool ok = s.exeAddr >= 0x401000 && s.exeAddr < 0x1000000 && s.dll >= 0x1000000 && Body(s.dll) >= 0x1000000; known &= ok;
        siteInfo.push_back({ {"name", s.name}, {"exe", s.exeAddr}, {"dll", s.dll}, {"body", Body(s.dll)}, {"ok", ok} });
    }
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& s : stubs) for (auto a : { s.exeAddr, s.dll, Body(s.dll) }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : all) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    std::vector<GlobalRange> globals; for (auto& g : Cl::Globals()) globals.push_back(g); for (auto& g : Sh::Globals()) globals.push_back(g); for (auto& g : Wl::Globals()) globals.push_back(g);
    std::vector<std::vector<uint8>> saved; for (auto& g : globals) saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(g.addr), g.size));
    auto RestoreGlobals = [&]() { for (size_t j = 0; j < globals.size(); ++j) std::memcpy(reinterpret_cast<void*>(globals[j].addr), saved[j].data(), saved[j].size()); };
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP} });

    bool passed = known; int pair = 0;
    if (known) {
        using ReversingProbe::EntryPatch;
        for (auto& part : parts) {
            std::vector<std::unique_ptr<EntryPatch>> patches;
            // Regla 11: todos los Body() de la parte ANTES de crear ningun EntryPatch.
            std::vector<uintptr_t> bodies; for (auto& s : part.stubs) bodies.push_back(Body(s.dll));
            for (size_t k = 0; k < part.stubs.size(); ++k) {
                const auto& s = part.stubs[k];
                patches.push_back(std::make_unique<EntryPatch>(s.exeAddr, reinterpret_cast<void*>(s.stub)));
                patches.push_back(std::make_unique<EntryPatch>(s.dll, reinterpret_cast<void*>(s.stub)));
                if (bodies[k] != s.dll) patches.push_back(std::make_unique<EntryPatch>(bodies[k], reinterpret_cast<void*>(s.stub)));
            }
            for (size_t f = part.first; f < part.first + part.count; ++f) {
                const auto& fn = all[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
                if (!only.empty() && only != full) continue;
                for (int i = 0; i < fn.cases; ++i) {
                    json original = RunCase(f, unsigned(i), false, hooks[f]); RestoreGlobals();
                    json candidate = RunCase(f, unsigned(i), true, hooks[f]); RestoreGlobals();
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
            patches.clear();   // restaura las entradas de esta parte
        }
    }
    for (size_t f = 0; f < all.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= saved[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].addr), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored} });
}
}
