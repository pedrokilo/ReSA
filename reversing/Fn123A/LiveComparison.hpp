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
#include "TaskComplexPartner.h"
#include "AnimManager.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn123A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S = RH_ScopedNamedGlobalInstall: enlace de la entrada hacia la DLL), con 16384 pares EXE/DLL:
//   0  CTaskComplexPartner::Destructor  0x683A40  thiscall; vtable 0x870664; libera el companero (+0x38) con CEntity::CleanUpOldReference 0x571A00; si el byte +0x6E != 0 y el primer byte del nombre
//      (+0x5E) != 0 llama a GetAnimationBlockIndex 0x4D3990 y a RemoveAnimBlockRef 0x4D3FD0; pone +0x6E a 0; llama a CTaskComplex::Destructor 0x61A3D0.
// Todos los colaboradores tienen hook RH (apagado durante el banco): el mismo falso va en la direccion del EXE y en el cuerpo de la DLL.
// El banco invoca la direccion del EXE con un objeto sintetico (arena propia, `this` en un desplazamiento alineado, desalineado o al borde de la ventana).
// Los falsos registran un evento con sus argumentos y vuelven: 0x571A00 [dir, ECX (companero), &m_partner, [this] al entrar], 0x4D3990 [dir, &nombre, FNV-1a de los 16 bytes del nombre] (devuelve el
// indice del caso), 0x4D3FD0 [dir, indice], 0x61A3D0 [dir, this, [this], byte +0x6E, dword +0x38].
// Se observan: los bytes cambiados de las arenas y de la region global (ms_aAnimBlocks), el dword final en [this], los eventos, EAX/ECX/EDX de salida en los residuos (informativos: ningun llamador del
// EXE los lee), pila, no volatiles, DF, SEH, x87 y la palabra de control.

namespace F123ALayout {
static_assert(sizeof(CTaskComplexPartner) == 0x70, "CTaskComplexPartner");
static_assert(offsetof(CTaskComplexPartner, m_partner) == 0x38 && offsetof(CTaskComplexPartner, m_animBlockName) == 0x5E && offsetof(CTaskComplexPartner, m_requiredAnimsStreamedIn) == 0x6E, "campos");
}

namespace F123ALive {
using namespace F123AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x683A40};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F123AFix::Arena(WIN);
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

// ================================================================================================ falsos de los colaboradores
inline u32 curIdx{};   // lo que devuelve el falso de GetAnimationBlockIndex (el caso)
// Lectura segura de un dword/16 bytes solo si caen dentro de la arena del objeto (un candidato mutado podria pasar otro puntero)
inline bool InObjArena(uintptr_t p, size_t n) {return !tagTable.empty() && p >= tagTable[0].base && p + n <= tagTable[0].base + tagTable[0].size;}
// 0x571A00 CEntity::CleanUpOldReference (thiscall, argumento en la pila, `ret 4`): [dir, ECX, &m_partner, dword en [&m_partner - 0x38]]
inline void __fastcall FkCleanUp(void* ent, void*, void* ref) {
    Ev& e = PushEv(0x571A00, 3);
    e.v[0] = Tag(ent);
    e.v[1] = Tag(ref);
    e.v[2] = InObjArena(uintptr_t(ref) - 0x38, 4) ? BitsOf(reinterpret_cast<const void*>(uintptr_t(ref) - 0x38)) : 0xBADBAD01u;
}
// 0x4D3990 CAnimManager::GetAnimationBlockIndex(const char*) (cdecl): [dir, &nombre, FNV-1a de los 16 bytes]; devuelve el indice del caso
inline u32 __cdecl FkBlockIndex(const char* name) {
    Ev& e = PushEv(0x4D3990, 2);
    e.v[0] = Tag(name);
    e.v[1] = InObjArena(uintptr_t(name), kNameLen) ? Fnv(name, kNameLen) : 0xBADBAD02u;
    return curIdx;
}
// 0x4D3FD0 CAnimManager::RemoveAnimBlockRef(int) (cdecl): [dir, indice]
inline void* __cdecl FkRemoveRef(u32 idx) {
    Ev& e = PushEv(0x4D3FD0, 1);
    e.v[0] = idx;
    return reinterpret_cast<void*>(0xB5D4A0);
}
// 0x61A3D0 CTaskComplex::Destructor (thiscall, `ret`): [dir, this, [this], byte +0x6E, dword +0x38]
inline void* __fastcall FkBase(void* self, void*) {
    Ev& e = PushEv(0x61A3D0, 4);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    e.v[2] = *reinterpret_cast<const u8*>(uintptr_t(self) + kOffFlag);
    e.v[3] = Tag(uintptr_t(BitsOf(reinterpret_cast<const void*>(uintptr_t(self) + kOffPartner))));
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

inline uintptr_t FromTag(u32 v) {
    if ((v >> 28) == 1) {const size_t i = (v >> 20) & 0xF; return tagTable[i].base + (v & 0xFFFFF);}
    return v;
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
        TagClear(); TagAdd(objA.m.data(), objA.m.size()); TagAdd(aA.m.data(), aA.m.size());
        // imagen previa del objeto: vtable previa, companero, nombre del bloque y byte de bandera
        objA.Put32(c.a[0], c.a[5]);
        objA.Put32(c.a[0] + kOffPartner, u32(FromTag(c.a[3])));
        for (int i = 0; i < 4; ++i) objA.Put32(c.a[0] + kOffName + 4 * i, c.a[6 + i]);
        objA.m[kGuard + c.a[0] + kOffFlag] = u8(c.a[4]);
        objA.Snap(); aA.Snap();
        curIdx = c.a[10];
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, nullptr, 0, c.eax, u32(self), c.edx, c.stack, 0);
        });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, aA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["vptr"] = json(objA.Get32(c.a[0]));
        obs["ret"] = json(nullptr);
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
    if (!std::filesystem::exists(dir / "fn123a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn123a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn123a"); }
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
    env.stubs[0] = {F(0x571A00, &FkCleanUp), F(0x4D3990, &FkBlockIndex), F(0x4D3FD0, &FkRemoveRef), F(0x61A3D0, &FkBase)};   // los cuatro con hook RH: falso identico en el EXE y en el cuerpo de la DLL
    const char* dllReal[kFunctions] = {
        "toda la funcion: escritura de la vtable (0x870664), prueba de companero, byte +0x6E y primer byte del nombre, puesta a cero del byte +0x6E y orden de las cuatro llamadas; las cuatro llamadas son falsos identicos"};
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
