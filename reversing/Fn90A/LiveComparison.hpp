#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn90A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diez funciones (hook S, sin colaboradores: ninguna llama a otra del juego): CPad::GetPad 0x53FB70, CRGBA::Constructor 0x7170C0, CTaskSimple::Constructor 0x61A390,
// CTaskComplex::Constructor 0x61A3B0, CAnimManager::RemoveAnimBlockRef 0x4D3FD0, Fx_c::GetFxQuality 0x49EA50, Fx_c::SetFxQuality 0x49EA40, CSphere::Set 0x40FCF0,
// FxPrtMult_c::Constructor 0x4AB270 y CPedDamageResponseCalculator::Destructor 0x4AD420.
// Memoria: una arena estatica de 0x180 bytes (guarda + ventana de 0x100 + guarda) alineada a 32; `this`, el centro de la esfera y el bloque de animacion caen en la ventana. Los
// punteros y registros se anotan como "A+desplazamiento" (respecto a la ventana) si caen en la arena, o en crudo.

namespace F90Layout {
static_assert(sizeof(CRGBA) == 4 && offsetof(CRGBA, r) == 0 && offsetof(CRGBA, a) == 3, "CRGBA: r g b a");
static_assert(sizeof(CSphere) == 0x10 && offsetof(CSphere, m_vecCenter) == 0 && offsetof(CSphere, m_fRadius) == 0xC, "CSphere: centro +0, radio +0xC");
static_assert(sizeof(FxPrtMult_c) == 0x1C, "FxPrtMult_c: 0x1C bytes");
static_assert(sizeof(CAnimBlock) == 0x20 && offsetof(CAnimBlock, RefCnt) == 0x12, "CAnimBlock: 0x20 bytes, contador en +0x12");
static_assert(sizeof(Fx_c) == 0x70, "Fx_c: 0x70 bytes");
}

namespace F90Live {
using namespace F90Fix;
inline constexpr const char* kFn[kFunctions] = {"CPad::GetPad", "CRGBA::Constructor", "CTaskSimple::Constructor", "CTaskComplex::Constructor", "CAnimManager::RemoveAnimBlockRef",
    "Fx_c::GetFxQuality", "Fx_c::SetFxQuality", "CSphere::Set", "FxPrtMult_c::Constructor", "CPedDamageResponseCalculator::Destructor"};
inline constexpr uintptr_t kEntries[kFunctions] = {0x53FB70, 0x7170C0, 0x61A390, 0x61A3B0, 0x4D3FD0, 0x49EA50, 0x49EA40, 0x40FCF0, 0x4AB270, 0x4AD420};
// Argumentos de pila y bytes que retira el LLAMADOR (cdecl: 4 por argumento; `ret n` del callee: 0)
inline constexpr unsigned kNargs[kFunctions] = {1, 4, 0, 0, 1, 0, 1, 2, 0, 0};
inline constexpr unsigned kPop[kFunctions]   = {4, 0, 0, 0, 4, 0, 0, 0, 0, 0};
// `this` en ECX
inline constexpr bool kThis[kFunctions] = {false, true, true, true, false, true, true, true, true, true};
// EAX de salida: 0 no se compara (resto sin contrato), 1 se compara (retorno definido, o EAX intacto)
inline constexpr unsigned kEaxMode[kFunctions] = {1, 1, 1, 1, 0, 1, 0, 0, 1, 1};
// ECX / EDX: se comparan solo si el EXE no los escribe (el llamador puede contar con ellos)
inline constexpr bool kEcxKeep[kFunctions] = {true, false, true, false, true, true, true, true, false, true};
inline constexpr bool kEdxKeep[kFunctions] = {true, false, true, true, true, true, true, false, true, true};

alignas(32) inline std::array<uint8, kArena> arena{}, pre{};
inline uintptr_t Base() { return reinterpret_cast<uintptr_t>(arena.data()); }
inline uint8* Win() { return arena.data() + kGuard; }
// Anotacion de un valor de 32 bits: dentro de la arena, "A+n"/"A-n" respecto al inicio de la ventana; si no, el numero en crudo
inline json Tag(u32 v) {
    const uintptr_t b = Base() + kGuard;
    if (v >= Base() && v < Base() + kArena) {
        const int64 d = int64(v) - int64(b);
        return std::string("A") + (d < 0 ? "-" : "+") + std::to_string(d < 0 ? -d : d);
    }
    return v;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            }
    });
    return found;
}

inline json ChangesJson() {
    json out = json::array();
    for (size_t i = 0; i < kArena; ++i) if (arena[i] != pre[i]) out.push_back(json::array({int64(i) - int64(kGuard), arena[i]}));
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        FillWords(arena.data(), kGuard, c.seed * 13u + 1u);
        FillWindow(Win(), c.fill, c.seed * 13u + 2u);
        FillWords(arena.data() + kGuard + kWin, kGuard, c.seed * 13u + 3u);
        if (c.pokeSize == 2) { const uint16 v = uint16(c.poke); memcpy(Win() + c.pokeOff, &v, 2); }
        else if (c.pokeSize == 4) memcpy(Win() + c.pokeOff, &c.poke, 4);
        pre = arena;
        const u32 thisPtr = u32(Base() + kGuard + c.off);
        u32 args[4]{};
        switch (fn) {
        case 0: args[0] = c.a[0]; break;
        case 1: for (unsigned k = 0; k < 4; ++k) args[k] = c.a[k]; break;
        case 4: args[0] = (((u32(Base() + kGuard + c.blk) - kAnimBase) >> 5) & 0x07FFFFFFu) | (c.hi << 27); break;
        case 6: args[0] = c.a[0]; break;
        case 7: args[0] = c.a[0]; args[1] = u32(Base() + kGuard + c.pOff); break;
        default: break;
        }
        const u32 seedEcx = kThis[fn] ? thisPtr : c.ecx;
        if (s == 0) {
            input["base"] = u32(Base());
            input["pre"] = HexBytes(Win(), kWin);
            json a = json::array(); for (unsigned k = 0; k < kNargs[fn]; ++k) a.push_back(Tag(args[k]));
            input["args"] = a;
            input["ecx_in"] = Tag(seedEcx);
        }
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args, kNargs[fn], c.eax, seedEcx, c.edx, c.stack, kPop[fn]); });
        json obs;
        obs["eax"] = kEaxMode[fn] ? Tag(sides[s].reg.eax) : json(nullptr);
        obs["ecx"] = kEcxKeep[fn] ? Tag(sides[s].reg.ecx) : json(nullptr);
        obs["edx"] = kEdxKeep[fn] ? Tag(sides[s].reg.edx) : json(nullptr);
        obs["changes"] = ChangesJson();
        auto fp = NormalizeFp(sides[s].fp);
        json out = CommonRow(sides[s], fp); out["observed"] = obs;
        RestoreFloatingState();
        out["globals_restored"] = env.WatchesIntact();
        out["floating_state_restored"] = FloatingStateRestored();
        rows[s] = out;
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", false}, {"collaborators", json::object()},
        {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR},
        {"layout", {{"guard", kGuard}, {"window", kWin}, {"arena", kArena}}}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact()}});
        }
    }
    const bool entriesIntact = env.WatchesIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn90a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn90a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn90a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
    }
    for (unsigned f = 0; f < kFunctions; ++f) {
        env.watches.emplace_back(kEntries[f], 16);
        if (env.route[f]) env.watches.emplace_back(env.route[f]->Body(), 16);
    }
    Run(env, ready, registered, bodies);
}
}
