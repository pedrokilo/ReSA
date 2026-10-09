#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1/GfxWave2, enganchado en CGame::Process): FxPost1.
// Clases: FxSystem_c (31 funciones) y CPostEffects (35 funciones), las pendientes de docs/hooks.csv con validated=0.
// Original (hooks desactivados: EXE puro) frente a candidato (hooks activos: DLL) sobre OBJETOS FALSOS; los colaboradores de dibujo,
// camara, raster, asignador y RNG se doblan con trazas (EntryPatch en la direccion del EXE y en el simbolo de la DLL) y el
// dispositivo de RenderWare se sustituye en memoria por uno falso. Los globales reales tocados se guardan al empezar, se restauran
// tras cada ejecucion y se verifican al final (state_restored). Hilos ajenos: ver Mine() en FxPostCommon.hpp.
// Detalle de lo que queda fuera del banco y de las mascaras de comparacion: README.md.
#include "FxPostCommon.hpp"
#include "FxPart.hpp"
#include "PostPart.hpp"

namespace FxPost1Live {
// Tabla (el run_local.py extrae de aqui {clase, nombre, direccion, vt, slot, casos}); es la concatenacion de las tablas de FxPart.hpp y PostPart.hpp.
// Tabla: {clase, nombre, direccion EXE, vt, slot, casos}
//   {"FxSystem_c","AddOffsetPos",0x4AA690,0,-1,256},
//   {"FxSystem_c","AttachToBone",0x4AA400,0,-1,256},
//   {"FxSystem_c","Constructor",0x4AAF00,0,-1,128},
//   {"FxSystem_c","CopyParentMatrix",0x4AA890,0,-1,128},
//   {"FxSystem_c","Destructor",0x4AA260,0,-1,256},
//   {"FxSystem_c","DoFxAudio",0x4AAC90,0,-1,512},
//   {"FxSystem_c","EnablePrim",0x4AA610,0,-1,128},
//   {"FxSystem_c","Exit",0x4AA840,0,-1,256},
//   {"FxSystem_c","ForAllParticles",0x4AA930,0,-1,512},
//   {"FxSystem_c","GetBoundingSphereLcl",0x4AAB50,0,-1,128},
//   {"FxSystem_c","GetBoundingSphereWld",0x4AAAD0,0,-1,512},
//   {"FxSystem_c","GetCompositeMatrix",0x4AA8C0,0,-1,256},
//   {"FxSystem_c","GetPlayStatus",0x4AA900,0,-1,64},
//   {"FxSystem_c","Init",0x4AA750,0,-1,512},
//   {"FxSystem_c","IsVisible",0x4AAF30,0,-1,512},
//   {"FxSystem_c","Kill",0x4AA3F0,0,-1,256},
//   {"FxSystem_c","Pause",0x4AA370,0,-1,128},
//   {"FxSystem_c","Play",0x4AA2F0,0,-1,512},
//   {"FxSystem_c","PlayAndKill",0x4AA3D0,0,-1,256},
//   {"FxSystem_c","ResetBoundingSphere",0x4AABF0,0,-1,256},
//   {"FxSystem_c","SetBoundingSphere",0x4AAB80,0,-1,256},
//   {"FxSystem_c","SetConstTime",0x4AA6C0,0,-1,256},
//   {"FxSystem_c","SetLocalParticles",0x4AA910,0,-1,128},
//   {"FxSystem_c","SetMatrix",0x4AA630,0,-1,128},
//   {"FxSystem_c","SetMustCreatePrts",0x4AAC70,0,-1,128},
//   {"FxSystem_c","SetOffsetPos",0x4AA660,0,-1,256},
//   {"FxSystem_c","SetRateMult",0x4AA6F0,0,-1,256},
//   {"FxSystem_c","SetTimeMult",0x4AA710,0,-1,256},
//   {"FxSystem_c","SetVelAdd",0x4AA730,0,-1,128},
//   {"FxSystem_c","SetZTestEnable",0x4AAC50,0,-1,128},
//   {"FxSystem_c","Stop",0x4AA390,0,-1,256},
//   {"CPostEffects","CCTV",0x702F40,0,-1,256},
//   {"CPostEffects","Close",0x7010C0,0,-1,64},
//   {"CPostEffects","ColourFilter",0x703650,0,-1,256},
//   {"CPostEffects","DarknessFilter",0x702F00,0,-1,256},
//   {"CPostEffects","DoScreenModeDependentInitializations",0x7046D0,0,-1,128},
//   {"CPostEffects","DrawQuad",0x700EC0,0,-1,512},
//   {"CPostEffects","FilterFX_RestoreDayNightBalance",0x7034D0,0,-1,128},
//   {"CPostEffects","FilterFX_StoreAndSetDayNightBalance",0x7034B0,0,-1,128},
//   {"CPostEffects","Fog",0x704150,0,-1,512},
//   {"CPostEffects","Grain",0x7037C0,0,-1,192},
//   {"CPostEffects","HeatHazeFXInit",0x701450,0,-1,256},
//   {"CPostEffects","ImmediateModeFilterStuffInitialize",0x703CC0,0,-1,256},
//   {"CPostEffects","ImmediateModeRenderStatesReStore",0x700E00,0,-1,128},
//   {"CPostEffects","ImmediateModeRenderStatesSet",0x700D70,0,-1,64},
//   {"CPostEffects","ImmediateModeRenderStatesStore",0x700CC0,0,-1,128},
//   {"CPostEffects","InfraredVision",0x703F80,0,-1,256},
//   {"CPostEffects","InfraredVisionRestoreLightsForHeatObjects",0x701410,0,-1,64},
//   {"CPostEffects","InfraredVisionSetLightsForDefaultObjects",0x701430,0,-1,64},
//   {"CPostEffects","InfraredVisionStoreAndSetLightsForHeatObjects",0x701320,0,-1,512},
//   {"CPostEffects","Initialise",0x704630,0,-1,64},
//   {"CPostEffects","IsVisionFXActive",0x7034F0,0,-1,64},
//   {"CPostEffects","NightVision",0x7011C0,0,-1,256},
//   {"CPostEffects","NightVisionSetLights",0x7012E0,0,-1,64},
//   {"CPostEffects","Render",0x7046E0,0,-1,1024},
//   {"CPostEffects","ScriptCCTVSwitch",0x7011B0,0,-1,64},
//   {"CPostEffects","ScriptDarknessFilterSwitch",0x701170,0,-1,128},
//   {"CPostEffects","ScriptHeatHazeFXSwitch",0x701160,0,-1,64},
//   {"CPostEffects","ScriptInfraredVisionSwitch",0x701140,0,-1,64},
//   {"CPostEffects","ScriptNightVisionSwitch",0x701120,0,-1,64},
//   {"CPostEffects","ScriptResetForEffects",0x7010F0,0,-1,64},
//   {"CPostEffects","SetFilterMainColour",0x703520,0,-1,256},
//   {"CPostEffects","SetSpeedFXManualSpeedCurrentFrame",0x700BE0,0,-1,256},
//   {"CPostEffects","SetupBackBufferVertex",0x7043D0,0,-1,256},
//   {"CPostEffects","UnderWaterRipple",0x7039C0,0,-1,512},
//   {"CPostEffects","Update",0x7046A0,0,-1,128},
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fxpost1-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fxpost1-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("fxpost1"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "fxpost1-only.txt"); if (f) std::getline(f, only); }

    // --- hooks ---
    std::vector<Hook*> fxHooks, postHooks, allHooks;
    for (auto& fn : Fx::fns) { auto* h = FindHook(fn.cls, fn.name); if (!h) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fn.cls) + "::" + fn.name} }); return; } fxHooks.push_back(h); }
    for (auto& fn : Post::fns) { auto* h = FindHook(fn.cls, fn.name); if (!h) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fn.cls) + "::" + fn.name} }); return; } postHooks.push_back(h); }
    allHooks = fxHooks; allHooks.insert(allHooks.end(), postHooks.begin(), postHooks.end());
    std::vector<bool> enabled, locked;
    for (auto* h : allHooks) { enabled.push_back(h->Hooked()); locked.push_back(h->Locked()); h->LockState(false); }

    // --- colaboradores simulados: comprobacion de direcciones ---
    const auto fxSites = Fx::Sites(), postSites = Post::SitesCommon(), renderSites = Post::SitesRender();
    bool known = true; json siteInfo = json::array();
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (const auto* list : { &fxSites, &postSites, &renderSites }) for (auto& s : *list) {
        bool ok = true;
        if (s.exe) { ok &= s.exe >= 0x401000 && s.exe < 0x1000000; entryBefore.push_back({ s.exe, ReversingProbe::Bytes(reinterpret_cast<void*>(s.exe), 8) }); }
        if (s.dll) { const auto b = Body(s.dll); ok &= s.dll >= 0x1000000 && b >= 0x1000000; entryBefore.push_back({ s.dll, ReversingProbe::Bytes(reinterpret_cast<void*>(s.dll), 8) }); if (b != s.dll) entryBefore.push_back({ b, ReversingProbe::Bytes(reinterpret_cast<void*>(b), 8) }); }
        known &= ok; siteInfo.push_back({ {"exe", s.exe}, {"dll", s.dll}, {"ok", ok} });
    }
    for (auto& fn : Fx::fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    for (auto& fn : Post::fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    bool layoutOk = false; const json layout = Post::Layout(layoutOk);
    known &= layoutOk;

    // --- globales ---
    const auto fxGlobals = Fx::Globals(), postGlobals = Post::Globals();
    const auto fxInitial = SaveRanges(fxGlobals), postInitial = SaveRanges(postGlobals);
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"layout", layout}, {"fp_control", savedFP} });

    bool passed = known; int pair = 0;
    auto Compare = [&](const std::string& full, int i, const json& original, const json& candidate, int& failures) {
        const bool ok = original == candidate && original.value("switches", false) == true && candidate.value("switches", false) == true
                        && original.value("bad_call", false) == false && !original.contains("error") && !candidate.contains("error");
        passed &= ok; failures += !ok;
        const std::string od = original.dump(), cd = candidate.dump();
        if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                        {"original", {{"trace", original["trace"]}, {"ret", original.value("ret", json(nullptr))}, {"state_fnv", Fnv32(od.data(), od.size())}}},
                        {"candidate", {{"trace", candidate["trace"]}, {"ret", candidate.value("ret", json(nullptr))}, {"state_fnv", Fnv32(cd.data(), cd.size())}}},
                        {"events", original["trace"].size()} });
        else { json diff = json::array(); for (auto& [k, v] : original.items()) if (!candidate.contains(k) || candidate[k] != v) diff.push_back(k);
            Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                    {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
    };

    if (known) {
        // ---- FxSystem_c ----
        {
            Iso iso(Fx::Sites());
            for (size_t f = 0; f < Fx::fns.size(); ++f) {
                const auto& fn = Fx::fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
                if (!only.empty() && only != full) continue;
                for (int i = 0; i < fn.cases; ++i) {
                    json original = Fx::Run(f, unsigned(i), false, fxHooks); RestoreRanges(fxInitial);
                    json candidate = Fx::Run(f, unsigned(i), true, fxHooks); RestoreRanges(fxInitial);
                    Compare(full, i, original, candidate, failures);
                }
                Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
            }
        }
        // ---- CPostEffects ----
        {
            Iso iso(Post::SitesCommon());
            for (size_t f = 0; f < Post::fns.size(); ++f) {
                const auto& fn = Post::fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
                if (!only.empty() && only != full) continue;
                std::unique_ptr<Iso> renderIso; if (std::string_view(fn.name) == "Render") renderIso = std::make_unique<Iso>(Post::SitesRender());
                for (int i = 0; i < fn.cases; ++i) {
                    json original = Post::Run(f, unsigned(i), false, postHooks, postHooks[f], postGlobals, postInitial); RestoreRanges(postInitial);
                    json candidate = Post::Run(f, unsigned(i), true, postHooks, postHooks[f], postGlobals, postInitial); RestoreRanges(postInitial);
                    Compare(full, i, original, candidate, failures);
                }
                renderIso.reset();
                Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
            }
        }
    }
    for (size_t k = 0; k < allHooks.size(); ++k) { allHooks[k]->State(enabled[k]); allHooks[k]->LockState(locked[k]); }
    RestoreRanges(fxInitial); RestoreRanges(postInitial);
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP && SameRanges(fxInitial) && SameRanges(postInitial);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored} });
}
}
