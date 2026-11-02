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
#include "Entity/Ped/Ped.h"
#include "Weapon.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "AnimBlendAssociation.h"
#include "PlayerPedData.h"
#include "WeaponInfo.h"
#include "AnimManager.h"
#include "AnimBlock.h"
#include "AnimBlendAssocGroup.h"
#include "Streaming.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn85F dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CTaskSimpleFight::FindTargetOnGround 0x61C9B0 (thiscall, ret 4). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado
// durante el banco): RpAnimBlendClumpGetAssociation(clump, id) 0x4D68B0 y CAnimManager::AddAnimation(clump, grupo, id) 0x4D3AA0 (ambos cdecl). Registran sus argumentos EN ORDEN y devuelven asociaciones de
// arenas propias (las cuatro existentes o nulas segun el caso; las nuevas de AddAnimation). La aritmetica del movimiento (cuadrados, suma, raiz, cociente, productos, guardado a float) corre de verdad en ambos lados.
// Mutaciones (una sola vez por sitio): tras la asociacion 0x29 cambian el comando siguiente y el combo (el EXE los lee DESPUES de las cuatro llamadas); tras la 0x2C los datos del jugador (+0x480) pasan a otros
// o sus floats cambian (el EXE los lee despues); tras la primera AddAnimation cambia el clump (+0x18, se relee en cada llamada) o los floats de los datos (no se releen).

namespace F85FLayout {
static_assert(offsetof(CAnimBlendAssociation, m_BlendAmount) == 0x18 && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C, "CAnimBlendAssociation: peso +0x18, retardo +0x1C");
static_assert(offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nNextCommand) == 0x26 && offsetof(CTaskSimpleFight, m_nLastCommand) == 0x27, "CTaskSimpleFight: combo +0x24, siguiente +0x26, ultimo +0x27");
static_assert(offsetof(CPlayerPedData, m_vecFightMovement) == 0xC, "CPlayerPedData: movimiento de pelea +0xC / +0x10");
}

namespace F85FLive {
using namespace F85FFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x61C9B0};
inline constexpr uintptr_t kPedClump = 0x18, kPedPlayerData = 0x480;
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
    u32 Get8(size_t off) const { return m[kGuard + off]; }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, kGuard); }
};
inline Arena<kTaskArena> taskA;
inline Arena<kPedArena> pedA;
inline Arena<kPdArena> pdA, pdB;
inline std::array<Arena<kAssocArena>, 4> asA, newA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[6]{}; unsigned nAdd{}; };
inline State g;
inline void Mutate(u32 site) {
    if (g.c->mutSite != site || g.mutDone[site]) return;
    g.mutDone[site] = true;
    if (site == 1) { taskA.Put8(0x26, g.c->mutA); taskA.Put8(0x24, g.c->mutB); }
    if (site == 2) pedA.Put32(kPedPlayerData, u32(pdB.Obj()));
    if (site == 3) pedA.Put32(kPedClump, g.c->mutA);
    if (site == 4 || site == 5) { pdA.Put32(0xC, g.c->mutA); pdA.Put32(0x10, g.c->mutB); }
}

// Eventos: 1 RpAnimBlendClumpGetAssociation [clump, id]; 2 AddAnimation [clump, grupo, id]. Ambos __cdecl, como los originales (el EXE no lee ECX ni EDX tras ninguno de ellos).
inline void* __cdecl FkGet(u32 clump, u32 id) {                 // 0x4D68B0
    Ev& e = PushEv(1, 2); e.v[0] = Tag(uintptr_t(clump)); e.v[1] = id;
    if (id == 0x29) Mutate(1);
    if (id == 0x2C) { Mutate(2); Mutate(5); }
    return (id >= 0x29 && id <= 0x2C && g.c->pres[id - 0x29]) ? reinterpret_cast<void*>(asA[id - 0x29].Obj()) : nullptr;
}
inline void* __cdecl FkAdd(u32 clump, u32 group, u32 id) {      // 0x4D3AA0
    Ev& e = PushEv(2, 3); e.v[0] = Tag(uintptr_t(clump)); e.v[1] = group; e.v[2] = id;
    void* const result = reinterpret_cast<void*>(newA[g.nAdd & 3u].Obj());
    ++g.nAdd;
    Mutate(3); Mutate(4);
    return result;
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
    taskA.Fill(c.seed * 13u + 50u);
    pedA.Fill(c.seed * 13u + 57u);
    pdA.Fill(c.seed * 13u + 60u);
    pdB.Fill(c.seed * 13u + 61u);
    for (unsigned i = 0; i < 4; ++i) { asA[i].Fill(c.seed * 13u + 70u + i); newA[i].Fill(c.seed * 13u + 80u + i); }
}
inline void SnapArenas() { taskA.Snap(); pedA.Snap(); pdA.Snap(); pdB.Snap(); for (unsigned i = 0; i < 4; ++i) { asA[i].Snap(); newA[i].Snap(); } }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pdA.m.data(), pdA.m.size());         // 2
        TagAdd(pdB.m.data(), pdB.m.size());         // 3
        for (unsigned i = 0; i < 4; ++i) TagAdd(asA[i].m.data(), asA[i].m.size());    // 4..7
        for (unsigned i = 0; i < 4; ++i) TagAdd(newA[i].m.data(), newA[i].m.size());  // 8..11
        taskA.Put8(0x24, c.combo);
        taskA.Put8(0x26, c.next);
        taskA.Put8(0x27, c.last);
        pedA.Put32(kPedClump, c.clump);
        pedA.Put32(kPedPlayerData, u32(pdA.Obj()));
        pdA.Put32(0xC, c.x); pdA.Put32(0x10, c.y);
        pdB.Put32(0xC, c.xb); pdB.Put32(0x10, c.yb);
        for (unsigned i = 0; i < 4; ++i) {
            asA[i].Put32(0x18, c.amt[i]); asA[i].Put32(0x1C, c.dlt[i]);
            newA[i].Put32(0x18, c.newamt[i]); newA[i].Put32(0x1C, c.newdlt[i]);
        }
        const std::vector<u32> args = { u32(pedA.Obj()) };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(taskA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["player_data"] = Tag(uintptr_t(pedA.Get32(kPedPlayerData)));
        pedA.Put32(kPedPlayerData, u32(pdA.Obj()));   // el puntero ya esta en la observacion: no cuenta como cambio de bytes
        obs["task_changes"] = taskA.Changes(); obs["ped_changes"] = pedA.Changes();
        obs["pda_changes"] = pdA.Changes(); obs["pdb_changes"] = pdB.Changes();
        json as = json::array(), nw = json::array();
        for (unsigned i = 0; i < 4; ++i) { as.push_back(asA[i].Changes()); nw.push_back(newA[i].Changes()); }
        obs["assoc_changes"] = as; obs["new_changes"] = nw;
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
    if (!std::filesystem::exists(dir / "fn85f-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85f-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85f"); }
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
    const Pair pGet = MakeCollab(env, 0x4D68B0, hooksFound), pAdd = MakeCollab(env, 0x4D3AA0, hooksFound);
    env.stubs[0] = {S(pGet, reinterpret_cast<void*>(&FkGet)), S(pAdd, reinterpret_cast<void*>(&FkAdd))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"aritmetica del movimiento (cuadrados, suma, raiz, cociente, productos y guardado a float, con los ayudantes x87 de 24 bits)"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
