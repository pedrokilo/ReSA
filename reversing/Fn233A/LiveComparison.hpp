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

// Banco par a par Fn233A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 12 funciones de la parte de descompresion de la libjpeg 6b embebida en el EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Cada caso construye una jpeg_decompress_struct sintetica en la arena A
// (con sus estructuras auxiliares: err, mem, src, inputctl, marker, entropy, coef, upsample y 6 componentes), el main controller y los arrays de punteros en la B, tablas de cuantificacion y Huffman en la D y el flujo
// de entrada en la C. TODOS los colaboradores se llaman por punteros de la propia estructura (error_exit, emit_message, fill_input_buffer, alloc_small, alloc_sarray, reset_error_mgr, reset_marker_reader,
// read_markers, entropy->start_pass, coef->start_input_pass): los falsos son los mismos en el EXE y en la DLL por construccion. Los colaboradores por direccion fija son codigo real del EXE: jdiv_round_up
// (0x5CF890) y, cuando una funcion llama a otra del lote (jpeg_huff_decode -> jpeg_fill_bit_buffer; consume_markers -> start_input_pass), el EXE corre las suyas (hook apagado) y la DLL las suyas.
// error_exit de la libjpeg real no vuelve: con P_ERRMODE = 0 el falso sale de la llamada con longjmp (ver Core.hpp, Gate); con P_ERRMODE = 1 vuelve y el cuerpo sigue (como el EXE).
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con un resumen FNV de la estructura y del controlador de entrada en las llamadas a submodulos, que fija el orden de las
// escrituras respecto a las llamadas), el valor devuelto y si la llamada salio por error_exit.

namespace F233ALive {
using namespace F233AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{0x5C9790, 0x5C9A60, 0x5C9B80, 0x5CA260, 0x5CA6E0, 0x5CA720, 0x5CA7E0, 0x5CA840, 0x5CA940, 0x5CAA90, 0x5CAB70, 0x5CAE30};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F233AFix::Arena(WIN);
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
inline u32 gNoise{};                   // bits altos de EAX de los falsos que devuelven un byte
inline u32 gRmRet{};                   // lo que devuelve read_markers
inline u32 gAllocBase{}, gAllocUsed{}, gAllocDummy{}, gSarrayCount{}, gFillCount{}, gC0{};
inline u32 gFillMask{}, gChOff[8]{}, gChLen[8]{};
inline DWORD gBenchThread{};

inline u32 StateHash(u32 cinfo, u32& inputHash) {
    inputHash = Fnv(reinterpret_cast<const void*>(Rd32(cinfo + 0x190)), 0x18);
    return Fnv(reinterpret_cast<const void*>(cinfo), kCiSize);
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
inline u32 __cdecl FkAlloc(void* ci, i32 pool, u32 size) {
    Ev& e = PushEv(4, 3);
    e.v[0] = u32(pool); e.v[1] = size; e.v[2] = Tag(uintptr_t(ci));
    const u64 need = (u64(size) + 7) & ~u64(7);
    if (u64(gAllocUsed) + need <= u64(kBAllocEnd - kBAlloc)) { const u32 p = gAllocBase + kBAlloc + gAllocUsed; gAllocUsed += u32(need); return p; }
    return 0x60000000u + 0x100u * gAllocDummy++;
}
inline u32 __cdecl FkAllocSarray(void* ci, i32 pool, u32 samplesPerRow, u32 numRows) {
    Ev& e = PushEv(8, 4);
    e.v[0] = u32(pool); e.v[1] = samplesPerRow; e.v[2] = numRows; e.v[3] = Tag(uintptr_t(ci));
    return 0x70000000u + 0x100u * gSarrayCount++;
}
inline u32 __cdecl FkFill(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci)), src = Rd32(cinfo + 0x18), idx = gFillCount++;
    Ev& e = PushEv(7, 4);
    e.v[0] = idx; e.v[1] = Tag(uintptr_t(Rd32(src))); e.v[2] = Rd32(src + 4); e.v[3] = Tag(uintptr_t(cinfo));
    const bool ok = idx < 8 && ((gFillMask >> idx) & 1);
    if (ok) { Wr32(src, gC0 + gChOff[idx]); Wr32(src + 4, gChLen[idx]); }
    return gNoise | (ok ? 1u : 0u);
}
template<u32 ID> inline void __cdecl FkStart(void* ci) {   // 10 = entropy->start_pass, 11 = coef->start_input_pass
    const u32 cinfo = u32(uintptr_t(ci));
    u32 ih{}; const u32 ch = StateHash(cinfo, ih);
    Ev& e = PushEv(ID, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = ch; e.v[2] = ih;
}
inline u32 __cdecl FkReadMarkers(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci));
    u32 ih{}; const u32 ch = StateHash(cinfo, ih);
    Ev& e = PushEv(12, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = ch; e.v[2] = ih;
    return gRmRet;
}
inline void __cdecl FkResetMarker(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci));
    u32 ih{}; const u32 ch = StateHash(cinfo, ih);
    Ev& e = PushEv(13, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = ch; e.v[2] = ih;
    Wr8(Rd32(cinfo + 0x190) + 0x10, 0x66); Wr32(cinfo + 0x8C, 0xAAAA0002u);   // el cuerpo debe sobrescribir coef_bits despues
}
inline void __cdecl FkResetErr(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci));
    u32 ih{}; const u32 ch = StateHash(cinfo, ih);
    Ev& e = PushEv(14, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = ch; e.v[2] = ih;
    Wr8(Rd32(cinfo + 0x190) + 0x11, 0x77); Wr32(cinfo + 0x8C, 0xAAAA0001u);
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

// Prepara arenas y la escena del caso (jpeg_decompress_struct y estructuras auxiliares); devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    const auto& a = c.a;
    const u32 fn = c.fn;
    arenaA.Pattern(a[P_FILL], a[SEED0]); arenaB.Pattern(a[P_FILL], a[SEED1]); arenaC.Pattern(a[P_FILL], a[SEED2]); arenaD.Pattern(a[P_FILL], a[SEED3]);
    const u32 S = u32(arenaA.Obj(a[P_OFF])), D0 = u32(arenaD.Obj(a[P_DOFF])), C0 = u32(arenaC.Obj(a[P_COFF])), B0 = u32(arenaB.Obj(a[P_BOFF]));
    gErrMode = a[P_ERRMODE]; gNoise = a[P_NOISE]; gRmRet = a[MK_RET];
    gAllocBase = B0; gAllocUsed = 0; gAllocDummy = 0; gSarrayCount = 0; gFillCount = 0; gC0 = C0; gFillMask = a[S_MASK];
    for (u32 i = 0; i < 8; ++i) { gChOff[i] = a[CH_OFF0 + i]; gChLen[i] = a[CH_LEN0 + i]; }
    nev = 0;

    // tablas Huffman y de cuantificacion (arena D)
    for (u32 j = 0; j < 8; ++j) {
        const u32 t = D0 + kHStride * j;
        std::array<u32, 17> bits; MakeBits(a[HDESC0 + j], bits);
        for (u32 i = 0; i <= 16; ++i) Wr8(t + i, bits[i]);
        for (u32 k = 0; k < 256; ++k) Wr8(t + 0x11 + k, HuffVal(a[HDESC0 + j], k));
    }
    for (u32 i = 0; i < 4; ++i) {
        const u32 t = D0 + kQBase + kQStride * i;
        for (u32 k = 0; k < 65; ++k) Wr16(t + 2 * k, QuantWord(a[QDESC0 + i], k));
    }
    // componentes
    for (u32 i = 0; i < kMaxComps; ++i) {
        const u32 p = S + kComps + 0x54 * i;
        const u32* k = &a[COMP0 + kCompFields * i];
        Wr32(p + 0x08, k[CF_H]); Wr32(p + 0x0C, k[CF_V]); Wr32(p + 0x10, k[CF_Q]); Wr32(p + 0x1C, k[CF_WIB]); Wr32(p + 0x20, k[CF_HIB]); Wr32(p + 0x24, k[CF_DCT]);
        Wr32(p + 0x28, k[CF_DSW]); Wr32(p + 0x2C, k[CF_DSH]); Wr32(p + 0x30, k[CF_NEED]); Wr32(p + 0x4C, k[CF_QT]);
    }
    // estructuras auxiliares
    Wr32(S + kErr + 0x00, u32(uintptr_t(&FkErrExit))); Wr32(S + kErr + 0x04, u32(uintptr_t(&FkEmitMsg))); Wr32(S + kErr + 0x10, u32(uintptr_t(&FkResetErr)));
    Wr32(S + kMem + 0x00, u32(uintptr_t(&FkAlloc))); Wr32(S + kMem + 0x08, u32(uintptr_t(&FkAllocSarray)));
    Wr32(S + kSrc + 0x00, C0 + 0x20); Wr32(S + kSrc + 0x04, 0); Wr32(S + kSrc + 0x0C, u32(uintptr_t(&FkFill)));
    Wr8(S + kInp + 0x10, a[IC_MULTI]); Wr8(S + kInp + 0x11, a[IC_EOI]); Wr8(S + kInp + 0x14, a[IC_INHDR]);
    Wr32(S + kMrk + 0x00, u32(uintptr_t(&FkResetMarker))); Wr32(S + kMrk + 0x04, u32(uintptr_t(&FkReadMarkers))); Wr8(S + kMrk + 0x0D, a[MK_SAWSOF]);
    Wr8(S + kEnt + 0x08, a[EN_WARN]); Wr32(S + kEnt + 0x00, u32(uintptr_t(&FkStart<10>)));
    Wr32(S + kCoef + 0x00, u32(uintptr_t(&FkStart<11>))); Wr32(S + kCoef + 0x04, a[CO_CONSUME]);
    Wr8(S + kUp + 0x08, a[UP_CTX]);
    // jpeg_decompress_struct
    const u32 ci = S + kCi;
    Wr32(ci + 0x00, S + kErr); Wr32(ci + 0x04, S + kMem); Wr32(ci + 0x18, S + kSrc);
    Wr32(ci + 0x1C, a[F_WIDTH]); Wr32(ci + 0x20, a[F_HEIGHT]); Wr32(ci + 0x24, a[F_NCOMP]); Wr32(ci + 0x7C, a[F_INSCAN]); Wr32(ci + 0x84, a[F_OUTSCAN]); Wr32(ci + 0x8C, a[F_COEFB]);
    for (u32 i = 0; i < 4; ++i) {
        Wr32(ci + 0x90 + 4 * i, (a[QMASK] >> i) & 1 ? 0u : D0 + kQBase + kQStride * i);
        Wr32(ci + 0xA0 + 4 * i, (a[DCMASK] >> i) & 1 ? 0u : D0 + kHStride * i);
        Wr32(ci + 0xB0 + 4 * i, (a[ACMASK] >> i) & 1 ? 0u : D0 + kHStride * (4 + i));
        Wr32(ci + 0x128 + 4 * i, S + kComps + 0x54 * a[F_CUR0 + i]);
    }
    Wr32(ci + 0xC0, a[F_PREC]); Wr32(ci + 0xC4, S + kComps); Wr8(ci + 0xC8, a[F_PROG]);
    Wr32(ci + 0x110, a[F_MAXH]); Wr32(ci + 0x114, a[F_MAXV]); Wr32(ci + 0x118, a[F_MINDCT]); Wr32(ci + 0x11C, a[F_TOTROWS]); Wr32(ci + 0x124, a[F_CINS]);
    Wr32(ci + 0x17C, a[F_UNREAD]);
    Wr32(ci + 0x184, B0 + kBMain); Wr32(ci + 0x188, S + kCoef); Wr32(ci + 0x190, S + kInp); Wr32(ci + 0x194, S + kMrk); Wr32(ci + 0x198, S + kEnt); Wr32(ci + 0x1A0, S + kUp);

    switch (fn) {
    case 0: Wr32(B0 + kBSlot, a[DT_EXIST] ? B0 + kBDtbl : 0u); break;
    case 1: case 2: {
        const u32 st = B0 + kBState;
        Wr32(st + 0x00, C0 + a[IN_OFF]); Wr32(st + 0x04, a[IN_LEN]); Wr32(st + 0x10, S);
        for (u32 k = 0; k < kStreamSize; ++k) Wr8(C0 + k, StreamByte(a[S_MODE], a[S_SEED], k));
        if (fn == 2) {
            const u32 d = B0 + kBAlloc;
            for (u32 l = 1; l <= 16; ++l) { Wr32(d + 4 * l, u32(MaxCodeVal(a[DT_DESC], l))); Wr32(d + 0x48 + 4 * l, u32(ValOffVal(a[DT_DESC], l))); }
            Wr32(d + 0x44, 0xFFFFF); Wr32(d + 0x8C, D0 + kPub);
        }
        break;
    }
    case 8: case 9: case 10: {
        const LayoutB L = MakeLayout(a);
        const u32 m = B0 + kBMain;
        for (u32 i = 0; i < 4; ++i) {
            Wr32(m + 0x08 + 4 * i, B0 + L.buf[i]);
            Wr32(B0 + kBX0 + 4 * i, B0 + L.xb0[i]); Wr32(B0 + kBX1 + 4 * i, B0 + L.xb1[i]);
            for (u32 k = 0; k < L.bufWords[i]; ++k) Wr32(B0 + L.buf[i] + 4 * k, BufWord(c.seed, i, k));
            for (u32 k = 0; k < L.blockWords[i]; ++k) Wr32(B0 + L.blockOff[i] + 4 * k, BlockWord(c.seed, i, k));
        }
        Wr32(m + 0x20, B0 + kBX0); Wr32(m + 0x24, B0 + kBX1); Wr32(m + 0x28, a[MN_WHICH]); Wr32(m + 0x30, a[MN_ROWS]);
        break;
    }
    default: break;
    }

    call.eax = c.eax; call.ecx = c.ecx; call.edx = c.edx;
    switch (fn) {
    case 0: call.args = {S, a[A0] | (a[P_NOISE] & 0xFFFFFF00u), a[A1], B0 + kBSlot}; call.argCount = 4; call.popBytes = 16; break;
    case 1: call.args = {B0 + kBState, a[A0], a[A1], a[A2]}; call.argCount = 4; call.popBytes = 16; break;
    case 2: call.args = {B0 + kBState, a[A0], a[A1], B0 + kBAlloc, a[A2]}; call.argCount = 5; call.popBytes = 20; break;
    case 11: call.args = {S, a[A0] | (a[P_NOISE] & 0xFFFFFF00u)}; call.argCount = 2; call.popBytes = 8; break;
    default: call.args[0] = S; call.argCount = 1; call.popBytes = 4; break;
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
        else if (fn == 1) obs["ret"] = eax & 0xFF;                           // boolean en AL
        else if (fn == 2 || fn == 5) obs["ret"] = eax;                       // int en EAX
        else obs["ret"] = nullptr;                                           // void
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN233A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN233A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn233a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn233a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn233a"); }
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
    {   // jdiv_round_up (0x5CF890) tiene hook en la DLL (lote 235A, sin validar): su ruta lo deja apagado; la DLL del banco llama por la direccion del EXE, que corre el codigo original
        const Pair jdiv = MakeCollab(env, 0x5CF890, hooksFound);
        if (!jdiv.hooked || !jdiv.dll) ready = false;
        env.reals[2] = {Real(0x5C9A60)};                 // jpeg_huff_decode -> jpeg_fill_bit_buffer
        env.reals[4] = {jdiv};                           // start_input_pass -> jdiv_round_up (per_scan_setup)
        env.reals[5] = {jdiv, Real(0x5CA6E0)};           // consume_markers -> jdiv_round_up (initial_setup, per_scan_setup) y start_input_pass
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
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
