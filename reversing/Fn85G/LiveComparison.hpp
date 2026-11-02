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
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn85G dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CTaskSimpleFight::ChooseAttackPlayer 0x624710 (thiscall, ret 4; devuelve AX). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL
// (con su hook RH apagado durante el banco cuando lo tienen): CPed::SetMoveState 0x5DEC00 (hook; thiscall, ret 4), CPed::IsAlive 0x5E0170 (hook; thiscall, devuelve AL) y
// CTaskSimpleFight::TargetInReach 0x61D6F0 (sin reconstruir: la DLL lo llama por su direccion; thiscall, ret 4, devuelve AL). Registran sus argumentos EN ORDEN. La aritmetica del rumbo
// (cuadrados, suma, restas, fpatan, vuelta de 2pi, comparaciones) corre de verdad en ambos lados; las filas de la tabla de combos son la memoria real del juego (se rellenan los bytes de banderas de los combos 0..16
// y se restauran tras cada lado).
// Mutaciones (una sola vez): 1 y 2 dentro del falso de 0x61D6F0 (tarea: siguiente, ultimo, movimiento y animacion; objetivo: tipo y banderas), 3 dentro de 0x5DEC00 (bandera del combo y byte +0x72E: el EXE ya los tiene en registro),
// 4, 5 y 6 dentro de la llamada (mutA & 15) a 0x5E0170 (posicion del ped consultado, rumbo del ped, elemento de la lista de peds cercanos): el EXE los relee despues.

namespace F85GLayout {
static_assert(offsetof(CTaskSimpleFight, m_nChainCounter) == 0x15 && offsetof(CTaskSimpleFight, m_pTargetEntity) == 0x18 && offsetof(CTaskSimpleFight, m_pAnim) == 0x1C, "CTaskSimpleFight: cadena +0x15, objetivo +0x18, animacion +0x1C");
static_assert(offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nCurrentMove) == 0x25 && offsetof(CTaskSimpleFight, m_nNextCommand) == 0x26 && offsetof(CTaskSimpleFight, m_nLastCommand) == 0x27, "CTaskSimpleFight: combo +0x24, movimiento +0x25, siguiente +0x26, ultimo +0x27");
static_assert(offsetof(CPed, m_nMoveState) == 0x534 && offsetof(CPed, m_fCurrentRotation) == 0x558 && offsetof(CPed, m_fAimingRotation) == 0x55C, "CPed: movimiento +0x534, rumbo +0x558, rumbo de apuntado +0x55C (la inteligencia +0x47C es protegida: la comprueba el banco por comportamiento)");
}

namespace F85GLive {
using namespace F85GFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x624710};
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
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, kGuard); }
};
inline Arena<kTaskArena> taskA;
inline Arena<kPedArena> pedA;
inline Arena<kTgtArena> tgtA;
inline Arena<kIntArena> intA;
inline Arena<kSMatArena> smatA;
inline std::array<Arena<kOthArena>, kOthers> othA;
inline std::array<Arena<kOMatArena>, kOthers> omatA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[7]{}; unsigned aliveCalls{}; };
inline State g;
inline uint8& TableFlag(u32 combo) { return G8(0xC170D0 + (static_cast<int32>(combo) - 4) * 0x88 + 0x84); }
inline void PutPos(unsigned k, const u32* p) {
    if (othA[k].Get32(0x14)) { omatA[k].Put32(0x30, p[0]); omatA[k].Put32(0x34, p[1]); omatA[k].Put32(0x38, p[2]); }
    else { othA[k].Put32(4, p[0]); othA[k].Put32(8, p[1]); othA[k].Put32(0xC, p[2]); }
}
inline bool Once(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }

// Eventos: 1 CPed::SetMoveState [ped, estado]; 2 TargetInReach [tarea, ped]; 3 CPed::IsAlive [ped]. Todos thiscall como los originales (el EXE no lee ECX ni EDX tras ninguno de ellos).
inline void __fastcall FkSetMoveState(void* self, void*, u32 state) {      // 0x5DEC00 (ret 4)
    Ev& e = PushEv(1, 2); e.v[0] = Tag(self); e.v[1] = state;
    if (Once(3)) { TableFlag(g.c->combo) = uint8(g.c->mutA); pedA.Put8(0x72E, g.c->mutB); }
}
inline u32 __fastcall FkReach(void* self, void*, void* ped) {              // 0x61D6F0 (ret 4)
    Ev& e = PushEv(2, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped);
    if (Once(1)) { taskA.Put8(0x26, g.c->mutA); taskA.Put8(0x27, g.c->mutB); taskA.Put8(0x25, g.c->mutC); taskA.Put32(0x1C, g.c->mutD); }
    if (Once(2)) { tgtA.Put8(0x36, g.c->mutA); tgtA.Put32(0x46C, g.c->mutB); }
    return g.c->reach;
}
inline u32 __fastcall FkIsAlive(void* self, void*) {                       // 0x5E0170 (devuelve AL)
    Ev& e = PushEv(3, 1); e.v[0] = Tag(self);
    unsigned k = 0;
    for (unsigned i = 0; i < kOthers; ++i) if (othA[i].Obj() == uintptr_t(self)) k = i;
    if (g.aliveCalls++ == (g.c->mutA & 15u)) {
        if (Once(4)) PutPos(k, g.c->mutPos);
        if (Once(5)) pedA.Put32(0x558, g.c->mutB);
        if (Once(6)) {   // el elemento que se esta consultando nunca pasa a nulo (el EXE lo relee y lo desreferencia)
            const u32 slot = g.c->mutC & 15u;
            u32 current = 0;
            for (u32 i = 0; i < kOthers; ++i) if (intA.Get32(0x130 + 4 * i) == u32(self)) { current = i; break; }
            intA.Put32(0x130 + 4 * slot, ((g.c->mutB & 1u) && slot != current) ? 0u : u32(othA[(g.c->mutB >> 1) & 15u].Obj()));
        }
    }
    return g.c->oalive[k];
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
    tgtA.Fill(c.seed * 13u + 60u);
    intA.Fill(c.seed * 13u + 61u);
    smatA.Fill(c.seed * 13u + 62u);
    for (unsigned i = 0; i < kOthers; ++i) { othA[i].Fill(c.seed * 13u + 100u + i); omatA[i].Fill(c.seed * 13u + 140u + i); }
}
inline void SnapArenas() { taskA.Snap(); pedA.Snap(); tgtA.Snap(); intA.Snap(); smatA.Snap(); for (unsigned i = 0; i < kOthers; ++i) { othA[i].Snap(); omatA[i].Snap(); } }

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
        TagAdd(tgtA.m.data(), tgtA.m.size());       // 2
        TagAdd(intA.m.data(), intA.m.size());       // 3
        TagAdd(smatA.m.data(), smatA.m.size());     // 4
        for (unsigned i = 0; i < kOthers; ++i) TagAdd(othA[i].m.data(), othA[i].m.size());     // 5..20
        for (unsigned i = 0; i < kOthers; ++i) TagAdd(omatA[i].m.data(), omatA[i].m.size());   // 21..36
        taskA.Put8(0x15, c.chain); taskA.Put8(0x24, c.combo); taskA.Put8(0x25, c.move); taskA.Put8(0x26, c.next); taskA.Put8(0x27, c.last);
        taskA.Put32(0x1C, c.animptr); taskA.Put32(0x18, c.tgt ? u32(tgtA.Obj()) : 0u);
        tgtA.Put8(0x36, c.ttype); tgtA.Put32(0x46C, c.tflags);
        pedA.Put32(0x14, c.smat ? u32(smatA.Obj()) : 0u); pedA.Put32(0x47C, u32(intA.Obj())); pedA.Put8(0x72E, c.p72e); pedA.Put32(0x534, c.mstate);
        pedA.Put32(0x558, c.rot); pedA.Put32(0x55C, c.aim);
        if (c.smat) { smatA.Put32(0x30, c.spos[0]); smatA.Put32(0x34, c.spos[1]); smatA.Put32(0x38, c.spos[2]); }
        else { pedA.Put32(4, c.spos[0]); pedA.Put32(8, c.spos[1]); pedA.Put32(0xC, c.spos[2]); }
        for (unsigned i = 0; i < kOthers; ++i) {
            intA.Put32(0x130 + 4 * i, c.opres[i] ? u32(othA[i].Obj()) : 0u);
            othA[i].Put32(0x14, c.omat[i] ? u32(omatA[i].Obj()) : 0u);
            PutPos(i, &c.opos[i * 3]);
        }
        for (u32 j = 0; j < kCombos; ++j) TableFlag(j) = uint8(c.flags[j]);
        const std::vector<u32> args = { u32(pedA.Obj()) };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(taskA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret"] = int(int16_t(sides[s].reg.eax & 0xFFFFu));
        obs["task_changes"] = taskA.Changes(); obs["ped_changes"] = pedA.Changes(); obs["tgt_changes"] = tgtA.Changes();
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
    if (!std::filesystem::exists(dir / "fn85g-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85g-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85g"); }
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
    const Pair pSet = MakeCollab(env, 0x5DEC00, hooksFound), pAlive = MakeCollab(env, 0x5E0170, hooksFound), pReach = MakeCollab(env, 0x61D6F0, hooksFound);
    env.stubs[0] = {S(pSet, reinterpret_cast<void*>(&FkSetMoveState)), S(pReach, reinterpret_cast<void*>(&FkReach)), S(pAlive, reinterpret_cast<void*>(&FkIsAlive))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"aritmetica del rumbo (distancia cuadrada, restas, fpatan, vuelta de 2pi y comparaciones, con los ayudantes x87 de 24 bits)"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
