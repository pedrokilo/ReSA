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
#include "X87Math.h"
#include "Core.hpp"

// Banco par a par Fn184A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 26 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor de tareas y de eventos
// (0x460D80, 0x669D20, 0x4B2070, 0x4B2130, 0x4B2210, 0x4B2470, 0x4B26A0, 0x674ED0, 0x4AC010), CTask::operator new 0x61A5A0 y operator delete 0x61A5B0, CEvent::operator new 0x4B5620 y operator delete 0x4B5630 (en el destructor
// con borrado de CEventInteriorUseInfo), constructores 0x460C80 y 0x4ABFC0, metodos de FxManager_c (0x4A98E0, 0x5C2420, 0x4A93E0, 0x4A98B0, 0x4A9BE0, 0x4A9810, 0x4A9A10), FxSystem_c::Play 0x4AA2F0 y Kill 0x4AA3F0,
// CTaskManager::FindActiveTaskByType 0x681740 y CPed::IsAlive 0x5E0170; operator new 0x82119A y operator delete 0x8214BD (la DLL los llama por la direccion del EXE: falso solo en ella); InitStaticSystems 0x49E660 y
// ExitStaticSystems 0x49E850 (funciones propias de la DLL sin hook: falso en la direccion del EXE y en ellas); ExitEntitySystems 0x4A12D0 (funcion probada del lote: su ruta y su cuerpo). Colaboradores REALES: el constructor
// del nodo 0x4A8DB0, el destructor del nodo 0x49EA70, AddItem 0x4A8DF0 (hook apagado) y RemoveItem 0x4A8E30 (en el EXE; la DLL lleva el codigo de la plantilla TList_c en linea) y, en los destructores con borrado de
// eventos, la liberacion en linea del pool (0xB744AC, sintetico). Memoria global tocada (restaurada tras cada lado): el puntero del pool de eventos 0xB744AC.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como
// etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue.

namespace F184ALive {
using namespace F184AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F184AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
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
inline u32 gRet2{};                  // EAX de CPed::IsAlive
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new (0 = sin memoria)
inline u32 gSelf{};                  // objeto cuyo destructor virtual consulta el falso del delete virtual
inline std::array<u32, 7> gPoison{}; // dwords que el falso del constructor de CEvent deja en +0..+0x18
inline DWORD gBenchThread{};
inline constexpr u32 kEvVDtor = 0xD7E1;

// Metodo de otro objeto con 0, 1 y 2 dwords de pila (thiscall, el callee limpia): evento [direccion del EXE, ECX (etiqueta), argumentos crudos]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkT0(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT1(void* self, void*, u32 a1) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = a1;
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT2(void* self, void*, u32 a1, u32 a2) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2;
    return gRet;
}
// CPed::IsAlive (0x5E0170; sin argumentos de pila): evento [direccion, ECX (etiqueta)]; devuelve gRet2.
inline u32 __fastcall FkIsAlive(void* self, void*) {
    Ev& e = PushEv(0x5E0170, 1);
    e.v[0] = Tag(self);
    return gRet2;
}
// Cuerpo de destructor sin argumentos de pila (thiscall): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
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
// operator new (cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// Constructor de CTaskComplexWanderCop (0x460C80; dos dwords de pila): evento [direccion, ECX (etiqueta), estado, direccion]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtor2(void* self, void*, u32 a, u32 b) {
    Ev& e = PushEv(0x460C80, 3);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CEvent (0x4ABFC0; sin argumentos de pila): evento [direccion, ECX (etiqueta), dwords +0xC, +0x10, +0x14 y +0x18 al entrar]; deja gPoison (28 bytes) en +0..+0x1B (asi se ve si la funcion probada escribe sus campos
// ANTES de llamar al constructor base); devuelve gRet.
inline u32 __fastcall FkEventCtor(void* self, void*) {
    Ev& e = PushEv(0x4ABFC0, 5);
    u8* p = static_cast<u8*>(self);
    e.v[0] = Tag(self); e.v[1] = BitsOf(p + 0xC); e.v[2] = BitsOf(p + 0x10); e.v[3] = BitsOf(p + 0x14); e.v[4] = BitsOf(p + 0x18);
    memcpy(self, gPoison.data(), 0x1C);
    return gRet;
}
// FxManager_c::LoadFxProject (0x5C2420; un puntero a cadena): evento [direccion, ECX, hash de la cadena, longitud] (la cadena de la DLL y la del EXE viven en sitios distintos: se compara el contenido).
inline u32 __fastcall FkLoadFx(void* self, void*, const char* path) {
    Ev& e = PushEv(0x5C2420, 3);
    e.v[0] = Tag(self); e.v[1] = FnvStr(path); e.v[2] = path ? u32(strlen(path)) : 0u;
    return gRet;
}
// FxManager_c::CreateFxSystem (0x4A9BE0; nombre, posicion, matriz, bandera): evento [direccion, ECX, nombre (etiqueta), posicion (etiqueta), x, y, z, matriz (etiqueta), bandera]; devuelve gRet.
inline u32 __fastcall FkCreateFx(void* self, void*, const void* name, const u32* pos, const void* matrix, u32 flag) {
    Ev& e = PushEv(0x4A9BE0, 8);
    e.v[0] = Tag(self); e.v[1] = Tag(name); e.v[2] = Tag(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = Tag(matrix); e.v[7] = flag;
    return gRet;
}
// Destructor con borrado virtual de la tarea hija (ranura 0, un dword de pila: la bandera): evento [marca, objeto (etiqueta), bandera, dword de [this] del destructor que se esta ejecutando]; devuelve el objeto.
inline u32 __fastcall FkVDtor(void* self, void*, u32 flags) {
    Ev& e = PushEv(kEvVDtor, 3);
    e.v[0] = Tag(self); e.v[1] = flags; e.v[2] = BitsOf(reinterpret_cast<const void*>(uintptr_t(gSelf)));
    return u32(uintptr_t(self));
}
inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline size_t Off(u32 base, i32 delta) { return size_t(int64(base) + delta); }

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

// Lista m_FxEntities de Fx_c (+0x44: primero, ultimo, recuento) con los nodos en la arena B: nodo i = (anterior, siguiente, sistema, entidad); la entidad de los nodos con el bit de a[10] es a[9] (solo en DestroyEntityFx)
inline void BuildList(u32 o, const std::array<u32, 64>& a, bool destroy) {
    const u32 n = a[3];
    std::array<u32, kMaxNodes> ptr{};
    for (u32 i = 0; i < kMaxNodes; ++i) ptr[i] = u32(arenaB.Obj(a[4 + i]));
    for (u32 i = 0; i < n; ++i) {
        arenaB.Put32(a[4 + i] + 0, i > 0 ? ptr[i - 1] : 0u);
        arenaB.Put32(a[4 + i] + 4, i + 1 < n ? ptr[i + 1] : 0u);
        arenaB.Put32(a[4 + i] + 8, a[11 + i]);
        arenaB.Put32(a[4 + i] + 0xC, destroy && ((a[10] >> i) & 1) ? a[9] : a[16 + i]);
    }
    arenaA.Put32(o + kListOff + 0, n ? ptr[0] : 0u);
    arenaA.Put32(o + kListOff + 4, n ? ptr[n - 1] : 0u);
    arenaA.Put32(o + kListOff + 8, n + a[21]);
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeed]); arenaB.Pattern(a[2], a[kSeed + 1]); arenaC.Pattern(a[2], a[kSeed + 2]); arenaD.Pattern(a[2], a[kSeed + 3]);
    gRet = 0; gRet2 = 0; gDtorWrite = 0; gNew = 0; gSelf = 0; gPoison = {};
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConst:
        arenaA.Put32(o, a[3]);
        break;
    case kWClone:
        arenaA.Put32(o + 0xC, a[3]); arenaA.Put32(o + 0x10, a[4]);
        gNew = a[5] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        gRet = a[7]; gDtorWrite = a[8];
        break;
    case kSdd:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kSddNB:
        arenaA.Put32(o, a[3]);
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kWDtor: {
        arenaA.Put32(o, a[3]);
        u32 child = 0;
        if (a[4]) {
            child = u32(arenaB.Obj(a[5]));
            arenaB.Put32(a[5], u32(arenaB.Obj(kVtOff)));
            arenaB.Put32(kVtOff, u32(reinterpret_cast<uintptr_t>(&FkVDtor)));
        }
        arenaA.Put32(o + 0x28, child);
        gSelf = thisA; gRet = a[6]; gDtorWrite = a[7];
        break;
    }
    case kFxCtor:
        break;
    case kFxInit:
        arenaA.Put32(o + 0x50, a[3]);
        gRet = a[4];
        break;
    case kFxReset:
    case kFxExit:
        gRet = a[4];
        break;
    case kFxCreate: {
        BuildList(o, a, false);
        const u32 name = u32(arenaD.Obj(a[23])), pos = u32(arenaD.Obj(a[24]));
        const u32 matrix = a[25] == 0 ? 0u : (a[25] == 1 ? a[26] : u32(arenaD.Obj(a[26])));
        gNew = u32(arenaC.Obj(a[28]));
        gRet = a[27];
        call.args[0] = a[22]; call.args[1] = name; call.args[2] = pos; call.args[3] = matrix; call.argCount = 4;
        break;
    }
    case kFxDestroy:
        BuildList(o, a, true);
        call.args[0] = a[9]; call.argCount = 1;
        break;
    case kFxExitEnt:
        BuildList(o, a, false);
        break;
    case kAffects: {
        const u32 ped = u32(arenaB.Obj(a[3])), intelligence = u32(arenaC.Obj(a[4]));
        arenaB.Put32(a[3] + 0x47C, intelligence);
        gRet = a[5]; gRet2 = a[6];
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kEvtSdd: {
        const i32 delta = i32(a[5]);
        const i32 idx = delta / 0x44;   // truncado hacia cero, como el EXE
        arenaB.Put32(a[4] + 0, thisA - u32(delta)); arenaB.Put32(a[4] + 4, u32(arenaD.Obj(a[6]))); arenaB.Put32(a[4] + 8, a[12]); arenaB.Put32(a[4] + 0xC, a[10]);
        arenaD.Put8(Off(a[6], idx), u8(a[11]));
        PutRaw32(kEventPoolGlobal, u32(arenaB.Obj(a[4])));
        arenaA.Put32(o, a[3]);
        gRet = a[8]; gDtorWrite = a[9];
        call.args[0] = a[7]; call.argCount = 1;
        break;
    }
    case kIuiCtor:
        for (u32 i = 0; i < 7; ++i) gPoison[i] = a[8 + i];
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.args[3] = a[6]; call.argCount = 4;
        break;
    case kIuiBody:
        arenaA.Put32(o, a[3]);
        gDtorWrite = a[5];
        break;
    case kIuiClone:
        arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[6]); arenaA.Put32(o + 0x14, a[7]); arenaA.Put32(o + 0x18, a[8]);
        gNew = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        for (u32 i = 0; i < 7; ++i) gPoison[i] = a[9 + i];
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
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kWClone: case kSdd: case kSddNB: case kFxCtor: case kEvtSdd: case kIuiCtor: case kIuiClone: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                                                       // valor completo de EAX
        case kAffects: obs["ret"] = eax & 0xFFu; break;                                                                                             // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                       // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN184A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN184A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Igual que MakeCollab para un colaborador con hook VIRTUAL (ranura de vtable): el cuerpo de la DLL es la entrada de la vtabla
inline Pair MakeCollabVirtual(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody) && vbody) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", true}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn184a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn184a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn184a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado salvo mientras se prueba ella misma); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto OWN = [&](unsigned idx, auto* fake) {
        const Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook RH en la DLL: el EXE corre sus bytes (hook apagado por su ruta) y la DLL su codigo en linea; solo se comprueba que el hook este apagado y que el EXE conserve sus bytes.
    auto REAL = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    constexpr uintptr_t kInitStatic = 0x49E660, kExitStatic = 0x49E850;
    const uintptr_t initStaticDll = std::bit_cast<uintptr_t>(&Fx_c::InitStaticSystems), exitStaticDll = std::bit_cast<uintptr_t>(&Fx_c::ExitStaticSystems);
    env.stubs[1]  = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x460C80, &FkCtor2)};
    env.stubs[3]  = {F(0x460D80, &FkBody<0x460D80>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[4]  = {F(0x669D20, &FkBody<0x669D20>)};
    env.stubs[6]  = {F(0x4A98E0, &FkT0<0x4A98E0>), F(0x5C2420, &FkLoadFx), F(0x4A93E0, &FkT2<0x4A93E0>), D(kInitStatic, initStaticDll, &FkT0<0x49E660>)};
    env.stubs[7]  = {F(0x4A98B0, &FkT0<0x4A98B0>), D(kInitStatic, initStaticDll, &FkT0<0x49E660>)};
    env.stubs[8]  = {ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[9]  = {F(0x4A9BE0, &FkCreateFx), ByExeAddress(0x82119A, &FkNew<0x82119A>), F(0x4AA2F0, &FkT0<0x4AA2F0>)};
    env.reals[9]  = {REAL(0x4A8DF0)};
    env.stubs[10] = {F(0x4AA3F0, &FkT0<0x4AA3F0>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[11] = {F(0x4A9810, &FkT1<0x4A9810>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[12] = {OWN(11, &FkT0<0x4A12D0>), D(kExitStatic, exitStaticDll, &FkT0<0x49E850>), F(0x4A9A10, &FkT0<0x4A9A10>)};
    env.stubs[16] = {F(0x681740, &FkT1<0x681740>), F(0x5E0170, &FkIsAlive)};
    env.stubs[17] = {F(0x4B2070, &FkBody<0x4B2070>)};
    env.stubs[18] = {F(0x4B2130, &FkBody<0x4B2130>)};
    env.stubs[19] = {F(0x4B2210, &FkBody<0x4B2210>)};
    env.stubs[20] = {F(0x4B2470, &FkBody<0x4B2470>)};
    env.stubs[21] = {F(0x4B26A0, &FkBody<0x4B26A0>)};
    env.stubs[22] = {F(0x4ABFC0, &FkEventCtor)};
    env.stubs[23] = {F(0x4AC010, &FkBody<0x4AC010>)};
    env.stubs[24] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4ABFC0, &FkEventCtor)};
    env.stubs[25] = {F(0x674ED0, &FkBody<0x674ED0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 2u, 5u, 13u, 14u, 15u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[8] = "toda la funcion salvo operator delete (0x8214BD: la DLL lo llama por la direccion del EXE, falso); el destructor del nodo (0x49EA70, `ret`) corre real en el EXE";
    dllReal[9] = "toda la funcion salvo CreateFxSystem, Play (con hook, falso identico en el EXE y en la DLL), operator new (0x82119A, por la direccion del EXE, falso); el constructor del nodo (0x4A8DB0), AddItem (0x4A8DF0, hook apagado) corren reales en el EXE y en linea en la DLL";
    dllReal[10] = "toda la funcion salvo Kill (con hook, falso) y operator delete (0x8214BD, falso); RemoveItem (0x4A8E30) y el destructor del nodo (0x49EA70) corren reales en el EXE y en linea en la DLL";
    dllReal[11] = "toda la funcion salvo DestroyFxSystem (con hook, falso) y operator delete (0x8214BD, falso); RemoveItem (0x4A8E30) corre real en el EXE y en linea en la DLL";
    dllReal[12] = "toda la funcion salvo ExitEntitySystems (0x4A12D0: funcion probada del lote, con hook; falso identico en su direccion del EXE y en su cuerpo de la DLL), ExitStaticSystems (0x49E850, funcion propia de la DLL sin hook: falso en el EXE y en ella) y FxManager_c::Exit";
    dllReal[17] = dllReal[18] = dllReal[19] = dllReal[20] = dllReal[21] = "toda la funcion salvo el cuerpo del destructor (falseado); el borrado de la ranura del pool (CEvent::operator delete) corre real sobre un pool sintetico";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& p : env.reals[f]) {
            env.watches.emplace_back(p.exe, 16);
            real.push_back({{"exe", p.exe}, {"dll", p.dll != 0}, {"hooked", p.hooked}, {"dll_addr", p.dll}, {"dll_module", ModuleOf(p.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kEventPoolGlobal, 4);
    for (unsigned f : {17u, 18u, 19u, 20u, 21u}) regionsOf[f] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
