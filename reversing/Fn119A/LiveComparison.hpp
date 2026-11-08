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
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn119A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 17 funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la lista con sus direcciones): el constructor y el cuerpo del destructor de siete eventos y el cuerpo del destructor de otros tres.
//   constructores (thiscall, argumentos en la pila, `ret N`, EAX = this): colaborador falseado 0x571B70 (CEntity::RegisterReference). El objeto parte de basura con una vtable previa conocida; no se comparan los bytes de la vtable
//   (el EXE deja la suya y la DLL la de la DLL: se observa `vptr_changed` y GetEventType / GetEventPriority / GetLifeTime por las ranuras 1..3 de la vtable que quedo escrita) ni los bytes de los punteros (se informan como etiquetas de arena).
//   destructores (cuerpo no virtual del EXE, thiscall, `ret`): colaborador falseado 0x571A00 (CEntity::CleanUpOldReference).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.

namespace F119ALive {
using namespace F119AFix;
inline constexpr auto KEntries() { std::array<uintptr_t, kFunctions> r{}; for (unsigned i = 0; i < kFunctions; ++i) r[i] = kSpecs[i].entry; return r; }
inline constexpr auto kEntries = KEntries();
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F119AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    u32 Was32(size_t off) const { u32 v; memcpy(&v, r.data() + kGuard + off, 4); return v; }
    std::string PreHex(size_t bytes) const { return HexBytes(r.data() + kGuard, bytes); }
    // Cambios respecto a la imagen de referencia, sin los rangos [desplazamiento, longitud) respecto al objeto
    json Changes(std::initializer_list<std::pair<int64, int64>> skip = {}) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            bool masked = false;
            for (const auto& s : skip) masked |= off >= s.first && off < s.first + s.second;
            if (m[i] != r[i] && !masked) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kEventWin> evA;         // 0: evento (hasta 0x40 bytes)
inline Arena<kEntWin> entA;          // 1: primera entidad referenciada
inline Arena<kEntWin> entB;          // 2: segunda entidad referenciada
inline Arena<kVecWin> vecA;          // 3: CVector que recibe por puntero CEventFireNearby (12 bytes con guardas)
inline constexpr u32 kRaw1 = 0x00BADC00u, kRaw2 = 0x00BADC40u;

// Estado del caso en curso para los falsos
struct State { const C* c{}; };
inline State g;

// Eventos: 11 CEntity::RegisterReference 0x571B70 [entidad, &referencia, 1 si la vtable ya no es la previa]; 20 CEntity::CleanUpOldReference 0x571A00 [entidad, &referencia, vtable del evento en ese momento].
inline void __fastcall FkRegRef(void* self, void*, void* ref) { Ev& e = PushEv(11, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = evA.Get32(0) != kPreVtable ? 1u : 0u; }
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(20, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = evA.Get32(0); }
inline u32 EntPtr(u32 mode, const uintptr_t arena, u32 raw) { return mode == 0 ? u32(arena) : (mode == 1 ? 0u : raw); }

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
    const Spec& sp = kSpecs[fn];
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 60u;
        evA.Fill(sd++); entA.Fill(sd++); entB.Fill(sd++); vecA.Fill(sd++);
        TagClear();
        TagAdd(evA.m.data(), evA.m.size());         // 0
        TagAdd(entA.m.data(), entA.m.size());       // 1
        TagAdd(entB.m.data(), entB.m.size());       // 2
        TagAdd(vecA.m.data(), vecA.m.size());       // 3
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(evA.Obj()), popBytes = 0;
        std::array<u32, 3> ptrOff{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu};   // desplazamientos de los punteros del constructor (se excluyen de los bytes y se informan como etiquetas)
        if (sp.kind == 0) {
            evA.Put32(0, c.a[2]);
            if (sp.refs >= 1) evA.Put32(sp.off[0], EntPtr(c.a[0], entA.Obj(), kRaw1));
            if (sp.refs == 2) evA.Put32(sp.off[1], EntPtr(c.a[1], entB.Obj(), kRaw2));
        } else {
            evA.Put32(0, kPreVtable);
            for (u32 i = 0; i < sp.nargs; ++i) {
                u32 v = c.a[i];
                if (sp.args[i].kind == 1) { v = EntPtr(c.a[i], i == 0 ? entA.Obj() : entB.Obj(), i == 0 ? kRaw1 : kRaw2); ptrOff[i] = sp.args[i].off; }
                if (sp.args[i].kind == 4) { v = u32(vecA.Obj()); for (u32 k = 0; k < 3; ++k) vecA.Put32(4 * k, c.a[8 + k]); }
                args.push_back(v);
            }
            // thiscall con `ret N`: el callee limpia la pila, popBytes = 0
        }
        evA.Snap(); entA.Snap(); entB.Snap(); vecA.Snap();
        const std::string pre = evA.PreHex(0x40);
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        if (sp.kind == 0) add(0, evA.Changes());
        else {
            std::vector<std::pair<int64, int64>> skip{{0, 4}};
            for (u32 i = 0; i < 3; ++i) if (ptrOff[i] != 0xFFFFFFFFu) skip.push_back({int64(ptrOff[i]), 4});
            json list = json::array();
            for (size_t i = 0; i < evA.N; ++i) {
                const int64 off = int64(i) - int64(kGuard);
                bool masked = false;
                for (const auto& k : skip) masked |= off >= k.first && off < k.first + k.second;
                if (evA.m[i] != evA.r[i] && !masked) list.push_back(json::array({off, evA.m[i]}));
            }
            add(0, list);
        }
        add(1, entA.Changes()); add(2, entB.Changes()); add(3, vecA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["pre"] = pre;
        if (sp.kind == 1) {
            obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
            json ptrs = json::array();
            for (u32 i = 0; i < 3; ++i) if (ptrOff[i] != 0xFFFFFFFFu) ptrs.push_back(Tag(uintptr_t(evA.Get32(ptrOff[i]))));
            obs["ptrs"] = ptrs;
            obs["vptr_changed"] = evA.Get32(0) != evA.Was32(0);
            // la vtable que dejo escrita el constructor: GetEventType (ranura 1), GetEventPriority (ranura 2) y GetLifeTime (ranura 3)
            auto* const self = reinterpret_cast<void*>(evA.Obj(0));
            auto* const vt = *reinterpret_cast<uintptr_t**>(self);
            json v = json::array();
            if (evA.Get32(0) != kPreVtable) for (unsigned k = 1; k <= 3; ++k) v.push_back(int32(reinterpret_cast<int(__thiscall*)(void*)>(vt[k])(self)));
            obs["vt"] = v;
        }
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
    // El juego sigue ejecutando fotogramas tras el ultimo evento: el runner cierra GTA al ver `complete` (sondeo cada 0,5 s). Una parada de 20 s dentro de este fotograma evita que un VERIFY ajeno al banco (p. ej. el de
    // CShadowCamera::DrawOutlineBorder, que falla si el dispositivo D3D9 pasa un rato en segundo plano durante la corrida de 15 minutos) se escriba como ultimo evento y se tome por fallo del banco.
    if (ready && totals.passed && stateRestored) Sleep(20000);
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
    if (!std::filesystem::exists(dir / "fn119a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn119a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn119a"); }
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
    for (unsigned f = 0; f < kFunctions; ++f) {
        const Spec& sp = kSpecs[f];
        env.stubs[f] = {sp.kind == 0 ? F(0x571A00, &FkCleanUp) : F(0x571B70, &FkRegRef)};
    }
    std::array<std::string, kFunctions> dllReal;
    for (unsigned f = 0; f < kFunctions; ++f) {
        const Spec& sp = kSpecs[f];
        dllReal[f] = sp.kind == 0 ? "CEntity::CleanUpOldReference (0x571A00) falso identico; el orden de las referencias y las vtables son el propio cuerpo"
                   : "CEntity::RegisterReference (0x571B70) falso identico; los campos, la vtable propia y el orden de los registros son el propio constructor";
    }
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
