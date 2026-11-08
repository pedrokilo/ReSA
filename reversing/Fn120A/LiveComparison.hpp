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


// Banco par a par Fn120A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco metodos virtuales diminutos (hook V = RH_ScopedVMTInstall: redirige la ranura de la vtable del EXE y la entrada de la funcion), cada uno con 16384 pares EXE/DLL:
//   0  CAEPedlessSpeechAudioEntity::AddScriptSayEvent 0x4E6080  thiscall, 5 argumentos de pila, `ret 14h`; no hace nada.
//   1  CTaskComplexGangJoinRespond::GetTaskType       0x661710  thiscall; devuelve 1219.
//   2  CTaskComplexKillPedOnFoot::GetTaskType         0x620EE0  thiscall; devuelve 1000.
//   3  CTaskComplexPartner::GetTaskType               0x681F10  thiscall; devuelve 256.
//   4  CTaskComplexPartnerDeal::GetTaskType           0x681DF0  thiscall; devuelve 1202.
// Sin colaboradores ni falsos: ninguna llama a nada ni lee memoria. El banco invoca la direccion del EXE con un objeto sintetico (arena propia) o, en la mitad de los casos, la ranura de la vtable del EXE
// (que es lo que usan los objetos construidos por el EXE hasta que se enganche su constructor). Memoria real del juego: ninguna.

namespace F120ALive {
using namespace F120AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4E6080, 0x661710, 0x620EE0, 0x681F10, 0x681DF0};
inline constexpr const char* kClass[kFunctions] = {"CAEPedlessSpeechAudioEntity", "CTaskComplexGangJoinRespond", "CTaskComplexKillPedOnFoot", "CTaskComplexPartner", "CTaskComplexPartnerDeal"};
inline constexpr const char* kName[kFunctions] = {"AddScriptSayEvent", "GetTaskType", "GetTaskType", "GetTaskType", "GetTaskType"};
inline uintptr_t SlotAddress(unsigned f) { return uintptr_t(kVtable[f]) + 4u * kSlot[f]; }

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F120AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
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
        objA.Pattern(c.a[4], sd++); aA.Fill(sd++);
        const uintptr_t self = c.a[0] == 0 ? objA.Obj(c.a[2]) : (c.a[0] == 1 ? 0u : uintptr_t(c.a[1]));
        if (c.a[0] == 0 && c.a[5]) objA.Put32(c.a[2], kVtable[fn]);   // objeto "real": la vtable del EXE en +0
        std::vector<u32> args;
        const u32 popBytes = 0;
        if (fn == 0) args = {c.a[6], c.a[7], c.a[8], c.a[9], c.a[10]};   // `ret 14h`: los limpia el callee
        objA.Snap(); aA.Snap();
        const bool viaSlot = c.a[3] != 0;
        sides[s] = Execute(env, fn, kEntries[fn], SlotAddress(fn), candidate, [&](uintptr_t entry) {
            const uintptr_t target = viaSlot ? *reinterpret_cast<const uintptr_t*>(SlotAddress(fn)) : entry;
            return Gate(target, args.data(), u32(args.size()), c.eax, u32(self), c.edx, c.stack, popBytes);
        });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, aA.Changes());
        obs["changes"] = ch;
        obs["events"] = json::array();
        obs["ret"] = fn == 0 ? json(nullptr) : json(sides[s].reg.eax);
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

// Busca el hook virtual registrado (categoria = clase, nombre) y devuelve la direccion de la funcion de la DLL (o 0).
inline uintptr_t FindVirtualBody(const char* cls, const char* name, uintptr_t exe) {
    uintptr_t body = 0;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() != cls) return;
        for (auto& h : c.Items())
            if (h->Name() == name && h->Symbol()[0] == 'V') {
                auto* v = static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get());
                if (uintptr_t(v->GetHookGTAAddress()) == exe) body = uintptr_t(v->GetHookOurAddress());
            }
    });
    return body;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn120a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn120a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn120a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t our = FindVirtualBody(kClass[f], kName[f], kEntries[f]);
        if (our) env.route[f].reset(new HookRoute(kClass[f], kName[f], kEntries[f], reinterpret_cast<void*>(our)));
        registered[kFn[f]] = our != 0 && env.route[f]->Registered(); bodies[kFn[f]] = our != 0 && env.route[f]->Body() != 0;
        ready &= our != 0 && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        // sin hook, la ranura del EXE debe ser la direccion original
        ready &= *reinterpret_cast<const uintptr_t*>(SlotAddress(f)) == kEntries[f];
    }
    const char* dllReal[kFunctions] = {
        "toda la funcion: no hace nada (la ranura 1 de la vtable de CAEPedlessSpeechAudioEntity); `ret 14h`",
        "toda la funcion: devuelve TASK_COMPLEX_GANG_JOIN_RESPOND (1219)",
        "toda la funcion: devuelve TASK_COMPLEX_KILL_PED_ON_FOOT (1000)",
        "toda la funcion: devuelve TASK_COMPLEX_PARTNER (256)",
        "toda la funcion: devuelve TASK_COMPLEX_PARTNER_DEAL (1202)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        env.watches.emplace_back(kEntries[f], 16);
        env.watches.emplace_back(SlotAddress(f), 4);
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", json::array()}, {"real", json::array()}, {"hooks_found", json::array()},
                                 {"own_hook", {{"class", kClass[f]}, {"name", kName[f]}, {"address", kEntries[f]}, {"vtable", kVtable[f]}, {"slot", kSlot[f]}}}, {"dll_only", json::array()}};
    }
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
