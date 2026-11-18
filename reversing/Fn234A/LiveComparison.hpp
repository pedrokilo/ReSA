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

// Banco par a par Fn234A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 10 funciones de la parte de jdmarker y jdmaster de la libjpeg 6b de descompresion embebida en el EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Cada caso construye una jpeg_decompress_struct sintetica en la
// arena A (con sus estructuras auxiliares), el main controller y el asignador (bump) en la B, tablas Huffman en la D y el flujo de entrada en la C. Los colaboradores se llaman por punteros de la propia estructura
// (error_exit, emit_message, alloc_small, alloc_sarray, fill_input_buffer, skip_input_data, resync_to_restart, realize_virt_arrays, start_pass de los modulos, start_input_pass): falsos identicos en el EXE y en la DLL.
// Los colaboradores por direccion fija son: falsos instalados en la entrada del EXE (jinit_color_deconverter, jinit_upsampler, jinit_d_post_controller, jinit_inverse_dct, jinit_d_coef_controller: los dos lados
// los llaman por esa direccion) o codigo real con el hook de la DLL apagado (jpeg_alloc_huff_table, jinit_huff_decoder, jinit_d_main_controller; y las funciones de este lote que se llaman entre si: next_marker
// y jpeg_calc_output_dimensions: el EXE corre las suyas y la DLL las suyas). error_exit de la libjpeg real no vuelve: con P_ERRMODE = 0 el falso sale de la llamada con longjmp (ver Core.hpp, Gate); con
// P_ERRMODE = 1 vuelve y el cuerpo sigue (como el EXE). Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos, el valor devuelto (AL) y si la llamada salio por error_exit.

namespace F234ALive {
using namespace F234AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries{0x5CB5B0, 0x5CBB20, 0x5CC050, 0x5CC0F0, 0x5CC1F0, 0x5CC610, 0x5CC7D0, 0x5CC8C0, 0x5CCB50, 0x5CCCC0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F234AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void RestoreByte(uintptr_t at) { const uintptr_t base = uintptr_t(m.data()); if (at >= base && at < base + N) m[at - base] = r[at - base]; }
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

// ================================================================================================ falsos (se llaman por punteros de la estructura o por la direccion del EXE: iguales en el EXE y en la DLL)
inline u32 gErrMode{};                 // 0: error_exit no vuelve (longjmp a Gate)
inline u32 gNoise{};                   // bits altos de EAX de los falsos que devuelven un byte
inline u32 gRsRet{};                   // lo que devuelve resync_to_restart
inline u32 gAllocBase{}, gAllocUsed{}, gAllocDummy{}, gSarrayCount{}, gFillCount{}, gC0{};
inline u32 gFillMask{}, gChOff[8]{}, gChLen[8]{};
inline DWORD gBenchThread{};

inline bool InArena(u32 p) { const u32 t = Tag(uintptr_t(p)); return t >= 0x10000000u && t < 0x10400000u; }
inline u32 MasterHash(u32 cinfo) {
    const u32 m = Rd32(cinfo + 0x180);
    return InArena(m) ? Fnv(reinterpret_cast<const void*>(m), 0x1C) : 0u;
}
inline void __cdecl FkErrExit(void* ci) {
    const u32 cinfo = u32(uintptr_t(ci)), err = Rd32(cinfo);
    Ev& e = PushEv(1, 4);
    e.v[0] = Rd32(err + 0x14); e.v[1] = Rd32(err + 0x18); e.v[2] = Rd32(err + 0x1C); e.v[3] = Tag(uintptr_t(cinfo));
    if (gErrMode == 0 && gEscArmed) longjmp(gEscBuf, 1);
}
inline void __cdecl FkEmitMsg(void* ci, i32 level) {
    const u32 cinfo = u32(uintptr_t(ci)), err = Rd32(cinfo);
    Ev& e = PushEv(2, 11);
    e.v[0] = u32(level); e.v[1] = Rd32(err + 0x14); e.v[2] = Tag(uintptr_t(cinfo));
    for (u32 k = 0; k < 8; ++k) e.v[3 + k] = Rd32(err + 0x18 + 4 * k);
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
inline void __cdecl FkSkip(void* ci, i32 n) {   // src->skip_input_data: avanza la fuente
    const u32 cinfo = u32(uintptr_t(ci)), src = Rd32(cinfo + 0x18);
    Ev& e = PushEv(6, 3);
    e.v[0] = u32(n); e.v[1] = Tag(uintptr_t(cinfo)); e.v[2] = Tag(uintptr_t(Rd32(src)));
    Wr32(src, Rd32(src) + u32(n)); Wr32(src + 4, Rd32(src + 4) - u32(n));
}
inline u32 __cdecl FkResync(void* ci, i32 desired) {   // src->resync_to_restart
    const u32 cinfo = u32(uintptr_t(ci)), marker = Rd32(cinfo + 0x194);
    Ev& e = PushEv(5, 3);
    e.v[0] = u32(desired); e.v[1] = Tag(uintptr_t(cinfo)); e.v[2] = Rd32(cinfo + 0x17C);
    Wr32(cinfo + 0x17C, 0x5A5A0000u + u32(desired));
    Wr32(marker + 0x10, Rd32(marker + 0x10) + 1u);   // el cuerpo debe releer next_restart_num tras la llamada
    return gNoise | (gRsRet & 0xFFu);
}
template<u32 ID> inline void __cdecl FkMod1(void* ci) {   // modulos de un argumento (cinfo)
    const u32 cinfo = u32(uintptr_t(ci));
    Ev& e = PushEv(ID, 3);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = Fnv(reinterpret_cast<const void*>(cinfo), kCiSize); e.v[2] = MasterHash(cinfo);
}
template<u32 ID> inline void __cdecl FkMod2(void* ci, u32 arg) {   // modulos de dos argumentos (cinfo, valor)
    const u32 cinfo = u32(uintptr_t(ci));
    Ev& e = PushEv(ID, 4);
    e.v[0] = Tag(uintptr_t(cinfo)); e.v[1] = arg; e.v[2] = Fnv(reinterpret_cast<const void*>(cinfo), kCiSize); e.v[3] = MasterHash(cinfo);
}

// Variante para argumentos BYTE (boolean de 1 byte del EXE): el llamador del EXE carga solo AL y deja basura en los 24 bits altos de EAX (0x72ED3x..), y el cuerpo del EXE lee solo el byte; se registra el byte.
template<u32 ID> inline void __cdecl FkMod2B(void* ci, u32 arg) { FkMod2<ID>(ci, arg & 0xFFu); }

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

// get_dht copia SIEMPRE 256 bytes de huffval aunque count sea menor: los que pasan de count son basura de la pila del EXE (distinta en la DLL). Tras la llamada se restauran a su valor previo (referencia) los bytes
// huffval[count..255] de todas las tablas Huffman de la estructura (count = suma de bits[1..16] resultante); el oraculo hace lo mismo.
inline void MaskHuffvalGarbage(uintptr_t cinfo) {
    for (u32 slot = 0; slot < 8; ++slot) {
        const u32 p = Rd32(cinfo + (slot < 4 ? 0xA0 : 0xB0) + 4 * (slot & 3));
        if (!p) continue;
        u32 count = 0;
        for (u32 l = 1; l <= 16; ++l) count += *reinterpret_cast<const u8*>(uintptr_t(p) + l);
        for (u32 k = count; k < 256; ++k) {
            const uintptr_t at = uintptr_t(p) + 0x11 + k;
            for (auto* ar : {&arenaA, &arenaB, &arenaC, &arenaD}) ar->RestoreByte(at);
        }
    }
}

// Prepara arenas y la escena del caso (jpeg_decompress_struct y estructuras auxiliares); devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    const auto& a = c.a;
    const u32 fn = c.fn;
    arenaA.Pattern(a[P_FILL], a[SEED0]); arenaB.Pattern(a[P_FILL], a[SEED1]); arenaC.Pattern(a[P_FILL], a[SEED2]); arenaD.Pattern(a[P_FILL], a[SEED3]);
    const u32 S = u32(arenaA.Obj(a[P_OFF])), D0 = u32(arenaD.Obj(a[P_DOFF])), C0 = u32(arenaC.Obj(a[P_COFF])), B0 = u32(arenaB.Obj(a[P_BOFF]));
    gErrMode = a[P_ERRMODE]; gNoise = a[P_NOISE]; gRsRet = a[RS_RET];
    gAllocBase = B0; gAllocUsed = 0; gAllocDummy = 0; gSarrayCount = 0; gFillCount = 0; gC0 = C0; gFillMask = a[S_MASK];
    for (u32 i = 0; i < 8; ++i) { gChOff[i] = a[CH_OFF0 + i]; gChLen[i] = a[CH_LEN0 + i]; }
    nev = 0;

    // tablas Huffman (arena D)
    for (u32 j = 0; j < 8; ++j) {
        const u32 t = D0 + kHStride * j;
        std::array<u32, 17> bits; MakeBits(a[HDESC0 + j], bits);
        for (u32 i = 0; i <= 16; ++i) Wr8(t + i, bits[i]);
        for (u32 k = 0; k < 256; ++k) Wr8(t + 0x11 + k, HuffVal(a[HDESC0 + j], k));
    }
    // componentes
    for (u32 i = 0; i < kMaxComps; ++i) {
        const u32 p = S + kComps + 0x54 * i;
        const u32* k = &a[COMP0 + kCompFields * i];
        Wr32(p + 0x08, k[CF_H]); Wr32(p + 0x0C, k[CF_V]); Wr32(p + 0x1C, k[CF_WIB]); Wr32(p + 0x24, k[CF_DCT]);
    }
    // estructuras auxiliares
    Wr32(S + kErr + 0x00, u32(uintptr_t(&FkErrExit))); Wr32(S + kErr + 0x04, u32(uintptr_t(&FkEmitMsg)));
    Wr32(S + kMem + 0x00, u32(uintptr_t(&FkAlloc))); Wr32(S + kMem + 0x08, u32(uintptr_t(&FkAllocSarray))); Wr32(S + kMem + 0x18, u32(uintptr_t(&FkMod1<25>)));
    Wr32(S + kSrc + 0x00, C0 + a[IN_OFF]); Wr32(S + kSrc + 0x04, a[IN_LEN]); Wr32(S + kSrc + 0x0C, u32(uintptr_t(&FkFill)));
    Wr32(S + kSrc + 0x10, u32(uintptr_t(&FkSkip))); Wr32(S + kSrc + 0x14, u32(uintptr_t(&FkResync)));
    Wr32(S + kInp + 0x08, u32(uintptr_t(&FkMod1<26>))); Wr8(S + kInp + 0x10, a[IC_MULTI]); Wr8(S + kInp + 0x11, a[IC_EOI]);
    Wr32(S + kMrk + 0x10, a[MK_NEXT]); Wr32(S + kMrk + 0x14, a[MK_DISC]);
    Wr32(S + kCoef + 0x08, u32(uintptr_t(&FkMod1<38>)));
    Wr32(S + kUp + 0x00, u32(uintptr_t(&FkMod1<32>))); Wr8(S + kUp + 0x08, a[UP_CTX]);
    Wr8(S + kMaster + 0x08, a[MS_DUMMY]); Wr32(S + kMaster + 0x0C, a[MS_PASS]); Wr8(S + kMaster + 0x10, a[MS_MERGED]); Wr32(S + kMaster + 0x14, S + kQ1); Wr32(S + kMaster + 0x18, S + kQ2);
    Wr32(S + kProgress + 0x0C, a[F_PRCOMP]); Wr32(S + kProgress + 0x10, a[F_PRTOTAL]);
    Wr32(S + kIdct + 0x00, u32(uintptr_t(&FkMod1<30>))); Wr32(S + kCconv + 0x00, u32(uintptr_t(&FkMod1<31>)));
    Wr32(S + kQ0 + 0x00, u32(uintptr_t(&FkMod2<33>))); Wr32(S + kQ1 + 0x00, u32(uintptr_t(&FkMod2<34>))); Wr32(S + kQ2 + 0x00, u32(uintptr_t(&FkMod2<35>)));
    Wr32(S + kPost + 0x00, u32(uintptr_t(&FkMod2<36>)));
    Wr32(B0 + kBMain + 0x00, u32(uintptr_t(&FkMod2<37>)));
    // jpeg_decompress_struct
    const u32 ci = S + kCi;
    Wr32(ci + 0x00, S + kErr); Wr32(ci + 0x04, S + kMem); Wr32(ci + 0x08, a[F_PROGRESS] ? S + kProgress : 0u); Wr32(ci + 0x14, a[F_GSTATE]); Wr32(ci + 0x18, S + kSrc);
    Wr32(ci + 0x1C, a[F_WIDTH]); Wr32(ci + 0x20, a[F_HEIGHT]); Wr32(ci + 0x24, a[F_NCOMP]); Wr32(ci + 0x28, a[F_JCS]); Wr32(ci + 0x2C, a[F_OUTCS]);
    Wr8(ci + 0x40, a[F_BUFIMG]); Wr8(ci + 0x41, a[F_RAW]); Wr8(ci + 0x4A, a[F_QUANT]); Wr8(ci + 0x50, a[F_TWOPASS]); Wr8(ci + 0x58, a[F_EN1]); Wr8(ci + 0x59, a[F_ENX]); Wr8(ci + 0x5A, a[F_EN2]);
    Wr32(ci + 0x74, a[F_COLORMAP]);
    for (u32 i = 0; i < 4; ++i) {
        Wr32(ci + 0xA0 + 4 * i, (a[DCMASK] >> i) & 1 ? 0u : D0 + kHStride * i);
        Wr32(ci + 0xB0 + 4 * i, (a[ACMASK] >> i) & 1 ? 0u : D0 + kHStride * (4 + i));
    }
    Wr32(ci + 0xC4, S + kComps); Wr8(ci + 0xC8, a[F_PROG]); Wr8(ci + 0xC9, a[F_ARITH]);
    Wr32(ci + 0x118, a[F_MINDCT]); Wr32(ci + 0x17C, a[F_UNREAD]);
    Wr32(ci + 0x180, S + kMaster); Wr32(ci + 0x184, B0 + kBMain); Wr32(ci + 0x188, S + kCoef); Wr32(ci + 0x18C, S + kPost); Wr32(ci + 0x190, S + kInp); Wr32(ci + 0x194, S + kMrk); Wr32(ci + 0x198, S + kEnt);
    Wr32(ci + 0x19C, S + kIdct); Wr32(ci + 0x1A0, S + kUp); Wr32(ci + 0x1A4, S + kCconv); Wr32(ci + 0x1A8, S + kQ0);
    // flujo de entrada (arena C)
    const std::vector<u8> head = StreamHead(fn, a);
    for (u32 k = 0; k < kStreamSize; ++k) Wr8(C0 + k, StreamAt(a, head, k));

    call.eax = c.eax; call.ecx = c.ecx; call.edx = c.edx;
    call.args[0] = S; call.argCount = 1; call.popBytes = 4;
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
        if (fn == 0) MaskHuffvalGarbage(call.args[0]);
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["escaped"] = escaped;
        const u32 eax = sides[s].reg.eax;
        if (escaped) obs["ret"] = nullptr;
        else if (fn <= 5) obs["ret"] = eax & 0xFF;                           // boolean en AL
        else obs["ret"] = nullptr;                                           // void
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN234A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN234A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn234a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn234a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn234a"); }
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
    // jpeg_alloc_huff_table (0x5C6BD0, hook de 231A), jinit_huff_decoder (0x5CA260) y jinit_d_main_controller (0x5CAE30) (hooks de 233A) son codigo real: su ruta deja el hook apagado (el EXE corre sus bytes; la DLL
    // llama a la direccion del EXE o a su propio cuerpo). next_marker y jpeg_calc_output_dimensions son del lote: el EXE corre las suyas y la DLL las suyas (sus rutas estan apagadas).
    {
        const Pair allocHuff = MakeCollab(env, 0x5C6BD0, hooksFound);
        const Pair huffDec = MakeCollab(env, 0x5CA260, hooksFound);
        const Pair mainCtl = MakeCollab(env, 0x5CAE30, hooksFound);
        if (!allocHuff.hooked || !huffDec.hooked || !mainCtl.hooked) ready = false;
        env.reals[0] = {allocHuff};
        env.reals[5] = {Real(0x5CC0F0)};                 // read_restart_marker -> next_marker
        env.reals[9] = {Real(0x5CC8C0), huffDec, mainCtl};   // jinit_master_decompress -> jpeg_calc_output_dimensions, jinit_huff_decoder, jinit_d_main_controller
        // Falsos en la entrada del EXE (los dos lados llaman por esa direccion); su hook (si lo hay) se deja apagado
        auto Stub = [&](uintptr_t exe, void* fake) { Pair p = MakeCollab(env, exe, hooksFound); p.dll = 0; return StubSpec{p, fake}; };
        env.stubs[9] = {Stub(0x5C9350, reinterpret_cast<void*>(&FkMod1<20>)), Stub(0x5CD350, reinterpret_cast<void*>(&FkMod1<21>)), Stub(0x5CCDF0, reinterpret_cast<void*>(&FkMod2<22>)),
                        Stub(0x5C9710, reinterpret_cast<void*>(&FkMod1<23>)), Stub(0x5C8E20, reinterpret_cast<void*>(&FkMod2B<24>))};
    }
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"hooked", s.pair.hooked}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"toda la funcion; los colaboradores se llaman por punteros de la estructura (falsos iguales en ambos lados) o son codigo real del EXE (jpeg_alloc_huff_table, jinit_huff_decoder, jinit_d_main_controller y las funciones del lote) o falsos en la direccion del EXE (jinit_*)"})},
                                 {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
