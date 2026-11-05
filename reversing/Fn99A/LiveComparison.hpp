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
#include "Core/Vector.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn99A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diez funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para las firmas):
//   0 CVector::Difference 0x40FE00 | 1 CMatrix::SetUnity 0x59AE70 | 2 ResetOrientation 0x59AEA0 | 3 SetScale(f) 0x59AED0 | 4 SetScale(fff) 0x59AF00 | 5 SetTranslateOnly 0x59AF80
//   6 SetRotate(xyz) 0x59B120 | 7 Invert(in, out) 0x59B920 | 8 MatrixMultiplyExe 0x59BE30 | 9 Multiply3x3VMExe 0x59C810
// SIN colaboradores: ninguna llama a otra funcion; toda la aritmetica x87 corre de verdad en ambos lados (x87 del EXE frente al C++ de la DLL, con la palabra de control del juego: 24 bits, redondeo
// al mas cercano). La memoria es una ventana de 96 dwords con punteros de entrada y salida que pueden solaparse. Ademas del resultado se compara EAX de salida (el puntero `out`) en Invert, la
// multiplicacion y Multiply3x3VMExe (los llamadores del EXE la leen, ver README.md). Las palabras de relleno de la salida de la multiplicacion (+0x0C, +0x1C, +0x2C, +0x3C: el EXE copia ahi basura
// de la pila) no se comparan.

namespace F99ALayout {
static_assert(sizeof(CVector) == 12, "CVector");
}

namespace F99ALive {
using namespace F99AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x40FE00, 0x59AE70, 0x59AEA0, 0x59AED0, 0x59AF00, 0x59AF80, 0x59B120, 0x59B920, 0x59BE30, 0x59C810};
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arena
template<size_t WIN> struct Arena {
    static constexpr size_t N = F99AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> memA;             // 0: ventana de 48 dwords con guarda de 0x40 bytes a cada lado

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
        memA.Fill(c.seed * 13u + 50u);
        TagClear();
        TagAdd(memA.m.data(), memA.m.size());     // 0
        nev = 0;
        for (unsigned k = 0; k < kWords; ++k) memA.Put32(4 * k, c.mem[k]);
        memA.Snap();
        const u32 pa = u32(memA.Obj(4 * c.pa)), pb = u32(memA.Obj(4 * c.pb)), po = u32(memA.Obj(4 * c.po));
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        bool eaxIsOut = false;
        switch (fn) {
        case 0: ecx = po; args = {pa, pb}; break;                                      // CVector::Difference, `ret 8`
        case 1: case 2: ecx = po; break;                                               // SetUnity / ResetOrientation, `ret`
        case 3: ecx = po; args = {c.f0}; break;                                        // SetScale(float), `ret 4`
        case 4: case 5: case 6: ecx = po; args = {c.f0, c.f1, c.f2}; break;            // SetScale(x,y,z) / SetTranslateOnly / SetRotate(x,y,z), `ret 0Ch`
        case 7: args = {pa, po}; popBytes = 8; eaxIsOut = true; break;                 // Invert(in, out) -> out
        case 8: args = {po, pa, pb}; popBytes = 12; eaxIsOut = true; break;            // MatrixMultiplyExe(out, a, b)
        default: args = {po, pa, pb}; popBytes = 12; eaxIsOut = true; break;           // Multiply3x3VMExe(out, vector (pa), matriz (pb))
        }
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        for (const auto& e : memA.Changes()) {
            // Multiplicacion de matrices: las palabras de relleno de la salida (+0x0C, +0x1C, +0x2C, +0x3C) reciben basura de la pila en el EXE y no se comparan
            if (fn == 8) {
                const int64 rel = e[0].get<int64>() - int64(4 * c.po);
                if (rel >= 0 && rel < 64 && (rel / 4) % 4 == 3) continue;
            }
            ch.push_back(json::array({0, e[0], e[1]}));
        }
        obs["changes"] = ch;
        // Lo que ven los llamadores del EXE: EAX (el puntero `out` o `this`, como etiqueta de arena) en las funciones que lo devuelven
        if (eaxIsOut) obs["exit_eax"] = Tag(uintptr_t(sides[s].reg.eax));
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
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
    // Fin de la prueba: se termina el proceso en el acto. Sin cuadros posteriores del juego (y con todos los hooks del lote activos) no puede saltar ninguna aserción ajena al banco, como la VERIFY de
    // CShadowCamera::DrawOutlineBorder (RwIm2DRenderIndexedPrimitive) que salto en 3 de 5 corridas largas mientras el analizador leia el registro con el juego aun vivo.
    ExitProcess(0);
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn99a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn99a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn99a"); }
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
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    for (unsigned f = 0; f < kFunctions; ++f) {
        collaborators[kFn[f]] = {{"dll_real", json::array({"toda la aritmetica x87 (productos, sumas, restas, seno y coseno y escrituras); la funcion no llama a ninguna otra"})}, {"stubbed", json::array()},
                                 {"hooks_found", json::array()}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);   // entradas del EXE: deben seguir siendo lo que eran al empezar
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
