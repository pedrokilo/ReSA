#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Cam.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn88A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CCam::GetArrestCamCopSourceNormal 0x515D80 (thiscall, ECX sin usar; target, cop, dest, source en pila, `ret 10h`; devuelve AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE
// y en el cuerpo de la DLL (que se resuelve por el puntero a la funcion): CVector::Normalise 0x59C910 (thiscall) y CrossProduct 0x59C730 (cdecl, devuelve out). Registran sus argumentos EN ORDEN (con el contenido de los
// vectores en la llamada). CrossProduct escribe `cross`; Normalise escribe nS (1a llamada), nD1 (2a) y nD2 (3a). La aritmetica (resta dest - cop, desplazamientos de 3.0f y 5.0f, aplanado de la direccion, distancia
// y reescalado a 8.0f; ayudantes x87 de 24 bits de la DLL frente a la x87 del EXE) corre de verdad en ambos lados. No hay globales del juego.
// Mutaciones (una sola vez, dentro de un falso): 1 (Normalise 1) dest.x; 2 y 3 (Normalise 2) dest.y y dest.z; 4 (CrossProduct) el puntero de matriz del ped; 5 (Normalise 2) la posicion activa del ped. El EXE relee dest
// tras cada llamada y copia la posicion del ped solo al empezar.

namespace F88ALive {
using namespace F88AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x515D80};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, sin los bytes [maskLo, maskHi) (campos de puntero que cambian por direccion)
    json Changes(size_t maskLo = 0, size_t maskHi = 0) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i] && !(off >= int64(maskLo) && off < int64(maskHi) && maskHi != 0)) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kCamArena> camA;
inline Arena<kEntArena> entA;
inline Arena<kPedArena> pedA;
inline Arena<kMatArena> matA;
inline Arena<kDestArena> destA;
inline Arena<kSrcArena> srcA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[8]{}; unsigned normCalls{}, crossCalls{}; };
inline State g;
inline bool Once(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}
inline void Vec3(Ev& e, u32 at, const void* p) { memcpy(&e.v[at], p, 12); }

// Eventos: 1 CrossProduct [out, a, b, a.xyz, b.xyz]; 2 Normalise [vector, x, y, z].
inline void __fastcall FkNormalise(float* self, void*) {                                            // 0x59C910
    Ev& e = PushEv(2, 4); e.v[0] = TagS(uintptr_t(self)); Vec3(e, 1, self);
    const unsigned call = g.normCalls++;
    const u32* v = call == 0 ? g.c->nS : (call == 1 ? g.c->nD1 : g.c->nD2);
    memcpy(self, v, 12);
    if (call == 0 && Once(1)) destA.Put32(kDestOff, g.c->mutVal);
    if (call == 1) {
        if (Once(2)) destA.Put32(kDestOff + 4, g.c->mutVal);
        if (Once(3)) destA.Put32(kDestOff + 8, g.c->mutVal);
        if (Once(5)) { if (g.c->copMat) matA.Put32(0x30, g.c->mutVal); else pedA.Put32(4, g.c->mutVal); }
    }
}
inline u32 __cdecl FkCross(float* out, float* a, float* b) {                                        // 0x59C730
    Ev& e = PushEv(1, 9); e.v[0] = TagS(uintptr_t(out)); e.v[1] = TagS(uintptr_t(a)); e.v[2] = TagS(uintptr_t(b)); Vec3(e, 3, a); Vec3(e, 6, b);
    ++g.crossCalls;
    memcpy(out, g.c->cross, 12);
    if (Once(4)) pedA.Put32(0x14, pedA.Get32(0x14) ? 0u : u32(matA.Obj()));
    return u32(uintptr_t(out));
}

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

inline void SetupArenas(const C& c) {
    camA.Fill(c.seed * 13u + 50u); entA.Fill(c.seed * 13u + 51u); pedA.Fill(c.seed * 13u + 52u);
    matA.Fill(c.seed * 13u + 53u); destA.Fill(c.seed * 13u + 54u); srcA.Fill(c.seed * 13u + 55u);
}
inline void SnapArenas() { camA.Snap(); entA.Snap(); pedA.Snap(); matA.Snap(); destA.Snap(); srcA.Snap(); }
inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(camA.m.data(), camA.m.size());          // 0
        TagAdd(entA.m.data(), entA.m.size());          // 1
        TagAdd(pedA.m.data(), pedA.m.size());          // 2
        TagAdd(matA.m.data(), matA.m.size());          // 3
        TagAdd(destA.m.data(), destA.m.size());        // 4
        TagAdd(srcA.m.data(), srcA.m.size());          // 5
        // ped: posicion en la ubicacion activa (matriz +0x30 o compacta +4) y en la otra
        pedA.Put32(0x14, c.copMat ? u32(matA.Obj()) : 0u);
        if (c.copMat) { Put3(matA, 0x30, c.copPos); Put3(pedA, 4, c.copAlt); }
        else { Put3(pedA, 4, c.copPos); Put3(matA, 0x30, c.copAlt); }
        Put3(destA, kDestOff, c.dest); Put3(srcA, kSrcOff, c.srcInit);
        const std::vector<u32> args = { c.targetNull ? 0u : u32(entA.Obj()), c.copNull ? 0u : u32(pedA.Obj()), u32(destA.Obj(kDestOff)), u32(srcA.Obj(kSrcOff)) };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(camA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret"] = int(sides[s].reg.eax & 0xFFu);
        obs["mats"] = json::array({pedA.Get32(0x14) != 0});
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, camA.Changes()); add(1, entA.Changes()); add(2, pedA.Changes(0x14, 0x18)); add(3, matA.Changes()); add(4, destA.Changes()); add(5, srcA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
    }
    for (const auto& s : env.dllOnly) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", true},
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
    if (!std::filesystem::exists(dir / "fn88a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn88a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn88a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    // CVector::Normalise y CrossProduct(out) tienen hook RH en la DLL pero no figuran como hooks Simple del catalogo: el cuerpo de la DLL se resuelve por el puntero a la funcion (como en Helpers66D y Fn86A).
    void (CVector::*normalise)() = &CVector::Normalise;
    CVector* (*crossOut)(CVector*, CVector*, CVector*) = &CrossProduct;
    const Pair pNormalise{0x59C910, Body(std::bit_cast<uintptr_t>(normalise)), true}, pCross{0x59C730, Body(reinterpret_cast<uintptr_t>(crossOut)), true};
    env.stubs[0] = {S(pNormalise, reinterpret_cast<void*>(&FkNormalise)), S(pCross, reinterpret_cast<void*>(&FkCross))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"resta dest - cop, desplazamientos de 3.0f y 5.0f, aplanado de la direccion, distancia y reescalado a 8.0f (ayudantes x87 de 24 bits de la DLL frente a la x87 del EXE) y comparaciones"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
