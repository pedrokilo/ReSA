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
#include "Entity/Vehicle/Automobile.h"
#include "Models/VehicleModelInfo.h"
#include "Models/ModelInfo.h"
#include "DamageManager.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn104A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S) con 16384 pares EXE/DLL:
//   0  CAutomobile::SetRandomDamage  0x6A2530  thiscall, coche en ECX, bRemoveStuff en pila (dword; el EXE lee el byte), `ret 4`; void.
// Colaboradores (todos con hook RH, apagado durante el banco; el falso va en la direccion del EXE y en el cuerpo del hook de la DLL):
//   CDamageManager::GetCarNodeIndexFromDoor 0x6C26F0 y ...FromPanel 0x6C26A0 (cdecl, un argumento), GetDoorStatus 0x6C2230 y GetPanelStatus 0x6C2180 (thiscall, `ret 4`, EAX completo),
//   SetDoorStatus 0x6C21C0 y SetPanelStatus 0x6C2150 (thiscall, `ret 8`), CVehicle::SetComponentVisibility 0x6D2700 (thiscall, `ret 8`);
//   y rand() del CRT del EXE 0x821B1E (sin hook: la DLL la llama por su direccion; un solo falso). Los hooks de CDamageManager son envoltorios estaticos: la funcion nueva llama a los METODOS de
//   CDamageManager de la DLL, asi que los mismos falsos se instalan ademas en los cuerpos de esos metodos (parches solo en la DLL).
// NO se sustituyen (corren de verdad en ambos lados): la aritmetica de las tiradas (x87 del EXE a 24 bits frente a floats de la DLL), la busqueda del n-esimo elemento sin marcar, la mascara del
// modelo (tabla de modelos 0xA9B0C8 real: la entrada del modelo del caso se sustituye por un model info propio y se restaura), el arreglo de nodos y los marcadores de elegidos.

namespace F104ALayout {
static_assert(sizeof(CAutomobile) == 0x988 && offsetof(CAutomobile, m_damageManager) == 0x5A0 && offsetof(CAutomobile, m_aCarNodes) == 0x648 && offsetof(CAutomobile, m_nModelIndex) == 0x22, "CAutomobile");
static_assert(offsetof(CVehicleModelInfo, m_pVehicleStruct) == 0x5C && offsetof(CVehicleModelInfo::CVehicleStructure, m_nMaskComponentsDamagable) == 0x310, "CVehicleModelInfo");
static_assert(sizeof(CDamageManager) == 0x18, "CDamageManager");
}

namespace F104ALive {
using namespace F104AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x6A2530};
inline constexpr uintptr_t kModelTable = 0xA9B0C8;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F104AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kCarWin> carA;      // 0: coche
inline Arena<kMiWin> miA;        // 1: model info falso
inline Arena<kVsWin> vsA;        // 2: estructura del vehiculo del model info

struct State { const C* c{}; unsigned randIdx{}; };
inline State g;

// Eventos: 1 rand() 0x821B1E [indice de tirada]; 2 GetCarNodeIndexFromDoor [puerta]; 3 GetCarNodeIndexFromPanel [panel]; 4 GetDoorStatus [gestor, puerta]; 5 GetPanelStatus [gestor, panel];
// 6 SetDoorStatus [gestor, puerta, estado]; 7 SetPanelStatus [gestor, panel, estado]; 8 SetComponentVisibility [coche, marco, estado]
inline u32 __cdecl FkRand() {
    Ev& e = PushEv(1, 1); e.v[0] = g.randIdx;
    const u32 v = g.randIdx < kRands ? g.c->a[1 + g.randIdx] : 0u;
    ++g.randIdx;
    return v;
}
inline u32 __cdecl FkDoorNode(u32 door) { Ev& e = PushEv(2, 1); e.v[0] = door; return door < 6 ? g.c->a[14 + door] : 0xFFFFFFFFu; }
inline u32 __cdecl FkPanelNode(u32 panel) { Ev& e = PushEv(3, 1); e.v[0] = panel; return panel < 7 ? g.c->a[20 + panel] : 0xFFFFFFFFu; }
inline u32 __fastcall FkDoorStatus(void* self, void*, u32 door) { Ev& e = PushEv(4, 2); e.v[0] = Tag(self); e.v[1] = door; return door < 6 ? g.c->a[27 + door] : 0u; }
inline u32 __fastcall FkPanelStatus(void* self, void*, u32 panel) { Ev& e = PushEv(5, 2); e.v[0] = Tag(self); e.v[1] = panel; return panel < 7 ? g.c->a[33 + panel] : 0u; }
inline void __fastcall FkSetDoor(void* self, void*, u32 door, u32 status) { Ev& e = PushEv(6, 3); e.v[0] = Tag(self); e.v[1] = door; e.v[2] = status; }
inline void __fastcall FkSetPanel(void* self, void*, u32 panel, u32 status) { Ev& e = PushEv(7, 3); e.v[0] = Tag(self); e.v[1] = panel; e.v[2] = status; }
inline void __fastcall FkSetVis(void* self, void*, u32 frame, u32 state) { Ev& e = PushEv(8, 3); e.v[0] = Tag(self); e.v[1] = frame; e.v[2] = state; }

inline json CloseSide(Env& env, Side& side, json obs, u32* entry, u32 entryAfterRestoreWant) {
    auto fp = NormalizeFp(side.fp);
    obs["entry_after"] = Tag(uintptr_t(*entry));
    obs["globals_hash"] = Fnv(entry, 4);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    *entry = entryAfterRestoreWant;
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    u32* entry = reinterpret_cast<u32*>(kModelTable + 4u * c.a[12]);
    const u32 entryOriginal = *entry;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        carA.Fill(sd++); miA.Fill(sd++); vsA.Fill(sd++);
        TagClear();
        TagAdd(carA.m.data(), carA.m.size());     // 0
        TagAdd(miA.m.data(), miA.m.size());       // 1
        TagAdd(vsA.m.data(), vsA.m.size());       // 2
        g = State{}; g.c = &c; nev = 0;
        carA.Put16(0x22, c.a[12]);
        miA.Put32(0x5C, u32(vsA.Obj()));
        vsA.Put32(0x310, c.a[13]);
        carA.Snap(); miA.Snap(); vsA.Snap();
        *entry = u32(miA.Obj());
        std::vector<u32> args{ c.a[0] };
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t e) { return Gate(e, args.data(), u32(args.size()), c.eax, u32(carA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, carA.Changes()); add(1, miA.Changes()); add(2, vsA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs, entry, entryOriginal);
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
    if (!std::filesystem::exists(dir / "fn104a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn104a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn104a"); }
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
    std::map<uintptr_t, Pair> collabCache;
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x821B1E, &FkRand), F(0x6C26F0, &FkDoorNode), F(0x6C26A0, &FkPanelNode), F(0x6C2230, &FkDoorStatus), F(0x6C2180, &FkPanelStatus),
                    F(0x6C21C0, &FkSetDoor), F(0x6C2150, &FkSetPanel), F(0x6D2700, &FkSetVis)};
    // La funcion nueva llama a los metodos de CDamageManager de la DLL (los hooks RH de esas direcciones son envoltorios estaticos con las comprobaciones del EXE): mismos falsos en esos cuerpos
    using DM = CDamageManager;
    const Pair dllMethods[] = {
        {0, Body(std::bit_cast<uintptr_t>(&DM::GetCarNodeIndexFromDoor)), false}, {0, Body(std::bit_cast<uintptr_t>(&DM::GetCarNodeIndexFromPanel)), false},
        {0, Body(std::bit_cast<uintptr_t>(static_cast<eDoorStatus (DM::*)(eDoors) const>(&DM::GetDoorStatus))), false},
        {0, Body(std::bit_cast<uintptr_t>(static_cast<ePanelDamageState (DM::*)(ePanels) const>(&DM::GetPanelStatus))), false},
        {0, Body(std::bit_cast<uintptr_t>(static_cast<void (DM::*)(eDoors, eDoorStatus)>(&DM::SetDoorStatus))), false},
        {0, Body(std::bit_cast<uintptr_t>(static_cast<void (DM::*)(ePanels, ePanelDamageState)>(&DM::SetPanelStatus))), false}};
    void* const dllFakes[] = {reinterpret_cast<void*>(&FkDoorNode), reinterpret_cast<void*>(&FkPanelNode), reinterpret_cast<void*>(&FkDoorStatus), reinterpret_cast<void*>(&FkPanelStatus),
                              reinterpret_cast<void*>(&FkSetDoor), reinterpret_cast<void*>(&FkSetPanel)};
    json dllOnly = json::array();
    for (unsigned k = 0; k < 6; ++k) { env.dllOnly.push_back(StubSpec{dllMethods[k], dllFakes[k]}); dllOnly.push_back({{"dll", dllMethods[k].dll != 0}}); }
    const char* dllReal[kFunctions] = {
        "tiradas de rand() del EXE (falso 0x821B1E identico), aritmetica float de las tiradas, busqueda del n-esimo elemento sin marcar, marcadores de elegidos, mascara de componentes danables del modelo (tabla 0xA9B0C8 con un model info propio) y arreglo de nodos +0x648"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", dllOnly}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
