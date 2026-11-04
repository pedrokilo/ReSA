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
#include "Tasks/TaskTypes/TaskComplexUseMobilePhone.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskComplexPartner.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn94A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro funciones de tareas (hook S), cada una con 16384 pares EXE/DLL:
//   0  CTaskComplexUseMobilePhone::Quit              0x634A40  thiscall, tarea en ECX, ped en pila, `ret 4`; void. Sin colaboradores directos: aborta por la ranura 6 de la vtabla de la tarea (arena con vtabla propia).
//   1  CTaskComplexKillPedOnFoot::SayGangAttack      0x65E9A0  cdecl (atacante, objetivo); void. Colaboradores (con hook RH): CWorld::FindPlayerPed 0x56E210 (cdecl, un argumento) y CPed::Say 0x5EFFE0 (thiscall, 6 en pila, `ret 18h`).
//   2  CTaskComplexWalkRoundObject::ComputeRouteRoundObject 0x6551D0  thiscall, tarea en ECX, ped en pila, `ret 4`; devuelve un float en ST0. Colaboradores (con hook RH, cdecl): 0x5F6110 (ruta alrededor del objeto),
//      0x5F3660 (planos del objeto) y 0x5F3AC0 (lado que toca el punto); ademas pone el margen 0x8D22B0 a 0.7 y lo restaura.
//   3  CTaskComplexPartner::ComputeTargetPoints      0x681FE0  thiscall, tarea en ECX, (ped, &salida1, &salida2) en pila, `ret 0Ch`; void. Colaborador (con hook RH): CVector::Normalise 0x59C910 (thiscall, in situ);
//      la tarea se consulta por la ranura 4 de su vtabla (GetTaskType, arena con vtabla propia).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.
// NO se sustituyen (corren de verdad en ambos lados): toda la aritmetica de las funciones (x87 del EXE frente al C++ de la DLL).
// Memoria real del juego (se rellena y se restaura tras cada lado): el margen del analizador de geometria 0x8D22B0.

namespace F94ALayout {
static_assert(offsetof(CTaskComplexUseMobilePhone, m_bIsAborting) == 0x1C && offsetof(CTaskComplexUseMobilePhone, m_bQuit) == 0x1D && sizeof(CTaskComplexUseMobilePhone) == 0x20, "CTaskComplexUseMobilePhone");
static_assert(offsetof(CTaskComplexWalkRoundObject, m_targetPoint) == 0x10 && offsetof(CTaskComplexWalkRoundObject, m_object) == 0x1C && offsetof(CTaskComplexWalkRoundObject, m_pointRoute) == 0x20 && sizeof(CTaskComplexWalkRoundObject) == 0x54, "CTaskComplexWalkRoundObject");
static_assert(offsetof(CTaskComplexPartner, m_partner) == 0x38 && offsetof(CTaskComplexPartner, m_distanceMultiplier) == 0x3C && offsetof(CTaskComplexPartner, m_point) == 0x40 && offsetof(CTaskComplexPartner, m_targetPoint) == 0x4C && offsetof(CTaskComplexPartner, m_taskCompleted) == 0x5C && sizeof(CTaskComplexPartner) == 0x70, "CTaskComplexPartner");
static_assert(offsetof(CPed, m_matrix) == 0x14 && offsetof(CPed, m_nPedType) == 0x598, "CPed");
}

namespace F94ALive {
using namespace F94AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x634A40, 0x65E9A0, 0x6551D0, 0x681FE0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F94AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
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
inline Arena<kTask3Win> taskA;       // 0: tarea (0x20, 0x54 o 0x70 bytes segun la funcion)
inline Arena<kPedWin> pedA;          // 1: ped (atacante / peaton)
inline Arena<kMatWin> pmatA;         // 2: matriz del ped
inline Arena<kPedWin> otherA;        // 3: objetivo (funcion 1) o companero (funcion 3)
inline Arena<kMatWin> omatA;         // 4: matriz del companero
inline Arena<kObjWin> objA;          // 5: objeto (funcion 2)
inline Arena<kRouteWin> routeA;      // 6: ruta (funcion 2)
inline constexpr u32 kRawPed = 0x00BADC00u, kRawPlayer = 0x00ABCD00u, kRawObject = 0x00C0FFEEu, kRawRoute = 0x00A0B0C0u;

// Estado del caso en curso para los falsos
struct State { const C* c{}; uintptr_t player{}; };
inline State g;
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}

// Eventos: 4 GetTaskType virtual [tarea]; 6 MakeAbortable virtual [tarea, ped, prioridad, evento, byte +0x1D de la tarea en el momento de la llamada]; 30 CWorld::FindPlayerPed 0x56E210 [id];
// 31 CPed::Say 0x5EFFE0 [ped, 6 argumentos de pila]; 40 0x5F6110 [ped, objeto, &punto, ruta, bandera, margen 0x8D22B0]; 41 0x5F3660 [z, objeto, &planos, &distancias];
// 42 0x5F3AC0 [&posicion, objeto]; 60 CVector::Normalise 0x59C910 [&vector, x, y, z]; 90+k: ranura k no esperada de la vtabla de las tareas.
inline std::array<uintptr_t, 16> taskVt{};
template <unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkTaskType(void* self, void*) {                                                 // CTask::GetTaskType (ranura 4)
    Ev& e = PushEv(4, 1); e.v[0] = Tag(self);
    return g.c->a[18];
}
inline u32 __fastcall FkAbort(void* self, void*, void* ped, u32 priority, void* event) {              // CTask::MakeAbortable (ranura 6, `ret 0Ch`)
    Ev& e = PushEv(6, 5); e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = priority; e.v[3] = Tag(event); e.v[4] = *reinterpret_cast<const uint8*>(uintptr_t(self) + 0x1D);
    return g.c->a[2];
}
inline void InitTaskVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((taskVt[I] = uintptr_t(&FkVt<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[4] = uintptr_t(&FkTaskType);
    taskVt[6] = uintptr_t(&FkAbort);
}
inline u32 __cdecl FkFindPlayer(u32 id) {                                                              // 0x56E210
    Ev& e = PushEv(30, 1); e.v[0] = id;
    return u32(g.player);
}
inline u32 __fastcall FkSay(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) {        // 0x5EFFE0 (`ret 18h`)
    Ev& e = PushEv(31, 7); e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; e.v[5] = a5; e.v[6] = a6;
    return 0;
}
inline u32 __cdecl Fk5F6110(void* ped, void* entity, void* point, void* route, u32 flag) {              // 0x5F6110
    Ev& e = PushEv(40, 6); e.v[0] = Tag(ped); e.v[1] = Tag(entity); e.v[2] = Tag(point); e.v[3] = Tag(route); e.v[4] = flag; e.v[5] = G32(kMarginAddr);
    return g.c->a[30];
}
inline void __cdecl Fk5F3660(u32 z, void* entity, void* planes, void* dots) {                          // 0x5F3660
    Ev& e = PushEv(41, 4); e.v[0] = z; e.v[1] = Tag(entity); e.v[2] = TagS(uintptr_t(planes)); e.v[3] = TagS(uintptr_t(dots));
    for (unsigned k = 0; k < 12; ++k) memcpy(static_cast<uint8*>(planes) + 4 * k, &g.c->a[13 + k], 4);
    for (unsigned k = 0; k < 4; ++k) memcpy(static_cast<uint8*>(dots) + 4 * k, &g.c->a[25 + k], 4);
}
inline u32 __cdecl Fk5F3AC0(void* pos, void* entity) {                                                  // 0x5F3AC0
    Ev& e = PushEv(42, 2); e.v[0] = Tag(pos); e.v[1] = Tag(entity);
    return g.c->a[29];
}
inline void __fastcall FkNormalise(void* self, void*) {                                                 // 0x59C910 (in situ; no deja nada en la x87)
    Ev& e = PushEv(60, 4); e.v[0] = TagS(uintptr_t(self));
    for (unsigned k = 0; k < 3; ++k) { memcpy(&e.v[1 + k], static_cast<uint8*>(self) + 4 * k, 4); memcpy(static_cast<uint8*>(self) + 4 * k, &g.c->a[15 + k], 4); }
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["margin_final"] = G32(kMarginAddr);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void Put3(auto& arena, size_t off, const u32* p) { arena.Put32(off, p[0]); arena.Put32(off + 4, p[1]); arena.Put32(off + 8, p[2]); }
// Posicion del ped (matriz +0x30 o placement +4): la activa recibe a[act..act+2] y la otra a[alt..alt+2]
inline void PutPed(auto& ped, auto& mat, bool hasMat, const u32* act, const u32* alt) {
    ped.Put32(0x14, hasMat ? u32(mat.Obj()) : 0u);
    if (hasMat) { Put3(mat, 0x30, act); Put3(ped, 4, alt); }
    else { Put3(ped, 4, act); Put3(mat, 0x30, alt); }
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    InitTaskVt();
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        taskA.Fill(sd++); pedA.Fill(sd++); pmatA.Fill(sd++); otherA.Fill(sd++); omatA.Fill(sd++); objA.Fill(sd++); routeA.Fill(sd++);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pmatA.m.data(), pmatA.m.size());     // 2
        TagAdd(otherA.m.data(), otherA.m.size());   // 3
        TagAdd(omatA.m.data(), omatA.m.size());     // 4
        TagAdd(objA.m.data(), objA.m.size());       // 5
        TagAdd(routeA.m.data(), routeA.m.size());   // 6
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(taskA.Obj()), popBytes = 0;
        taskA.Put32(0, u32(Addr(taskVt.data())));
        switch (fn) {
        case 0: {
            taskA.Put32(0x1C, c.a[0]);
            args = { c.a[1] == 0 ? u32(pedA.Obj()) : (c.a[1] == 1 ? 0u : kRawPed) };
            break;
        }
        case 1: {
            pedA.Put32(0x598, c.a[2]);
            otherA.Put32(0x598, c.a[3]);
            const u32 pedPtr = c.a[0] == 0 ? u32(pedA.Obj()) : 0u;
            const u32 tgtPtr = c.a[1] == 0 ? u32(otherA.Obj()) : (c.a[1] == 1 ? 0u : u32(pedA.Obj()));
            g.player = c.a[4] == 0 ? tgtPtr : (c.a[4] == 1 ? 0u : (c.a[4] == 2 ? u32(pedA.Obj()) : kRawPlayer));
            args = { pedPtr, tgtPtr };
            ecx = c.seed; popBytes = 8;
            break;
        }
        case 2: {
            Put3(taskA, 0x10, &c.a[8]);
            taskA.Put32(0x1C, c.a[11] == 0 ? u32(objA.Obj()) : kRawObject);
            taskA.Put32(0x20, c.a[12] == 0 ? u32(routeA.Obj()) : kRawRoute);
            PutPed(pedA, pmatA, c.a[0] != 0, &c.a[1], &c.a[4]);
            G32(kMarginAddr) = c.a[7];
            args = { u32(pedA.Obj()) };
            break;
        }
        default: {
            taskA.Put32(0x38, u32(otherA.Obj()));
            taskA.Put32(0x3C, c.a[14]);
            Put3(taskA, 0x40, &c.a[20]); Put3(taskA, 0x4C, &c.a[23]);
            taskA.Put8(0x5C, c.a[19]);
            PutPed(pedA, pmatA, c.a[0] != 0, &c.a[1], &c.a[4]);
            PutPed(otherA, omatA, c.a[7] != 0, &c.a[8], &c.a[11]);
            args = { u32(pedA.Obj()), u32(taskA.Obj(0x40)), u32(taskA.Obj(0x4C)) };
            break;
        }
        }
        taskA.Snap(); pedA.Snap(); pmatA.Snap(); otherA.Snap(); omatA.Snap(); objA.Snap(); routeA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, taskA.Changes()); add(1, pedA.Changes()); add(2, pmatA.Changes()); add(3, otherA.Changes()); add(4, omatA.Changes()); add(5, objA.Changes()); add(6, routeA.Changes());
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
    if (!std::filesystem::exists(dir / "fn94a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn94a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn94a"); }
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
    env.stubs[1] = {F(0x56E210, &FkFindPlayer), F(0x5EFFE0, &FkSay)};
    env.stubs[2] = {F(0x5F6110, &Fk5F6110), F(0x5F3660, &Fk5F3660), F(0x5F3AC0, &Fk5F3AC0)};
    // CVector::Normalise no tiene hook RH (CVector::InjectHooks esta apagado): la DLL llama a su propio cuerpo, que se parchea con el mismo falso; el EXE, en 0x59C910
    uintptr_t normaliseDll{};
    { auto pm = &CVector::Normalise; memcpy(&normaliseDll, &pm, sizeof(normaliseDll)); normaliseDll = Body(normaliseDll); }
    env.stubs[3] = {S(Pair{0x59C910, normaliseDll, false}, reinterpret_cast<void*>(&FkNormalise))};
    const char* dllReal[kFunctions] = {
        "ranura 6 (MakeAbortable) de la vtabla propia de la tarea: falso identico; la aritmetica es el byte +0x1D de la tarea",
        "comprobaciones de tipo de peaton con signo; la aritmetica es la del propio cuerpo",
        "toda la aritmetica x87 (margen 0x8D22B0, producto escalar con el plano y suma de la distancia, devuelto en ST0)",
        "toda la aritmetica x87 (direccion, punto base, pasadas de empuje con raiz, puntos finales) y la ranura 4 (GetTaskType) de la vtabla propia de la tarea (falso identico)"};
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
