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


// Banco par a par Fn115A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 20 funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la lista con sus direcciones): nueve cuerpos de destructor y once constructores de tareas y eventos.
//   constructores (thiscall, argumentos en la pila, `ret N`, EAX = this): el objeto parte de basura con una vtable previa conocida; las vtables que escribe cada lado son las del EXE (0x86xxxx), asi que SE COMPARAN
//   todos los bytes cambiados (salvo el puntero que guarda CTaskSimpleArrestPed, que se informa como etiqueta de arena).
//   destructores (cuerpo no virtual del EXE, thiscall, `ret`).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN, los punteros a arenas como etiquetas
// y la vtable del objeto en ese momento (con lo que se comprueba el orden de las escrituras).

namespace F115ALive {
using namespace F115AFix;
inline constexpr auto KEntries() { std::array<uintptr_t, kFunctions> r{}; for (unsigned i = 0; i < kFunctions; ++i) r[i] = kSpecs[i].entry; return r; }
inline constexpr auto kEntries = KEntries();
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F115AFix::Arena(WIN);
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
inline Arena<kObjWin> ob;            // 0: la tarea o el evento (hasta 0x80 bytes)
inline Arena<kEntWin> entA;          // 1: primera entidad
inline Arena<kEntWin> entB;          // 2: segunda entidad
inline constexpr u32 kRaw1 = 0x00BADC00u, kRaw2 = 0x00BADC40u;

// Estado del caso en curso para los falsos
struct State { const C* c{}; };
inline State g;

inline u32 EntPtr(u32 mode, const uintptr_t arena, u32 raw) { return mode == 0 ? u32(arena) : (mode == 1 ? 0u : raw); }
inline u32 VtNow() { return ob.Get32(0); }
inline void SetVt(void* self, u32 vt) { *reinterpret_cast<uint32*>(self) = vt; }

// Eventos de los falsos (todos llevan la vtable del objeto en el momento de la llamada):
//   10 CTaskSimple::Constructor 0x61A390 [this, vt]; 12 CTaskComplex::Constructor 0x61A3B0 [this, vt]; 11 CEntity::RegisterReference 0x571B70 [entidad, &referencia, vt];
//   20 CEntity::CleanUpOldReference 0x571A00 [entidad, &referencia, vt]; 30 CTask::Destructor 0x61A3A0 [this, vt] (escribe 0x86D48C); 31 CTaskComplex::Destructor 0x61A3D0 [this, vt] (escribe 0x86D48C);
//   32 CEvent destructor 0x4AC010 [this, vt] (escribe 0x85AB38); 33 CTaskComplexSequence::Flush 0x632C10 [this, vt]; 40 CTaskComplexUseEffect::Constructor 0x6321F0 [this, efecto, entidad, vt];
//   41 CTaskAllocatorAttack::Constructor 0x69C240 [this, objetivo, grupo, jugador, vt]; 50 FindPlayerPed 0x56E210 [argumento].
inline void* __fastcall FkSimpleCtor(void* self, void*) { Ev& e = PushEv(10, 2); e.v[0] = Tag(self); e.v[1] = VtNow(); return self; }
inline void* __fastcall FkComplexCtor(void* self, void*) { Ev& e = PushEv(12, 2); e.v[0] = Tag(self); e.v[1] = VtNow(); return self; }
inline void __fastcall FkRegRef(void* self, void*, void* ref) { Ev& e = PushEv(11, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = VtNow(); }
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(20, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = VtNow(); }
inline void __fastcall FkTaskDtor(void* self, void*) { Ev& e = PushEv(30, 2); e.v[0] = Tag(self); e.v[1] = VtNow(); SetVt(self, 0x86D48C); }
inline void __fastcall FkComplexDtor(void* self, void*) { Ev& e = PushEv(31, 2); e.v[0] = Tag(self); e.v[1] = VtNow(); SetVt(self, 0x86D48C); }
inline void __fastcall FkEventDtor(void* self, void*) { Ev& e = PushEv(32, 2); e.v[0] = Tag(self); e.v[1] = VtNow(); SetVt(self, 0x85AB38); }
inline void __fastcall FkFlush(void* self, void*) { Ev& e = PushEv(33, 2); e.v[0] = Tag(self); e.v[1] = VtNow(); }
inline void* __fastcall FkUseEffectCtor(void* self, void*, void* fx, void* ent) { Ev& e = PushEv(40, 4); e.v[0] = Tag(self); e.v[1] = Tag(fx); e.v[2] = Tag(ent); e.v[3] = VtNow(); return self; }
inline void* __fastcall FkAllocCtor(void* self, void*, void* target, u32 group, void* player) { Ev& e = PushEv(41, 5); e.v[0] = Tag(self); e.v[1] = Tag(target); e.v[2] = group; e.v[3] = Tag(player); e.v[4] = VtNow(); return self; }
inline void* __cdecl FkFindPlayer(int id) { Ev& e = PushEv(50, 1); e.v[0] = u32(id); return reinterpret_cast<void*>(EntPtr(g.c->a[8], entB.Obj(), kRaw2)); }

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
        ob.Fill(sd++); entA.Fill(sd++); entB.Fill(sd++);
        TagClear();
        TagAdd(ob.m.data(), ob.m.size());           // 0
        TagAdd(entA.m.data(), entA.m.size());       // 1
        TagAdd(entB.m.data(), entB.m.size());       // 2
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(ob.Obj());
        const u32 popBytes = 0;                      // thiscall con `ret N`: el callee limpia la pila
        if (sp.kind == 0) {
            ob.Put32(0, c.a[2]);
            if (sp.refOff != 0xFFFF) ob.Put32(sp.refOff, EntPtr(c.a[0], entA.Obj(), kRaw1));
        } else {
            ob.Put32(0, kPreVtable);
            for (u32 i = 0; sp.types[i]; ++i) {
                u32 v = c.a[i];
                if (sp.types[i] == 'P') v = EntPtr(c.a[i], i == 0 ? entA.Obj() : entB.Obj(), i == 0 ? kRaw1 : kRaw2);
                args.push_back(v);
            }
        }
        ob.Snap(); entA.Snap(); entB.Snap();
        const std::string pre = ob.PreHex(0x80);
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        // CTaskSimpleArrestPed guarda en +8 el puntero recibido: sus bytes no se comparan (se informan como etiqueta)
        if (sp.kind == 1 && fn == 17) add(0, ob.Changes({{8, 4}})); else add(0, ob.Changes());
        add(1, entA.Changes()); add(2, entB.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        obs["pre"] = pre;
        if (sp.kind == 1) {
            obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
            json ptrs = json::array();
            if (fn == 17) ptrs.push_back(Tag(uintptr_t(ob.Get32(8))));
            obs["ptrs"] = ptrs;
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
    // CShadowCamera::DrawOutlineBorder, que falla si el dispositivo D3D9 pasa un rato en segundo plano durante la corrida) se escriba como ultimo evento y se tome por fallo del banco.
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
    if (!std::filesystem::exists(dir / "fn115a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn115a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn115a"); }
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
    std::array<std::string, kFunctions> dllReal;
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::vector<StubSpec>& v = env.stubs[f];
        switch (f) {
        case 0: case 1: v = {F(0x571A00, &FkCleanUp), F(0x4AC010, &FkEventDtor)}; dllReal[f] = "CEntity::CleanUpOldReference (0x571A00) y el cuerpo de ~CEvent (0x4AC010) falsos identicos; la vtable propia, el orden y el desplazamiento son el propio cuerpo"; break;
        case 2: v = {F(0x571A00, &FkCleanUp), F(0x61A3D0, &FkComplexDtor)}; dllReal[f] = "CEntity::CleanUpOldReference (0x571A00) y ~CTaskComplex (0x61A3D0) falsos identicos; la vtable propia, el orden y el desplazamiento son el propio cuerpo"; break;
        case 3: v = {F(0x632C10, &FkFlush), F(0x61A3D0, &FkComplexDtor)}; dllReal[f] = "CTaskComplexSequence::Flush (0x632C10) y ~CTaskComplex (0x61A3D0) falsos identicos; la vtable propia y el orden son el propio cuerpo"; break;
        case 4: case 5: v = {F(0x571A00, &FkCleanUp), F(0x61A3A0, &FkTaskDtor)}; dllReal[f] = "CEntity::CleanUpOldReference (0x571A00) y ~CTask (0x61A3A0) falsos identicos; la vtable propia, el orden y el desplazamiento son el propio cuerpo"; break;
        case 6: v = {F(0x61A3A0, &FkTaskDtor)}; dllReal[f] = "~CTask (0x61A3A0) falso identico; la vtable propia es el propio cuerpo"; break;
        case 7: case 8: v = {F(0x61A3D0, &FkComplexDtor)}; dllReal[f] = "~CTaskComplex (0x61A3D0) falso identico; la vtable propia es el propio cuerpo"; break;
        case 9: case 10: case 11: case 19: v = {F(0x61A390, &FkSimpleCtor)}; dllReal[f] = "CTaskSimple::Constructor (0x61A390) falso identico; los campos, la vtable propia y los bits son el propio constructor"; break;
        case 12: case 13: case 16: v = {F(0x61A3B0, &FkComplexCtor)}; dllReal[f] = "CTaskComplex::Constructor (0x61A3B0) falso identico; los campos y la vtable propia son el propio constructor"; break;
        case 14: case 15: v = {F(0x6321F0, &FkUseEffectCtor)}; dllReal[f] = "CTaskComplexUseEffect::Constructor (0x6321F0) falso identico; la vtable propia y el estado de movimiento son el propio constructor"; break;
        case 17: v = {F(0x61A390, &FkSimpleCtor), F(0x571B70, &FkRegRef)}; dllReal[f] = "CTaskSimple::Constructor (0x61A390) y CEntity::RegisterReference (0x571B70) falsos identicos; los campos, la vtable propia y el orden son el propio constructor"; break;
        case 18: v = {F(0x56E210, &FkFindPlayer), F(0x69C240, &FkAllocCtor)}; dllReal[f] = "FindPlayerPed (0x56E210) y CTaskAllocatorAttack::Constructor (0x69C240) falsos identicos; el orden, los argumentos, la vtable propia y el tercer campo son el propio constructor"; break;
        default: break;
        }
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
