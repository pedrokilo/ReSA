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
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Events/EventScanner.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Core.hpp"

// Banco par a par Fn607B (COLAB-607A) dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Tres funciones del EXE que 0x607A90 (CPedAcquaintanceScanner::ScanEntities) llama, 16384 pares EXE/DLL cada una (ver Fixtures.hpp), hook S (RH_ScopedInstall) en las tres:
//   0 = 0x603AF0 CPedAcquaintanceScanner::CanJoinRiotAgainst (cdecl, `ret`);  1 = 0x6042B0 CDecisionMakerTypes::HasResponseToAnyEvent (thiscall, `ret 0Ch`);  2 = 0x607560 CPedAcquaintanceScanner::FindAcquaintance (thiscall, `ret 18h`).
// El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A: el ped; B: el otro ped / candidato; C: escaner, inteligencia, lista de eventos, grupos y celdas de salida; D: el
// CDecisionMakerTypes de la funcion 1; guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair):
//   funcion 0: CPed::IsPlayer 0x5DF8F0, CPedIntelligence::Respects 0x601C90, CPedGroups::GetPedsGroup 0x5F7E80 y CPedGroupMembership::GetLeader 0x5F69A0 (con hook RH, apagado); IsPedTypeGang 0x5FE9C0 corre REAL
//     (en el EXE con el hook apagado; en la DLL esta expandido en linea);
//   funcion 1: ninguno;
//   funcion 2: CPedIntelligence::CanSeeEntityWithLights 0x605550 (float en ST0), CAcquaintance::GetAcquaintances 0x608970, CPedType::GetPedFlag 0x608830, CGameLogic::LaRiotsActiveHere 0x441C10 y 0x603AF0 (CanJoinRiotAgainst,
//     con hook RH desde este caso); 0x606BA0 (sin hook ni funcion propia: el falso va solo en la direccion del EXE, la DLL la llama por esa direccion).
// GetAcquaintances y GetPedFlag aplican la regla exacta del EXE sobre la memoria de la arena; los demas devuelven la secuencia del caso (los bool con basura en los bits 8..31, porque el EXE solo mira AL).
// Se observan: bytes cambiados de las arenas, eventos de los falsos (con sus argumentos como etiquetas de arena), valor devuelto (AL en las funciones bool; EAX completo en la 2), el estado x87 (siempre vacio) y el
// hash de las regiones globales del juego (funcion 2: CTimer, 0x8D235C/0x8D2360). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F607BLive {
using namespace F607BFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{kEntryAddresses[0], kEntryAddresses[1], kEntryAddresses[2]};
inline constexpr uintptr_t kTimerAddress = 0xB7CB84, kIntervalsAddress = 0x8D235C;   // CTimer::m_snTimeInMilliseconds; int de 0x8D235C y de 0x8D2360 (contiguos)

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F607BFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWinA> arenaA;
inline Arena<kWinB> arenaB;
inline Arena<kWinC> arenaC;
inline Arena<kWinD> arenaD;

// ================================================================================================ falsos de los colaboradores
inline DWORD gBenchThread{};
// Estado del caso en curso que consultan los falsos
struct FakeState {
    const C* c{};
    unsigned iPlayer{}, iLeader{}, iRespects{}, iGroup{}, iRiot{}, i603{}, i606{}, iVis{};
    u32 pedAddr{}, otherAddr{}, groupAddr{}, leaderAddr{};
};
inline FakeState fk;
inline u32 Rd32(uintptr_t at) { u32 v; memcpy(&v, reinterpret_cast<const void*>(at), 4); return v; }
inline u32 SeqWord(u32 base, u32 len, unsigned i) { return i < len ? fk.c->w[base + i] : 0u; }

// ---- funcion 0
// CPed::IsPlayer (0x5DF8F0; thiscall, sin pila, `ret`): [direccion, ECX (etiqueta)]; devuelve la secuencia (4 por caso)
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    return SeqWord(0, 4, fk.iPlayer++);
}
// CPedIntelligence::Respects (0x601C90; thiscall, un dword de pila, `ret 4`): [direccion, ECX (etiqueta), ped (etiqueta)]; devuelve w[7]
inline u32 __fastcall FkRespects(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x601C90, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(ped));
    return SeqWord(7, 1, fk.iRespects++);
}
// CPedGroups::GetPedsGroup (0x5F7E80; cdecl, un dword, `ret`): [direccion, ped (etiqueta)]; devuelve el grupo de la arena C si w[8] vale 1 y nulo si no
inline u32 __cdecl FkGetGroup(u32 ped) {
    Ev& e = PushEv(0x5F7E80, 1);
    e.v[0] = Tag(uintptr_t(ped));
    return SeqWord(8, 1, fk.iGroup++) == 1u ? fk.groupAddr : 0u;
}
// CPedGroupMembership::GetLeader (0x5F69A0; thiscall, sin pila, `ret`): [direccion, ECX (etiqueta)]; codigo por llamada (w[4..6]): 0 nulo, 1 el ped, 2 el otro, 3 el lider de C, 4 el valor crudo a[16]
inline u32 __fastcall FkGetLeader(void* self, void*) {
    Ev& e = PushEv(0x5F69A0, 1);
    e.v[0] = Tag(self);
    switch (SeqWord(4, 3, fk.iLeader++)) {
    case 1: return fk.pedAddr;
    case 2: return fk.otherAddr;
    case 3: return fk.leaderAddr;
    case 4: return fk.c->a[16];
    default: return 0u;
    }
}

// ---- funcion 2
// CPedIntelligence::CanSeeEntityWithLights (0x605550; thiscall, dos dwords de pila, `ret 8`; float en ST0): [direccion, ECX (etiqueta), entidad (etiqueta), segundo argumento]; devuelve w[0] como float
inline float __fastcall FkVis(void* self, void*, u32 entity, u32 flag) {
    Ev& e = PushEv(0x605550, 3);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(entity));
    e.v[2] = flag;
    ++fk.iVis;
    return std::bit_cast<float>(fk.c->w[0]);
}
// CAcquaintance::GetAcquaintances (0x608970; thiscall, un dword de pila, `ret 4`): [direccion, ECX (etiqueta), id]; devuelve el dword [ECX + id*4] (la regla del EXE)
inline u32 __fastcall FkGetAcq(void* self, void*, u32 id) {
    Ev& e = PushEv(0x608970, 2);
    e.v[0] = Tag(self);
    e.v[1] = id;
    return Rd32(uintptr_t(self) + 4u * id);
}
// CPedType::GetPedFlag (0x608830; cdecl, un dword, `ret`): [direccion, tipo]; 1 << (tipo & 31) si tipo < 32 (con signo), 0 si no
inline u32 __cdecl FkGetPedFlag(u32 type) {
    Ev& e = PushEv(0x608830, 1);
    e.v[0] = type;
    return i32(type) < 32 ? (1u << (type & 31)) : 0u;
}
// CGameLogic::LaRiotsActiveHere (0x441C10; cdecl, sin argumentos): [direccion]; devuelve la secuencia w[1..5]
inline u32 __cdecl FkRiots() {
    PushEv(0x441C10, 0);
    return SeqWord(1, 5, fk.iRiot++);
}
// 0x603AF0 (cdecl, dos punteros, `ret`): [direccion, ped (etiqueta), otro (etiqueta)]; devuelve la secuencia w[6..10]
inline u32 __cdecl Fk603(u32 ped, u32 other) {
    Ev& e = PushEv(0x603AF0, 2);
    e.v[0] = Tag(uintptr_t(ped));
    e.v[1] = Tag(uintptr_t(other));
    return SeqWord(6, 5, fk.i603++);
}
// 0x606BA0 (thiscall, tres dwords de pila, `ret 0Ch`): [direccion, ECX (etiqueta), ped, id, entidad (etiquetas)]; devuelve la secuencia w[11..15]
inline u32 __fastcall Fk606(void* self, void*, u32 ped, u32 id, u32 entity) {
    Ev& e = PushEv(0x606BA0, 4);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(ped));
    e.v[2] = id;
    e.v[3] = Tag(uintptr_t(entity));
    return SeqWord(11, 5, fk.i606++);
}

// ================================================================================================ cierre de un lado
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

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

// Prepara las arenas del caso y el estado de los falsos; devuelve la llamada. El orden de las escrituras es el mismo que el del oraculo (fixture_oracle.py).
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const auto& w = c.w;
    arenaA.Pattern(a[1], a[2]); arenaB.Pattern(a[1], a[3]); arenaC.Pattern(a[1], a[4]);
    if (c.fn == 1) arenaD.Pattern(a[1], a[5]);
    fk = FakeState{};
    fk.c = &c;
    const u32 pedAddr = u32(arenaA.Obj(a[6]));
    fk.pedAddr = pedAddr;
    if (c.fn == 0) {
        const u32 otherAddr = u32(arenaB.Obj(a[7]));
        arenaA.Put32(a[6] + 0x598, a[8]);
        arenaB.Put32(a[7] + 0x598, a[9]);
        arenaA.Put8(a[6] + 0x484, a[10]);
        arenaA.Put32(a[6] + 0x47C, a[11] == 0 ? u32(arenaC.Obj(a[12])) : a[13]);
        fk.otherAddr = otherAddr; fk.groupAddr = u32(arenaC.Obj(a[14])); fk.leaderAddr = u32(arenaC.Obj(a[15]));
        call.args = {pedAddr, otherAddr, 0, 0, 0, 0, 0, 0};
        call.argCount = 2; call.popBytes = 8;   // cdecl: limpia el llamador
    } else if (c.fn == 1) {
        arenaA.Put32(a[6] + 0x47C, u32(arenaC.Obj(a[7])));
        arenaA.Put8(a[6] + 0x484, a[11]);
        arenaC.Put32(a[7] + 0xB4, a[10]);
        const u32 n = a[13];
        for (u32 j = 0; j < n; ++j) {
            const u32* r = &w[6 * j];
            arenaC.Put32(a[8] + 4 * j, r[0]);
            arenaD.Put32(size_t(int64(a[9]) + kEventTable + 4 * int64(i32(r[0]))), r[1]);
            if (r[3] != 2) {
                for (u32 k = 0; k < 6; ++k) arenaD.Put32(r[2] + 4 * k, 200u);
                if (r[3] == 1) arenaD.Put32(r[2] + 4 * r[4], r[5]);
            }
        }
        call.ecx = u32(arenaD.Obj(a[9]));
        call.args = {pedAddr, u32(arenaC.Obj(a[8])), a[12], 0, 0, 0, 0, 0};
        call.argCount = 3; call.popBytes = 0;   // `ret 0Ch`: la limpia el callee
    } else {
        const u32 candAddr = u32(arenaB.Obj(a[7]));
        arenaA.Put32(a[6] + 0x47C, u32(arenaC.Obj(a[11])));
        for (unsigned k = 0; k < 5; ++k) arenaA.Put32(a[6] + 0x4E0 + 4 * k, a[15 + k]);
        arenaB.Put32(a[7] + 0x598, a[14]);
        arenaC.Put32(a[8] + 0, a[25]);
        arenaC.Put32(a[8] + 4, a[26]);
        arenaC.Put8(a[8] + 8, a[27] & 0xFFu);
        arenaC.Put8(a[8] + 9, (a[27] >> 8) & 0xFFu);
        arenaC.Put32(a[9], a[23]);
        arenaC.Put32(a[10], a[24]);
        *reinterpret_cast<volatile u32*>(kTimerAddress) = a[20];
        *reinterpret_cast<volatile u32*>(kIntervalsAddress) = a[21];
        *reinterpret_cast<volatile u32*>(kIntervalsAddress + 4) = a[22];
        fk.otherAddr = candAddr;
        call.ecx = u32(arenaC.Obj(a[8]));
        call.args = {pedAddr, a[12], a[13], candAddr, u32(arenaC.Obj(a[9])), u32(arenaC.Obj(a[10])), 0, 0};
        call.argCount = 6; call.popBytes = 0;   // `ret 18h`
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); if (fn == 1) arenaD.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", fn == 1 ? arenaD.Changes() : json::array()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        obs["ret"] = fn == 2 ? sides[s].reg.eax : (sides[s].reg.eax & 0xFFu);   // EAX completo en la funcion 2; AL (bool) en las otras dos
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN607B_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN607B_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
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
    if (!std::filesystem::exists(dir / "fn607b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn607b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn607b"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH ni funcion propia en la DLL: la DLL lo llama por la direccion del EXE; el falso va solo ahi. Se exige que de verdad no tenga hook.
    auto N = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook RH (apagado durante el banco): el EXE corre sus bytes originales y la DLL su cuerpo (aqui, IsPedTypeGang esta expandida en linea: no hay llamada)
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    env.stubs[0] = {F(0x5DF8F0, &FkIsPlayer), F(0x601C90, &FkRespects), F(0x5F7E80, &FkGetGroup), F(0x5F69A0, &FkGetLeader)};
    env.reals[0] = {Real(0x5FE9C0)};
    env.stubs[1] = {};
    env.stubs[2] = {F(0x605550, &FkVis), F(0x608970, &FkGetAcq), F(0x608830, &FkGetPedFlag), F(0x441C10, &FkRiots), F(0x603AF0, &Fk603), N(0x606BA0, &Fk606)};
    const char* dllReal[kFunctions]{
        "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL); IsPedTypeGang (0x5FE9C0) real: en el EXE con el hook apagado, en la DLL expandida en linea",
        "toda la funcion (no tiene colaboradores)",
        "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL; 0x606BA0 solo en la direccion del EXE, que la DLL llama por esa direccion)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Memoria global del juego que la funcion 2 lee: CTimer::m_snTimeInMilliseconds (0xB7CB84) y los dos enteros de 0x8D235C/0x8D2360; el resto ocurre en las cuatro arenas propias
    regions.clear();
    regions.emplace_back(kTimerAddress, 4);
    regions.emplace_back(kIntervalsAddress, 8);
    for (auto& v : regionsOf) v.clear();
    regionsOf[2] = {0, 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
