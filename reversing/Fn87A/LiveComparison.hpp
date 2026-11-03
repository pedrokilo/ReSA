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
#include "Entity/Ped/PlayerPed.h"
#include "CutsceneMgr.h"
#include "World.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn87A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CPlayerPed::ProcessWeaponSwitch 0x60D850 (thiscall, un argumento: el CPad*; ret 4; void). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL:
// CDarkel::FrenzyOnGoing 0x43D1F0, CPedIntelligence::GetTaskJetPack 0x601110 y GetTaskUseGun 0x600F70, CPad::CycleWeaponRightJustDown 0x540640 / CycleWeaponLeftJustDown 0x540610 / GetWeapon 0x540180,
// CWeapon::HasWeaponAmmoToBeUsed 0x73B2A0 y CanBeUsedFor2Player 0x73DEF0, CGameLogic::IsCoopGameGoingOn 0x441390, CWeaponInfo::GetWeaponInfo 0x743C60, CPed::RemoveWeaponAnims 0x5F0250 y
// CPlayerPed::MakeChangesForNewWeapon 0x60B460. Los falsos registran sus argumentos EN ORDEN. El jugador, sus datos, la inteligencia, el pad, la tarea de disparo y la informacion del arma son arenas;
// el modo de la camara (word 0xB6F858) es memoria real del juego (se guarda y se restaura tras cada lado).

namespace F87ALayout {
static_assert(offsetof(CPed, m_aWeapons) == 0x5A0 && offsetof(CPed, m_nActiveWeaponSlot) == 0x718
              && offsetof(CPed, m_pTargetedObject) == 0x71C, "campos de CPed: armas +0x5A0, ranura activa +0x718, objetivo +0x71C");
static_assert(offsetof(CPhysical, m_pAttachedTo) == 0xFC, "CPhysical: m_pAttachedTo +0xFC");
static_assert(sizeof(CWeapon) == 0x1C, "CWeapon mide 0x1C");
static_assert(sizeof(CPlayerPed) <= 0x800, "la arena del jugador cubre el objeto");
}

namespace F87ALive {
using namespace F87AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x60D850};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kPedArena> pedA;
inline Arena<kPdArena> pdA;
inline Arena<kIntelArena> intelA;
inline Arena<kPadArena> padA;
inline Arena<kTaskArena> taskA;
inline Arena<kInfoArena> infoA;

struct State { const C* c{}; };
inline State g;

// Eventos: 1 FrenzyOnGoing []; 2 GetTaskJetPack [inteligencia]; 3 CycleWeaponRightJustDown [pad]; 4 CycleWeaponLeftJustDown [pad]; 5 HasWeaponAmmoToBeUsed [arma]; 6 IsCoopGameGoingOn [];
// 7 CanBeUsedFor2Player [arma]; 8 GetWeaponInfo [tipo, habilidad]; 9 CPad::GetWeapon [pad, jugador]; 10 GetTaskUseGun [inteligencia]; 11 RemoveWeaponAnims [jugador, ranura, bits del float];
// 12 MakeChangesForNewWeapon [jugador, tipo].
inline u32 WeaponIndex(const void* weapon) { return u32((int64(Addr(weapon)) - int64(pedA.Obj(0x5A0))) / 0x1C); }
inline u32 MaskBit(u32 mask, u32 idx) { return idx < 32 ? (mask >> idx) & 1u : 0u; }
inline u32 __cdecl FkFrenzy() {                                                                       // 0x43D1F0
    PushEv(1, 0);
    return g.c->frenzy;
}
inline u32 __fastcall FkJetpack(void* self, void*) {                                                  // 0x601110
    Ev& e = PushEv(2, 1); e.v[0] = Tag(self);
    return g.c->jet ? u32(taskA.Obj()) : 0u;
}
inline u32 __fastcall FkRight(void* self, void*) {                                                    // 0x540640
    Ev& e = PushEv(3, 1); e.v[0] = Tag(self);
    return g.c->right;
}
inline u32 __fastcall FkLeft(void* self, void*) {                                                     // 0x540610
    Ev& e = PushEv(4, 1); e.v[0] = Tag(self);
    return g.c->left;
}
inline u32 __fastcall FkHasAmmo(void* self, void*) {                                                  // 0x73B2A0
    Ev& e = PushEv(5, 1); e.v[0] = Tag(self);
    return MaskBit(g.c->hasmask, WeaponIndex(self));
}
inline u32 __cdecl FkCoop() {                                                                         // 0x441390
    PushEv(6, 0);
    return g.c->coop;
}
inline u32 __fastcall FkCan2P(void* self, void*) {                                                    // 0x73DEF0
    Ev& e = PushEv(7, 1); e.v[0] = Tag(self);
    return MaskBit(g.c->can2mask, WeaponIndex(self));
}
inline u32 __cdecl FkWeaponInfo(u32 type, u32 skill) {                                                // 0x743C60 (cdecl)
    Ev& e = PushEv(8, 2); e.v[0] = type; e.v[1] = skill;
    infoA.Put32(0, ((g.c->infomask >> (type & 31)) & 1u) ? g.c->infoval : 0u);
    return u32(infoA.Obj());
}
inline u32 __fastcall FkGetWeapon(void* self, void*, void* ped) {                                     // 0x540180 (ret 4)
    Ev& e = PushEv(9, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return g.c->padw;
}
inline u32 __fastcall FkUseGun(void* self, void*) {                                                   // 0x600F70
    Ev& e = PushEv(10, 1); e.v[0] = Tag(self);
    return g.c->ug ? u32(taskA.Obj()) : 0u;
}
inline void __fastcall FkRemoveAnims(void* self, void*, u32 slot, u32 blendBits) {                    // 0x5F0250 (ret 8)
    Ev& e = PushEv(11, 3); e.v[0] = Tag(self); e.v[1] = slot; e.v[2] = blendBits;
}
inline void __fastcall FkMakeChanges(void* self, void*, u32 type) {                                   // 0x60B460 (ret 4)
    Ev& e = PushEv(12, 2); e.v[0] = Tag(self); e.v[1] = type;
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["cam"] = G16(kCamModeAddr);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void SetupArenas(const C& c) {
    pedA.Fill(c.seed * 13u + 50u); pdA.Fill(c.seed * 13u + 51u); intelA.Fill(c.seed * 13u + 52u); padA.Fill(c.seed * 13u + 53u); taskA.Fill(c.seed * 13u + 54u); infoA.Fill(c.seed * 13u + 55u);
}
inline void SnapArenas() { pedA.Snap(); pdA.Snap(); intelA.Snap(); padA.Snap(); taskA.Snap(); infoA.Snap(); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(pedA.m.data(), pedA.m.size());       // 0
        TagAdd(pdA.m.data(), pdA.m.size());         // 1
        TagAdd(intelA.m.data(), intelA.m.size());   // 2
        TagAdd(padA.m.data(), padA.m.size());       // 3
        TagAdd(taskA.m.data(), taskA.m.size());     // 4
        TagAdd(infoA.m.data(), infoA.m.size());     // 5
        for (unsigned k = 0; k < kSlots; ++k) {
            pedA.Put32(0x5A0 + k * 0x1C, c.wtype[k]);
            if (k != 13) pedA.Put32(0x5A0 + k * 0x1C + 0xC, c.wammo[k]);
        }
        pedA.Put32(0x718, (c.slot & 0xFFu) | (c.slothi << 8));
        pedA.Put32(0xFC, c.attached);
        pedA.Put32(0x71C, c.targeted);
        pedA.Put32(0x47C, u32(intelA.Obj()));
        pedA.Put32(0x480, u32(pdA.Obj()));
        pdA.Put32(0x34, c.pflags);
        pdA.Put8(0x85, c.dontallow);
        pdA.Put8(0x20, c.chosen0);
        taskA.Put8(0x10, c.ugcmd);
        taskA.Put32(0x2C, c.uganim);
        G16(kCamModeAddr) = uint16(c.cam);
        const std::vector<u32> args = { u32(padA.Obj()) };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(pedA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, pedA.Changes()); add(1, pdA.Changes()); add(2, intelA.Changes()); add(3, padA.Changes()); add(4, taskA.Changes());
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
    for (const auto& s : env.dllOnly) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
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
    if (!std::filesystem::exists(dir / "fn87a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn87a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn87a"); }
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
    auto F = [&](uintptr_t exe, auto* fake) { return S(MakeCollab(env, exe, hooksFound), reinterpret_cast<void*>(fake)); };
    env.stubs[0] = {F(0x43D1F0, &FkFrenzy), F(0x601110, &FkJetpack), F(0x540640, &FkRight), F(0x540610, &FkLeft), F(0x73B2A0, &FkHasAmmo), F(0x441390, &FkCoop), F(0x73DEF0, &FkCan2P),
                    F(0x743C60, &FkWeaponInfo), F(0x540180, &FkGetWeapon), F(0x600F70, &FkUseGun), F(0x5F0250, &FkRemoveAnims), F(0x60B460, &FkMakeChanges)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"lectura con signo de las ranuras y del modo de la camara, ciclo derecho e izquierdo, retroceso por falta de municion, comparacion final con la ranura activa"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
