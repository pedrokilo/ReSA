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

// Banco par a par Fn86A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CCam::ProcessArrestCamCheese 0x512EF0 (thiscall, ret 8: from, initialise; devuelve AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE y, los que tienen hook RH en la DLL
// (apagado durante el banco), tambien en el cuerpo de la DLL: rand 0x821B1E (cdecl, CRT del EXE), GetAnimHierarchyFromSkinClump 0x734A40 (cdecl), RpHAnimIDGetIndex 0x7C51A0 (cdecl), RpHAnimHierarchyGetMatrixArray 0x7C5120 (cdecl),
// CVector::Normalise 0x59C910 (thiscall), CrossProduct 0x59C730 (cdecl, devuelve out) y CWorld::GetIsLineOfSightClear 0x56A490 (cdecl, 9 argumentos). Registran sus argumentos EN ORDEN (con el contenido de los vectores en la llamada).
// Los dos Normalise y los dos CrossProduct escriben lo que dicta el caso (n1/n2, c1/c2). La aritmetica (tiempo, t, punto mirado, source += desplazamiento, front = P - source, comparaciones) corre de verdad en ambos lados;
// los estaticos 0xB70000..0xB7002F, 0xB6EC60, 0xB6F980 y 0xB7CB84 son memoria real del juego (se rellenan y se restauran tras cada lado).
// Mutaciones (una sola vez): 1..9 en rand (cambian el objetivo de la camara, el tiempo, 0xB6EC60, 0xB70024, el clump del ped, las banderas 0xB7001C, 0xB70004, la matriz o el tipo del objetivo actual); 10 en
// RpHAnimHierarchyGetMatrixArray (z del hueso); 11 en la primera linea de vista (objetivo de la camara). En todos los casos el EXE relee despues esa memoria.

namespace F86ALayout {
static_assert(offsetof(CCam, m_vecFront) == 0x190 && offsetof(CCam, m_vecSource) == 0x19C && offsetof(CCam, m_vecUp) == 0x1B4 && offsetof(CCam, m_fFOV) == 0xB4, "CCam: front +0x190, source +0x19C, up +0x1B4, FOV +0xB4");
}

namespace F86ALive {
using namespace F86AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x512EF0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
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
inline std::array<Arena<kEntArena>, 2> entA;
inline std::array<Arena<kMatArena>, 2> matA;
inline Arena<kPedArena> pedA;
inline Arena<kBoneArena> boneA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[12]{}; unsigned normCalls{}, crossCalls{}, losCalls{}; };
inline State g;
inline bool Once(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }
inline unsigned CurTarget() { return G32(0xB6F980) == u32(entA[1].Obj()) ? 1u : 0u; }
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}
inline void Vec3(Ev& e, u32 at, const void* p) { memcpy(&e.v[at], p, 12); }

// Eventos: 1 rand []; 2 GetAnimHierarchyFromSkinClump [clump]; 3 RpHAnimIDGetIndex [jerarquia, id]; 4 RpHAnimHierarchyGetMatrixArray [jerarquia]; 5 Normalise [vector, x, y, z];
// 6 CrossProduct [out, a, b, a.xyz, b.xyz]; 7 GetIsLineOfSightClear [origen, origen.xyz, destino, destino.xyz, arg3..arg9 (byte bajo)].
inline u32 __cdecl FkRand() {                                                                       // 0x821B1E
    PushEv(1, 0);
    if (Once(1)) G32(0xB6F980) = u32(entA[1].Obj());
    if (Once(2)) G32(0xB7CB84) = g.c->mutVal;
    if (Once(3)) G32(0xB6EC60) = g.c->mutVal;
    if (Once(4)) G32(0xB70024) = g.c->mutVal;
    if (Once(5)) pedA.Put32(0x18, g.c->mutVal);
    if (Once(6)) G32(0xB7001C) = g.c->mutVal;
    if (Once(7)) G32(0xB70004) = g.c->mutVal;
    if (Once(8)) { auto& e = entA[CurTarget()]; e.Put32(0x14, e.Get32(0x14) ? 0u : u32(matA[CurTarget()].Obj())); }
    if (Once(9)) entA[CurTarget()].Put8(0x36, g.c->mutVal);
    return g.c->rnd;
}
inline u32 __cdecl FkHier(u32 clump) {                                                              // 0x734A40
    Ev& e = PushEv(2, 1); e.v[0] = clump;
    return g.c->hier;
}
inline u32 __cdecl FkIndex(u32 hier, u32 id) {                                                      // 0x7C51A0
    Ev& e = PushEv(3, 2); e.v[0] = hier; e.v[1] = id;
    return g.c->idx;
}
inline u32 __cdecl FkMatrices(u32 hier) {                                                           // 0x7C5120
    Ev& e = PushEv(4, 1); e.v[0] = hier;
    if (Once(10)) boneA.Put32(size_t(g.c->idx) * 0x40 + 0x38, g.c->mutVal);
    return u32(boneA.Obj());
}
inline void __fastcall FkNormalise(float* self, void*) {                                            // 0x59C910
    Ev& e = PushEv(5, 4); e.v[0] = TagS(uintptr_t(self)); Vec3(e, 1, self);
    const u32* v = g.normCalls++ == 0 ? g.c->n1 : g.c->n2;
    memcpy(self, v, 12);
}
inline u32 __cdecl FkCross(float* out, float* a, float* b) {                                        // 0x59C730
    Ev& e = PushEv(6, 9); e.v[0] = TagS(uintptr_t(out)); e.v[1] = TagS(uintptr_t(a)); e.v[2] = TagS(uintptr_t(b)); Vec3(e, 3, a); Vec3(e, 6, b);
    const u32* v = g.crossCalls++ == 0 ? g.c->c1 : g.c->c2;
    memcpy(out, v, 12);
    return u32(uintptr_t(out));
}
inline u32 __cdecl FkLos(float* origin, float* target, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9) {   // 0x56A490 (AL)
    Ev& e = PushEv(7, 15); e.v[0] = TagS(uintptr_t(origin)); Vec3(e, 1, origin); e.v[4] = TagS(uintptr_t(target)); Vec3(e, 5, target);
    const u32 args[7]{a3, a4, a5, a6, a7, a8, a9};
    for (u32 i = 0; i < 7; ++i) e.v[8 + i] = args[i] & 0xFFu;
    const unsigned call = g.losCalls++;
    if (call == 0 && Once(11)) G32(0xB6F980) = u32(entA[1].Obj());
    return call == 0 ? g.c->los1 : g.c->los2;
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
    camA.Fill(c.seed * 13u + 50u); pedA.Fill(c.seed * 13u + 57u); boneA.Fill(c.seed * 13u + 58u);
    for (unsigned i = 0; i < 2; ++i) { entA[i].Fill(c.seed * 13u + 60u + i); matA[i].Fill(c.seed * 13u + 62u + i); }
}
inline void SnapArenas() {
    camA.Snap(); pedA.Snap(); boneA.Snap();
    for (unsigned i = 0; i < 2; ++i) { entA[i].Snap(); matA[i].Snap(); }
}
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
        TagAdd(entA[0].m.data(), entA[0].m.size());    // 1
        TagAdd(entA[1].m.data(), entA[1].m.size());    // 2
        TagAdd(pedA.m.data(), pedA.m.size());          // 3
        TagAdd(matA[0].m.data(), matA[0].m.size());    // 4
        TagAdd(matA[1].m.data(), matA[1].m.size());    // 5
        TagAdd(boneA.m.data(), boneA.m.size());        // 6
        // camara
        Put3(camA, 0x19C, c.csrc); Put3(camA, 0x190, c.cfront); Put3(camA, 0x1B4, c.cup); camA.Put32(0xB4, c.cfov);
        // objetivos 0 y 1: posicion en la ubicacion activa (matriz +0x30 o compacta +4) y en la otra
        for (unsigned e = 0; e < 2; ++e) {
            auto& ent = entA[e]; auto& mat = matA[e];
            ent.Put8(0x36, c.etype[e]); ent.Put32(0x1C, c.eflags[e]);
            ent.Put32(0x14, c.emat[e] ? u32(mat.Obj()) : 0u);
            if (c.emat[e]) { Put3(mat, 0x30, &c.epos[e * 3]); Put3(ent, 4, &c.ealt[e * 3]); }
            else { Put3(ent, 4, &c.epos[e * 3]); Put3(mat, 0x30, &c.ealt[e * 3]); }
        }
        pedA.Put32(0x18, c.clump);
        Put3(boneA, size_t(c.idx) * 0x40 + 0x30, c.bone);
        // globales reales del juego
        for (unsigned i = 0; i < kStaticBytes / 4; ++i) G32(kStaticLow + 4 * i) = Rand(c.seed, 500 + i);
        G32(0xB7001C) = c.st; G8(0xB70020) = uint8(c.b20); G32(0xB70024) = c.f2;
        for (unsigned k = 0; k < 3; ++k) { G32(0xB70004 + 4 * k) = c.voff[k]; G32(0xB70010 + 4 * k) = c.vzero[k]; }
        G32(0xB6EC60) = c.f1; G32(0xB7CB84) = c.ms; G32(0xB6F980) = u32(entA[0].Obj());
        const std::vector<u32> args = { c.fromNull ? 0u : u32(pedA.Obj()), c.init };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(camA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret"] = int(sides[s].reg.eax & 0xFFu);
        json gl = json::array();
        for (unsigned i = 0; i < kStaticBytes / 4; ++i) gl.push_back(G32(kStaticLow + 4 * i));
        gl.push_back(G32(0xB6EC60)); gl.push_back(Tag(uintptr_t(G32(0xB6F980)))); gl.push_back(G32(0xB7CB84));
        obs["globals"] = gl;
        obs["mats"] = json::array({entA[0].Get32(0x14) != 0, entA[1].Get32(0x14) != 0});
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, camA.Changes()); add(1, entA[0].Changes(0x14, 0x18)); add(2, entA[1].Changes(0x14, 0x18)); add(3, pedA.Changes());
        add(4, matA[0].Changes()); add(5, matA[1].Changes()); add(6, boneA.Changes());
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
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn86a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn86a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn86a"); }
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
    // CVector::Normalise y CrossProduct(out) tienen hook RH en la DLL pero no figuran como hooks Simple del catalogo: el cuerpo de la DLL se resuelve por el puntero a la funcion (como en Helpers66D).
    void (CVector::*normalise)() = &CVector::Normalise;
    CVector* (*crossOut)(CVector*, CVector*, CVector*) = &CrossProduct;
    const Pair pNormalise{0x59C910, Body(std::bit_cast<uintptr_t>(normalise)), true}, pCross{0x59C730, Body(reinterpret_cast<uintptr_t>(crossOut)), true};
    const Pair pRand = MakeCollab(env, 0x821B1E, hooksFound), pHier = MakeCollab(env, 0x734A40, hooksFound), pIndex = MakeCollab(env, 0x7C51A0, hooksFound), pMatrices = MakeCollab(env, 0x7C5120, hooksFound),
               pLos = MakeCollab(env, 0x56A490, hooksFound);
    env.stubs[0] = {S(pRand, reinterpret_cast<void*>(&FkRand)), S(pHier, reinterpret_cast<void*>(&FkHier)), S(pIndex, reinterpret_cast<void*>(&FkIndex)), S(pMatrices, reinterpret_cast<void*>(&FkMatrices)),
                    S(pNormalise, reinterpret_cast<void*>(&FkNormalise)), S(pCross, reinterpret_cast<void*>(&FkCross)), S(pLos, reinterpret_cast<void*>(&FkLos))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"aritmetica del tiempo, de t, del punto mirado, de source y de front (ayudantes x87 de 24 bits de la DLL frente a la x87 del EXE) y comparaciones"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
