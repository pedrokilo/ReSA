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

// Banco par a par Fn85E dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CTaskSimpleFight::GetAvailableComboSet 0x61C7F0 (thiscall, ret 8; devuelve AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL
// (con su hook RH apagado durante el banco): CWeaponInfo::GetWeaponInfo 0x743C60, CAnimManager::GetAnimBlockName 0x4D3A30, GetAnimationBlock(const char*) 0x4D3940, AddAnimBlockRef 0x4D3FB0 y
// CStreaming::RequestModel 0x4087E0 (todos con hook) y, sin hook (funcion C++ de la DLL sin RH_ScopedInstall; falso tambien en su cuerpo), CAnimManager::RemoveAnimBlockRef 0x4D3FD0.
// Registran sus argumentos EN ORDEN. Los bloques de animacion son los reales de ms_aAnimBlocks (0..15: solo se lee y se escribe su byte IsLoaded), el arreglo de grupos 0xB4EA34 apunta a una arena,
// las 12 filas de la tabla de combos 0xC170D0 se fijan por caso (grupo +0 y banderas altas +0x85) y todo se restaura. Mutaciones (una sola vez por sitio): tras RemoveAnimBlockRef cambia el grupo de
// todas las filas (el EXE relee la fila), tras RequestModel cambia el comando siguiente (+0x26) y dentro de GetWeaponInfo cambia el estilo de pelea del ped (+0x72D, que el EXE lee DESPUES).

namespace F85ELayout {
struct FightPed : CPed {
    static constexpr size_t OffSlot() { return offsetof(FightPed, m_nActiveWeaponSlot); }
    static constexpr size_t OffStyle() { return offsetof(FightPed, m_nFightingStyle); }
    static constexpr size_t OffWeapons() { return offsetof(FightPed, m_aWeapons); }
};
static_assert(FightPed::OffSlot() == 0x718 && FightPed::OffStyle() == 0x72D && FightPed::OffWeapons() == 0x5A0 && sizeof(CWeapon) == 0x1C && offsetof(CWeapon, m_Type) == 0, "CPed: armas +0x5A0 (0x1C), ranura +0x718, estilo +0x72D");
static_assert(offsetof(CTaskSimpleFight, m_bAnimsReferenced) == 0xA && offsetof(CTaskSimpleFight, m_nRequiredAnimGroup) == 0xC && offsetof(CTaskSimpleFight, m_nNextCommand) == 0x26, "CTaskSimpleFight: +0xA, +0xC, +0x26");
static_assert(offsetof(CWeaponInfo, m_nBaseCombo) == 0x6E, "CWeaponInfo: combo base +0x6E");
static_assert(offsetof(CAnimBlock, IsLoaded) == 0x10 && sizeof(CAnimBlock) == 0x20 && sizeof(CAnimBlendAssocGroup) == 0x14 && offsetof(CAnimBlendAssocGroup, m_AnimBlock) == 0, "bloques de 0x20 (IsLoaded +0x10), grupos de 0x14 (bloque +0)");
static_assert(sizeof(CMeleeInfo) == 0x88 && offsetof(CMeleeInfo, m_wFlags) == 0x84, "CMeleeInfo: 0x88 bytes, banderas +0x84 (+0x85 = byte alto)");
}

namespace F85ELive {
using namespace F85EFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x61C7F0};
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
inline Arena<kInfoArena> infoA;
inline Arena<kNameArena> nameA;
inline Arena<kGroupsArena> groupsA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[4]{}; };
inline State g;
inline void Mutate(u32 site) {
    if (g.c->mutSite != site || g.mutDone[site]) return;
    g.mutDone[site] = true;
    if (site == 1) for (unsigned i = 0; i < 12; ++i) G32(kComboAddr + i * 0x88) = g.c->mutVal;
    if (site == 2) taskA.Put8(0x26, g.c->mutVal);
    if (site == 3) pedA.Put8(0x72D, g.c->mutVal);
}

// Eventos: 1 GetWeaponInfo [tipo, habilidad]; 2 GetAnimBlockName [grupo]; 3 GetAnimationBlock [nombre]; 4 AddAnimBlockRef [indice]; 5 RemoveAnimBlockRef [indice]; 6 RequestModel [modelo, banderas]
// Todos __cdecl, como los originales (el EXE no lee ECX ni EDX tras ninguno de ellos).
inline void* __cdecl FkInfo(u32 type, u32 skill) { Ev& e = PushEv(1, 2); e.v[0] = type; e.v[1] = skill; Mutate(3); return reinterpret_cast<void*>(infoA.Obj()); }     // 0x743C60
inline const char* __cdecl FkName(u32 group) { Ev& e = PushEv(2, 1); e.v[0] = group; return reinterpret_cast<const char*>(nameA.Obj(size_t(group & 0x3Fu) * 4)); }  // 0x4D3A30
inline void* __cdecl FkBlock(const char* name) { Ev& e = PushEv(3, 1); e.v[0] = Tag(name); return reinterpret_cast<void*>(uintptr_t(kBlocksAddr) + 0x20u * g.c->lookup); }  // 0x4D3940
inline void __cdecl FkAdd(u32 idx) { Ev& e = PushEv(4, 1); e.v[0] = idx; }                                                                                              // 0x4D3FB0
inline void __cdecl FkRemove(u32 idx) { Ev& e = PushEv(5, 1); e.v[0] = idx; Mutate(1); }                                                                                 // 0x4D3FD0 (sin hook)
inline void __cdecl FkRequest(u32 model, u32 flags) { Ev& e = PushEv(6, 2); e.v[0] = model; e.v[1] = flags; Mutate(2); }                                                 // 0x4087E0

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
    infoA.Fill(c.seed * 13u + 60u);
    nameA.Fill(c.seed * 13u + 61u);
    groupsA.Fill(c.seed * 13u + 62u);
}
inline void SnapArenas() { taskA.Snap(); pedA.Snap(); infoA.Snap(); nameA.Snap(); groupsA.Snap(); }

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
        TagAdd(infoA.m.data(), infoA.m.size());     // 2
        TagAdd(nameA.m.data(), nameA.m.size());     // 3
        TagAdd(groupsA.m.data(), groupsA.m.size()); // 4
        taskA.Put8(0xA, c.ref);
        taskA.Put32(0xC, c.cur);
        taskA.Put8(0x26, c.nextCmd);
        pedA.Put8(0x718, c.slot);
        pedA.Put32(size_t(i32(0x5A0) + i32(int8(c.slot)) * 0x1C), c.wtype);
        pedA.Put8(0x72D, c.fstyle);
        infoA.Put8(0x6E, c.binfo);
        for (unsigned gi = 0; gi < kGroups; ++gi) groupsA.Put32(gi * 0x14, c.gblk[gi] ? u32(kBlocksAddr + 0x20u * (c.gblk[gi] - 1)) : 0u);
        G32(kGroupsAddr) = u32(groupsA.Obj());
        for (unsigned k = 0; k < 16; ++k) G8(kBlocksAddr + 0x20u * k + 0x10u) = uint8(c.loaded[k]);
        for (unsigned i = 0; i < 12; ++i) { G32(kComboAddr + i * 0x88) = c.rowGroup[i]; G8(kComboAddr + i * 0x88 + 0x85) = uint8(c.rowFlags[i]); }
        const std::vector<u32> args = { u32(pedA.Obj()), c.cmd };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(taskA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret"] = sides[s].reg.eax & 0xFFu;
        obs["task_changes"] = taskA.Changes(); obs["ped_changes"] = pedA.Changes(); obs["info_changes"] = infoA.Changes();
        obs["name_changes"] = nameA.Changes(); obs["groups_changes"] = groupsA.Changes();
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
    if (!std::filesystem::exists(dir / "fn85e-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85e-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85e"); }
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
    const Pair pInfo = MakeCollab(env, 0x743C60, hooksFound), pName = MakeCollab(env, 0x4D3A30, hooksFound), pBlock = MakeCollab(env, 0x4D3940, hooksFound);
    const Pair pAdd = MakeCollab(env, 0x4D3FB0, hooksFound), pRequest = MakeCollab(env, 0x4087E0, hooksFound);
    // RemoveAnimBlockRef (0x4D3FD0): sin hook RH; la DLL llama a su propia funcion C++, asi que el mismo falso va tambien en su cuerpo
    const Pair pRemove{0x4D3FD0, Body(Addr(&CAnimManager::RemoveAnimBlockRef)), false};
    env.stubs[0] = {S(pInfo, reinterpret_cast<void*>(&FkInfo)), S(pName, reinterpret_cast<void*>(&FkName)), S(pBlock, reinterpret_cast<void*>(&FkBlock)), S(pAdd, reinterpret_cast<void*>(&FkAdd)),
                    S(pRemove, reinterpret_cast<void*>(&FkRemove)), S(pRequest, reinterpret_cast<void*>(&FkRequest))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"aritmetica de indices de bloque (sar 5) y de filas de la tabla de combos"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
