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

// Banco par a par Fn171A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en
// cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): constructor base de CTaskSimple 0x61A390,
// ~CTask 0x61A3A0, ~CTaskComplex 0x61A3D0, CTask::operator new 0x61A5A0 y operator delete 0x61A5B0, constructor de CTaskSimpleStandStill 0x62F310 y su cuerpo del destructor 0x62F360, ProcessPed de CTaskSimpleStandStill 0x62F370 (V),
// CGeneral 0x53CBE0 y 0x53CB50, Multiply3x3 0x59C790, CVehicleAnimGroup::GetGroup 0x6E3B00 y ComputeCriticalBlendTime 0x6E3C80, CEntity::CleanUpOldReference 0x571A00, CPointRoute::operator delete 0x41B7D0, constructores de
// CTaskSimpleArrestPed 0x68B620 y CTaskComplexArrestPed 0x68B990, cuerpo del destructor de CTaskSimpleArrestPed 0x68B690, CTaskComplexArrestPed 0x68BA00 y CTaskSimpleCreateCarAndGetIn 0x64CEA0 (funcion propia de la DLL), y
// CreateRouteTask 0x655140 (todos con hook). Memoria global del juego tocada (restaurada tras cada lado): el reloj 0xB7CB84, los dos modelos de escalon 0x8CD674 / 0x8CD678 y la tabla de model infos 0xA9B0C8.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool; bits del float de ST0 en ComputeCriticalBlendTime) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).

uintptr_t Lote171ABody_CTaskSimpleCreateCarAndGetIn();   // 0x64CEA0 (con hook)
uintptr_t Lote171ABody_CTaskComplexArrestPed();          // 0x68BA00 (con hook)

namespace F171ALive {
using namespace F171AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F171AFix::Arena(WIN);
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
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline u32 gRet2{};                  // EAX que devuelve el falso de CreateRouteTask
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de CTask::operator new (0 = sin memoria)
inline u32 gAtan{}, gLimit{}, gCbt{};   // bits de los floats que devuelven los falsos de 0x53CBE0, 0x53CB50 y 0x6E3C80
inline std::array<u32, 3> gMul{};    // lo que escribe el falso de Multiply3x3
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CTask::operator new (0x61A5A0; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// Cuerpo del destructor / constructor sin argumentos (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con un argumento de pila (thiscall, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(a));
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con cuatro dwords de pila (thiscall, `ret 10h`): evento [direccion, ECX (etiqueta), a, b, c, d]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor4(void* self, void*, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(ADDR, 5);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::CleanUpOldReference (0x571A00) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// ProcessPed de CTaskSimpleStandStill (0x62F370; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), ped (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkStandStillProcess(void* self, void*, void* ped) {
    Ev& e = PushEv(0x62F370, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ped);
    return gRet;
}
// CGeneral::GetRadianAngleBetweenPoints (0x53CBE0; cdecl, cuatro floats; resultado float en ST0): evento [direccion, bits de los cuatro]; devuelve gAtan.
inline float __cdecl FkAtan(u32 x1, u32 y1, u32 x2, u32 y2) {
    Ev& e = PushEv(0x53CBE0, 4);
    e.v[0] = x1; e.v[1] = y1; e.v[2] = x2; e.v[3] = y2;
    return std::bit_cast<float>(gAtan);
}
// CGeneral::LimitRadianAngle (0x53CB50; cdecl, un float; resultado float en ST0): evento [direccion, bits]; devuelve gLimit.
inline float __cdecl FkLimit(u32 angle) {
    Ev& e = PushEv(0x53CB50, 1);
    e.v[0] = angle;
    return std::bit_cast<float>(gLimit);
}
// Multiply3x3 (0x59C790; cdecl, resultado, matriz, vector): evento [direccion, resultado (etiqueta), matriz (etiqueta), vector (etiqueta), x, y, z del vector]; escribe gMul en el resultado y lo devuelve.
inline u32* __cdecl FkMul(u32* out, u32* matrix, u32* vector) {
    Ev& e = PushEv(0x59C790, 6);
    e.v[0] = Tag(out); e.v[1] = Tag(matrix); e.v[2] = Tag(vector);
    e.v[3] = vector[0]; e.v[4] = vector[1]; e.v[5] = vector[2];
    out[0] = gMul[0]; out[1] = gMul[1]; out[2] = gMul[2];
    return out;
}
// CVehicleAnimGroup::GetGroup (0x6E3B00; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX (crudo), argumento]; devuelve gRet.
inline u32 __fastcall FkGetGroup(void* self, void*, u32 anim) {
    Ev& e = PushEv(0x6E3B00, 2);
    e.v[0] = u32(uintptr_t(self));
    e.v[1] = anim;
    return gRet;
}
// CVehicleAnimGroup::ComputeCriticalBlendTime (0x6E3C80; thiscall, un dword de pila, `ret 4`; float en ST0): evento [direccion, ECX (crudo), argumento]; devuelve gCbt.
inline float __fastcall FkCbt(void* self, void*, u32 anim) {
    Ev& e = PushEv(0x6E3C80, 2);
    e.v[0] = u32(uintptr_t(self));
    e.v[1] = anim;
    return std::bit_cast<float>(gCbt);
}
// CTaskComplexWalkRoundObject::CreateRouteTask (0x655140; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), ped (etiqueta)]; devuelve gRet2.
inline u32 __fastcall FkRoute(void* self, void*, void* ped) {
    Ev& e = PushEv(0x655140, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ped);
    return gRet2;
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

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gRet2 = 0; gDtorWrite = 0; gNew = 0; gAtan = gLimit = gCbt = 0; gMul = {};
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kMakeAb:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3;
        break;
    case kPizzaCtor:
    case kBody:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kPizzaProc:
        arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, a[5]);
        arenaA.Put32(o + 0x10, (a[6] & 0xFFu) | ((a[7] & 0xFFu) << 8) | (a[10] << 16));
        *reinterpret_cast<u32*>(0xB7CB84) = a[8];
        call.args[0] = a[9]; call.argCount = 1;
        break;
    case kClonePizza:
    case kCloneEsc:
    case kCloneCtor1:
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        if (c.fn == 16) arenaA.Put32(o + 8, a[8]);
        if (c.fn == 17) arenaA.Put32(o + 0x10, a[8]);
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kEscProc: {
        const u32 ped = u32(arenaB.Obj(a[4]));
        const u32 contact = a[5] == 0 ? 0u : u32(arenaC.Obj(a[6]));
        arenaB.Put32(a[4] + 0x584, contact); arenaB.Put32(a[4] + 0x55C, a[15]);
        if (contact) { arenaC.Put32(a[6] + 0x20, a[7]); arenaC.Put32(a[6] + 0x44, a[10]); arenaC.Put32(a[6] + 0x48, a[11]); }
        gRet = a[12]; gAtan = a[13]; gLimit = a[14];
        *reinterpret_cast<u32*>(0x8CD674) = a[8]; *reinterpret_cast<u32*>(0x8CD678) = a[9];
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kTargetPos: {
        const u32 veh = u32(arenaB.Obj(a[4]));
        const u32 matrix = a[8] == 0 ? 0u : u32(arenaC.Obj(a[9]));
        arenaA.Put32(o + 0x18, veh);
        arenaB.Put32(a[4] + 4, a[5]); arenaB.Put32(a[4] + 8, a[6]); arenaB.Put32(a[4] + 0xC, a[7]);
        arenaB.Put32(a[4] + 0x14, matrix);
        arenaB.Put32(a[4] + 0x20, (a[13] << 16) | (a[14] & 0xFFFFu));
        if (matrix) { arenaC.Put32(a[9] + 0x30, a[10]); arenaC.Put32(a[9] + 0x34, a[11]); arenaC.Put32(a[9] + 0x38, a[12]); }
        arenaD.Put32(a[15] + 0x3C, a[16]); arenaD.Put32(a[15] + 0x5C, u32(arenaD.Obj(a[17])));
        for (u32 i = 0; i < 3; ++i) { arenaD.Put32(a[17] + 4 * i, a[18 + i]); arenaD.Put32(a[17] + 0x30 + 4 * i, a[21 + i]); }
        *reinterpret_cast<u32*>(kModelTable + 4 * a[13]) = u32(arenaD.Obj(a[15]));
        gMul = {a[24], a[25], a[26]};
        break;
    }
    case kCbt:
        gRet = a[6]; gCbt = a[7];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 8; call.floatRet = true;
        break;
    case kWalkBody: {
        const u32 obj = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : (a[4] == 2 ? u32(arenaC.Obj(a[5])) : a[6]));
        const u32 route = a[7] == 0 ? 0u : u32(arenaB.Obj(a[8]));
        arenaA.Put32(o + 0x1C, obj); arenaA.Put32(o + 0x20, route);
        gRet = a[9]; gDtorWrite = a[10];
        break;
    }
    case kCreateSub:
        gNew = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        gRet = a[8]; gDtorWrite = a[9]; gRet2 = a[10];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
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
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        const u32 floatBits = gFloatRet;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPizzaCtor: case kClonePizza: case kCloneEsc: case kSdd: case kCloneCtor1: case kCreateSub: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                                                      // valor completo de EAX
        case kMakeAb: case kPizzaProc: case kEscProc: obs["ret"] = eax & 0xFFu; break;                                                             // bool: solo AL
        case kCbt: obs["ret"] = floatBits; break;                                                                                                  // bits del float de ST0
        default: obs["ret"] = nullptr; break;                                                                                                      // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN171A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN171A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn171a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn171a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn171a"); }
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
    // Colaborador con hook V (la DLL lo llama de forma no virtual: el cuerpo es la funcion de la ranura)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabV(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x61A390, &FkBody<0x61A390>)};
    env.stubs[3] = {F(0x61A3A0, &FkBody<0x61A3A0>)};
    env.stubs[5] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x61A390, &FkBody<0x61A390>)};
    env.stubs[6] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x62F310, &FkCtor4<0x62F310>)};
    env.stubs[8] = {Rt(9, &FkBody<0x4B8750>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[9] = {F(0x62F360, &FkBody<0x62F360>)};
    env.stubs[10] = {FV(0x62F370, &FkStandStillProcess), F(0x53CBE0, &FkAtan), F(0x53CB50, &FkLimit)};
    env.stubs[13] = {F(0x59C790, &FkMul)};
    env.stubs[14] = {H(0x64CEA0, Lote171ABody_CTaskSimpleCreateCarAndGetIn(), &FkBody<0x64CEA0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[15] = {F(0x6E3B00, &FkGetGroup), F(0x6E3C80, &FkCbt)};
    env.stubs[16] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x68B620, &FkCtor1<0x68B620>)};
    env.stubs[17] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x68B990, &FkCtor1<0x68B990>)};
    env.stubs[18] = {F(0x68B690, &FkBody<0x68B690>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[19] = {H(0x68BA00, Lote171ABody_CTaskComplexArrestPed(), &FkBody<0x68BA00>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[21] = {F(0x571A00, &FkRef<0x571A00>), F(0x41B7D0, &FkDelete<0x41B7D0>), F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[22] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x62F310, &FkCtor4<0x62F310>), F(0x655140, &FkRoute)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {1u, 2u, 7u, 11u, 12u, 20u, 4u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: el reloj (kPizzaProc), los dos modelos de escalon (kEscProc) y la tabla de model infos (kTargetPos); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(0xB7CB84, 4); regionsOf[4] = {regions.size() - 1};
    regions.emplace_back(0x8CD674, 8); regionsOf[10] = {regions.size() - 1};
    regions.emplace_back(kModelTable, kModelCount * 4); regionsOf[13] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
