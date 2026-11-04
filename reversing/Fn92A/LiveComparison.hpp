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
#include "Events/EventScanner.h"
#include "Events/Event.h"
#include "Events/EventFireNearby.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventSexyPed.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Fire.h"
#include "Entity/Ped/Ped.h"
#include "Timer.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn92A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funcion 0 (hook S): CPedAcquaintanceScanner::ScanForPedAcquaintances 0x607D80 (thiscall: ECX = escaner, ped, entidades y numero en pila, `ret 0Ch`; void).
// Funcion 1 (hook S): CNearbyFireScanner::ScanForNearbyFires 0x603E70 (thiscall: ECX = escaner, ped en pila, `ret 4`; void). Falsos: GetSimplestActiveTask 0x6819D0, GetActiveTask 0x681720, FindNearestFire 0x538F40
// (ECX = 0xB71F80) y CEventGroup::Add 0x4AB420 (el evento se decodifica con su GetEventType virtual y sus campos definidos). Las tareas son arenas con una vtabla propia (ranura 4 = GetTaskType con el tipo en +0x20):
// CTask::IsGoToTask 0x61A360 y las llamadas virtuales corren de verdad en ambos lados. Los constructores y destructores de CEventFireNearby (0x4B1F10) y CEventPotentialWalkIntoFire (0x4B1E20, hook ACTIVO) tambien.
// Funcion 2 (hook S): CSexyPedScanner::ScanForSexyPeds 0x603BF0 (thiscall: ECX = escaner, ped, entidades y numero en pila, `ret 0Ch`; void). Falsos: GetIsLineOfSightClear 0x56A490 (cdecl, 9 argumentos),
// CInterestingEvents::Add 0x602590, CEventGroup::Add 0x4AB420, CEntity::RegisterReference 0x571B70 y CEntity::CleanUpOldReference 0x571A00 (los pide el constructor/destructor de CEventSexyPed 0x4AEDF0/0x4AEEA0).
// Colaboradores sustituidos por falsos identicos en la direccion del EXE (la DLL los llama por esa direccion): 0x603A30 (puede actuar el escaner; thiscall, ped en pila, `ret 4`, devuelve AL)
// y 0x607A90 (escaneo real; thiscall, 6 argumentos en pila, `ret 0x18`). Registran sus argumentos EN ORDEN (con el contenido de las dos variables locales cuyos punteros recibe 0x607A90).
// NO se sustituyen: el temporizador en linea de la propia funcion (se compara contra el EXE con la hora real CTimer::m_snTimeInMilliseconds 0xB7CB84 y el dword de intervalo 0x8D2358, ambos
// memoria real del juego que se rellena y se restaura tras cada lado).

namespace F92ALayout {
static_assert(offsetof(CPedAcquaintanceScanner, m_timer) == 0 && offsetof(CPedAcquaintanceScanner, m_bScanAllowedScriptPed) == 0xC, "campos del escaner");
static_assert(offsetof(CTaskTimer, m_nStartTime) == 0 && offsetof(CTaskTimer, m_nInterval) == 4 && offsetof(CTaskTimer, m_bStarted) == 8 && offsetof(CTaskTimer, m_bStopped) == 9, "CTaskTimer");
static_assert(sizeof(CPedAcquaintanceScanner) == 0x10, "CPedAcquaintanceScanner mide 0x10");
}

namespace F92ALive {
using namespace F92AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x607D80, 0x603E70, 0x603BF0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F92AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, sin los bytes de hasta dos campos de puntero [lo, hi) (cambian por direccion)
    json Changes(size_t lo1 = 0, size_t hi1 = 0, size_t lo2 = 0, size_t hi2 = 0) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            const bool masked = (hi1 && off >= int64(lo1) && off < int64(hi1)) || (hi2 && off >= int64(lo2) && off < int64(hi2));
            if (m[i] != r[i] && !masked) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kScanWin> scanA;
inline Arena<kPedWin> pedA;
inline Arena<kEntsWin> entsA;
inline Arena<kMatWin> pmatA;
inline Arena<kIntelWin> intelA;
inline Arena<kTaskWin> t1A, t2A, actA;
inline Arena<kFireWin> fireA;
inline Arena<kMatWin> pdataA;
inline Arena<kStatWin> pstatA;
inline std::array<Arena<kPedWin>, kEnts> entA;
inline std::array<Arena<kMatWin>, kEnts> emA;
inline std::array<Arena<kStatWin>, kEnts> esA;
// etiquetas de arenas (orden de TagAdd en CaseRun): 0 escaner, 1 ped, 2 tabla de entidades, 3 matriz del ped, 4 inteligencia, 5/6/7 tareas, 8 fuego, 9 datos de jugador, 10 estadisticas del ped, 11.. entidades, 27.. matrices, 43.. estadisticas

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned simplestCalls{}, losCalls{}; };
inline State g;
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}
inline void Vec3(Ev& e, u32 at, const void* p) { memcpy(&e.v[at], p, 12); }

// Eventos: 1 GetSimplestActiveTask 0x6819D0 [taskmgr]; 2 GetActiveTask 0x681720 [taskmgr]; 3 FindNearestFire 0x538F40 [gestor, pos, pos.xyz, a2, a3]; 4 GetTaskType virtual [tarea];
// 5 CEventGroup::Add 0x4AB420 [grupo, tipo, +4, +8, +0xC, +0xE, +0x10, carga util..., valido]; 6 CInterestingEvents::Add 0x602590 [gestor, tipo, entidad];
// 7 GetIsLineOfSightClear 0x56A490 [desde, hasta, desde.xyz, hasta.xyz, 7 banderas]; 8 CEntity::RegisterReference 0x571B70 [entidad, ref]; 9 CEntity::CleanUpOldReference 0x571A00 [entidad, ref];
// 1 (funcion 0) 0x603A30 [escaner, ped] y 2 (funcion 0) 0x607A90 [escaner, ped, -1, entidades, numero, &a, &b, *a, *b]; 90+k: ranura k no esperada de la vtabla de las tareas.
inline u32 __fastcall FkAllowed(void* self, void*, void* ped) {                                       // 0x603A30 (ret 4)
    Ev& e = PushEv(1, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return g.c->allowed;
}
inline void __fastcall FkScan(void* self, void*, void* ped, u32 id, void* ents, u32 count, int32* a, int32* b) {   // 0x607A90 (ret 0x18)
    Ev& e = PushEv(2, 9); e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = id; e.v[3] = Tag(ents); e.v[4] = count; e.v[5] = TagS(uintptr_t(a)); e.v[6] = TagS(uintptr_t(b)); e.v[7] = u32(*a); e.v[8] = u32(*b);
}
inline std::array<uintptr_t, 16> taskVt{};
template <unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkTaskType(void* self, void*) {                                                 // CTask::GetTaskType (ranura 4, `call [eax+10h]`)
    Ev& e = PushEv(4, 1); e.v[0] = Tag(self);
    return *reinterpret_cast<u32*>(uintptr_t(self) + 0x20);
}
inline void InitTaskVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((taskVt[I] = uintptr_t(&FkVt<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[4] = uintptr_t(&FkTaskType);
}
inline u32 __fastcall FkSimplest(void* self, void*) {                                                 // 0x6819D0
    Ev& e = PushEv(1, 1); e.v[0] = Tag(self);
    const bool first = g.simplestCalls++ == 0;
    if (first) return g.c->t1 ? u32(t1A.Obj()) : 0u;
    return g.c->t2 ? u32(t2A.Obj()) : 0u;
}
inline u32 __fastcall FkActive(void* self, void*) {                                                   // 0x681720
    Ev& e = PushEv(2, 1); e.v[0] = Tag(self);
    return g.c->act ? u32(actA.Obj()) : 0u;
}
inline u32 __fastcall FkFind(void* self, void*, float* pos, u32 a2, u32 a3) {                         // 0x538F40 (ret 0xC)
    Ev& e = PushEv(3, 7); e.v[0] = Tag(self); e.v[1] = Tag(pos); Vec3(e, 2, pos); e.v[5] = a2; e.v[6] = a3;
    return g.c->fire ? u32(fireA.Obj()) : 0u;
}
inline u32 __fastcall FkAdd(void* self, void*, void* ev, u32 flag) {                                  // 0x4AB420 (ret 8)
    const auto* p = static_cast<const uint8*>(ev);
    const auto type = static_cast<CEvent*>(ev)->GetEventType();
    Ev& e = PushEv(5, 0); e.v[0] = Tag(self); e.v[1] = u32(type);
    e.v[2] = *reinterpret_cast<const u32*>(p + 4); e.v[3] = p[8]; e.v[4] = p[0xC]; e.v[5] = *reinterpret_cast<const uint16*>(p + 0xE); e.v[6] = *reinterpret_cast<const uint16*>(p + 0x10);
    u32 k = 7;
    if (type == EVENT_FIRE_NEARBY) { for (u32 i = 0; i < 3; ++i) e.v[k++] = *reinterpret_cast<const u32*>(p + 0x14 + 4 * i); }
    else if (type == EVENT_POTENTIAL_WALK_INTO_FIRE) { for (u32 i = 0; i < 6; ++i) e.v[k++] = *reinterpret_cast<const u32*>(p + 0x14 + 4 * i); }
    else if (type == EVENT_SEXY_PED) { e.v[k++] = Tag(*reinterpret_cast<const uintptr_t*>(p + 0x14)); }
    e.v[k++] = flag;
    e.len = k;
    return 0;
}
inline void __fastcall FkInteresting(void* self, void*, u32 type, void* entity) {                     // 0x602590 (ret 8)
    Ev& e = PushEv(6, 3); e.v[0] = Tag(self); e.v[1] = type; e.v[2] = Tag(entity);
}
inline u32 __cdecl FkLos(float* from, float* to, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9) {   // 0x56A490 (cdecl, 9 argumentos)
    Ev& e = PushEv(7, 14); e.v[0] = TagS(uintptr_t(from)); e.v[1] = TagS(uintptr_t(to)); Vec3(e, 2, from); Vec3(e, 5, to);
    e.v[8] = a3; e.v[9] = a4; e.v[10] = a5; e.v[11] = a6; e.v[12] = a7; e.v[13] = a8; (void)a9;
    e.len = 15; e.v[14] = a9;
    const u32 call = g.losCalls++;
    return ((g.c->losJunk << 8) | ((call < 32 ? (g.c->los >> call) : 0u) & 1u));
}
inline void __fastcall FkRegister(void* self, void*, void* ref) {                                      // 0x571B70 (ret 4)
    Ev& e = PushEv(8, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(ref));
}
inline void __fastcall FkCleanUp(void* self, void*, void* ref) {                                       // 0x571A00 (ret 4)
    Ev& e = PushEv(9, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(ref));
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


inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }

// Rellena el ped (posicion en la matriz o en el placement) y lo comun a las funciones 1 y 2
inline void SetupPed(const C& c) {
    pedA.Put32(0x14, c.pmat ? u32(pmatA.Obj()) : 0u);
    if (c.pmat) { Put3(pmatA, 0x30, c.ppos); Put3(pedA, 4, c.palt); }
    else { Put3(pedA, 4, c.ppos); Put3(pmatA, 0x30, c.palt); }
    pedA.Put32(0x47C, u32(intelA.Obj()));
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
        scanA.Fill(sd++); pedA.Fill(sd++); entsA.Fill(sd++); pmatA.Fill(sd++); intelA.Fill(sd++); t1A.Fill(sd++); t2A.Fill(sd++); actA.Fill(sd++); fireA.Fill(sd++); pdataA.Fill(sd++); pstatA.Fill(sd++);
        for (unsigned i = 0; i < kEnts; ++i) { entA[i].Fill(sd++); emA[i].Fill(sd++); esA[i].Fill(sd++); }
        TagClear();
        TagAdd(scanA.m.data(), scanA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(entsA.m.data(), entsA.m.size());     // 2
        TagAdd(pmatA.m.data(), pmatA.m.size());     // 3
        TagAdd(intelA.m.data(), intelA.m.size());   // 4
        TagAdd(t1A.m.data(), t1A.m.size());         // 5
        TagAdd(t2A.m.data(), t2A.m.size());         // 6
        TagAdd(actA.m.data(), actA.m.size());       // 7
        TagAdd(fireA.m.data(), fireA.m.size());     // 8
        TagAdd(pdataA.m.data(), pdataA.m.size());   // 9
        TagAdd(pstatA.m.data(), pstatA.m.size());   // 10
        for (unsigned i = 0; i < kEnts; ++i) TagAdd(entA[i].m.data(), entA[i].m.size());   // 11..26
        for (unsigned i = 0; i < kEnts; ++i) TagAdd(emA[i].m.data(), emA[i].m.size());     // 27..42
        for (unsigned i = 0; i < kEnts; ++i) TagAdd(esA[i].m.data(), esA[i].m.size());     // 43..58
        for (unsigned k = 0; k < 4; ++k) scanA.Put32(4 * k, c.scan[k]);
        G32(kTimeAddr) = c.now;
        if (fn == 0) G32(kIntervalAddr) = c.gint;
        std::vector<u32> args;
        if (fn == 0) args = { u32(pedA.Obj()), u32(entsA.Obj()), c.count };
        if (fn == 1) {
            SetupPed(c);
            Put3(fireA, 4, c.fpos); fireA.Put32(0x1C, c.fsize);
            for (auto* ta : {&t1A, &t2A, &actA}) ta->Put32(0, u32(Addr(taskVt.data())));
            t1A.Put32(8, c.t1move); t1A.Put32(0x20, c.t1type); t2A.Put32(0x20, c.t2type); actA.Put32(0x20, c.acttype);
            args = { u32(pedA.Obj()) };
        }
        if (fn == 2) {
            SetupPed(c);
            Put3(pmatA, 0x10, c.fwd);
            pedA.Put32(0x480, c.pdata ? u32(pdataA.Obj()) : 0u);
            pedA.Put8(0x484, c.created);
            pedA.Put32(0x46C, c.pflags);
            pedA.Put32(0x59C, u32(pstatA.Obj()));
            pstatA.Put8(0x27, c.psex);
            for (unsigned i = 0; i < kEnts; ++i) {
                entsA.Put32(4 * i, c.ePresent[i] ? u32(entA[i].Obj()) : 0u);
                entA[i].Put32(0x14, c.eHasMat[i] ? u32(emA[i].Obj()) : 0u);
                if (c.eHasMat[i]) { Put3(emA[i], 0x30, c.ePos[i]); Put3(entA[i], 4, c.eAlt[i]); }
                else { Put3(entA[i], 4, c.ePos[i]); Put3(emA[i], 0x30, c.eAlt[i]); }
                entA[i].Put32(0x46C, c.eFlags[i]);
                entA[i].Put32(0x598, c.eType[i]);
                entA[i].Put32(0x59C, u32(esA[i].Obj()));
                esA[i].Put8(0x27, c.eSex[i]);
            }
            args = { u32(pedA.Obj()), u32(entsA.Obj()), c.count };
        }
        scanA.Snap(); pedA.Snap(); entsA.Snap(); pmatA.Snap(); intelA.Snap(); t1A.Snap(); t2A.Snap(); actA.Snap(); fireA.Snap(); pdataA.Snap(); pstatA.Snap();
        for (unsigned i = 0; i < kEnts; ++i) { entA[i].Snap(); emA[i].Snap(); esA[i].Snap(); }
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(scanA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, scanA.Changes()); add(1, pedA.Changes()); add(2, entsA.Changes()); add(3, pmatA.Changes()); add(4, intelA.Changes()); add(5, t1A.Changes()); add(6, t2A.Changes());
        add(7, actA.Changes()); add(8, fireA.Changes()); add(9, pdataA.Changes()); add(10, pstatA.Changes());
        for (unsigned i = 0; i < kEnts; ++i) { add(11 + i, entA[i].Changes()); add(27 + i, emA[i].Changes()); add(43 + i, esA[i].Changes()); }
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
    if (!std::filesystem::exists(dir / "fn92a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn92a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn92a"); }
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
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x603A30, &FkAllowed), F(0x607A90, &FkScan)};
    env.stubs[1] = {F(0x6819D0, &FkSimplest), F(0x681720, &FkActive), F(0x538F40, &FkFind), F(0x4AB420, &FkAdd)};
    env.stubs[2] = {F(0x56A490, &FkLos), F(0x602590, &FkInteresting), F(0x4AB420, &FkAdd), F(0x571B70, &FkRegister), F(0x571A00, &FkCleanUp)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"temporizador en linea del escaner (CTaskTimer: arrancado, detenido, inicio, intervalo, hora actual del juego) y la tabla de intervalo del EXE 0x8D2358"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
