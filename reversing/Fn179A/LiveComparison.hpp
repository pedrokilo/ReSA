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
#include "Core.hpp"

// Banco par a par Fn179A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Rt / Pair): CEvent::operator new 0x4B5620,
// CEvent::operator delete 0x4B5630, CEventPlayerCommandToGroup constructor 0x4B23D0 y cuerpo del destructor 0x4B2470, CAnimManager::RemoveAnimBlockRef 0x4D3FD0, CPedGroups::RemoveGroup 0x5FB870, ~CPed 0x5E8620,
// CVector::FromMultiply3x3 0x59C6D0, CTask::operator delete 0x61A5B0, cuerpo de CTaskSimpleGoToPointFine 0x65EEF0 y cuerpos de los asignadores 0x69BBF0, 0x69C2D0, 0x69C780 y 0x69CB60 (todos con hook, apagado durante el
// banco); los cuerpos 0x609300 y 0x69DB40 son OTRAS funciones probadas del lote (Rt). Solo en la direccion del EXE (la DLL los llama por ella): operator new[] 0x821195, delete[] 0x8213AE y operator delete 0x8214BD.
// Memoria global del juego tocada (restaurada tras cada lado): puntero del pool de asignadores 0xB744BC, puntero de los grupos de animacion 0xB4EA34, tablas de desplazamientos de la formacion 0xC196E8 / 0xC19728,
// camara 0xB6F028 y palabra de la region del tiempo 0xC81314.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; sin valor en las funciones void) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F179ALive {
using namespace F179AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F179AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(i32 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(i32 off, u32 v) { m[kGuard + off] = uint8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gMem{};                   // memoria que devuelve el falso de CEvent::operator new
inline std::array<u32, 2> gNewArr{}; // lo que devuelve operator new[] falso, en orden
inline unsigned gNewArrN{};
inline u32 gSelf{}, gAlt{}, gSwapN{}, gRemoveCalls{};   // ~CPlayerPed: `this`, datos alternativos y llamada de RemoveAnimBlockRef tras la que cambia [this + 0x480]
inline DWORD gBenchThread{};

// operator delete escalar / CEvent::operator delete / CTask::operator delete (cdecl, un argumento VALIDO): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada. En otros hilos no hace nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    if (GetCurrentThreadId() != gBenchThread) return;
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// delete[] del CRT (0x8213AE; cdecl, un argumento CRUDO): evento [direccion, argumento (etiqueta o crudo)]. No libera nada. En otros hilos no hace nada.
inline void __cdecl FkDeleteArray(void* p) {
    if (GetCurrentThreadId() != gBenchThread) return;
    Ev& e = PushEv(0x8213AE, 1);
    e.v[0] = Tag(p);
}
// operator new[] del EXE (0x821195; cdecl, un tamano): en el hilo del banco evento [direccion, tamano] y devuelve el siguiente de gNewArr; en otros hilos llama al operator new real (0x82119A, al que salta 0x821195).
inline void* __cdecl FkNewArr(u32 size) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<void*(__cdecl*)(u32)>(0x82119A)(size);
    Ev& e = PushEv(0x821195, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gNewArr[gNewArrN < 2 ? gNewArrN++ : 1]));
}
// CEvent::operator new (0x4B5620; cdecl, un argumento): evento [direccion, tamano]; devuelve gMem (memoria de B o nulo).
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gMem;
}
// Cuerpo de destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con dos dwords de pila (CEventPlayerCommandToGroup 0x4B23D0; `ret 8`): evento [direccion, ECX (etiqueta), primer argumento (etiqueta), segundo argumento (dword completo)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor2(void* self, void*, u32 a1, u32 a2) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(a1)); e.v[2] = a2;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CAnimManager::RemoveAnimBlockRef (0x4D3FD0; cdecl, un indice): evento [direccion, indice]; en la llamada gSwapN cambia [this + 0x480] de ~CPlayerPed por los datos alternativos (el EXE y la DLL deben releerlo).
inline void __cdecl FkRemoveAnimRef(u32 index) {
    Ev& e = PushEv(0x4D3FD0, 1);
    e.v[0] = index;
    if (++gRemoveCalls == gSwapN) memcpy(reinterpret_cast<void*>(gSelf + 0x480), &gAlt, 4);
}
// CPedGroups::RemoveGroup (0x5FB870; cdecl, un entero): evento [direccion, argumento].
inline void __cdecl FkRemoveGroup(u32 group) {
    Ev& e = PushEv(0x5FB870, 1);
    e.v[0] = group;
}
// CVector::FromMultiply3x3 (0x59C6D0; thiscall, matriz y vector por puntero, `ret 8`): evento [direccion, ECX (etiqueta), matriz (etiqueta), tres dwords del vector]; escribe en [ECX] (x ^ 0x11111111, y ^ 0x22222222, z ^ 0x33333333); devuelve ECX.
inline void* __fastcall FkMult3x3(void* self, void*, void* matrix, const u32* vec) {
    Ev& e = PushEv(0x59C6D0, 5);
    e.v[0] = Tag(self); e.v[1] = Tag(matrix); e.v[2] = vec[0]; e.v[3] = vec[1]; e.v[4] = vec[2];
    const u32 out[3]{vec[0] ^ 0x11111111u, vec[1] ^ 0x22222222u, vec[2] ^ 0x33333333u};
    memcpy(self, out, 12);
    return self;
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
    bool floatRet{};
};

// EAX que devuelve el falso de un constructor segun el selector (0 la memoria, 1 la memoria + 4, 2 cero, 3 un valor crudo)
inline u32 CtorEax(u32 selector, u32 memory, u32 raw) {
    switch (selector) {
    case 0: return memory;
    case 1: return memory + 4;
    case 2: return 0;
    default: return raw;
    }
}
inline u32 Ptr(u32 mode, const u32 arena, u32 raw) { return mode == 0 ? 0u : (mode == 1 ? arena : raw); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[60]); arenaB.Pattern(a[2], a[61]); arenaC.Pattern(a[2], a[62]); arenaD.Pattern(a[2], a[63]);
    gRet = 0; gDtorWrite = 0; gMem = 0; gNewArr = {}; gNewArrN = 0; gSelf = 0; gAlt = 0; gSwapN = 0; gRemoveCalls = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    const auto k = kKind[c.fn];
    if (k != kClone && k != kWRegion) arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (k) {
    case kConst:
    case kWCtor:
        break;
    case kFlush:
    case kPSdd:
        arenaA.Put32(o + 0, Ptr(a[4], u32(arenaB.Obj(a[5])), a[6])); arenaA.Put32(o + 4, Ptr(a[7], u32(arenaC.Obj(a[8])), a[9]));
        arenaA.Put32(o + 8, a[10]); arenaA.Put32(o + 0xC, a[11]); arenaA.Put8(o + 0x10, a[12]);
        if (k == kPSdd) { call.args[0] = a[13]; call.argCount = 1; call.popBytes = 0; }
        break;
    case kPCtor:
        gNewArr = {Ptr(a[4], u32(arenaB.Obj(a[5])), a[6]), Ptr(a[7], u32(arenaC.Obj(a[8])), a[9])};
        call.args[0] = a[10]; call.args[1] = a[11]; call.argCount = 2; call.popBytes = 0;
        break;
    case kPNew: {
        arenaA.Put32(o + 0, a[4]); arenaA.Put32(o + 4, u32(arenaD.Obj(a[5]))); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]); arenaA.Put8(o + 0x10, a[8]);
        for (u32 i = 0; i < a[6]; ++i) {
            const u32 counter = a[11] == 0 ? 0u : (a[11] == 1 ? 0x7Fu : Rand(a[63], 1000 + i) & 0x7Fu);
            bool free = false;
            switch (a[9]) {
            case 0: free = false; break;
            case 1: free = i == a[10]; break;
            case 2: free = Rand(a[63], 5000 + i) % 10 == 0; break;
            case 3: free = (Rand(a[63], 6000 + i) & 1) != 0; break;
            default: free = true; break;
            }
            arenaD.Put8(i32(a[5] + i), counter | (free ? 0x80u : 0u));
        }
        break;
    }
    case kClone:
        arenaA.Put32(o + 0x18, a[8]);
        gMem = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        gRet = CtorEax(a[7], gMem, a[6]); gDtorWrite = a[5];
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kEvBody:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kPlayerDtor: {
        arenaA.Put32(o + 0x478, a[5]); arenaA.Put32(o + 0x480, u32(arenaB.Obj(a[4])));
        arenaB.Put32(a[4] + 0x48, a[6]); arenaB.Put32(a[4] + 0x4C, a[7]); arenaB.Put32(a[4] + 0x38, a[8]);
        const i32 g1 = i32(a[6]), g2 = i32(a[7]);
        if (g1 != 0) arenaD.Put32(i32(a[9]) + g1 * 20, a[10]);
        if (g2 != 0) arenaD.Put32(i32(a[9]) + g2 * 20, a[11]);
        arenaB.Put32(a[13] + 0x48, a[15]); arenaB.Put32(a[13] + 0x4C, a[16]); arenaB.Put32(a[13] + 0x38, a[17]);
        if (i32(a[15]) != 0) arenaD.Put32(i32(a[9]) + i32(a[15]) * 20, a[18]);
        if (i32(a[16]) != 0) arenaD.Put32(i32(a[9]) + i32(a[16]) * 20, a[19]);
        *reinterpret_cast<u32*>(kAnimGroups) = u32(arenaD.Obj(a[9]));
        gDtorWrite = a[14]; gSelf = thisA; gAlt = u32(arenaB.Obj(a[13])); gSwapN = a[12];
        break;
    }
    case kGangOff: {
        arenaA.Put32(o + 0x10, u32(arenaB.Obj(a[4])));
        for (u32 i = 0; i < 3; ++i) { arenaA.Put32(o + 0x14 + 4 * i, a[14 + i]); arenaA.Put32(o + 0x20 + 4 * i, a[17 + i]); }
        arenaA.Put8(o + 0x3C, a[20]); arenaA.Put8(o + 0x3D, a[21]);
        for (u32 i = 0; i < 3; ++i) arenaB.Put32(a[4] + 4 + 4 * i, a[8 + i]);
        arenaB.Put32(a[4] + 0x14, a[5] ? u32(arenaC.Obj(a[6])) : 0u);
        arenaB.Put32(a[4] + 0x534, a[7]);
        if (a[5]) for (u32 i = 0; i < 3; ++i) arenaC.Put32(a[6] + 0x30 + 4 * i, a[11 + i]);
        for (u32 i = 0; i < 8; ++i) { *reinterpret_cast<u32*>(kOffsetsTable + 4 * i) = a[24 + i]; *reinterpret_cast<u32*>(kMovingTable + 4 * i) = a[32 + i]; }
        call.args[0] = u32(arenaD.Obj(a[22])); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kAllocSdd:
    case kAllocBase: {
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        arenaB.Put32(a[7] + 0, thisA - 32u * a[9]); arenaB.Put32(a[7] + 4, u32(arenaD.Obj(a[10]))); arenaB.Put32(a[7] + 8, a[8]); arenaB.Put32(a[7] + 0xC, a[11]); arenaB.Put8(a[7] + 0x10, 1);
        arenaD.Put8(i32(a[10] + a[9]), a[12] & 0x7Fu);
        *reinterpret_cast<u32*>(kAllocPool) = u32(arenaB.Obj(a[7]));
        break;
    }
    case kWRegion: {
        if (a[4]) { arenaA.Put32(o, a[5]); arenaA.Put32(o + 4, a[6]); arenaA.Put32(o + 8, a[7]); }
        call.args[0] = a[4] ? thisA : 0u; call.argCount = 1; call.popBytes = 4;
        for (u32 i = 0; i < 3; ++i) *reinterpret_cast<u32*>(kCameraBase + 4 + 4 * i) = a[10 + i];
        *reinterpret_cast<u32*>(kCameraBase + 0x14) = a[8] ? u32(arenaC.Obj(a[9])) : 0u;
        if (a[8]) for (u32 i = 0; i < 3; ++i) arenaC.Put32(a[9] + 0x30 + 4 * i, a[13 + i]);
        *reinterpret_cast<u32*>(kWeatherRegion) = a[16];
        break;
    }
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
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0; gNewArrN = 0; gRemoveCalls = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPCtor: case kPNew: case kPSdd: case kClone: case kSdd: case kAllocSdd: case kAllocBase: case kWCtor: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                                                                  // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                                                                                                  // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN179A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN179A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn179a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn179a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn179a"); }
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
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator new[] 0x821195, delete[] 0x8213AE, operator delete 0x8214BD): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {ByExeAddress(0x8213AE, &FkDeleteArray)};
    env.stubs[1] = {ByExeAddress(0x821195, &FkNewArr)};
    env.stubs[2] = {ByExeAddress(0x8213AE, &FkDeleteArray)};
    env.stubs[4] = {ByExeAddress(0x8213AE, &FkDeleteArray), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[6] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4B23D0, &FkCtor2<0x4B23D0>)};
    env.stubs[7] = {Rt(8, &FkBody<0x609300>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[8] = {F(0x4B2470, &FkBody<0x4B2470>)};
    env.stubs[12] = {F(0x4D3FD0, &FkRemoveAnimRef), F(0x5FB870, &FkRemoveGroup), F(0x5E8620, &FkBody<0x5E8620>)};
    env.stubs[14] = {F(0x59C6D0, &FkMult3x3)};
    env.stubs[15] = {F(0x65EEF0, &FkBody<0x65EEF0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[16] = {F(0x69BBF0, &FkBody<0x69BBF0>)};
    env.stubs[17] = {F(0x69C2D0, &FkBody<0x69C2D0>)};
    env.stubs[18] = {F(0x69C780, &FkBody<0x69C780>)};
    env.stubs[19] = {F(0x69CB60, &FkBody<0x69CB60>)};
    env.stubs[21] = {Rt(22, &FkBody<0x69DB40>)};
    env.stubs[22] = {F(0x69C2D0, &FkBody<0x69C2D0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {3u, 5u, 9u, 10u, 11u, 13u, 20u, 23u, 24u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[16] = dllReal[17] = dllReal[18] = dllReal[19] = dllReal[21] = "toda la funcion salvo el cuerpo del destructor (falso identico); la liberacion en linea del pool de asignadores (0xB744BC) corre real en el EXE y por CTaskAllocator::operator delete en la DLL";
    dllReal[14] = "toda la funcion salvo CVector::FromMultiply3x3 (falso identico); la aritmetica x87 corre real en ambos lados";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: puntero del pool de asignadores, puntero de los grupos de animacion, tablas de desplazamientos de la formacion, camara y palabra de la region del tiempo; se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kAllocPool, 4); for (unsigned f : {16u, 17u, 18u, 19u, 20u, 21u}) regionsOf[f] = {regions.size() - 1};
    regions.emplace_back(kAnimGroups, 4); regionsOf[12] = {regions.size() - 1};
    regions.emplace_back(kOffsetsTable, 0x20); regions.emplace_back(kMovingTable, 0x20); regionsOf[14] = {regions.size() - 2, regions.size() - 1};
    regions.emplace_back(kCameraBase, 0x18); regions.emplace_back(kWeatherRegion, 4); regionsOf[24] = {regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
