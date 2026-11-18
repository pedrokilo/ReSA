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

// Banco par a par Fn231A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 18 funciones de la libjpeg 6b embebida en el EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Cada caso construye una jpeg_compress_struct sintetica en la arena A (con sus estructuras
// auxiliares: err, mem, progress, dest, master, marker, submodulos y 8 componentes), tablas de cuantificacion y Huffman en la arena D, el bufer de salida en la arena C y el destino de alloc_small en la B.
// TODOS los colaboradores de estas funciones se llaman por punteros de la propia estructura (error_exit, emit_message, empty_output_buffer, alloc_small, free_pool, self_destruct, start_pass de los submodulos,
// write_frame_header / write_scan_header del escritor de marcadores): los falsos son los mismos en el EXE y en la DLL por construccion. Los colaboradores por direccion fija son codigo real del EXE en ambos lados:
// jdiv_round_up (0x5CF890) y, cuando una funcion llama a otra del lote (WriteFileHeader -> EmitJfifApp0 / EmitAdobeApp14; WriteFrameHeader -> EmitDqt / EmitSof), el EXE corre las suyas (hook apagado) y la DLL las
// suyas (cuerpo propio). error_exit de la libjpeg real no vuelve: con P_ERRMODE = 0 el falso sale de la llamada con longjmp (ver Core.hpp, Gate); con P_ERRMODE = 1 vuelve y el cuerpo sigue (como el EXE).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos y un resumen FNV de la
// estructura y del maestro en cada llamada a un submodulo o escritor, que fija el orden de las escrituras respecto a las llamadas), el valor devuelto y si la llamada salio por error_exit.

namespace F231ALive {
using namespace F231AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{0x5C55D0, 0x5C5780, 0x5C5910, 0x5C5940, 0x5C5AC0, 0x5C5CA0, 0x5C5EC0, 0x5C60B0, 0x5C6100, 0x5C62D0, 0x5C63D0, 0x5C6850, 0x5C69E0, 0x5C6A90, 0x5C6B50, 0x5C6B90, 0x5C6BB0, 0x5C6BD0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F231AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    // Cambios respecto a la imagen de referencia, en tramos contiguos: [[desplazamiento respecto a la ventana del primer byte, bytes nuevos en hexadecimal]]
    json Changes() const {
        json out = json::array();
        constexpr char d[] = "0123456789abcdef";
        for (size_t i = 0; i < N;) {
            if (m[i] == r[i]) { ++i; continue; }
            std::string hex;
            const size_t start = i;
            while (i < N && m[i] != r[i]) { hex += d[m[i] >> 4]; hex += d[m[i] & 15]; ++i; }
            out.push_back(json::array({int64(start) - int64(kGuard), hex}));
        }
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

inline u32 Rd32(uintptr_t a) { u32 v; memcpy(&v, reinterpret_cast<const void*>(a), 4); return v; }
inline void Wr32(uintptr_t a, u32 v) { memcpy(reinterpret_cast<void*>(a), &v, 4); }
inline void Wr16(uintptr_t a, u32 v) { const u16 w = u16(v); memcpy(reinterpret_cast<void*>(a), &w, 2); }
inline void Wr8(uintptr_t a, u32 v) { *reinterpret_cast<u8*>(a) = u8(v); }

// ================================================================================================ falsos (se llaman por punteros de la estructura: iguales en el EXE y en la DLL)
inline u32 gErrMode{};                 // 0: error_exit no vuelve (longjmp a Gate)
inline u32 gEobRet{};                  // EAX que devuelve empty_output_buffer (AL = resultado; el resto, ruido)
inline u32 gDestStart{}, gDestSize{};  // bufer de salida: tras empty_output_buffer el destino vuelve a empezar
inline u32 gAllocRet{};                // lo que devuelve alloc_small
inline DWORD gBenchThread{};

inline u32 StateHash(u32 cinfo, u32& masterHash) {
    masterHash = Fnv(reinterpret_cast<const void*>(Rd32(cinfo + 0x13C)), 0x20);
    return Fnv(reinterpret_cast<const void*>(cinfo), 0x160);
}
inline void __cdecl FkErrExit(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci)), err = Rd32(cinfo);
    Ev& e = PushEv(1, 4);
    e.v[0] = Rd32(err + 0x14); e.v[1] = Rd32(err + 0x18); e.v[2] = Rd32(err + 0x1C); e.v[3] = Tag(uintptr_t(cinfo));
    if (gErrMode == 0 && gEscArmed) longjmp(gEscBuf, 1);
}
inline void __cdecl FkEmitMsg(void* ci, i32 level) {
    const u32 cinfo = u32(uintptr_t(ci));
    Ev& e = PushEv(2, 3);
    e.v[0] = u32(level); e.v[1] = Rd32(Rd32(cinfo) + 0x14); e.v[2] = Tag(uintptr_t(cinfo));
}
inline u32 __cdecl FkEmpty(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci)), dest = Rd32(cinfo + 0x18);
    Ev& e = PushEv(3, 4);
    e.v[0] = Tag(uintptr_t(Rd32(dest))); e.v[1] = Rd32(dest + 4); e.v[2] = Fnv(reinterpret_cast<const void*>(gDestStart), gDestSize); e.v[3] = Tag(uintptr_t(cinfo));
    Wr32(dest, gDestStart); Wr32(dest + 4, gDestSize);
    return gEobRet;
}
inline u32 __cdecl FkAlloc(void* ci, i32 pool, u32 size) {
    Ev& e = PushEv(4, 3);
    e.v[0] = u32(pool); e.v[1] = size; e.v[2] = Tag(uintptr_t(ci));
    return gAllocRet;
}
inline void __cdecl FkFreePool(void* ci, i32 pool) {
    Ev& e = PushEv(5, 2);
    e.v[0] = u32(pool); e.v[1] = Tag(uintptr_t(ci));
    const u32 cinfo = u32(uintptr_t(ci));
    Wr32(cinfo + 0x14, 0xDEAD0002u); Wr32(cinfo + 0x10C, 0xDEAD0003u);   // el cuerpo debe sobrescribirlos despues de llamar
}
inline void __cdecl FkSelfDestruct(void* ci) {
    Ev& e = PushEv(6, 1);
    e.v[0] = Tag(uintptr_t(ci));
    Wr32(u32(uintptr_t(ci)) + 0x14, 0xDEAD0001u);
}
template<u32 ID> inline void __cdecl FkStart1(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci));
    u32 mh{}; const u32 ch = StateHash(cinfo, mh);
    Ev& e = PushEv(10 + ID, 4);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = 0; e.v[2] = ch; e.v[3] = mh;
}
template<u32 ID> inline void __cdecl FkStart2(void* ci, u32 mode) {
    const u32 cinfo = u32(uintptr_t(ci));
    u32 mh{}; const u32 ch = StateHash(cinfo, mh);
    Ev& e = PushEv(10 + ID, 4);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = mode; e.v[2] = ch; e.v[3] = mh;
}
template<u32 ID> inline void __cdecl FkWrite(void* ci) {   // 20 = write_frame_header, 21 = write_scan_header
    const u32 cinfo = u32(uintptr_t(ci));
    u32 mh{}; const u32 ch = StateHash(cinfo, mh);
    Ev& e = PushEv(ID, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = ch; e.v[2] = mh;
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
    u32 argCount{}, popBytes{}, eax{}, ecx{}, edx{};
};

// Prepara arenas y la escena del caso (jpeg_compress_struct y estructuras auxiliares); devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    const auto& a = c.a;
    arenaA.Pattern(a[P_FILL], a[SEED0]); arenaB.Pattern(a[P_FILL], a[SEED1]); arenaC.Pattern(a[P_FILL], a[SEED2]); arenaD.Pattern(a[P_FILL], a[SEED3]);
    const u32 S = u32(arenaA.Obj(a[P_OFF])), D0 = u32(arenaD.Obj(a[P_DOFF])), C0 = u32(arenaC.Obj(a[P_COFF])), B0 = u32(arenaB.Obj(a[P_BOFF]));
    gErrMode = a[P_ERRMODE]; gEobRet = a[P_EAXHI] | (a[P_EOBRET] & 0xFF); gDestStart = C0; gDestSize = a[P_DSIZE]; gAllocRet = B0;
    nev = 0;

    // tablas de cuantificacion y Huffman (arena D)
    for (u32 i = 0; i < 4; ++i) {
        const u32 t = D0 + kQStride * i;
        for (u32 k = 0; k < 64; ++k) Wr16(t + 2 * k, QuantWord(a[QDESC0 + i], k));
        Wr8(t + 0x80, a[QSENT0 + i]);
    }
    for (u32 j = 0; j < 8; ++j) {
        const u32 t = D0 + kHBase + kHStride * j;
        for (u32 i = 0; i <= 16; ++i) Wr8(t + i, HuffBits(a[HDESC0 + j], i));
        for (u32 k = 0; k < 256; ++k) Wr8(t + 0x11 + k, HuffVal(a[HDESC0 + j], k));
        Wr8(t + 0x111, a[HSENT0 + j]);
    }
    // componentes
    for (u32 i = 0; i < 8; ++i) {
        const u32 p = S + kComps + 0x54 * i;
        const u32* k = &a[COMP0 + kCompFields * i];
        Wr32(p + 0x00, k[CF_ID]); Wr32(p + 0x08, k[CF_H]); Wr32(p + 0x0C, k[CF_V]); Wr32(p + 0x10, k[CF_Q]); Wr32(p + 0x14, k[CF_DC]); Wr32(p + 0x18, k[CF_AC]);
        Wr32(p + 0x1C, k[CF_WIB]); Wr32(p + 0x20, k[CF_HIB]);
    }
    // estructuras auxiliares
    Wr32(S + kErr + 0x00, u32(uintptr_t(&FkErrExit))); Wr32(S + kErr + 0x04, u32(uintptr_t(&FkEmitMsg)));
    Wr32(S + kMem + 0x00, u32(uintptr_t(&FkAlloc))); Wr32(S + kMem + 0x24, u32(uintptr_t(&FkFreePool))); Wr32(S + kMem + 0x28, u32(uintptr_t(&FkSelfDestruct)));
    Wr32(S + kDst + 0x00, C0 + (a[P_DSIZE] - a[P_DFREE])); Wr32(S + kDst + 0x04, a[P_DFREE]); Wr32(S + kDst + 0x0C, u32(uintptr_t(&FkEmpty)));
    Wr8(S + kMst + 0x0C, a[M_CALLST]); Wr8(S + kMst + 0x0D, a[M_ISLAST]); Wr32(S + kMst + 0x10, a[M_PTYPE]); Wr32(S + kMst + 0x14, a[M_PNUM]); Wr32(S + kMst + 0x18, a[M_TOTAL]); Wr32(S + kMst + 0x1C, a[M_SCANNO]);
    Wr32(S + kMrk + 0x04, u32(uintptr_t(&FkWrite<20>))); Wr32(S + kMrk + 0x08, u32(uintptr_t(&FkWrite<21>))); Wr32(S + kMrk + 0x1C, a[K_LASTRI]);
    Wr32(S + kSubs + 8 * 0, u32(uintptr_t(&FkStart1<0>))); Wr32(S + kSubs + 8 * 1, u32(uintptr_t(&FkStart1<1>))); Wr32(S + kSubs + 8 * 2, u32(uintptr_t(&FkStart2<2>)));
    Wr32(S + kSubs + 8 * 3, u32(uintptr_t(&FkStart1<3>))); Wr32(S + kSubs + 8 * 4, u32(uintptr_t(&FkStart2<4>))); Wr32(S + kSubs + 8 * 5, u32(uintptr_t(&FkStart2<5>))); Wr32(S + kSubs + 8 * 6, u32(uintptr_t(&FkStart2<6>)));
    // jpeg_compress_struct
    const u32 ci = S + kCi;
    Wr32(ci + 0x00, S + kErr); Wr32(ci + 0x04, a[P_NOMEM] ? 0u : S + kMem); Wr32(ci + 0x08, a[P_NOPROG] ? 0u : S + kPrg);
    Wr8(ci + 0x10, a[F_ISDEC]); Wr32(ci + 0x14, a[F_STATE]); Wr32(ci + 0x18, S + kDst);
    Wr32(ci + 0x1C, a[F_WIDTH]); Wr32(ci + 0x20, a[F_HEIGHT]); Wr32(ci + 0x24, a[F_INCOMP]); Wr32(ci + 0x38, a[F_PREC]); Wr32(ci + 0x3C, a[F_NCOMP]); Wr32(ci + 0x40, a[F_CSPACE]);
    Wr32(ci + 0x44, S + kComps);
    for (u32 i = 0; i < 4; ++i) {
        Wr32(ci + 0x48 + 4 * i, (a[P_QMASK] >> i) & 1 ? 0u : D0 + kQStride * i);
        Wr32(ci + 0x58 + 4 * i, (a[P_DCMASK] >> i) & 1 ? 0u : D0 + kHBase + kHStride * i);
        Wr32(ci + 0x68 + 4 * i, (a[P_ACMASK] >> i) & 1 ? 0u : D0 + kHBase + kHStride * (4 + i));
        Wr32(ci + 0xE8 + 4 * i, S + kComps + 0x54 * a[CUR0 + i]);
    }
    Wr32(ci + 0xA8, a[F_NSCANS]); Wr32(ci + 0xAC, a[F_SCANINFO]);
    Wr8(ci + 0xB0, a[F_RAWIN]); Wr8(ci + 0xB1, a[F_ARITH]); Wr8(ci + 0xB2, a[F_OPT]);
    Wr32(ci + 0xBC, a[F_RINT]); Wr32(ci + 0xC0, a[F_RROWS]);
    Wr8(ci + 0xC4, a[F_JFIF]); Wr8(ci + 0xC5, a[F_JMAJ]); Wr8(ci + 0xC6, a[F_JMIN]); Wr8(ci + 0xC7, a[F_DUNIT]); Wr16(ci + 0xC8, a[F_XD]); Wr16(ci + 0xCA, a[F_YD]); Wr8(ci + 0xCC, a[F_ADOBE]);
    Wr8(ci + 0xD4, a[F_PROG]); Wr32(ci + 0xD8, a[F_MAXH]); Wr32(ci + 0xDC, a[F_MAXV]); Wr32(ci + 0xE4, a[F_CINSCAN]);
    Wr32(ci + 0x12C, a[F_SS]); Wr32(ci + 0x130, a[F_SE]); Wr32(ci + 0x134, a[F_AH]); Wr32(ci + 0x138, a[F_AL]);
    Wr32(ci + 0x13C, S + kMst); Wr32(ci + 0x140, S + kSubs + 8 * 6); Wr32(ci + 0x144, S + kSubs + 8 * 2); Wr32(ci + 0x148, S + kSubs + 8 * 5); Wr32(ci + 0x14C, S + kMrk);
    Wr32(ci + 0x150, S + kSubs + 8 * 0); Wr32(ci + 0x154, S + kSubs + 8 * 1); Wr32(ci + 0x158, S + kSubs + 8 * 3); Wr32(ci + 0x15C, S + kSubs + 8 * 4);

    call.eax = c.eax; call.ecx = c.ecx; call.edx = c.edx;
    switch (c.fn) {
    case 0: call.eax = S; call.args[0] = a[P_ARG]; call.argCount = 1; call.popBytes = 4; break;                                  // EAX = cinfo, indice en pila
    case 1: call.ecx = S; call.eax = a[P_EAXHI] | (a[P_ARG2] & 0xFF); call.args[0] = a[P_ARG]; call.argCount = 1; call.popBytes = 4; break;   // ECX = cinfo, AL = es AC
    case 2: case 4: case 5: case 6: call.eax = S; break;                                                                         // EAX = cinfo
    case 3: call.ecx = S; call.eax = a[P_EAXHI] | (a[P_ARG2] & 0xFF); break;                                                     // ECX = cinfo, AL = marcador
    case 13: call.args[0] = S; call.args[1] = a[P_ARG]; call.argCount = 2; call.popBytes = 8; break;                             // cdecl(cinfo, transcode_only)
    default: call.args[0] = S; call.argCount = 1; call.popBytes = 4; break;                                                      // cdecl(cinfo)
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
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, call.edx, c.stack, call.popBytes, false);
        });
        const bool escaped = gEscaped;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["escaped"] = escaped;
        const u32 eax = sides[s].reg.eax;
        if (escaped) obs["ret"] = nullptr;
        else if (fn == 0) obs["ret"] = eax;                                  // la precision (EAX completo)
        else if (fn == 16 || fn == 17) obs["ret"] = Tag(uintptr_t(eax));     // la tabla devuelta
        else obs["ret"] = nullptr;                                           // void
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN231A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN231A_CASES", buffer, sizeof(buffer)) > 0) {
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

// Busca el hook registrado para una direccion del EXE (categoria y nombre).
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

// Colaborador REAL con hook RH en la DLL (jdiv_round_up, de otro lote): la ruta deja su hook apagado (en ambos lados el EXE corre sus bytes y la DLL la llama por la direccion del EXE).
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
    if (!std::filesystem::exists(dir / "fn231a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn231a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn231a"); }
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
    // Colaboradores REALES por direccion fija (sin falso): el EXE corre sus bytes (hook apagado) y la DLL llama a su propio cuerpo (o a la direccion del EXE si no tiene hook: jdiv_round_up)
    auto Real = [](uintptr_t exe) { return Pair{exe, 0, false}; };
    env.reals[7] = {Real(0x5C5CA0), Real(0x5C5EC0)};     // WriteFileHeader -> EmitJfifApp0, EmitAdobeApp14
    env.reals[8] = {Real(0x5C55D0), Real(0x5C5940)};     // WriteFrameHeader -> EmitDqt, EmitSof
    {   // jdiv_round_up (0x5CF890) tiene hook en la DLL (lote 235A, sin validar): su ruta lo deja apagado; la DLL del banco llama por la direccion del EXE, que corre el codigo original
        const Pair jdiv = MakeCollab(env, 0x5CF890, hooksFound);
        if (!jdiv.hooked || !jdiv.dll) ready = false;
        env.reals[11] = {jdiv};                          // PrepareForPass -> jdiv_round_up (PerScanSetup)
        env.reals[13] = {jdiv};                          // JinitCMasterControl -> jdiv_round_up (InitialSetup)
    }
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"toda la funcion; los colaboradores se llaman por punteros de la estructura (falsos iguales en ambos lados) o son codigo real del EXE (jdiv_round_up y las funciones del lote)"})},
                                 {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    env.watches.emplace_back(0x86B818, 0x100);   // jpeg_natural_order (EmitDqt)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
