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
#include "NodeAddress.h"
#include "World.h"
#include "Collision/ColModel.h"
#include "Events/EventDeath.h"
#include "Tasks/TaskTypes/TaskSimpleScratchHead.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn90B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Ocho funciones (hook S): CNodeAddress::operator== 0x420920, operator!= 0x420950, operator< 0x420980, CWorld::AdvanceCurrentScanCode 0x4072E0, y los constructores
// CTaskSimpleScratchHead 0x48DF30, CTaskSimpleAchieveHeading(float,float,float) 0x667E20, CColModel 0x40FB60 y CEventDeath(bool) 0x4ADDF0.
// Colaborador sustituido por un falso identico en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco): CWorld::ClearScanCodes 0x563470 (cdecl, sin argumentos),
// solo en AdvanceCurrentScanCode. Los constructores NO sustituyen a sus bases (CTaskSimpleRunAnim 0x61A900 / CTask 0x61A390 en el EXE; en la DLL son constructores C++ en linea sin direccion propia):
// corren de verdad en ambos lados y se compara el objeto entero (vtable normalizada: el EXE deja la suya, la DLL la de su clase en el ASI).

namespace F90Layout {
static_assert(sizeof(CNodeAddress) == 4 && offsetof(CNodeAddress, m_wAreaId) == 0 && offsetof(CNodeAddress, m_wNodeId) == 2, "CNodeAddress: area en +0, nodo en +2");
static_assert(sizeof(CColModel) == 0x30 && sizeof(CEventDeath) == 0x14 && sizeof(CTaskSimpleAchieveHeading) == 0x18, "tamanos de los objetos construidos");
static_assert(sizeof(CTaskSimpleScratchHead) == 0x20, "CTaskSimpleScratchHead: 0x20 bytes");
}

namespace F90Live {
using namespace F90Fix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x420920, 0x420950, 0x420980, 0x4072E0, 0x48DF30, 0x667E20, 0x40FB60, 0x4ADDF0};
inline constexpr u32 kExeVtable[kFunctions] = {0, 0, 0, 0, 0x85A100, 0x86FD74, 0, 0x85ADC0};   // CColModel no tiene vtable
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }

// ================================================================================================ arenas
struct Arena {
    alignas(16) std::array<uint8, kArena> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + kGuard; }
    void Fill(u32 seed, u32 mode) { FillMode(m.data(), kArena, seed, mode); }
    void Snap() { r = m; }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), kArena, kGuard); }
};
inline Arena arA, arB;

// Eventos: 1 ClearScanCodes [word bajo de 0xB7CD78 en el momento de la llamada]
inline void __cdecl FakeClear() { Ev& e = PushEv(1, 1); e.v[0] = G32(kScanAddr) & 0xFFFFu; }

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
    json input;
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        arA.Fill(c.fillSeed, c.fillA);
        arB.Fill(c.fillSeed + 1u, 0);
        G32(kScanAddr) = c.scan; G32(kTimeAddr) = c.time;
        std::vector<u32> args; u32 seedEcx = u32(arA.Obj());
        switch (fn) {
        case 0: case 1: case 2:
            arA.Put16(0, c.a & 0xFFFFu); arA.Put16(2, c.a >> 16);
            arB.Put16(0, c.b & 0xFFFFu); arB.Put16(2, c.b >> 16);
            if (c.alias) { arB.Put16(0, c.a & 0xFFFFu); arB.Put16(2, c.a >> 16); }
            args = { u32(c.alias ? arA.Obj() : arB.Obj()) };
            break;
        case 3: seedEcx = c.ecx; break;
        case 4: case 6: break;
        case 5: args = { c.f0, c.f1, c.f2 }; break;
        default: args = { c.drown }; break;
        }
        const std::string win = HexBytes(arA.m.data() + kGuard, kWin);
        if (s == 0) input = json::parse(JsonC(c) + ",\"win\":\"" + win + "\"}");
        arA.Snap(); arB.Snap();
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, seedEcx, c.edx, c.stack, 0u); });
        json obs;
        bool vtOk = true;
        if (kExeVtable[fn]) {
            u32 v; memcpy(&v, arA.m.data() + kGuard, 4);
            vtOk = candidate ? ModuleOf(v) == env.asi : v == kExeVtable[fn];
            if (vtOk) { const u32 e = kExeVtable[fn]; memcpy(arA.m.data() + kGuard, &e, 4); }   // normalizada: el objeto queda como si ambos lados dejaran la vtable del EXE
        }
        if (fn <= 2) obs["ret"] = sides[s].reg.eax & 0xFFu;
        else if (fn != 3) obs["ret"] = sides[s].reg.eax == seedEcx ? 1u : 0u;   // el constructor devuelve this
        obs["vtable_ok"] = vtOk;
        obs["changes"] = arA.Changes();
        obs["b_changes"] = arB.Changes();
        obs["scan"] = G32(kScanAddr);
        obs["time"] = G32(kTimeAddr);
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
    if (!std::filesystem::exists(dir / "fn90b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn90b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn90b"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
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
    const Pair pClear = MakeCollab(env, 0x563470, hooksFound);
    env.stubs[3] = {S(pClear, reinterpret_cast<void*>(&FakeClear))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        const bool ctor = f >= 4;
        collaborators[kFn[f]] = {{"dll_real", ctor ? json::array({"constructores base (CTaskSimpleRunAnim/CTask/CBoundingBox/CEvent) reales en ambos lados"}) : json::array()},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
