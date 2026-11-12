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

// Banco par a par Fn169A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (cuerpo del destructor, destructor con borrado, Clone y cinco constantes virtuales de CEventLeaderEnteredCarAsDriver; TakesPriorityOver de CEventLeaderExitedCarAsDriver; AffectsPed, AffectsPedGroup,
// CanBeInterruptedBySameEvent e IsValid de CEvent; cuatro funciones de un elemento de CInformFriendsEvent; GetSubTask, IsSimple, MakeAbortable y el destructor con borrado de CTaskComplex; el destructor con borrado de
// CTask y el cuerpo del destructor de CTaskSimpleAnim; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los
// colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook, `F`; funcion probada del lote, `Rt`), CEvent::operator delete 0x4B5630 (con hook),
// CEntity::CleanUpOldReference 0x571A00 y RegisterReference 0x571B70, CEvent::operator new 0x4B5620, el constructor de CEventLeaderEnteredCarAsDriver 0x48E1C0, CEventGroup::Add 0x4AB420, CAnimBlendAssociation::
// SetFinishCallback 0x4CEBE0, ~CEventEditableResponse 0x4AC480, ~CTaskComplex 0x61A3D0 y ~CTask 0x61A3A0; y, sin parche, las vtables falsas de la arena C (GetLifeTime, destructor con borrado de un evento y MakeAbortable de una
// subtarea). La liberacion del pool de tareas es EN LINEA: el banco apunta 0xB744A8 a un pool falso de la arena B (estados en la arena C); el reloj de juego (0xB7CB84) se fija por caso.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o la salida como etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F169ALive {
using namespace F169AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F169AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline DWORD gBenchThread{};

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CEntity::CleanUpOldReference (0x571A00) y RegisterReference (0x571B70) (thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No tocan la referencia.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) { Ev& e = PushEv(ADDR, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// CEvent::operator new (0x4B5620; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew (puntero a la arena B o nulo).
inline u32 gNew{};
inline void* __cdecl FkNew(u32 size) { Ev& e = PushEv(0x4B5620, 1); e.v[0] = size; return reinterpret_cast<void*>(uintptr_t(gNew)); }
// Constructor de CEventLeaderEnteredCarAsDriver (0x48E1C0; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]; deja gDtorWrite en [ECX]; devuelve this.
template<u32 ADDR> inline void* __fastcall FkCtor(void* self, void*, void* arg) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(arg);
    memcpy(self, &gDtorWrite, 4);
    return self;
}
// CEventGroup::Add (0x4AB420; thiscall, evento y bandera en la pila, `ret 8`): evento [direccion, ECX (etiqueta), evento (etiqueta), bandera (dword completo)]; devuelve gRet.
inline u32 __fastcall FkAdd(void* self, void*, void* event, u32 flag) { Ev& e = PushEv(0x4AB420, 3); e.v[0] = Tag(self); e.v[1] = Tag(event); e.v[2] = flag; return gRet; }
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos argumentos de pila, `ret 8`): evento [direccion, ECX (etiqueta), funcion, dato (etiqueta)]. El DefaultAnimCB de la DLL se normaliza al del EXE
// (la direccion de la funcion no es observable por el juego; si el valor no es ninguno de los dos se anota crudo).
inline u32 gDllDefaultCb{};
inline void __fastcall FkSetFinish(void* self, void*, void* cb, void* data) {
    Ev& e = PushEv(0x4CEBE0, 3);
    const u32 f = u32(uintptr_t(cb));
    e.v[0] = Tag(self);
    e.v[1] = (f == kEvDefaultCb || f == gDllDefaultCb) ? kEvDefaultCb : f;
    e.v[2] = Tag(data);
}
// GetLifeTime falso (ranura 3 de la vtable falsa de la arena C): evento [identificador, ECX (etiqueta)]; devuelve gLife.
inline u32 gLife{};
inline u32 __fastcall FkLife(void* self, void*) { Ev& e = PushEv(kEvLife, 1); e.v[0] = Tag(self); return gLife; }
// Destructor virtual con borrado falso de un evento (ranura 0 de la vtable falsa de la arena C; thiscall, banderas en la pila, `ret 4`): evento [identificador, ECX (etiqueta), banderas]; devuelve gRet.
inline u32 __fastcall FkEvDtor(void* self, void*, u32 flags) { Ev& e = PushEv(kEvDtor, 2); e.v[0] = Tag(self); e.v[1] = flags; return gRet; }
// MakeAbortable virtual falso de una subtarea (ranura 6 de la vtable falsa de la arena C; thiscall, tres argumentos de pila, `ret 0Ch`): evento [identificador, ECX (etiqueta), ped, prioridad, evento]; devuelve gRet.
inline u32 __fastcall FkMakeAbort(void* self, void*, u32 ped, u32 priority, u32 event) { Ev& e = PushEv(kEvAbort, 4); e.v[0] = Tag(self); e.v[1] = ped; e.v[2] = priority; e.v[3] = event; return gRet; }
inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
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

// Pool falso de tareas (arena B, a[pool]): +0 almacenamiento = obj - d, +4 estados (la celda de estados[indice] cae en la arena C; indice = d >> 7, desplazamiento aritmetico), +8 relleno, +0xC cursor; 0xB744A8 apunta a el.
inline void PutPool(u32 obj, u32 d, u32 cursor, u32 cellC, u32 poolB, u32 fill) {
    const i32 index = i32(d) >> 7;
    arenaB.Put32(poolB + 0x0, obj - d);
    arenaB.Put32(poolB + 0x4, u32(arenaC.Obj(cellC)) - u32(index));
    arenaB.Put32(poolB + 0x8, fill);
    arenaB.Put32(poolB + 0xC, cursor);
    PutGlobal32(kTaskPoolPtr, u32(arenaB.Obj(poolB)));
}

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gLife = 0; gNew = 0;
    gDllDefaultCb = u32(uintptr_t(&CDefaultAnimCallback::DefaultAnimCB));
    const u32 self = u32(arenaA.Obj(a[0]));
    const u32 fvt = u32(arenaC.Obj(kFakeVtable));
    const auto pedOf = [&](u32 mode, u32 off, u32 raw) { return mode == 0 ? 0u : (mode == 1 ? u32(arenaB.Obj(off)) : raw); };
    switch (kKind[c.fn]) {
    case kConst: case kConstAL0:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kConstAL1:   // thiscall con un argumento de pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kRead8:
        arenaA.Put32(a[0] + 8, a[3]);
        call.ecx = self;
        break;
    case kEntBody:
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x14, a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[8]));
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kClone:
        arenaA.Put32(a[0] + 0x14, a[4] ? u32(arenaC.Obj(a[5])) : a[3]);
        gNew = a[6] ? u32(arenaB.Obj(a[7])) : 0u;
        gDtorWrite = a[8];
        call.ecx = self;
        break;
    case kSdd: case kSddPool:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        if (kKind[c.fn] == kSddPool) PutPool(self, a[7], a[8], a[9], a[10], a[11]);
        break;
    case kIsValid:      // this con vtable falsa (ranura 3 = GetLifeTime falso) y un argumento de pila (el ped, no se lee)
        arenaA.Put32(a[0] + 0, u32(arenaC.Obj(a[7])));
        arenaA.Put32(a[0] + 4, a[4]);
        arenaA.Put32(a[0] + 8, a[5]);
        arenaC.Put32(a[7] + 12, u32(uintptr_t(&FkLife)));
        gLife = a[3];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kFlush: {      // elemento de la cola: ped +0, evento +4 (objeto de la arena C con la vtable falsa), instante +8
        arenaA.Put32(a[0], pedOf(a[3], a[4], a[5]));
        arenaA.Put32(a[0] + 4, a[6] ? u32(arenaC.Obj(a[7])) : 0u);
        arenaA.Put32(a[0] + 8, a[8]);
        if (a[6]) arenaC.Put32(a[7], fvt);
        arenaC.Put32(kFakeVtable, u32(uintptr_t(&FkEvDtor)));
        gRet = a[9];
        call.ecx = self;
        break;
    }
    case kSet:          // thiscall(ped, evento, retardo) (`ret 0Ch`)
        arenaA.Put32(a[0] + 8, a[9]);
        PutGlobal32(kGlobalTimer, a[8]);
        call.ecx = self;
        call.args[0] = pedOf(a[3], a[4], a[5]); call.args[1] = a[6]; call.args[2] = a[7]; call.argCount = 3; call.popBytes = 0;
        break;
    case kProc: {       // elemento con ped en B (inteligencia en +47Ch apuntando a C) y evento en C
        const u32 ped = a[3] ? u32(arenaB.Obj(a[4])) : 0u;
        arenaA.Put32(a[0], ped);
        arenaA.Put32(a[0] + 4, a[6] ? u32(arenaC.Obj(a[7])) : 0u);
        arenaA.Put32(a[0] + 8, a[8]);
        if (a[3]) arenaB.Put32(a[4] + 0x47C, u32(arenaC.Obj(a[5])));
        if (a[6]) arenaC.Put32(a[7], fvt);
        arenaC.Put32(kFakeVtable, u32(uintptr_t(&FkEvDtor)));
        PutGlobal32(kGlobalTimer, a[9]);
        gRet = a[10];
        call.ecx = self;
        break;
    }
    case kMakeAbort:    // thiscall(ped, prioridad, evento) (`ret 0Ch`): la subtarea (+8) es un objeto de C con la vtable falsa (ranura 6)
        arenaA.Put32(a[0] + 8, u32(arenaC.Obj(a[6])));
        arenaC.Put32(a[6], fvt);
        arenaC.Put32(kFakeVtable + 0x18, u32(uintptr_t(&FkMakeAbort)));
        gRet = a[7];
        call.ecx = self;
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.argCount = 3; call.popBytes = 0;
        break;
    case kAnimDtor:
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 8, a[6] ? u32(arenaB.Obj(a[7])) : 0u);
        arenaA.Put32(a[0] + 0xC, a[4]);
        if (a[6]) {
            arenaB.Put32(a[7] + 0x18, a[8]);
            arenaB.Put32(a[7] + 0x1C, a[9]);
            arenaB.Put32(a[7] + 0x2C, a[10]);
        }
        gRet = a[11]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
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
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kConst: case kRead8: obs["ret"] = eax; break;                                                 // valor completo de EAX
        case kConstAL0: case kConstAL1: case kIsValid: case kMakeAbort: obs["ret"] = eax & 0xFFu; break;   // bool: solo AL
        case kSdd: case kSddPool: case kClone: obs["ret"] = Tag(uintptr_t(eax)); break;                    // this / el objeto nuevo como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN169A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN169A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn169a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn169a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn169a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    json hooksFoundDebug = json::object();
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); if (found && ModuleOf(env.route[f]->Body()) != env.asi) hooksFoundDebug[kFn[f]] = {{"body", env.route[f]->Body()}, {"module", ModuleOf(env.route[f]->Body())}}; bodies[kFn[f]] = found && env.route[f]->Body() != 0 && ModuleOf(env.route[f]->Body()) == env.asi;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { collaborators["debug"] = hooksFoundDebug; Run(env, false, registered, bodies, collaborators); return; }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete del CRT): un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto EvDel = [&] { return F(0x4B5630, &FkDelete<0x4B5630>); }; // CEvent::operator delete (con hook desde el lote 153A)
    const auto& K = kBodyFakes;
    env.stubs[0] = {F(0x571A00, &FkRef<0x571A00>), F(0x4AC480, K[0])};
    env.stubs[3] = {F(0x4B5620, &FkNew), F(0x48E1C0, &FkCtor<0x48E1C0>)};
    env.stubs[6] = {Rt(0, K[6]), EvDel()};
    env.stubs[13] = {F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[14] = {F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[15] = {F(0x4AB420, &FkAdd), F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[16] = {F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[20] = {F(0x61A3D0, K[20])};
    env.stubs[22] = {F(0x4CEBE0, &FkSetFinish), F(0x61A3A0, K[22])};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {1u, 2u, 4u, 5u, 7u, 8u, 9u, 10u, 11u, 12u, 17u, 18u, 19u, 21u}) dllReal[f] = "toda la funcion (sin colaboradores; las vtables falsas de la arena C son memoria sintetica)";
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
    // Memoria global del juego: el reloj de juego (0xB7CB84), que escriben las funciones 14 y 15, y el puntero del pool de tareas (0xB744A8), que apuntan al pool falso de la arena B las funciones 20 y 21; el resto
    // ocurre en las tres arenas propias
    regions.clear();
    regions.emplace_back(kGlobalTimer, 4);
    regions.emplace_back(kTaskPoolPtr, 4);
    for (auto& v : regionsOf) v.clear();
    regionsOf[14] = {0};
    regionsOf[15] = {0};
    regionsOf[20] = {1};
    regionsOf[21] = {1};
    Run(env, ready, registered, bodies, collaborators);
}
}
