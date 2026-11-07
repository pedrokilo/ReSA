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
#include "Entity/Placeable.h"
#include "PedIntelligence.h"
#include "MissionCleanup.h"
#include "PedGeometryAnalyser.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn114A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Catorce funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la ABI de cada una):
//   0 CPlaceable::GetRightVector 0x41CC70   1 CPlaceable::GetUpVector 0x50E420   2 CPhysical::ApplyMoveForce(x, y, z) 0x4ABBA0   3 CPedIntelligence::GetPedFOVRange 0x4ABEA0
//   4 CPedGroups::RegisterKillByPlayer 0x5F7E30   5 CMissionCleanup::AddEntityToList 0x4637E0   6 CEventDamage::IsSameEventForAI 0x4ADBF0   7 CEntryExitManager::WeAreInInteriorTransition 0x43E400
//   8 CPointRoute::AddPoint 0x48E180   9 CPointRoute::RemovePoint 0x5F13A0   10 CPedGroupIntelligence::GetTask 0x5F7660   11 CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent 0x4AE100
//   12 CEvent::TakesPriorityOverBase 0x4ABC30   13 IsPedPointerValid 0x5E8A30
// Colaboradores (con hook RH): 0x5429F0 (CPhysical::ApplyMoveForce(CVector), funcion 2) y 0x533310 (IsEntityPointerValid) y 0x56E210 (CWorld::FindPlayerPed) (funcion 13). Las funciones 11 y 12 llaman a las ranuras 1, 2 y 11 de la
// vtabla de un evento (arena con vtabla propia: falsos identicos; el resto de ranuras registra llamadas inesperadas).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.
// NO se sustituyen (corren de verdad en ambos lados): toda la logica de las funciones (incluidos fsin/fcos de GetRightVector).
// Memoria real del juego (se rellena y se restaura tras cada lado): 0xC098E8/0xC098EC (muertes del jugador), 0x96A7CC (estado de entrada/salida de interiores) y 0xB74490 (puntero al pool de peds).

namespace F114ALayout {
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) == 0x4 && offsetof(CSimpleTransform, m_fHeading) == 0xC, "CPlaceable");
static_assert(offsetof(CPedIntelligence, m_fHearingRange) == 0xBC && offsetof(CPedIntelligence, m_fSeeingRange) == 0xC0, "CPedIntelligence");
static_assert(NUM_MISSION_CLEANUP_OBJECTS == 75 && sizeof(tMissionCleanupEntity) == 8 && offsetof(tMissionCleanupEntity, handle) == 4 && offsetof(CMissionCleanup, m_Count) == 0x258, "CMissionCleanup");
static_assert(offsetof(CPointRoute, m_NumEntries) == 0 && offsetof(CPointRoute, m_Entries) == 4 && sizeof(CPointRoute) == 0x64, "CPointRoute");
}

namespace F114ALive {
using namespace F114AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x41CC70, 0x50E420, 0x4ABBA0, 0x4ABEA0, 0x5F7E30, 0x4637E0, 0x4ADBF0, 0x43E400, 0x48E180, 0x5F13A0, 0x5F7660, 0x4AE100, 0x4ABC30, 0x5E8A30};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F114AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(int64 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
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
inline Arena<kObjWin> objA;          // 0: objeto principal (placeable, fisico, inteligencia, lista de limpieza, evento de dano, ruta, evento de grupo)
inline Arena<kOutWin> outA;          // 1: destino de vector / punto
inline Arena<kMatWin> matA;          // 2: matriz del placeable
inline Arena<kObj2Win> o2A;          // 3: segundo objeto (otro evento de dano, otro evento de grupo, entidad o vehiculo)
inline Arena<kEvWin> evA;            // 4: evento 1 (vtabla propia)
inline Arena<kEvWin> ev2A;           // 5: evento 2 (vtabla propia)
inline Arena<kPairsWin> pairsA;      // 6: ocho pares de tareas de 0x14 bytes
inline Arena<kPoolWin> poolA;        // 7: pool de peds (+0 almacenamiento, +4 estados, +8 capacidad)
inline Arena<kFlagsWin> flagsA;      // 8: estados de los huecos del pool
inline Arena<kStoreWin> storeA;      // 9: almacenamiento del pool (huecos de 0x7C4 bytes desde +0x800)
inline constexpr u32 kRawVeh = 0x00C0FFEEu, kRawPlayer = 0x00ABCD00u;
inline constexpr size_t kStoreBase = 0x800, kPedSize = 0x7C4;

// Estado del caso en curso para los falsos
struct State { const C* c{}; uintptr_t ped{}; };
inline State g;

// Eventos: 11 GetEventType (ranura 1) [evento]; 12 GetEventPriority (ranura 2) [evento]; 13 TakesPriorityOver (ranura 11) [evento, otro]; 20 CPhysical::ApplyMoveForce(CVector) 0x5429F0 [this, x, y, z];
// 30 CWorld::FindPlayerPed 0x56E210 [id]; 33 IsEntityPointerValid 0x533310 [entidad]; 90+k: ranura k no esperada de la vtabla de los eventos.
inline std::array<uintptr_t, 16> evVt{};
template <unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkEvType(void* self, void*) {                                                      // ranura 1 (GetEventType)
    Ev& e = PushEv(11, 1); e.v[0] = Tag(self);
    return Addr(self) == evA.Obj() ? g.c->a[0] : g.c->a[1];
}
inline u32 __fastcall FkEvPriority(void* self, void*) {                                                  // ranura 2 (GetEventPriority)
    Ev& e = PushEv(12, 1); e.v[0] = Tag(self);
    return Addr(self) == evA.Obj() ? g.c->a[0] : g.c->a[1];
}
inline u32 __fastcall FkEvTakes(void* self, void*, void* other) {                                        // ranura 11 (TakesPriorityOver, `ret 4`)
    Ev& e = PushEv(13, 2); e.v[0] = Tag(self); e.v[1] = Tag(other);
    return g.c->a[2];
}
inline void InitEvVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((evVt[I] = uintptr_t(&FkVt<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    evVt[1] = uintptr_t(&FkEvType);
    evVt[2] = uintptr_t(&FkEvPriority);
    evVt[11] = uintptr_t(&FkEvTakes);
}
inline void __fastcall FkMoveForce(void* self, void*, u32 x, u32 y, u32 z) {                              // 0x5429F0 (`ret 0Ch`)
    Ev& e = PushEv(20, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z;
}
inline u32 __cdecl FkFindPlayer(u32 id) {                                                                // 0x56E210
    Ev& e = PushEv(30, 1); e.v[0] = id;
    switch (g.c->a[8]) {
    case 0: return u32(g.ped);
    case 1: return 0u;
    case 2: return u32(storeA.Obj(kStoreBase + 0x10));
    default: return kRawPlayer;
    }
}
inline u32 __cdecl FkIsEntityValid(void* entity) {                                                       // 0x533310
    Ev& e = PushEv(33, 1); e.v[0] = Tag(entity);
    return g.c->a[9];
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["globals"] = json::array({G32(kKillFlagAddr), G32(kKillCountAddr), G32(kExitStateAddr)});
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// Valor del ped del par j en GetTask (el oraculo reproduce la misma regla)
inline u32 PairPed(const C& c, unsigned j, u32 target) {
    if (c.a[0] == 1) return ((c.a[4] >> j) & 1u) ? 0u : (Rand(c.a[3], 2 * j) | 1u);
    if (j == c.a[1] || j == c.a[2]) return target;
    const u32 v = Rand(c.a[3], 2 * j);
    return v == target ? (v ^ 0x00010000u) : v;
}
inline u32 CleanTypeByte(const C& c, unsigned s) { return 1u + Rand(c.a[9], s * 3) % 255u; }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    InitEvVt();
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); outA.Fill(sd++); matA.Fill(sd++); o2A.Fill(sd++); evA.Fill(sd++); ev2A.Fill(sd++); pairsA.Fill(sd++); poolA.Fill(sd++); flagsA.Fill(sd++); storeA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());       // 0
        TagAdd(outA.m.data(), outA.m.size());       // 1
        TagAdd(matA.m.data(), matA.m.size());       // 2
        TagAdd(o2A.m.data(), o2A.m.size());         // 3
        TagAdd(evA.m.data(), evA.m.size());         // 4
        TagAdd(ev2A.m.data(), ev2A.m.size());       // 5
        TagAdd(pairsA.m.data(), pairsA.m.size());   // 6
        TagAdd(poolA.m.data(), poolA.m.size());     // 7
        TagAdd(flagsA.m.data(), flagsA.m.size());   // 8
        TagAdd(storeA.m.data(), storeA.m.size());   // 9
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(objA.Obj()), popBytes = 0;
        switch (fn) {
        case 0:   // CPlaceable::GetRightVector(&out)
            objA.Put32(0x10, c.a[1]);
            objA.Put32(0x14, c.a[0] ? u32(matA.Obj()) : 0u);
            matA.Put32(0, c.a[2]); matA.Put32(4, c.a[3]); matA.Put32(8, c.a[4]);
            outA.Put32(0, c.a[5]); outA.Put32(4, c.a[6]); outA.Put32(8, c.a[7]);
            args = { u32(outA.Obj()) };
            break;
        case 1:   // CPlaceable::GetUpVector(&out)
            objA.Put32(0x10, c.a[7]);
            objA.Put32(0x14, c.a[0] ? u32(matA.Obj()) : 0u);
            matA.Put32(0x20, c.a[1]); matA.Put32(0x24, c.a[2]); matA.Put32(0x28, c.a[3]);
            outA.Put32(0, c.a[4]); outA.Put32(4, c.a[5]); outA.Put32(8, c.a[6]);
            args = { u32(outA.Obj()) };
            break;
        case 2:   // CPhysical::ApplyMoveForce(x, y, z)
            args = { c.a[0], c.a[1], c.a[2] };
            break;
        case 3:   // CPedIntelligence::GetPedFOVRange()
            objA.Put32(0xBC, c.a[0]); objA.Put32(0xC0, c.a[1]);
            break;
        case 4:   // CPedGroups::RegisterKillByPlayer()
            G32(kKillFlagAddr) = c.a[0]; G32(kKillCountAddr) = c.a[1];
            ecx = c.seed;
            break;
        case 5: { // CMissionCleanup::AddEntityToList(handle, tipo)
            const unsigned k = c.a[2];
            for (unsigned sl = 0; sl < kCleanSlots; ++sl) {
                const bool occ = CleanOccupied(c, sl);
                objA.Put8(8 * sl, occ ? CleanTypeByte(c, sl) : 0u);
                objA.Put8(8 * sl + 1, Rand(c.a[9], sl * 3 + 1)); objA.Put8(8 * sl + 2, Rand(c.a[9], sl * 3 + 1) >> 8); objA.Put8(8 * sl + 3, Rand(c.a[9], sl * 3 + 1) >> 16);
                objA.Put32(8 * sl + 4, sl == k ? c.a[8] : Rand(c.a[9], sl * 3 + 2));
            }
            objA.Put32(0x258, (c.a[3] & 0xFFu) | (Rand(c.a[9], 300) & 0xFFFFFF00u));
            args = { c.a[0], c.a[1] };
            break;
        }
        case 6:   // CEventDamage::IsSameEventForAI(otro)
            objA.Put32(0x14, c.a[0]); objA.Put32(0x1C, c.a[1]);
            o2A.Put32(0x14, c.a[2]); o2A.Put32(0x1C, c.a[3]);
            args = { c.a[4] ? u32(objA.Obj()) : u32(o2A.Obj()) };
            break;
        case 7:   // CEntryExitManager::WeAreInInteriorTransition()
            G32(kExitStateAddr) = c.a[0];
            ecx = c.seed;
            break;
        case 8: { // CPointRoute::AddPoint(&punto)
            const int count = int(c.a[0]);
            if (count < 8) for (unsigned k = 0; k < 3; ++k) objA.Put32(4 + 12 * count + 4 * k, c.a[4 + k]);
            objA.Put32(0, c.a[0]);
            outA.Put32(0, c.a[1]); outA.Put32(4, c.a[2]); outA.Put32(8, c.a[3]);
            args = { u32(outA.Obj()) };
            break;
        }
        case 9:   // CPointRoute::RemovePoint(indice)
            for (int j = -5; j <= 10; ++j) for (int k = 0; k < 3; ++k) objA.Put32(4 + 12 * j + 4 * k, RouteWord(c.a[2], j, k));
            objA.Put32(0, c.a[0]);
            args = { c.a[1] };
            break;
        case 10: { // CPedGroupIntelligence::GetTask(ped, &pares)
            const u32 target = c.a[0] == 0 ? u32(o2A.Obj()) : (c.a[0] == 1 ? 0u : kRawVeh);
            for (unsigned j = 0; j < 8; ++j) { pairsA.Put32(0x14 * j, PairPed(c, j, target)); pairsA.Put32(0x14 * j + 4, Rand(c.a[3], 2 * j + 1)); }
            args = { target, u32(pairsA.Obj()) };
            ecx = c.seed;
            break;
        }
        case 11: { // CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent(otro)
            evA.Put32(0, u32(Addr(evVt.data()))); ev2A.Put32(0, u32(Addr(evVt.data())));
            objA.Put32(0x10, u32(evA.Obj()));
            o2A.Put32(0x10, c.a[3] ? u32(evA.Obj()) : u32(ev2A.Obj()));
            args = { u32(o2A.Obj()) };
            break;
        }
        case 12:  // CEvent::TakesPriorityOverBase(otro)
            evA.Put32(0, u32(Addr(evVt.data()))); ev2A.Put32(0, u32(Addr(evVt.data())));
            ecx = u32(evA.Obj());
            args = { c.a[2] ? u32(evA.Obj()) : u32(ev2A.Obj()) };
            break;
        default: { // IsPedPointerValid(ped)
            poolA.Put32(0, u32(storeA.Obj(kStoreBase))); poolA.Put32(4, u32(flagsA.Obj())); poolA.Put32(8, c.a[0]);
            for (unsigned j = 0; j < 4; ++j) flagsA.Put8(j, c.a[4] >> (8 * j));
            flagsA.Put8(4, c.a[10]);
            G32(kPedPoolCell) = u32(poolA.Obj());
            u32 ped = 0;
            if (c.a[1] == 0) {
                const int64 off = int64(kStoreBase) + int64(c.a[2]) * int64(kPedSize) + int64(int32(c.a[3]));
                ped = u32(storeA.Obj(size_t(off)));
                storeA.Put32(off + 0x46C, c.a[5]);
                storeA.Put32(off + 0x58C, c.a[6] == 0 ? u32(o2A.Obj()) : (c.a[6] == 1 ? 0u : kRawVeh));
                storeA.Put32(off + 0xB0, c.a[7] ? 0x00C0DE00u : 0u);
            } else if (c.a[1] == 2) {
                ped = u32(storeA.Obj(kStoreBase)) - 0x20000000u;
            } else if (c.a[1] == 3) {
                ped = u32(storeA.Obj(kStoreBase)) + 0x20000000u;
            }
            g.ped = ped;
            args = { ped };
            popBytes = 4;
            ecx = c.seed;
            break;
        }
        }
        objA.Snap(); outA.Snap(); matA.Snap(); o2A.Snap(); evA.Snap(); ev2A.Snap(); pairsA.Snap(); poolA.Snap(); flagsA.Snap(); storeA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, outA.Changes()); add(2, matA.Changes()); add(3, o2A.Changes()); add(4, evA.Changes()); add(5, ev2A.Changes()); add(6, pairsA.Changes()); add(7, poolA.Changes()); add(8, flagsA.Changes()); add(9, storeA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        // valor devuelto: puntero al destino (funciones 0 y 1), AL (bool), EAX completo (funcion 10: la tarea); las demas no devuelven nada que lea un llamador (la funcion 3 devuelve en ST0)
        if (fn == 0 || fn == 1) obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        else if (fn == 6 || fn == 7 || fn == 8 || fn == 11 || fn == 12 || fn == 13) obs["ret"] = sides[s].reg.eax & 0xFFu;
        else if (fn == 10) obs["ret"] = sides[s].reg.eax;
        else obs["ret"] = nullptr;
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
    if (!std::filesystem::exists(dir / "fn114a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn114a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn114a"); }
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
    for (unsigned f = 0; f < kFunctions; ++f) env.stubs[f] = {};
    env.stubs[2] = {F(0x5429F0, &FkMoveForce)};
    env.stubs[13] = {F(0x533310, &FkIsEntityValid), F(0x56E210, &FkFindPlayer)};
    const char* dllReal[kFunctions] = {
        "fsin/fcos del x87 (TrigSimpleSinCpp/TrigSimpleCosCpp) y la copia de la fila derecha de la matriz",
        "copia de la fila arriba de la matriz o (0, 0, 1)",
        "la propia funcion (reenvio de los tres dwords); el colaborador ApplyMoveForce(CVector) es un falso identico",
        "comparacion ordenada de los alcances (x87 del EXE frente a la comparacion de la DLL)",
        "lectura del byte de mision y del contador",
        "busqueda del primer hueco y escritura de la entrada",
        "comparacion de fuente y arma",
        "lectura del dword 0x96A7CC",
        "escritura de la entrada y del contador con signo",
        "desplazamiento de las entradas con el contador releido",
        "busqueda del par del ped",
        "ranuras 1 y 11 de la vtabla propia del evento (falsos identicos); la logica de prioridad es la de la funcion",
        "ranura 2 de la vtabla propia del evento (falsos identicos) y la comparacion con signo",
        "indice del pool por division con signo y estados; IsEntityPointerValid y FindPlayerPed son falsos identicos"};
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
