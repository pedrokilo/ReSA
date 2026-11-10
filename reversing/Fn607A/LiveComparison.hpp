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

// Banco par a par Fn607A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion del EXE: 0x607A90, el escaneo de entidades de CPedAcquaintanceScanner (ver Fixtures.hpp), con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall). El banco invoca la direccion del EXE con un peaton sintetico
// y hasta 16 candidatos en arenas propias (A: el ped; B: los candidatos; C: el escaner, la instancia falsa de 0x4684F0, la tabla de entidades y las celdas de salida; D: las matrices; guardas de 0x40) y con el
// umbral 0xC0B034 (global real del juego, guardado y restaurado). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair):
//   con hook RH (apagado): CPed::IsAlive 0x5E0170, CVector::Normalise 0x59C910, CAcquaintance::GetAcquaintances 0x608970, CPedType::GetPedFlag 0x608830, CGameLogic::LaRiotsActiveHere 0x441C10,
//   CPedGeometryAnalyser::CanPedTargetPed 0x5F1C40 y CDecisionMakerTypes::GetInstance 0x4684F0 (SEH en el EXE);
//   y, desde el caso COLAB-607A (banco Fn607B), tambien con hook RH (el falso va en la direccion del EXE y en el cuerpo de la DLL): 0x603AF0 (CPedAcquaintanceScanner::CanJoinRiotAgainst),
//   0x6042B0 (CDecisionMakerTypes::HasResponseToAnyEvent) y 0x607560 (CPedAcquaintanceScanner::FindAcquaintance).
// Los falsos tienen comportamiento por llamada: IsAlive, GetAcquaintances y GetPedFlag aplican la regla exacta del EXE sobre la memoria de la arena; los demas devuelven la secuencia del caso (los bool con basura en
// los bits 8..31, porque el EXE solo mira AL); la normalizacion reescribe el vector local con la secuencia y, segun el caso, hace una escritura lateral sobre la matriz del ped (reescribe fwd.x o cambia el puntero de
// matriz a la alternativa) para probar que el vector adelante y la matriz se leen DESPUES de la llamada; 0x607560 escribe en salidaA y salidaB los valores de la secuencia.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos como etiquetas de
// arena), el valor devuelto (EAX completo) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F607ALive {
using namespace F607AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{kEntryAddress};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F607AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
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
    u32 nc{}, ns{};
    unsigned iAlive{}, iNorm{}, iRiot{}, i603{}, i6042{}, iCant{}, i607{};
    u32 pedAddr{}, instAddr{}, altMat{};
};
inline FakeState fk;
inline u32 Rd32(uintptr_t at) { u32 v; memcpy(&v, reinterpret_cast<const void*>(at), 4); return v; }
inline void Wr32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
// Palabra `i` de una secuencia que empieza en w[base] y tiene `len` palabras (0 si el EXE o la DLL piden mas de las previstas: las diferencias salen en los eventos)
inline bool InArena(uintptr_t v) { for (const auto& t : tagTable) if (v >= t.base && v + 4 <= t.base + t.size) return true; return false; }
inline u32 SeqWord(u32 base, u32 len, unsigned i) { return i < len ? fk.c->w[base + i] : 0u; }

// CPed::IsAlive (0x5E0170; thiscall, sin pila, `ret`): [direccion, ECX (etiqueta), dword de +0x530]; devuelve 0 con el estado 0x36 o 0x37 (dword completo) y 1 en otro caso, con basura en los bits 8..31 (el EXE solo mira AL)
inline u32 __fastcall FkIsAlive(void* self, void*) {
    const u32 state = Rd32(uintptr_t(self) + 0x530);
    Ev& e = PushEv(0x5E0170, 2);
    e.v[0] = Tag(self);
    e.v[1] = state;
    const u32 alive = (state != 0x36u && state != 0x37u) ? 1u : 0u;
    return alive | (Mix(fk.c->a[28] + fk.iAlive++) & 0xFFFFFF00u);
}
// CVector::Normalise (0x59C910; thiscall, sin pila, `ret`): [direccion, x, y, z (bits) antes de la llamada]; deja en el vector la terna de la secuencia (3 dwords por llamada) y, segun el caso, escribe en la
// matriz del ped: modo 1 reescribe fwd.x de la matriz ACTUAL (puntero de +0x14 del ped); modo 2 cambia el puntero de +0x14 a la matriz alternativa.
inline void __fastcall FkNormalise(void* self, void*) {
    u32 before[3];
    memcpy(before, self, 12);
    Ev& e = PushEv(0x59C910, 3);
    e.v[0] = before[0]; e.v[1] = before[1]; e.v[2] = before[2];
    const unsigned k = fk.iNorm++;
    u32 now[3];
    for (unsigned comp = 0; comp < 3; ++comp) now[comp] = SeqWord(SeqNorm(fk.nc, fk.ns), 3 * fk.ns, 3 * k + comp);
    memcpy(self, now, 12);
    if (fk.c->a[25] && k == fk.c->a[26]) {
        if (fk.c->a[25] == 1) Wr32(Rd32(fk.pedAddr + 0x14) + 0x10, fk.c->a[27]);
        else Wr32(fk.pedAddr + 0x14, fk.altMat);
    }
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
// CGameLogic::LaRiotsActiveHere (0x441C10; cdecl, sin argumentos): [direccion]; devuelve la secuencia de disturbios (2 palabras por candidato)
inline u32 __cdecl FkRiots() {
    PushEv(0x441C10, 0);
    return SeqWord(SeqRiot(fk.nc, fk.ns), 2 * fk.ns, fk.iRiot++);
}
// 0x603AF0 (cdecl, dos punteros, `ret`): [direccion, ped (etiqueta), otro (etiqueta)]; devuelve la secuencia
inline u32 __cdecl Fk603(u32 ped, u32 other) {
    Ev& e = PushEv(0x603AF0, 2);
    e.v[0] = Tag(uintptr_t(ped));
    e.v[1] = Tag(uintptr_t(other));
    return SeqWord(Seq603(fk.nc, fk.ns), 2 * fk.ns, fk.i603++);
}
// CDecisionMakerTypes::GetInstance (0x4684F0; cdecl, sin argumentos, el EXE le deja los tres argumentos de 0x6042B0 en la pila): [direccion]; devuelve la instancia falsa (en la arena C)
inline u32 __cdecl FkInstance() {
    PushEv(0x4684F0, 0);
    return fk.instAddr;
}
// 0x6042B0 (thiscall, tres dwords de pila, `ret 0Ch`): [direccion, ECX (etiqueta), ped (etiqueta), contador, los cuatro primeros dwords de la lista]; el quinto es un resto de pila del EXE (no se lee).
inline u32 __fastcall Fk6042(void* self, void*, u32 ped, u32 list, u32 count) {
    Ev& e = PushEv(0x6042B0, 7);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(ped));
    e.v[2] = count;
    for (unsigned i = 0; i < 4; ++i) e.v[3 + i] = Rd32(uintptr_t(list) + 4 * i);
    return SeqWord(Seq6042(fk.nc, fk.ns), fk.ns, fk.i6042++);
}
// CPedGeometryAnalyser::CanPedTargetPed (0x5F1C40; cdecl, tres dwords, `ret`): [direccion, ped (etiqueta), objetivo (etiqueta), byte bajo de la bandera]; devuelve la secuencia
inline u32 __cdecl FkCant(u32 ped, u32 target, u32 flag) {
    Ev& e = PushEv(0x5F1C40, 3);
    e.v[0] = Tag(uintptr_t(ped));
    e.v[1] = Tag(uintptr_t(target));
    e.v[2] = flag & 0xFFu;
    return SeqWord(SeqCant(fk.nc, fk.ns), fk.ns, fk.iCant++);
}
// 0x607560 (thiscall, seis dwords de pila, `ret 18h`): [direccion, ECX (etiqueta), ped, prioridad, acumulador, candidato, salidaA, salidaB (punteros como etiquetas)]; escribe en [salidaA] y [salidaB] los dwords
// de la secuencia (si los punteros son direcciones validas) y devuelve el primero (3 dwords por llamada)
inline u32 __fastcall Fk607(void* self, void*, u32 ped, u32 prio, u32 acc, u32 cand, u32 outA, u32 outB) {
    Ev& e = PushEv(0x607560, 7);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(ped));
    e.v[2] = prio;
    e.v[3] = acc;
    e.v[4] = Tag(uintptr_t(cand));
    e.v[5] = Tag(uintptr_t(outA));
    e.v[6] = Tag(uintptr_t(outB));
    const unsigned k = fk.i607++;
    const u32 base = Seq607(fk.nc, fk.ns);
    const u32 ret = SeqWord(base, 3 * fk.ns, 3 * k), wa = SeqWord(base, 3 * fk.ns, 3 * k + 1), wb = SeqWord(base, 3 * fk.ns, 3 * k + 2);
    if (InArena(outA)) Wr32(outA, wa);
    if (InArena(outB)) Wr32(outB, wb);
    return ret;
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

// Prepara las arenas del caso y el estado de los falsos; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const auto& w = c.w;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]); arenaD.Pattern(a[2], a[15]);
    const u32 nc = a[23], ns = a[24];
    const u32 pedAddr = u32(arenaA.Obj(a[0]));
    const u32 pedMat = u32(arenaD.Obj(a[29])), altMat = u32(arenaD.Obj(a[30]));
    // el ped: puntero de matriz (+0x14), banderas (+0x46C), acquaintances (+0x4E0..), estado (+0x530), tipo (+0x598)
    arenaA.Put32(a[0] + 0x14, pedMat);
    arenaA.Put32(a[0] + 0x46C, a[16]);
    for (unsigned k = 0; k < 5; ++k) arenaA.Put32(a[0] + 0x4E0 + 4 * k, a[18 + k]);
    arenaA.Put32(a[0] + 0x530, a[32]);
    arenaA.Put32(a[0] + 0x598, a[17]);
    // las dos matrices: fwd (x, y, z) en +0x10 y pos (x, y, z) en +0x30
    for (unsigned i = 0; i < 3; ++i) {
        arenaD.Put32(a[29] + 0x10 + 4 * i, w[i]);      arenaD.Put32(a[29] + 0x30 + 4 * i, w[3 + i]);
        arenaD.Put32(a[30] + 0x10 + 4 * i, w[6 + i]);  arenaD.Put32(a[30] + 0x30 + 4 * i, w[9 + i]);
    }
    // los candidatos
    for (u32 j = 0; j < nc; ++j) {
        const u32* r = &w[kHdr + kCandWords * j];
        arenaB.Put32(r[0] + 0x46C, r[7]);
        arenaB.Put32(r[0] + 0x530, r[6]);
        arenaB.Put32(r[0] + 0x598, r[8]);
        if (r[4]) {
            arenaB.Put32(r[0] + 0x14, u32(arenaD.Obj(r[5])));
            for (unsigned i = 0; i < 3; ++i) arenaD.Put32(r[5] + 0x30 + 4 * i, r[1 + i]);
        } else {
            arenaB.Put32(r[0] + 0x14, 0);
            for (unsigned i = 0; i < 3; ++i) arenaB.Put32(r[0] + 4 + 4 * i, r[1 + i]);
        }
    }
    // la tabla de entidades y las celdas de salida
    for (u32 i = 0; i < ns; ++i) {
        const u32 e = w[kHdr + kCandWords * nc + i];
        const u32 ptr = e == 0xFFFFFFFFu ? 0u : (e == nc ? pedAddr : u32(arenaB.Obj(w[kHdr + kCandWords * e])));
        arenaC.Put32(a[6] + 4 * i, ptr);
    }
    arenaC.Put32(a[7], a[11]);
    arenaC.Put32(a[8], a[31]);
    // el umbral del producto escalar (global del juego; se restaura tras cada lado)
    *reinterpret_cast<volatile u32*>(kThresholdAddress) = a[5];
    // la llamada: ECX = escaner; pila: ped, prioridad, tabla de entidades, contador, salidaA, salidaB (`ret 18h`: la limpia el callee)
    call.ecx = u32(arenaC.Obj(a[9]));
    call.args = {pedAddr, a[3], u32(arenaC.Obj(a[6])), a[4], u32(arenaC.Obj(a[7])), u32(arenaC.Obj(a[8])), 0, 0};
    call.argCount = 6; call.popBytes = 0;
    // estado de los falsos
    fk = FakeState{};
    fk.c = &c; fk.nc = nc; fk.ns = ns; fk.pedAddr = pedAddr; fk.altMat = altMat; fk.instAddr = u32(arenaC.Obj(a[10]));
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
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        obs["ret"] = sides[s].reg.eax;   // EAX completo: numero de candidatos
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN607A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN607A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn607a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn607a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn607a"); }
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
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x5E0170, &FkIsAlive), F(0x59C910, &FkNormalise), F(0x608970, &FkGetAcq), F(0x608830, &FkGetPedFlag), F(0x441C10, &FkRiots), F(0x5F1C40, &FkCant), F(0x4684F0, &FkInstance),
                    F(0x603AF0, &Fk603),
                    F(0x6042B0, &Fk6042),
                    F(0x607560, &Fk607)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL); la x87 vive en X87Math.cpp (AcquaintanceDeltaCpp, AcquaintanceInViewCpp)";
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
    // Memoria global del juego: solo el umbral del producto escalar (0xC0B034), que la funcion lee; el resto ocurre en las cuatro arenas propias
    regions.clear();
    regions.emplace_back(kThresholdAddress, 4);
    for (auto& v : regionsOf) v.clear();
    regionsOf[0] = {0};
    Run(env, ready, registered, bodies, collaborators);
}
}
