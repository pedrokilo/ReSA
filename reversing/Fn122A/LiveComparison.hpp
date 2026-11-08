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
#include "Buoyancy.h"
#include "Debug.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn122A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diez funciones diminutas (hook S = RH_ScopedInstall / RH_ScopedNamedGlobalInstall: enlace de la entrada hacia la DLL), cada una con 16384 pares EXE/DLL:
//   0  CDebug::DebugInitTextBuffer                 0x532240  sin argumentos; pone a 0 la palabra de 0xB71508. Sin colaboradores. Memoria global real: 16 bytes desde 0xB71508 (se siembran y se restauran).
//   1  CCivilianPed::DestructorBody                0x5DDBE0  thiscall; deja la vtable 0x86C0A8 y salta a CPed::Destructor 0x5E8620 (hook RH: falso identico en el EXE y en el cuerpo de la DLL).
//   2  cBuoyancy::Constructor                      0x6C2740  thiscall; pone a 0 los dwords de this+0x4C y this+0x50 y devuelve this. Sin colaboradores.
//   3  CTaskComplexEnterCarAsDriver::Destructor    0x640330  thiscall; vtable 0x86EAAC; salta a CTaskComplexEnterCar::Destructor 0x63DFA0 (hook RH).
//   4  CTaskComplexEnterCarAsPassenger::Destructor 0x640380  thiscall; vtable 0x86EADC; salta a 0x63DFA0.
//   5  CTaskComplexFacial::Destructor              0x690D90  thiscall; vtable 0x870ADC; salta a CTaskComplex::Destructor 0x61A3D0 (hook RH).
//   6  CTaskComplexPartnerDeal::Destructor         0x684200  thiscall; vtable 0x870754; salta a ~CTaskComplexPartner 0x683A40 (SIN hook: el falso va solo en la direccion del EXE; la DLL la llama por esa direccion).
//   7  CTaskSimpleAchieveHeading::Destructor       0x667E70  thiscall; vtable 0x86FD74; salta a CTask::Destructor 0x61A3A0 (hook RH).
//   8  CTaskSimpleGoTo::DestructorBody             0x667A00  thiscall; vtable 0x86FD1C; salta a 0x61A3A0.
//   9  CTaskSimpleSetStayInSamePlace::Destructor   0x62F5D0  thiscall; vtable 0x86DD50; salta a 0x61A3A0.
// El banco invoca la direccion del EXE con un objeto sintetico (arena propia, `this` en un desplazamiento alineado, desalineado o al borde de la ventana).
// Falsos de los colaboradores: registran un evento (direccion del colaborador, this como etiqueta de arena, dword [this] en ese instante = la vtable recien escrita) y vuelven. Con hook RH (apagado
// durante el banco) el mismo falso va en la direccion del EXE y en el cuerpo de la DLL; sin hook (0x683A40) solo en la del EXE.
// Se observan: los bytes cambiados de las arenas y de la region global, el dword final en [this], los eventos, EAX (etiqueta de arena) en el constructor, y EAX/ECX/EDX de salida en los residuos
// (informativos: ningun llamador del EXE los lee; solo ESI se conserva y es una de las compuertas de ABI).

namespace F122ALayout {
static_assert(offsetof(cBuoyancy, m_EntityMatrix) == 0xC && sizeof(cBuoyancy) == 0xD0, "cBuoyancy");
static_assert(offsetof(CMatrix, m_pAttachMatrix) == 0x40 && offsetof(CMatrix, m_bOwnsAttachedMatrix) == 0x44 && sizeof(CMatrix) == 0x48, "CMatrix");
}

namespace F122ALive {
using namespace F122AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x532240, 0x5DDBE0, 0x6C2740, 0x640330, 0x640380, 0x690D90, 0x684200, 0x667E70, 0x667A00, 0x62F5D0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F122AFix::Arena(WIN);
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

// ================================================================================================ falsos de los colaboradores (todos thiscall sin argumentos de pila: `ret`)
// Evento: [direccion del EXE del colaborador, this (etiqueta de arena), dword en [this] al entrar]
template<u32 ADDR> inline void* __fastcall FkDtor(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    return self;
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
        std::array<u32, 4> globalPrev{};
        if (fn == 0) {
            // memoria global real: cuatro dwords previos desde 0xB71508 (la region vigilada se restaura tras el lado)
            for (int i = 0; i < 4; ++i) globalPrev[i] = c.a[6 + i];
            memcpy(reinterpret_cast<void*>(kGlobalAddr), globalPrev.data(), kGlobalBytes);
        } else if (fn == 2) {
            objA.Put32(c.a[0] + 0x4C, c.a[6]);
            objA.Put32(c.a[0] + 0x50, c.a[7]);
        } else {
            objA.Put32(c.a[0], c.a[6]);   // imagen previa de [this]
        }
        objA.Snap(); aA.Snap();
        TagClear(); TagAdd(objA.m.data(), objA.m.size()); TagAdd(aA.m.data(), aA.m.size());
        nev = 0;
        const u32 ecxSeed = fn == 0 ? c.ecx : u32(self);
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, nullptr, 0, c.eax, ecxSeed, c.edx, c.stack, 0);
        });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, aA.Changes());
        if (fn == 0) {
            const uint8* now = reinterpret_cast<const uint8*>(kGlobalAddr);
            const uint8* was = reinterpret_cast<const uint8*>(globalPrev.data());
            for (size_t i = 0; i < kGlobalBytes; ++i) if (now[i] != was[i]) ch.push_back(json::array({2, i, now[i]}));
        }
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["vptr"] = kVtable[fn] ? json(objA.Get32(c.a[0])) : json(nullptr);
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
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
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
    if (!std::filesystem::exists(dir / "fn122a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn122a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn122a"); }
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
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {};
    env.stubs[1] = {F(0x5E8620, &FkDtor<0x5E8620>)};
    env.stubs[2] = {};
    env.stubs[3] = {F(0x63DFA0, &FkDtor<0x63DFA0>)};
    env.stubs[4] = {F(0x63DFA0, &FkDtor<0x63DFA0>)};
    env.stubs[5] = {F(0x61A3D0, &FkDtor<0x61A3D0>)};
    env.stubs[6] = {F(0x683A40, &FkDtor<0x683A40>)};   // sin hook RH: Pair{exe, 0, false}; la DLL llama a la direccion del EXE
    env.stubs[7] = {F(0x61A3A0, &FkDtor<0x61A3A0>)};
    env.stubs[8] = {F(0x61A3A0, &FkDtor<0x61A3A0>)};
    env.stubs[9] = {F(0x61A3A0, &FkDtor<0x61A3A0>)};
    const char* dllReal[kFunctions] = {
        "toda la funcion: pone a 0 la palabra de 16 bits de 0xB71508",
        "escritura de la vtable (0x86C0A8) en [this]; el salto a CPed::Destructor es un falso identico",
        "toda la funcion: pone a 0 los dwords de this+0x4C y this+0x50 y devuelve this",
        "escritura de la vtable (0x86EAAC) en [this]; el salto a CTaskComplexEnterCar::Destructor es un falso identico",
        "escritura de la vtable (0x86EADC) en [this]; el salto a CTaskComplexEnterCar::Destructor es un falso identico",
        "escritura de la vtable (0x870ADC) en [this]; el salto a CTaskComplex::Destructor es un falso identico",
        "escritura de la vtable (0x870754) en [this]; la llamada a 0x683A40 (sin hook) es un falso en la direccion del EXE",
        "escritura de la vtable (0x86FD74) en [this]; el salto a CTask::Destructor es un falso identico",
        "escritura de la vtable (0x86FD1C) en [this]; el salto a CTask::Destructor es un falso identico",
        "escritura de la vtable (0x86DD50) en [this]; el salto a CTask::Destructor es un falso identico"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", json::array()}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
