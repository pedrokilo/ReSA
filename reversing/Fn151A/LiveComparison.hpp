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
#include "TaskComplexSeekEntity.h"

// Banco par a par Fn151A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 24 funciones del EXE (destructores con borrado de CTaskComplexSeekEntity<XYOffset>, CTaskComplexWalkAlongsidePed, CTaskComplexWaitForPed, CTaskComplexBeInCouple, CTaskComplexFacial, CTaskSimpleAffectSecondaryBehaviour,
// CTaskSimpleHoldEntity, CEventEscalator, CTaskSimpleHandsUp y CTaskSimpleLookAbout; cuerpos de destructor de CTaskComplexPartnerShove, CTaskSimpleFacial y CTaskSimpleLookAbout; copias de ~CInformGroupEvent y ~CTask;
// constructor con argumentos de CInformGroupEvent; constructor de CTaskSimpleChat; GetSeekPos de CTaskComplexSeekEntity<XYOffset>; Get de CPedGroupDefaultTaskAllocators; GetTaskType y Clone de
// CTaskComplexGoPickUpEntity; Clone de CEventEscalator; CTaskComplex::SetSubTask; GetPlayerRelationshipRecorder; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall /
// RH_ScopedInstall / RH_ScopedNamedInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTInstall: GetTaskType y los dos Clone). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con
// guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la
// envoltura de la DLL; sin hook, `U`: funcion propia de la DLL; con hook y funcion propia, `F`: el cuerpo del hook), CTask::operator delete 0x61A5B0 (con hook), CEvent::operator delete 0x4B5630 (sin hook), ~CTask 0x61A3A0,
// CEntity::CleanUpOldReference 0x571A00 y RegisterReference 0x571B70, el constructor de CTaskSimpleRunTimedAnim 0x61ABE0, CTask::operator new 0x61A5A0, el constructor de CTaskComplexGoPickUpEntity 0x6919C0,
// CEvent::operator new 0x4B5620 y su constructor 0x4ABFC0, y el operator new del CRT 0x82119A (sin hook, la DLL lo llama por esa direccion). El calculador de posiciones de GetSeekPos y los destructores virtuales de los
// objetos borrados (CInformGroupEvent::m_Event, subtarea antigua de SetSubTask) son objetos sinteticos en las arenas con una vtable propia (ranura 0 falsa).
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o la salida como etiqueta de arena; EAX completo en getters, constantes y el reloj de InformGroupEvent), la variable global del registrador (0xC17084) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada
// quedan en original_residue / candidate_residue (el oraculo exige ademas que 0x61A350 y 0x5F7200 conserven EDX y 0x61A350 tambien EAX y ECX).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote150ABody_CTaskSimpleAnim();                           // 0x61A6F0 (sin hook; el cuerpo de ~CTaskSimpleLookAbout lo llama como el `jmp 0x61A6F0` del EXE)
uintptr_t Lote151ABody_CTaskComplexWalkAlongsidePed();              // 0x6832C0 (con hook)
uintptr_t Lote151ABody_CTaskComplexWaitForPed();                    // 0x6833D0 (sin hook)
uintptr_t Lote151ABody_CTaskComplexBeInCouple();                    // 0x683780 (con hook)
uintptr_t Lote151ABody_CTaskSimpleHoldEntity();                     // 0x691630 (sin hook)
uintptr_t Lote151ABody_CEventEscalator();                           // 0x5FF860 (sin hook)
uintptr_t Lote151ABody_CEventOperatorDelete();                      // 0x4B5630 (sin hook)
uintptr_t Lote151ABody_CTaskSimpleHandsUp();                        // 0x48EA60 (con hook)

namespace F151ALive {
using namespace F151AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F151AFix::Arena(WIN);
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
inline Arena<kWin> arenaA, arenaB, arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gMem{};                   // memoria que devuelven los falsos de operator new
inline std::array<u32, 3> gOut{};    // los tres floats que escribe el falso del calculador
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator new (cdecl, un argumento): evento [direccion del EXE, tamano]; devuelve gMem.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gMem;
}
// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::CleanUpOldReference (0x571A00) y RegisterReference (0x571B70) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escriben nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// Destructor virtual con borrado (ranura 0 de la vtable falsa de los objetos de las arenas; thiscall, un dword de pila, `ret 4`): evento [marca, ECX (etiqueta), argumento]; devuelve gRet.
inline u32 __fastcall FkVDel(void* self, void*, u32 flag) {
    Ev& e = PushEv(0xFA11D001u, 2);
    e.v[0] = Tag(self);
    e.v[1] = flag;
    return gRet;
}
// Etiqueta de un puntero que puede ser un local de la pila (cambia de un lado al otro): 0x7F000000 si no esta en ninguna arena
inline u32 TagOrStack(const void* p) {
    const u32 t = Tag(p);
    return t == u32(uintptr_t(p)) ? 0x7F000000u : t;
}
// Calculador de posiciones (ComputeEntitySeekPos, ranura 0 de la vtable falsa de +0x40; thiscall, tres punteros de pila, `ret 0Ch`): evento [marca, ECX (etiqueta), peaton, entidad, salida (etiqueta o pila)];
// escribe los tres floats del caso en la salida (como bits).
inline void __fastcall FkCalc(void* self, void*, void* seeker, void* target, u32* out) {
    Ev& e = PushEv(0xFA11CA1Cu, 4);
    e.v[0] = Tag(self);
    e.v[1] = u32(uintptr_t(seeker));
    e.v[2] = u32(uintptr_t(target));
    e.v[3] = TagOrStack(out);
    out[0] = gOut[0]; out[1] = gOut[1]; out[2] = gOut[2];
}
// Constructor de CTaskSimpleRunTimedAnim (0x61ABE0; thiscall, ocho dwords de pila, `ret 20h`): evento [direccion, ECX, grupo, anim, mezcla, sin uso, duracion, tipo, FNV del nombre, ultimo argumento];
// deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkRunTimedCtor(void* self, void*, u32 group, u32 anim, u32 blend, u32 unused, u32 duration, u32 type, const char* name, u32 hold) {
    Ev& e = PushEv(0x61ABE0, 9);
    e.v[0] = Tag(self); e.v[1] = group; e.v[2] = anim; e.v[3] = blend; e.v[4] = unused; e.v[5] = duration; e.v[6] = type; e.v[7] = FnvStr(name); e.v[8] = hold;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexGoPickUpEntity (0x6919C0; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX, entidad, grupo]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkPickCtor(void* self, void*, u32 entity, u32 group) {
    Ev& e = PushEv(0x6919C0, 3);
    e.v[0] = Tag(self); e.v[1] = entity; e.v[2] = group;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CEvent (0x4ABFC0; thiscall, sin argumentos de pila): evento [direccion, ECX, dword de [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkEventCtor(void* self, void*) {
    Ev& e = PushEv(0x4ABFC0, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
inline std::array<void*, 2> gVDelVtbl{reinterpret_cast<void*>(&FkVDel), nullptr};     // vtable falsa de los objetos borrados
inline std::array<void*, 2> gCalcVtbl{reinterpret_cast<void*>(&FkCalc), nullptr};     // vtable falsa del calculador de posiciones
template<size_t... I> inline std::array<void*, kFunctions> MakeBodyFakes(std::index_sequence<I...>) { return {reinterpret_cast<void*>(&FkBody<kBodyExe[I]>)...}; }
inline const std::array<void*, kFunctions> kBodyFakes = MakeBodyFakes(std::make_index_sequence<kFunctions>{});

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

// EAX que devuelve el falso de un constructor segun el selector (0 la memoria, 1 la memoria + 4, 2 cero, 3 un valor crudo)
inline u32 CtorEax(u32 selector, u32 memory, u32 raw) {
    switch (selector) {
    case 0: return memory;
    case 1: return memory + 4;
    case 2: return 0;
    default: return raw;
    }
}

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gMem = 0; gOut = {};
    const u32 thisA = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kSdd:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kGetSeek:   // thiscall con la salida y el peaton en la pila (`ret 8`)
        arenaA.Put32(a[0] + 0xC, a[3]);
        arenaA.Put32(a[0] + 0x40, u32(uintptr_t(gCalcVtbl.data())));
        gOut = {a[5], a[6], a[7]};
        call.ecx = thisA;
        call.args[0] = a[8] == 0 ? u32(arenaB.Obj(a[9])) : u32(arenaA.Obj(a[0] + kSeekAlias[a[8] - 1]));
        call.args[1] = a[4]; call.argCount = 2; call.popBytes = 0;
        break;
    case kInfDtor: {
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]); arenaA.Put32(a[0] + 0xC, a[7]);
        u32 event = 0;
        if (a[5] == 1) { arenaB.Put32(a[6], u32(uintptr_t(gVDelVtbl.data()))); event = u32(arenaB.Obj(a[6])); }
        arenaA.Put32(a[0] + 8, event);
        gRet = a[8];
        call.ecx = thisA;
        break;
    }
    case kInfCtor:   // thiscall con cuatro dwords de pila (`ret 10h`); reloj de juego en la memoria global
        arenaA.Put32(a[0], a[8]); arenaA.Put32(a[0] + 4, a[9]); arenaA.Put32(a[0] + 8, a[10]); arenaA.Put32(a[0] + 0xC, a[11]);
        *reinterpret_cast<volatile u32*>(kGlobalTimer) = a[7];
        call.ecx = thisA;
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.args[3] = a[6]; call.argCount = 4; call.popBytes = 0;
        break;
    case kVtBody:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kChat:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kAlloc:   // cdecl con el tipo en la pila (lo limpia el llamador)
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kConst:
        arenaA.Put32(a[0], a[3]);
        call.ecx = thisA;
        break;
    case kClonePick: {
        arenaA.Put32(a[0] + 0xC, a[3]); arenaA.Put32(a[0] + 0x2C, a[4]);
        gMem = a[5] == 0 ? 0u : (a[5] == 1 ? u32(arenaB.Obj(a[6])) : u32(arenaA.Obj(a[6])));
        gRet = CtorEax(a[7], gMem, a[9]); gDtorWrite = a[8];
        call.ecx = thisA;
        break;
    }
    case kEscClone:
        gMem = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        gRet = CtorEax(a[7], gMem, a[6]); gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kTaskVt:
        arenaA.Put32(a[0], a[3]);
        call.ecx = thisA;
        break;
    case kSetSub: {
        u32 cur = 0;
        if (a[3] == 1) { arenaB.Put32(a[5], u32(uintptr_t(gVDelVtbl.data()))); cur = u32(arenaB.Obj(a[5])); }
        arenaA.Put32(a[0], a[8]); arenaA.Put32(a[0] + 4, a[7]); arenaA.Put32(a[0] + 8, cur);
        u32 arg = 0;
        switch (a[4]) {
        case 0: arg = 0; break;
        case 2: arg = cur; break;
        case 3: arg = thisA; break;
        default: arg = u32(arenaC.Obj(a[6])); break;
        }
        gRet = a[9];
        call.ecx = thisA;
        call.args[0] = arg; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kRelRec:   // cdecl sin argumentos
        *reinterpret_cast<volatile u32*>(kGlobalRecorder) = a[3] == 0 ? 0u : u32(arenaC.Obj(a[4]));
        gMem = a[5] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    input["fakes"] = json::array({u32(uintptr_t(gVDelVtbl.data())), u32(uintptr_t(gCalcVtbl.data()))});   // direcciones de las vtables falsas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        if (kKind[fn] == kRelRec) obs["glob"] = Tag(uintptr_t(*reinterpret_cast<volatile u32*>(kGlobalRecorder)));   // el registrador unico tras la llamada
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kGetSeek: case kChat: case kClonePick: case kEscClone: case kRelRec: obs["ret"] = Tag(uintptr_t(eax)); break;   // this, la salida o la memoria como etiqueta de arena
        case kInfCtor: case kAlloc: case kConst: obs["ret"] = eax; break;                                                              // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                                                                         // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN151A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN151A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn151a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn151a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn151a"); }
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
    // Colaborador CON hook RH cuya llamada, en la funcion probada, va a una funcion propia de la DLL distinta de la envoltura del hook (direccion dada): el hook se apaga (ruta) y el falso va en la
    // direccion del EXE y en esa funcion.
    auto H = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        Pair p = it->second;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator new del CRT): un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    const auto& K = kBodyFakes;
    using SeekXY = CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>;
    env.stubs[0] = {H(0x661F30, reinterpret_cast<uintptr_t>(&SeekXY::DestructorBodyExe), K[0]), Del()};
    env.stubs[2] = {F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[3] = {F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[4] = {F(0x683A40, K[4])};
    env.stubs[5] = {H(0x6832C0, Lote151ABody_CTaskComplexWalkAlongsidePed(), K[5]), Del()};
    env.stubs[6] = {U(0x6833D0, Lote151ABody_CTaskComplexWaitForPed(), K[6]), Del()};
    env.stubs[7] = {H(0x683780, Lote151ABody_CTaskComplexBeInCouple(), K[7]), Del()};
    env.stubs[8] = {F(0x61A3A0, K[8])};
    env.stubs[9] = {F(0x690D90, K[9]), Del()};
    env.stubs[10] = {F(0x6912C0, K[10]), Del()};
    env.stubs[11] = {U(0x691630, Lote151ABody_CTaskSimpleHoldEntity(), K[11]), Del()};
    env.stubs[12] = {F(0x61ABE0, &FkRunTimedCtor)};
    env.stubs[15] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x6919C0, &FkPickCtor)};
    MakeCollab(env, 0x4AC010, hooksFound);   // 0x5FF860 es `jmp 0x4AC010` y ese cuerpo de ~CEvent tiene hook ajeno: se apaga (ruta de colaborador) para que el salto lleve al codigo del EXE
    env.stubs[16] = {U(0x5FF860, Lote151ABody_CEventEscalator(), K[16]), U(0x4B5630, Lote151ABody_CEventOperatorDelete(), &FkDelete<0x4B5630>)};
    env.stubs[17] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4ABFC0, &FkEventCtor)};
    env.stubs[20] = {E(0x82119A, &FkNew<0x82119A>)};
    env.stubs[21] = {U(0x61A6F0, Lote150ABody_CTaskSimpleAnim(), K[21])};
    env.stubs[22] = {H(0x48EA60, Lote151ABody_CTaskSimpleHandsUp(), K[22]), Del()};
    env.stubs[23] = {F(0x48EA90, K[23]), Del()};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {1u, 13u, 14u, 18u, 19u}) dllReal[f] = "toda la funcion (sin colaboradores de la lista; las llamadas virtuales van a la vtable falsa de las arenas)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Memoria global del juego: el reloj de juego (0xB7CB84), que lee el constructor de CInformGroupEvent (funcion 3), y el puntero al registrador (0xC17084), que lee y escribe la funcion 20; el resto ocurre en las tres arenas propias
    regions.clear();
    regions.emplace_back(kGlobalTimer, 4);
    regions.emplace_back(kGlobalRecorder, 4);
    for (auto& v : regionsOf) v.clear();
    regionsOf[3] = {0};
    regionsOf[20] = {1};
    Run(env, ready, registered, bodies, collaborators);
}
}
