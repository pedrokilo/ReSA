#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn121A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Seis funciones diminutas (hook S = RH_ScopedInstall / RH_ScopedNamedGlobalInstall: enlace de la entrada hacia la DLL), cada una con 16384 pares EXE/DLL:
//   0  CTaskAllocator::Destructor (cuerpo)                       0x69BB40  thiscall, sin argumentos; deja la vtable 0x870E30. EAX no definido.
//   1  CEntitySeekPosCalculator::Destructor (cuerpo)             0x46A610  thiscall, sin argumentos; deja la vtable 0x859DC4. EAX no definido.
//   2  CPedGroupDefaultTaskAllocatorFollowLimited::Constructor   0x5F6480  thiscall; deja la vtable 0x86C764 y devuelve this.
//   3  CPedGroupDefaultTaskAllocatorStandStill::Constructor      0x5F64B0  idem, vtable 0x86C76C.
//   4  CPedGroupDefaultTaskAllocatorChat::Constructor            0x5F64E0  idem, vtable 0x86C774.
//   5  CPedGroupDefaultTaskAllocatorSitInLeaderCar::Constructor  0x5F6540  idem, vtable 0x86C784.
// Sin colaboradores ni falsos: ninguna llama a nada ni lee memoria. El banco invoca la direccion del EXE con un objeto sintetico (arena propia, `this` en un desplazamiento 0..0xFF).
// Se observan: los bytes cambiados de las dos arenas (la escritura de la vtable, incluida la que entra en la guarda), el dword final en [this], EAX (etiqueta de arena) en los constructores,
// y EAX/ECX/EDX de salida en los residuos (solo informativos: ningun llamador del EXE los lee). Memoria real del juego: ninguna.

namespace F121ALive {
using namespace F121AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x69BB40, 0x46A610, 0x5F6480, 0x5F64B0, 0x5F64E0, 0x5F6540};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F121AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kObjWin> objA;          // 0: objeto (this)
inline Arena<kAWin> aA;              // 1: centinela

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Pattern(c.a[2], sd++); aA.Fill(sd++);
        const uintptr_t self = objA.Obj(c.a[0]);
        objA.Put32(c.a[0], c.a[6]);   // imagen previa de [this]
        objA.Snap(); aA.Snap();
        TagClear(); TagAdd(objA.m.data(), objA.m.size()); TagAdd(aA.m.data(), aA.m.size());
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, nullptr, 0, c.eax, u32(self), c.edx, c.stack, 0);
        });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, aA.Changes());
        obs["changes"] = ch;
        obs["events"] = json::array();
        obs["vptr"] = objA.Get32(c.a[0]);
        obs["ret"] = kReturnsThis[fn] ? json(Tag(uintptr_t(sides[s].reg.eax))) : json(nullptr);
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook simple registrado en la direccion del EXE y devuelve su categoria y su nombre.
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn121a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn121a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn121a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    const char* dllReal[kFunctions] = {
        "toda la funcion: deja la vtable de CTaskAllocator (0x870E30) en [this]",
        "toda la funcion: deja la vtable de CEntitySeekPosCalculator (0x859DC4) en [this]",
        "toda la funcion: deja la vtable de FollowLimited (0x86C764) en [this] y devuelve this",
        "toda la funcion: deja la vtable de StandStill (0x86C76C) en [this] y devuelve this",
        "toda la funcion: deja la vtable de Chat (0x86C774) en [this] y devuelve this",
        "toda la funcion: deja la vtable de SitInLeaderCar (0x86C784) en [this] y devuelve this"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        env.watches.emplace_back(kEntries[f], 16);
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", json::array()}, {"real", json::array()}, {"hooks_found", json::array()}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
