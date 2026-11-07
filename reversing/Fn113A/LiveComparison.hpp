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
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskComplexWanderStandard.h"
#include "Tasks/TaskTypes/TaskComplexPartner.h"
#include "Tasks/Allocators/TaskAllocator.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTimer.h"
#include "Events/EventScanner.h"
#include "PedIntelligence.h"
#include "PlayerRelationshipRecorder.h"
#include "Entity/Ped/Ped.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn113A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 17 funciones pequenas (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la lista, las convenciones y las familias de casos).
// Colaboradores con hook RH (apagados durante el banco) y su tratamiento:
//   - CTaskManager::FindActiveTaskByType 0x681740 (funcion 4): falso identico en la direccion del EXE y en el cuerpo de la DLL (registra tipo y this; devuelve la tarea de la arena o nulo).
//   - CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents 0x603720 (funcion 15): falso identico (registra this, ped, entidades y contador).
//   - CTaskAllocatorPool::New 0x69D8E0 (funcion 7) y CPedIntelligencePool::New 0x605EC0 (funcion 9): corren DE VERDAD en ambos lados (el EXE con su hook apagado; la DLL con su propia busqueda de ranura); se comprueba que el EXE
//     ejecuta sus bytes originales.
// NO se sustituye nada mas: lo demas corre de verdad (bits, comparaciones y escrituras de las arenas; los operator delete de los pools).
// Memoria real del juego (se fija y se restaura tras cada lado): los punteros de pool 0xB744BC / 0xB744C0 (apuntan a un pool de arenas) y CTimer::m_snTimeInMilliseconds 0xB7CB84.

namespace F113ALayout {
static_assert(offsetof(CTaskComplexWander, m_nDir) == 0x10 && offsetof(CTaskComplexWander, m_LastNode) == 0x18 && offsetof(CTaskComplexWander, m_NextNode) == 0x1C && offsetof(CTaskComplexWander, m_nFlags) == 0x24, "CTaskComplexWander");
static_assert(offsetof(CTaskComplexWanderStandard, m_nMinNextScanTime) == 0x34, "CTaskComplexWanderStandard");
static_assert(offsetof(CTaskComplexPartner, m_taskId) == 0x34 && offsetof(CTaskComplexPartner, m_partner) == 0x38 && offsetof(CTaskComplexPartner, m_partnerState) == 0x59, "CTaskComplexPartner");
static_assert(offsetof(CPed, m_nModelIndex) == 0x22, "CPed");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPedIntelligence");
static_assert(offsetof(CEventScanner, m_vehiclePotentialCollisionScanner) == 4 && offsetof(CVehiclePotentialCollisionScanner, m_timer) == 0, "CEventScanner");
static_assert(offsetof(CTaskTimer, m_nStartTime) == 0 && offsetof(CTaskTimer, m_nInterval) == 4 && offsetof(CTaskTimer, m_bStarted) == 8, "CTaskTimer");
static_assert(sizeof(PlayerRelationship) == 8 && sizeof(CPlayerRelationshipRecorder) == 0x80 && offsetof(CPlayerRelationshipRecorder, m_Relationships) == 0, "CPlayerRelationshipRecorder");
}

namespace F113ALive {
using namespace F113AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x63AB70, 0x669D30, 0x669D50, 0x66B150, 0x6822B0, 0x65E6F0, 0x65E710, 0x69D980, 0x69D990, 0x6074A0, 0x6074E0, 0x61A150, 0x61A180, 0x61A1A0, 0x61A2A0, 0x6053D0, 0x62EB80};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F113AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
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
inline Arena<kObjWin> objA;          // 0: this (tarea, registro de relaciones, escaner de eventos o temporizador segun la funcion)
inline Arena<kOtherWin> otherA;      // 1: otra tarea (funcion 2)
inline Arena<kPedWin> pedA;          // 2: ped
inline Arena<kIntelWin> intelA;      // 3: inteligencia del ped (funcion 15)
inline Arena<kPed2Win> ped2A;        // 4: companero (funcion 4)
inline Arena<kIntel2Win> intel2A;    // 5: inteligencia del companero (funcion 4)
inline Arena<kPTaskWin> ptaskA;      // 6: tarea del companero (funcion 4)
inline Arena<kPoolWin> poolA;        // 7: cabecera del pool (almacenamiento, estados, capacidad, cursor)
inline Arena<kStoWin> stoA;          // 8: almacenamiento del pool
inline Arena<kStWin> stA;            // 9: bytes de estado de las ranuras

// Estado del caso en curso para los falsos
struct State { const C* c{}; };
inline State g;

// Eventos: 50 CTaskManager::FindActiveTaskByType 0x681740 [this, tipo]; 60 ScanForVehiclePotentialCollisionEvents 0x603720 [this, ped, entidades, contador].
inline u32 __fastcall FkFindActive(void* self, void*, u32 type) {                                      // 0x681740 (thiscall, `ret 4`)
    Ev& e = PushEv(50, 2); e.v[0] = Tag(self); e.v[1] = type;
    return g.c->a[1] == 0 ? 0u : u32(ptaskA.Obj());
}
inline void __fastcall FkScanVeh(void* self, void*, void* ped, void* entities, u32 count) {             // 0x603720 (thiscall, `ret 0Ch`)
    Ev& e = PushEv(60, 4); e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = Tag(entities); e.v[3] = count;
}

// Tipo de retorno que se registra de cada funcion: 0 nada (void), 1 AL, 2 AX, 3 EAX numerico, 4 EAX como puntero (etiquetado)
inline constexpr int kRetKind[kFunctions] = {3, 0, 0, 0, 3, 3, 3, 4, 0, 4, 0, 0, 0, 1, 0, 0, 3};

inline json CloseSide(Env& env, Side& side, json obs, unsigned fn) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    if (kRetKind[fn] == 1) obs["ret"] = side.reg.eax & 0xFFu;
    if (kRetKind[fn] == 2) obs["ret"] = side.reg.eax & 0xFFFFu;
    if (kRetKind[fn] == 3) obs["ret"] = side.reg.eax;
    if (kRetKind[fn] == 4) obs["ret"] = Tag(uintptr_t(side.reg.eax));
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// Entradas del registro de relaciones (16 x {ped, relacion}): la entrada j lleva el argumento si el bit j de la mascara esta puesto; si no, un valor distinto de cualquier argumento
inline u32 RecPed(const C& c, u32 j) { return ((c.a[1] >> j) & 1u) ? c.a[0] : (0x20000000u | ((Mix(c.a[2] + j) & 0xFFFFu) << 8)); }
inline u32 RecRel(const C& c, u32 j) { return Mix(c.a[2] + 0x100u + j); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); otherA.Fill(sd++); pedA.Fill(sd++); intelA.Fill(sd++); ped2A.Fill(sd++); intel2A.Fill(sd++); ptaskA.Fill(sd++); poolA.Fill(sd++); stoA.Fill(sd++); stA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());       // 0
        TagAdd(otherA.m.data(), otherA.m.size());   // 1
        TagAdd(pedA.m.data(), pedA.m.size());       // 2
        TagAdd(intelA.m.data(), intelA.m.size());   // 3
        TagAdd(ped2A.m.data(), ped2A.m.size());     // 4
        TagAdd(intel2A.m.data(), intel2A.m.size()); // 5
        TagAdd(ptaskA.m.data(), ptaskA.m.size());   // 6
        TagAdd(poolA.m.data(), poolA.m.size());     // 7
        TagAdd(stoA.m.data(), stoA.m.size());       // 8
        TagAdd(stA.m.data(), stA.m.size());         // 9
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        switch (fn) {
        case 0:
            ecx = u32(objA.Obj());
            objA.Put8(0x10, c.a[0]); objA.Put32(0x14, c.a[1]);
            break;
        case 1:
            ecx = u32(objA.Obj());
            objA.Put8(0x10, c.a[0]); objA.Put8(0x24, c.a[2]);
            args = { c.a[1] };
            break;
        case 2:
            ecx = u32(objA.Obj());
            for (u32 k = 0; k < 4; ++k) { objA.Put16(0x18 + 2 * k, c.a[k]); otherA.Put16(0x18 + 2 * k, c.a[4 + k]); }
            objA.Put8(0x10, c.a[8]); otherA.Put8(0x10, c.a[9]); objA.Put8(0x24, c.a[10]);
            args = { u32(otherA.Obj()) };
            break;
        case 3:
            ecx = u32(objA.Obj());
            objA.Put32(0x34, c.a[0]);
            args = { c.a[1] };
            break;
        case 4:
            ecx = u32(objA.Obj());
            objA.Put32(0x34, c.a[0]); objA.Put32(0x38, u32(ped2A.Obj()));
            ped2A.Put32(0x47C, u32(intel2A.Obj()));
            ptaskA.Put8(0x59, c.a[2]);
            break;
        case 5: case 6:
            pedA.Put16(0x22, c.a[0]);
            args = { u32(pedA.Obj()) }; popBytes = 4;
            break;
        case 7: case 8: case 9: case 10: {
            const bool task = fn == 7 || fn == 8;
            const u32 size = task ? 0x20u : 0x294u;
            poolA.Put32(0, u32(stoA.Obj())); poolA.Put32(4, u32(stA.Obj())); poolA.Put32(8, c.a[0]); poolA.Put32(0xC, c.a[1]);
            for (u32 k = 0; k < 8; ++k) stA.Put8(k, c.a[2 + k]);
            G32(task ? kPoolGlobals : kPoolGlobals + 4) = u32(poolA.Obj());
            if (fn == 7 || fn == 9) {
                args = { c.a[12] };
            } else {
                const u32 off = c.a[11] == 0 ? 0u : (c.a[11] == 2 ? size - 1 : 1 + c.a[13] % (size - 1));
                args = { u32(stoA.Obj(c.a[10] * size + off)) };
            }
            popBytes = 4;
            break;
        }
        case 11: case 12: case 13: case 14:
            ecx = u32(objA.Obj());
            for (u32 j = 0; j < 16; ++j) { objA.Put32(8 * j, RecPed(c, j)); objA.Put32(8 * j + 4, RecRel(c, j)); }
            if (c.a[4]) objA.Put32(0, 0);
            if (fn == 11 || fn == 13) args = { c.a[0] };
            if (fn == 12) args = { c.a[0], c.a[3] };
            break;
        case 15:
            ecx = u32(objA.Obj());
            objA.Put32(4, c.a[1]); objA.Put32(8, c.a[2]); objA.Put8(0xC, c.a[3]);
            pedA.Put32(0x47C, u32(intelA.Obj()));
            G32(kTimerAddr) = c.a[0];
            args = { u32(pedA.Obj()), c.a[4] };
            break;
        default:
            ecx = u32(objA.Obj());
            objA.Put32(0, c.a[0]); objA.Put32(4, c.a[1]); objA.Put8(8, c.a[2]); objA.Put8(9, c.a[3]);
            G32(kTimerAddr) = c.a[4];
            break;
        }
        objA.Snap(); otherA.Snap(); pedA.Snap(); intelA.Snap(); ped2A.Snap(); intel2A.Snap(); ptaskA.Snap(); poolA.Snap(); stoA.Snap(); stA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs = json::object();
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, otherA.Changes()); add(2, pedA.Changes()); add(3, intelA.Changes()); add(4, ped2A.Changes()); add(5, intel2A.Changes());
        add(6, ptaskA.Changes()); add(7, poolA.Changes()); add(8, stoA.Changes()); add(9, stA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs, fn);
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
    if (!std::filesystem::exists(dir / "fn113a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn113a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn113a"); }
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
    auto cache = [&](uintptr_t exe) -> const Pair& {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    auto F = [&](uintptr_t exe, auto* fake) { return S(cache(exe), reinterpret_cast<void*>(fake)); };
    env.stubs[4] = {F(0x681740, &FkFindActive)};
    env.stubs[15] = {F(0x603720, &FkScanVeh)};
    env.real[7] = {cache(0x69D8E0)};
    env.real[9] = {cache(0x605EC0)};
    const char* dllReal[kFunctions] = {
        "bits de la tarea (sin colaboradores)",
        "comparacion y escritura de la tarea (sin colaboradores)",
        "comparacion de los nodos y copia de la tarea (CNodeAddress::operator!= de la DLL; sin colaboradores)",
        "escritura de la tarea (sin colaboradores)",
        "CTaskManager::FindActiveTaskByType 0x681740 (falso identico); el resto corre de verdad",
        "comparaciones del indice de modelo (sin colaboradores)",
        "comparaciones del indice de modelo (sin colaboradores)",
        "pool de TaskAllocator: CTaskAllocatorPool::New 0x69D8E0 DE VERDAD en ambos lados (hook apagado)",
        "pool de TaskAllocator: borrado de la ranura (sin colaboradores)",
        "pool de PedIntelligence: CPedIntelligencePool::New 0x605EC0 DE VERDAD en ambos lados (hook apagado)",
        "pool de PedIntelligence: borrado de la ranura (sin colaboradores)",
        "busqueda y borrado en el registro (sin colaboradores)",
        "escritura de la primera entrada (sin colaboradores)",
        "busqueda en el registro (sin colaboradores)",
        "borrado de las 16 entradas (sin colaboradores)",
        "temporizador del escaner y ScanForVehiclePotentialCollisionEvents 0x603720 (falso identico)",
        "aritmetica de 32 bits sobre el tiempo real del juego (0xB7CB84, memoria real restaurada)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        json realList = json::array();
        for (const auto& p : env.real[f]) { env.watches.emplace_back(p.exe, 16); realList.push_back({{"exe", p.exe}, {"hooked", p.hooked}}); }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real_with_hook_off", realList}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
