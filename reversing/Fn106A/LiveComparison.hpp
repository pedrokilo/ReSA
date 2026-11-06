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


// Banco par a par Fn106A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Siete funciones (hook S), cada una con 16384 pares EXE/DLL:
//   0  CrtPowThunk                  0x470000  cdecl (float, float); pow en ST0. Sin colaboradores: el _CIpow del EXE (0x822130) frente a ExeCompatiblePowCpp de la DLL corren de verdad.
//   1  RoundToDecimals              0x50A0A0  cdecl (float, int); ST0. Sin colaboradores: _CIpow (0x822130) y modf (0x8232F0) del EXE frente al C++ de la DLL.
//   2  CTaskSimpleCarSetPedOut ctor 0x6478B0  thiscall (vehiculo, puerta, byte), `ret 0Ch`; EAX = this. Colaboradores falseados: 0x61A390 (constructor de CTaskSimple) y 0x571B70 (CEntity::RegisterReference).
//   3  GxtCharToAscii               0x69F7E0  cdecl (cadena, desplazamiento); EAX = 0xC1BDD0. Sin colaboradores; el buffer estatico 0xC1BDD0 se rellena y se restaura.
//   4  CEventKnockOffBike dtor      0x4AFE70  thiscall, `ret`. Colaborador falseado: 0x571A00 (CEntity::CleanUpOldReference).
//   5  CTaskSimpleStandStill dtor   0x62F360  thiscall, `ret`. Colaborador falseado: 0x61A3A0 (CTask::Destructor; el falso escribe la vtable 0x86D48C como el original).
//   6  RwIm2DRenderPrimitiveThunk   0x734E90  cdecl (tipo, vertices, numero); salta a [RwEngineInstance + 0x30]. El puntero 0xC97B24 se apunta a un RwGlobals falso con la funcion del dispositivo falsa.
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.

namespace F106ALive {
using namespace F106AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x470000, 0x50A0A0, 0x6478B0, 0x69F7E0, 0x4AFE70, 0x62F360, 0x734E90};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F106AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    std::string PreHex(size_t bytes) const { return HexBytes(r.data() + kGuard, bytes); }
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
inline Arena<kTaskWin> taskA;        // 0: tarea (0x18 o 0x20 bytes)
inline Arena<kEventWin> evA;         // 1: evento (0x3C bytes)
inline Arena<kEntWin> entA;          // 2: vehiculo
inline Arena<kEntWin> entB;          // 3: peaton
inline Arena<kStrWin> strA;          // 4: cadena GXT
inline Arena<kGlobalsWin> globA;     // 5: RwGlobals falso
inline Arena<kEntWin> vtxA;          // 6: vertices
inline constexpr u32 kRawVeh = 0x00C0FFEEu, kRawKVeh = 0x00BADC00u, kRawKPed = 0x00BADC40u, kRawVtx = 0x00C80468u;

// Estado del caso en curso para los falsos
struct State { const C* c{}; };
inline State g;

// Eventos: 10 constructor de CTaskSimple 0x61A390 [this]; 11 CEntity::RegisterReference 0x571B70 [entidad, &referencia]; 20 CEntity::CleanUpOldReference 0x571A00 [entidad, &referencia];
// 30 CTask::Destructor 0x61A3A0 [this]; 40 funcion 2D del dispositivo falso (+0x30) [tipo, vertices, numero]; 99 ranura inesperada del RwGlobals falso.
inline void* __fastcall FkSimpleCtor(void* self, void*) { Ev& e = PushEv(10, 1); e.v[0] = Tag(self); return self; }
inline void __fastcall FkRegRef(void* self, void*, void* ref) { Ev& e = PushEv(11, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(20, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = evA.Get32(0); }   // + la vtable del evento en ese momento
inline void __fastcall FkTaskDtor(void* self, void*) { Ev& e = PushEv(30, 2); e.v[0] = Tag(self); e.v[1] = *reinterpret_cast<uint32*>(self); *reinterpret_cast<uint32*>(self) = 0x86D48C; }   // + la vtable de la tarea en ese momento
inline u32 __cdecl FkIm2D(u32 prim, void* verts, u32 num) { Ev& e = PushEv(40, 3); e.v[0] = prim; e.v[1] = Tag(verts); e.v[2] = num; return g.c->a[3]; }
inline u32 __cdecl FkTrap() { Ev& e = PushEv(99, 0); (void)e; return 0; }

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["engine_final"] = Tag(uintptr_t(G32(kEngineAddr)));
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline u32 EntPtr(u32 mode, const uintptr_t arena, u32 raw) { return mode == 0 ? u32(arena) : (mode == 1 ? 0u : raw); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 60u;
        taskA.Fill(sd++); evA.Fill(sd++); entA.Fill(sd++); entB.Fill(sd++); strA.Fill(sd++); globA.Fill(sd++); vtxA.Fill(sd++);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(evA.m.data(), evA.m.size());         // 1
        TagAdd(entA.m.data(), entA.m.size());       // 2
        TagAdd(entB.m.data(), entB.m.size());       // 3
        TagAdd(strA.m.data(), strA.m.size());       // 4
        TagAdd(globA.m.data(), globA.m.size());     // 5
        TagAdd(vtxA.m.data(), vtxA.m.size());       // 6
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        std::string pre;
        size_t preBytes = 0;
        switch (fn) {
        case 0:
            args = { c.a[0], c.a[1] }; popBytes = 8;
            break;
        case 1:
            args = { c.a[0], c.a[1] }; popBytes = 8;
            break;
        case 2: {
            ecx = u32(taskA.Obj()); preBytes = 0x18;
            args = { EntPtr(c.a[0], entA.Obj(), kRawVeh), c.a[1], (c.a[3] & 0xFFFFFF00u) | (c.a[2] & 0xFFu) };
            break;
        }
        case 3: {
            // cadena en la ventana de 0x400 bytes: len bytes distintos de cero y, salvo en el modo 1, el terminador; el resto sigue siendo la basura del relleno
            const auto bytes = StrBytes(c);
            for (size_t i = 0; i < bytes.size(); ++i) strA.Put8(i, bytes[i]);
            if (c.a[1] != 1) strA.Put8(bytes.size(), 0);
            args = { c.a[1] == 2 ? 0u : u32(strA.Obj()), (c.a[3] & 0xFFFFFF00u) | (c.a[2] & 0xFFu) };
            popBytes = 8;
            break;
        }
        case 4: {
            ecx = u32(evA.Obj()); preBytes = 0x3C;
            evA.Put32(0, c.a[2]);
            evA.Put32(0x38, EntPtr(c.a[0], entA.Obj(), kRawKVeh));
            evA.Put32(0x30, EntPtr(c.a[1], entB.Obj(), kRawKPed));
            break;
        }
        case 5: {
            ecx = u32(taskA.Obj()); preBytes = 0x20;
            taskA.Put32(0, c.a[0]);
            break;
        }
        default: {
            globA.Put32(0x28, u32(Addr(&FkTrap))); globA.Put32(0x2C, u32(Addr(&FkTrap))); globA.Put32(0x34, u32(Addr(&FkTrap)));
            globA.Put32(0x30, u32(Addr(&FkIm2D)));
            G32(kEngineAddr) = u32(globA.Obj());
            args = { c.a[0], EntPtr(c.a[1], vtxA.Obj(), kRawVtx), c.a[2] }; popBytes = 12;
            break;
        }
        }
        // buffer estatico de GxtCharToAscii: patron propio del caso (visible lo que no se escribe)
        FillWords(reinterpret_cast<uint8*>(kGxtBufAddr), 256, sd++);
        taskA.Snap(); evA.Snap(); entA.Snap(); entB.Snap(); strA.Snap(); globA.Snap(); vtxA.Snap();
        if (fn == 2 || fn == 5) pre = taskA.PreHex(preBytes);
        if (fn == 4) pre = evA.PreHex(preBytes);
        std::string srcHex;
        if (fn == 3) srcHex = HexBytes(strA.m.data() + kGuard, std::min<size_t>(kStrWin, size_t(c.a[2] & 0xFFu) + 258));
        const std::string bufPre = fn == 3 ? HexBytes(reinterpret_cast<const void*>(kGxtBufAddr), 256) : std::string();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, taskA.Changes()); add(1, evA.Changes()); add(2, entA.Changes()); add(3, entB.Changes()); add(4, strA.Changes()); add(5, globA.Changes()); add(6, vtxA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        if (!pre.empty()) obs["pre"] = pre;
        if (fn == 2) { obs["ret_eax"] = Tag(uintptr_t(sides[s].reg.eax)); obs["ent_a"] = u32(entA.Obj()); }
        if (fn == 3 || fn == 6) obs["ret_eax"] = sides[s].reg.eax;
        if (fn == 3) {
            const uint8* buf = reinterpret_cast<const uint8*>(kGxtBufAddr);
            obs["src"] = srcHex;
            obs["buf"] = HexBytes(buf, 256);
            obs["buf_pre"] = bufPre;
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
    if (!std::filesystem::exists(dir / "fn106a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn106a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn106a"); }
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
    env.stubs[1] = {};
    env.stubs[2] = {F(0x61A390, &FkSimpleCtor), F(0x571B70, &FkRegRef)};
    env.stubs[3] = {};
    env.stubs[4] = {F(0x571A00, &FkCleanUp)};
    env.stubs[5] = {F(0x61A3A0, &FkTaskDtor)};
    env.stubs[6] = {};
    const char* dllReal[kFunctions] = {
        "pow(base, exponente) en ST0: el _CIpow del EXE (0x822130) frente a ExeCompatiblePowCpp de la DLL, sin sustituir",
        "pow(10, n + 1) y pow(10, n), la x87 de 24 bits y el modf (0x8232F0) del EXE frente al C++ de la DLL, sin sustituir",
        "constructor de CTaskSimple (0x61A390) y CEntity::RegisterReference (0x571B70) falsos identicos; el resto (vtable, puerta, byte, bytes a cero) es el propio cuerpo",
        "toda la conversion y el buffer estatico 0xC1BDD0",
        "CEntity::CleanUpOldReference (0x571A00) falso identico; el orden de las referencias y las vtables son el propio cuerpo",
        "CTask::Destructor (0x61A3A0) falso identico (escribe la vtable 0x86D48C); la vtable 0x86DD2C es el propio cuerpo",
        "el salto por [RwEngineInstance + 0x30] hacia una funcion falsa del dispositivo (el RwGlobals falso lo pone el banco en 0xC97B24)"};
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
