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

// Banco par a par Fn177A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 26 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): constructor de CEvent 0x4ABFC0, cuerpo del
// destructor de CEvent 0x4AC010, CEvent::operator new 0x4B5620 y operator delete 0x4B5630, CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00, constructor base de CTaskSimple 0x61A390, ~CTask 0x61A3A0,
// CTask::operator new 0x61A5A0 y operator delete 0x61A5B0, cuerpo del destructor de CTaskSimpleDie 0x62FB40, de CTaskComplexDie 0x6300C0, de CTaskSimpleDead 0x6305F0, de CTaskSimpleTired 0x630F60, de CTaskSimpleSitDown 0x631070 y de
// CTaskLeanOnVehicle 0x661030, StartAnim de CTaskSimpleTired 0x630F70, constructor de CTaskComplexEnterCar 0x63A220 y su cuerpo del destructor 0x63DFA0, y CFileMgr::SetDir 0x5387D0, OpenFile 0x538900 y CloseFile 0x5389D0 y
// CFileLoader::LoadLine 0x536F80 (todos con hook), mas las propias funciones del lote que llaman otras (constructores 0x5F6190 y 0x640430 y cuerpos 0x5F6200, 0x62FF10 y 0x62FF60). CMatrix * CVector (0x59C890, con hook) se falsea en
// la direccion del EXE (cdecl: resultado, matriz, vector) y, en la DLL, en el ayudante Hp65MatrixTimesPointCpp que llama CalcTargetPos (cdecl: matriz, vector, resultado): el mismo evento y el mismo resultado. sscanf corre real
// en ambos lados (el CRT del EXE y el de la DLL; los textos de surface.dat usan fracciones binarias exactas y los valores reales de surface.dat). Memoria global del juego tocada (restaurada tras cada lado): el reloj 0xB7CB84
// la tabla de model infos 0xA9B0C8 y la lista de emisoras favoritas 0xB78E58 (solo la marca de la prueba negativa).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como
// etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue.

namespace F177ALive {
using namespace F177AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F177AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new (0 = sin memoria)
inline std::array<u32, 3> gMul{};    // lo que escribe el falso de CMatrix * CVector
inline std::vector<std::string> gLines;   // lineas de surface.dat que sirve el falso de LoadLine
inline u32 gLineIdx{};
inline std::array<char, 1024> gLineBuf{};
inline std::array<void*, 16> gFakeVtable{};   // vtable falsa de CTaskSimpleTired::ProcessPed: la ranura 6 (MakeAbortable) es FkTiredAbort
inline DWORD gBenchThread{};

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
// Cuerpo del destructor / constructor sin argumentos de pila (thiscall): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con dos dwords de pila (thiscall, `ret 8`): evento [direccion, ECX (etiqueta), a, b]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor2(void* self, void*, u32 a, u32 b) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b;
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
// Constructor con cinco dwords de pila (thiscall, `ret 14h`): evento [direccion, ECX (etiqueta), a, b, c, d, e]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor5(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 f) {
    Ev& e = PushEv(ADDR, 6);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = f;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::RegisterReference / CleanUpOldReference y StartAnim (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// CFileMgr::SetDir (0x5387D0; cdecl, una cadena): evento [direccion, FNV de la cadena]; devuelve 0.
inline int32 __cdecl FkSetDir(const char* path) {
    Ev& e = PushEv(0x5387D0, 1);
    e.v[0] = FnvStr(path);
    return 0;
}
// CFileMgr::OpenFile (0x538900; cdecl, ruta y modo): evento [direccion, FNV de la ruta, FNV del modo]; devuelve gRet (el "FILE*" de la prueba).
inline u32 __cdecl FkOpen(const char* path, const char* mode) {
    Ev& e = PushEv(0x538900, 2);
    e.v[0] = FnvStr(path); e.v[1] = FnvStr(mode);
    return gRet;
}
// CFileLoader::LoadLine (0x536F80; cdecl, el "FILE*"): evento [direccion, argumento, indice de la linea]; devuelve la linea siguiente (copia en un buffer propio) o nulo al acabar.
inline char* __cdecl FkLoadLine(u32 stream) {
    Ev& e = PushEv(0x536F80, 2);
    e.v[0] = stream; e.v[1] = gLineIdx;
    if (gLineIdx >= gLines.size()) return nullptr;
    const std::string& l = gLines[gLineIdx++];
    gLineBuf.fill(0);
    memcpy(gLineBuf.data(), l.data(), std::min(l.size(), gLineBuf.size() - 1));
    return gLineBuf.data();
}
// CFileMgr::CloseFile (0x5389D0; cdecl, el "FILE*"): evento [direccion, argumento]; devuelve 0.
inline int32 __cdecl FkClose(u32 stream) {
    Ev& e = PushEv(0x5389D0, 1);
    e.v[0] = stream;
    return 0;
}
// Ranura 6 (MakeAbortable) de la vtable falsa de CTaskSimpleTired (thiscall, tres dwords de pila, `ret 0Ch`): evento [0x630FF0, ECX (etiqueta), ped (etiqueta), prioridad, evento]; devuelve 1.
inline u32 __fastcall FkTiredAbort(void* self, void*, void* ped, u32 priority, u32 event) {
    Ev& e = PushEv(0x630FF0, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = priority; e.v[3] = event;
    return 1;
}
// CMatrix * CVector (0x59C890) en el EXE (cdecl: resultado, matriz, vector): evento [direccion, resultado (etiqueta), matriz (etiqueta), vector (los de pila como 0x7F000000), x, y, z del vector]; escribe gMul en el resultado y lo devuelve.
inline u32* __cdecl FkMulExe(u32* out, u32* matrix, u32* vector) {
    Ev& e = PushEv(0x59C890, 6);
    e.v[0] = Tag(out); e.v[1] = Tag(matrix); e.v[2] = TagStk(vector);
    e.v[3] = vector[0]; e.v[4] = vector[1]; e.v[5] = vector[2];
    out[0] = gMul[0]; out[1] = gMul[1]; out[2] = gMul[2];
    return out;
}
// Lo mismo en la DLL: Hp65MatrixTimesPointCpp (cdecl: matriz, vector, resultado; sin valor de retorno): el evento y el resultado son los de FkMulExe.
inline void __cdecl FkMulDll(u32* matrix, u32* vector, u32* out) {
    Ev& e = PushEv(0x59C890, 6);
    e.v[0] = Tag(out); e.v[1] = Tag(matrix); e.v[2] = TagStk(vector);
    e.v[3] = vector[0]; e.v[4] = vector[1]; e.v[5] = vector[2];
    out[0] = gMul[0]; out[1] = gMul[1]; out[2] = gMul[2];
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
    arenaA.Pattern(a[2], a[kSeed]); arenaB.Pattern(a[2], a[kSeed + 1]); arenaC.Pattern(a[2], a[kSeed + 2]); arenaD.Pattern(a[2], a[kSeed + 3]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gMul = {}; gLines.clear(); gLineIdx = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConst:
        if (c.fn == 0) *reinterpret_cast<u32*>(0xB78E58) = 0x5A5A5A5Au;   // marca que la prueba negativa (mutate.py) lee para que la funcion sin `this` tenga algo observable; el juego real nunca la pone (son indices de emisora)
        break;
    case kBaseCtor:
    case kBody:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kMakeAb:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.argCount = 3;
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kLoadAdh: {
        gRet = a[4];
        size_t at = 0;
        while (at < c.s.size()) {
            size_t end = at;
            while (end < c.s.size() && c.s[end] != 0) ++end;
            gLines.emplace_back(reinterpret_cast<const char*>(c.s.data()) + at, end - at);
            at = end + 1;
        }
        break;
    }
    case kEvCtor:
        gRet = a[8]; gDtorWrite = a[9];
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.args[3] = a[7]; call.argCount = 4;
        break;
    case kEvBody:
        arenaA.Put32(o + 0x10, a[4]);
        gDtorWrite = a[5]; gRet = a[6];
        break;
    case kEvClone:
        arenaA.Put32(o + 0xC, a[8]); arenaA.Put32(o + 0x10, a[9]); arenaA.Put32(o + 0x14, a[10]); arenaA.Put32(o + 0x18, a[11]);
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        break;
    case kTiredProc:
        arenaA.Put32(o, u32(uintptr_t(gFakeVtable.data())));
        arenaA.Put32(o + 0xC, a[7]); arenaA.Put32(o + 0x10, a[8]);
        arenaA.Put32(o + 0x14, (a[5] & 0xFFu) | ((a[6] & 0xFFu) << 8) | (a[11] << 16));
        arenaA.Put32(o + 0x18, a[4]);
        *reinterpret_cast<u32*>(kClockAddr) = a[9];
        call.args[0] = a[10]; call.argCount = 1;
        break;
    case kDragCtor:
        gRet = a[7]; gDtorWrite = a[6];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
    case kDragBody:
        arenaA.Put32(o + 0x50, a[4]);
        gDtorWrite = a[5]; gRet = a[6];
        break;
    case kDragClone:
        arenaA.Put32(o + 0x20, a[8]); arenaA.Put32(o + 0x50, a[9]);
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        break;
    case kCalc: {
        const u32 veh = u32(arenaB.Obj(a[4])), ped = u32(arenaB.Obj(a[5]));
        const u32 vmat = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7])), pmat = a[8] == 0 ? 0u : u32(arenaB.Obj(a[9]));
        const u32 mi = u32(arenaC.Obj(a[28])), col = u32(arenaC.Obj(a[29]));
        arenaA.Put32(o + 0xC, veh);
        arenaB.Put32(a[4] + 4, a[10]); arenaB.Put32(a[4] + 8, a[11]); arenaB.Put32(a[4] + 0xC, a[12]);
        arenaB.Put32(a[4] + 0x10, a[13]);
        arenaB.Put32(a[4] + 0x14, vmat);
        arenaB.Put32(a[4] + 0x20, (a[26] << 16) | (a[27] & 0xFFFFu));
        if (vmat) {
            for (u32 i = 0; i < 3; ++i) { arenaB.Put32(a[7] + 4 * i, a[14 + i]); arenaB.Put32(a[7] + 0x30 + 4 * i, a[17 + i]); }
        }
        arenaB.Put32(a[5] + 4, a[20]); arenaB.Put32(a[5] + 8, a[21]); arenaB.Put32(a[5] + 0xC, a[22]);
        arenaB.Put32(a[5] + 0x14, pmat);
        if (pmat) {
            for (u32 i = 0; i < 3; ++i) arenaB.Put32(a[9] + 0x30 + 4 * i, a[23 + i]);
        }
        arenaC.Put32(a[28] + 0x14, col);
        for (u32 i = 0; i < 3; ++i) { arenaC.Put32(a[29] + 4 * i, a[30 + i]); arenaC.Put32(a[29] + 0xC + 4 * i, a[33 + i]); }
        *reinterpret_cast<u32*>(kModelTable + 4 * a[26]) = mi;
        gMul = {a[36], a[37], a[38]};
        call.args[0] = u32(arenaC.Obj(a[39])); call.args[1] = ped; call.argCount = 2;
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
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = kKind[fn] == kCalc ? json::array() : RegionChanges();   // la tabla de model infos (80000 bytes) solo se vigila con el hash
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kBaseCtor: case kSdd: case kEvCtor: case kEvClone: case kDragCtor: case kDragClone: case kCalc: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                                                          // valor completo de EAX
        case kMakeAb: case kTiredProc: obs["ret"] = eax & 0xFFu; break;                                                                                // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN177A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN177A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn177a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn177a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn177a"); }
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
    gFakeVtable.fill(nullptr);
    gFakeVtable[6] = reinterpret_cast<void*>(&FkTiredAbort);
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
    env.stubs[1] = {F(0x5387D0, &FkSetDir), F(0x538900, &FkOpen), F(0x536F80, &FkLoadLine), F(0x5389D0, &FkClose)};
    env.stubs[2] = {F(0x4ABFC0, &FkBody<0x4ABFC0>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[3] = {F(0x571A00, &FkRef<0x571A00>), F(0x4AC010, &FkBody<0x4AC010>)};
    env.stubs[4] = {F(0x4B5620, &FkNew<0x4B5620>), Rt(2, &FkCtor4<0x5F6190>)};
    env.stubs[5] = {Rt(3, &FkBody<0x5F6200>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[7] = {F(0x62FB40, &FkBody<0x62FB40>)};
    env.stubs[8] = {F(0x61A390, &FkBody<0x61A390>)};
    env.stubs[11] = {F(0x61A3A0, &FkBody<0x61A3A0>)};
    env.stubs[12] = {Rt(7, &FkBody<0x62FF10>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[13] = {Rt(11, &FkBody<0x62FF60>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[14] = {F(0x6300C0, &FkBody<0x6300C0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[15] = {F(0x6305F0, &FkBody<0x6305F0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[16] = {F(0x630F60, &FkBody<0x630F60>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[17] = {F(0x630F70, &FkRef<0x630F70>)};
    env.stubs[18] = {F(0x631070, &FkBody<0x631070>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[19] = {F(0x63A220, &FkCtor5<0x63A220>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[21] = {F(0x571A00, &FkRef<0x571A00>), F(0x63DFA0, &FkBody<0x63DFA0>)};
    env.stubs[22] = {F(0x61A5A0, &FkNew<0x61A5A0>), Rt(19, &FkCtor2<0x640430>)};
    env.stubs[25] = {F(0x661030, &FkBody<0x661030>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    {   // CMatrix * CVector (0x59C890, con hook): se apaga el hook (ruta) y se falsea en la direccion del EXE; la DLL no lo llama por su hook sino por el ayudante Hp65MatrixTimesPointCpp (falso solo en la DLL)
        auto it = collabCache.find(0x59C890);
        if (it == collabCache.end()) it = collabCache.emplace(0x59C890, MakeCollab(env, 0x59C890, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        const uintptr_t helper = Body(uintptr_t(&Hp65MatrixTimesPointCpp));
        if (!helper || ModuleOf(helper) != env.asi) ready = false;
        env.stubs[24] = {S(Pair{0x59C890, 0, false}, reinterpret_cast<void*>(&FkMulExe))};
        env.dllOnly.push_back(StubSpec{Pair{0, helper, true}, reinterpret_cast<void*>(&FkMulDll)});
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 6u, 9u, 10u, 20u, 23u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[1] = "toda la funcion salvo SetDir, OpenFile, LoadLine y CloseFile (falsos); sscanf corre real (CRT de la DLL frente al CRT del EXE)";
    dllReal[24] = "toda la funcion salvo CMatrix * CVector (falso en la direccion del EXE y en el ayudante Hp65MatrixTimesPointCpp de la DLL, con el orden de argumentos de cada lado)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array(), dllOnly = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        if (f == 24) for (const auto& s : env.dllOnly) dllOnly.push_back({{"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", dllOnly}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: el reloj (kTiredProc) y la tabla de model infos (kCalc); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(0xB78E58, 56); regionsOf[0] = {regions.size() - 1};   // CStats::FavoriteRadioStationList (14 dwords): la marca de la prueba negativa
    regions.emplace_back(kClockAddr, 4); regionsOf[17] = {regions.size() - 1};
    regions.emplace_back(kModelTable, kModelCount * 4); regionsOf[24] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
