#pragma once
// Fixtures del banco Fn235A (lote 235A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 30 funciones del EXE (libjpeg 6b embebida: jdpostct, jdsample, jerror, jmemmgr, jmemnobs, jutils y jcapimin; todas con hook y validated=0 antes del banco; ver README.md y fixture_oracle.py):
//   idx direccion nombre
//    0  0x5CCDF0 jinit_post_controller(cinfo, need_full_buffer BYTE)
//    1  0x5CCE80 sep_upsample(cinfo, input_buf, in_row_group_ctr*, in_row_groups_avail, output_buf, out_row_ctr*, out_rows_avail)
//    2  0x5CD350 jinit_upsampler(cinfo)
//    3  0x5CD6B0 error_exit(cinfo)                  (no vuelve: exit del EXE sustituido por una salida no local)
//    4  0x5CD6D0 output_message(cinfo)
//    5  0x5CD810 jpeg_std_error(err) -> err
//    6  0x5CEBC0 alloc_small(cinfo, pool_id, size) -> ptr
//    7  0x5CED10 alloc_large(cinfo, pool_id, size) -> ptr
//    8  0x5CEDD0 alloc_sarray(cinfo, pool_id, samplesperrow, numrows) -> ptr
//    9  0x5CEE80 alloc_barray(cinfo, pool_id, blocksperrow, numrows) -> ptr
//   10  0x5CEF30 request_virt_sarray(cinfo, pool_id, pre_zero BYTE, samplesperrow, numrows, maxaccess) -> ptr
//   11  0x5CEFA0 request_virt_barray(cinfo, pool_id, pre_zero BYTE, blocksperrow, numrows, maxaccess) -> ptr
//   12  0x5CF010 realize_virt_arrays(cinfo)
//   13  0x5CF580 free_pool(cinfo, pool_id)
//   14  0x5CF6D0 jinit_memory_mgr(cinfo)
//   15  0x5CF800 jpeg_get_small(cinfo, size) -> ptr
//   16  0x5CF810 jpeg_free_small(cinfo, object, size)
//   17  0x5CF820 jpeg_get_large(cinfo, size) -> ptr
//   18  0x5CF830 jpeg_free_large(cinfo, object, size)
//   19  0x5CF840 jpeg_mem_available(cinfo, min, max, already) -> max
//   20  0x5CF850 jpeg_open_backing_store(cinfo, info, total)
//   21  0x5CF870 jpeg_mem_init(cinfo) -> 0
//   22  0x5CF880 jpeg_mem_term(cinfo)
//   23  0x5CF890 jdiv_round_up(a, b) -> (a + b - 1) / b
//   24  0x5CF8A0 jround_up(a, b)
//   25  0x5CF8C0 jcopy_sample_rows(input_array, source_row, output_array, dest_row, num_rows, num_cols)
//   26  0x5CF940 jzero_far(target, bytes)
//   27  0x5CF960 jpeg_CreateCompress(cinfo, version, structsize)
//   28  0x5CFA50 jpeg_suppress_tables(cinfo, suppress BYTE)
//   29  0x5CFAC0 jpeg_finish_compress(cinfo)
//
// El caso es DECLARATIVO: la cabecera a[] (argumentos, relleno, semillas, efectos laterales, salida no local) mas una lista de escrituras p (destino, desplazamiento, ancho, valor) y una lista de retornos
// programados r (id del falso, numero de llamada, valor) que el banco aplica tal cual; el oraculo Python aplica las mismas escrituras a su modelo de memoria y ejecuta la funcion como un programa sobre el.
// Cuatro arenas A..D (guardas de 0x40, ventana de 0x2000). Familia = n % 16: tipo = familia % 4 (posicion del objeto principal en A: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y
// relleno = familia / 4.
//   a[0] = numero de argumentos, a[1..8] = argumentos (especificaciones de valor), a[9] = relleno de las arenas, a[10..13] = semillas de A..D, a[14] = libre,
//   a[15..20] = efecto lateral 1: id del falso, numero de llamada, arena, desplazamiento, modo (0 crudo, 1 especificacion), valor; a[21] = desplazamiento del objeto en A; a[22] = EAX de entrada (especificacion; 0 = semilla),
//   a[23] = libre, a[24..29] = efecto lateral 2 (igual que el 1), a[30..31] = id del falso y numero de llamada tras los que el falso NO vuelve (salida no local; id 0 = ninguno).
// Escritura: p = {(destino << 8) | ancho, desplazamiento (o direccion absoluta si destino == 4), valor}; destino 0..3 = arena A..D, 4 = region global de 4 bytes (sin uso en este lote).
// Especificaciones de valor: PS(arena, desplazamiento) = ((0xA + arena) << 28) | desplazamiento (el banco la resuelve con la direccion real);
// 0xF00000kk = la funcion falsa kk que se llama por puntero (0 error_exit, 1 funcion de un argumento, 2 format_message, 3 alloc_small, 4 alloc_sarray, 5 metodo de sobremuestreo, 6 color_convert,
// 7 close_backing_store, 8 prepare_for_pass, 9 finish_pass, 10 progress_monitor, 11 compress_data, 12 write_file_trailer, 13 term_destination; su id de evento es 0xC100 + kk); otro valor es crudo.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F235AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kWords = 40;
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kWords> a{};
    std::vector<u32> p;   // escrituras (triples)
    std::vector<u32> r;   // retornos programados (triples)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { u32 w = Rand(n, nk()); const u32 t = w >> 28; if ((t >= 0xAu && t <= 0xDu) || (w >> 8) == 0xF00000u) w ^= 0x50000000u; return w; }   // nunca una especificacion de puntero ni de funcion falsa
};

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 30;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 PS(u32 arena, u32 off) { return ((0xAu + arena) << 28) | off; }
constexpr bool IsPS(u32 v) { const u32 n = v >> 28; return n >= 0xAu && n <= 0xDu; }
constexpr u32 FS(u32 k) { return 0xF0000000u + k; }
constexpr bool IsFS(u32 v) { return (v >> 8) == 0xF00000u; }
constexpr bool IsSpecial(u32 v) { return IsPS(v) || IsFS(v); }
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 NoPS(u32 v) { return IsSpecial(v) ? (v ^ 0x50000000u) : v; }
// Un dword "interesante" para valores crudos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return NoPS(AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu})));
}
// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// ------------------------------------------------------------------------------------------------ funciones
enum Kind : u32 { kPost, kSep, kUpInit, kErrExit, kOutMsg, kStdErr, kAllocS, kAllocL, kAllocSa, kAllocBa, kReqS, kReqB, kRealize, kFreePool, kInitMem, kGetS, kFreeS, kGetL, kFreeL, kMemAvail, kOpenBs,
                  kMemInit, kMemTerm, kDivUp, kRoundUp, kCopyRows, kZeroFar, kCreateC, kSuppress, kFinish };
inline constexpr std::array<u32, kFunctions> kEntry{0x5CCDF0, 0x5CCE80, 0x5CD350, 0x5CD6B0, 0x5CD6D0, 0x5CD810, 0x5CEBC0, 0x5CED10, 0x5CEDD0, 0x5CEE80, 0x5CEF30, 0x5CEFA0, 0x5CF010, 0x5CF580, 0x5CF6D0,
                                                    0x5CF800, 0x5CF810, 0x5CF820, 0x5CF830, 0x5CF840, 0x5CF850, 0x5CF870, 0x5CF880, 0x5CF890, 0x5CF8A0, 0x5CF8C0, 0x5CF940, 0x5CF960, 0x5CFA50, 0x5CFAC0};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x1B0, 0x1B0, 0x1B0, 0x20, 0x20, 0x90, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x110, 0x170, 0x80, 0x160};
inline constexpr std::array<u32, 0> kGlobal{};   // sin regiones globales en este lote
inline std::vector<u32> GlobalsOf(u32) { return {}; }

// ------------------------------------------------------------------------------------------------ generador
struct Cx {
    C& c;
    Gen g;
    u32 o{};     // desplazamiento del objeto principal en A
    Cx(C& c_, unsigned n, u32 salt) : c(c_), g(n, salt) {}
    void Put(u32 ar, u32 off, u32 w, u32 v) { c.p.push_back((ar << 8) | w); c.p.push_back(off); c.p.push_back(v); }
    void P32(u32 ar, u32 off, u32 v) { Put(ar, off, 4, v); }
    void P16(u32 ar, u32 off, u32 v) { Put(ar, off, 2, v & 0xFFFFu); }
    void P8(u32 ar, u32 off, u32 v) { Put(ar, off, 1, v & 0xFFu); }
    void A32(u32 off, u32 v) { P32(0, o + off, v); }   // campo del objeto principal (A)
    void A8(u32 off, u32 v) { P8(0, o + off, v); }
    void Ret(u32 id, u32 call, u32 v) { c.r.push_back(id); c.r.push_back(call); c.r.push_back(v); }
    void Args(std::initializer_list<u32> l) { c.a[0] = u32(l.size()); u32 i = 1; for (u32 v : l) c.a[i++] = v; }
    void Poke(u32 id, u32 call, u32 arena, u32 off, u32 mode, u32 val) { c.a[15] = id; c.a[16] = call; c.a[17] = arena; c.a[18] = off; c.a[19] = mode; c.a[20] = val; }
    void Poke2(u32 id, u32 call, u32 arena, u32 off, u32 mode, u32 val) { c.a[24] = id; c.a[25] = call; c.a[26] = arena; c.a[27] = off; c.a[28] = mode; c.a[29] = val; }
    void Throw(u32 id, u32 call) { c.a[30] = id; c.a[31] = call; }
};

// ids de los falsos llamados por direccion
constexpr u32 kDestroy = 0x5C6B90, kAbort = 0x5C6B50, kInitMm = 0x5CF6D0, kRoundUpId = 0x5CF8A0, kGetSmall = 0x5CF800, kGetLarge = 0x5CF820, kFreeSmall = 0x5CF810, kFreeLarge = 0x5CF830,
              kMemAvailId = 0x5CF840, kOpenBsId = 0x5CF850, kMemInitId = 0x5CF870, kMemTermId = 0x5CF880, kAllocSmallId = 0x5CEBC0, kAllocLargeId = 0x5CED10, kAllocSarrayId = 0x5CEDD0,
              kAllocBarrayId = 0x5CEE80, kMalloc = 0x72F420, kFree = 0x72F430, kExit = 0x823CAE, kFprintf = 0x823A30, kGetenv = 0x8240C7, kSscanf = 0x8220AD;
constexpr u32 kErrFk = 0xC100;   // id de un falso por puntero k: 0xC100 + k

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Cx x(c, n, 100 + c.fn * 100);
    auto& g = x.g;
    auto& a = c.a;
    const u32 kind = f % 4;
    a[9] = f / 4;
    a[10] = c.seed * 13u + 50u; a[11] = a[10] + 1u; a[12] = a[10] + 2u; a[13] = a[10] + 3u;
    a[14] = g.W();
    x.o = Offset(g, kind, kSpan[c.fn]);
    a[21] = x.o;
    const u32 thisA = PS(0, x.o);
    const u32 fn = c.fn;
    auto A32 = [&](u32 off, u32 v) { x.A32(off, v); };
    auto A8 = [&](u32 off, u32 v) { x.A8(off, v); };
    // estructuras comunes en B: error_mgr (err: +0 error_exit, +8 output_message, +0xC format_message) y su copia con otros falsos; la copia hace observable una relectura de cinfo->err
    const u32 errO = 0x100u + 4u * g.R(8), err2O = 0x200u + 4u * g.R(8);
    auto MkErr = [&] {
        x.P32(1, errO + 0, FS(0)); x.P32(1, errO + 8, FS(1)); x.P32(1, errO + 0xC, FS(2));
        x.P32(1, err2O + 0, FS(0)); x.P32(1, err2O + 8, FS(0)); x.P32(1, err2O + 0xC, FS(1));   // error_exit igual que en err: una salida no local programada sobre el id 0xC100 sigue valiendo si el cinfo cambia de err
        A32(0x00, PS(1, errO));
    };
    // mem (my_memory_mgr) en B: la tabla de metodos que usa el codigo bajo prueba (+0 alloc_small, +8 alloc_sarray) y una copia con las ranuras cambiadas
    const u32 memO = 0x300u + 4u * g.R(8), mem2O = 0x400u + 4u * g.R(8);
    auto MkMemFns = [&] {
        x.P32(1, memO + 0, FS(3)); x.P32(1, memO + 8, FS(4));
        x.P32(1, mem2O + 0, FS(4)); x.P32(1, mem2O + 8, FS(3));
        A32(0x04, PS(1, memO));
    };
    auto Pool = [&](bool bad) -> u32 { return bad ? g.Pk({2u, 3u, 0xFFFFFFFEu, 4u, 5u, 0xFFFFFFFDu}) : g.Pk({1u, 1u, 0u}); };
    switch (fn) {
    case kPost: {
        MkErr(); MkMemFns();
        A8(0x4A, g.Pk({0u, 0u, 1u, 0xFFu, 0x80u}));
        A32(0x114, g.Pk({1u, 2u, 2u, 3u, 4u, 8u, 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, g.R(20)}));
        A32(0x64, g.Pk({1u, 3u, 3u, 4u, 0u, 0xFFFFFFFFu, 0x10000u, g.R(100), g.W()}));
        A32(0x5C, g.Pk({0u, 1u, 100u, 640u, 0x7FFFFFFFu, 0x80000000u, g.R(2000), g.W()}));
        A32(0x18C, g.W());
        const u32 need = g.Pk({0u, 0u, 0u, 1u, 0xFFu, 0x100u, 0x01010100u, 0x80u, g.W()});
        x.Ret(kErrFk + 3, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        x.Ret(kErrFk + 4, 0, g.P(60) ? PS(3, 0x400u + 4u * g.R(0x20)) : Raw(g));
        x.Args({thisA, need});
        if (g.P(45)) {
            const u32 id = g.Pk({kErrFk + 3, kErrFk + 3, kErrFk + 0, kErrFk + 4});
            const u32 m = g.R(6);
            if (m == 0) x.Poke(id, 0, 0, x.o + 0x04, 1, PS(1, mem2O));
            else if (m == 1) x.Poke(id, 0, 0, x.o + 0x4A, 0, g.Pk({0u, 1u, 0x100u, 0x00010000u}));
            else if (m == 2) x.Poke(id, 0, 0, x.o + 0x114, 0, g.Pk({1u, 2u, 3u, 7u}));
            else if (m == 3) x.Poke(id, 0, 0, x.o + 0x64, 0, g.Pk({1u, 3u, 4u, 0u}));
            else if (m == 4) x.Poke(id, 0, 0, x.o + 0x5C, 0, g.Pk({0u, 8u, 800u}));
            else x.Poke(id, 0, 0, x.o + 0x00, 1, PS(1, err2O));
        }
        break;
    }
    case kSep: {
        // up (my_upsampler) en B, cconvert en B, matrices de punteros en B y C
        const u32 upO = 0x500u + 4u * g.R(8), up2O = 0x600u + 4u * g.R(8), ccO = 0x700u + 4u * g.R(8), cc2O = 0x780u + 4u * g.R(8), ibO = 0x800u + 4u * g.R(4);
        const u32 compBase = 0x100u + 4u * g.R(0x40);
        A32(0x1A0, PS(1, upO)); A32(0x1A4, PS(1, ccO)); A32(0xC4, PS(2, compBase));
        x.P32(1, ccO + 4, FS(6)); x.P32(1, cc2O + 4, FS(5));
        const u32 maxv = g.Pk({1u, 2u, 2u, 3u, 4u, 8u, 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u});
        A32(0x114, maxv);
        const u32 nc = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 4u, 4u, 0xFFFFFFFFu, 0x80000000u});
        A32(0x24, nc);
        for (u32 k = 0; k < 4; ++k) {
            x.P32(1, upO + 0x1C + 4 * k, g.P(85) ? FS(5) : FS(g.Pk({1u, 5u, 7u})));
            x.P32(1, up2O + 0x1C + 4 * k, FS(7));
            x.P32(1, upO + 0x34 + 4 * k, g.Pk({1u, 2u, 3u, 4u, 0u, 8u, 0x40000000u, g.R(6)}));
            x.P32(1, upO + 0x0C + 4 * k, g.P(50) ? PS(3, 0x100u + 0x40u * k) : Raw(g));
        }
        x.P32(1, up2O + 0x2C, g.R(4)); x.P32(1, up2O + 0x30, g.R(4));
        x.P32(1, upO + 0x2C, g.Pk({0u, 1u, 2u, 3u, maxv, maxv + 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, g.R(5)}));
        x.P32(1, upO + 0x30, g.Pk({0u, 1u, 2u, 3u, 5u, 0xFFFFFFFFu, 100u, g.R(8)}));
        for (u32 k = 0; k < 4; ++k) x.P32(1, ibO + 4 * k, g.P(60) ? PS(3, 0x400u + 0x100u * k + 4u * g.R(8)) : Raw(g));
        const u32 ctrO = 0x40u + 4u * g.R(4), outCtrO = 0x80u + 4u * g.R(4);
        x.P32(2, ctrO, g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, g.R(10), 0x40000000u}));
        x.P32(2, outCtrO, g.Pk({0u, 0u, 1u, 2u, 3u, 5u, 0xFFFFFFFFu, g.R(10)}));
        const u32 avail = g.Pk({0u, 1u, 2u, 3u, 4u, 8u, 100u, 0xFFFFFFFFu, g.R(10)});
        x.Args({thisA, PS(1, ibO), PS(2, ctrO), g.R(20), g.P(60) ? PS(3, 0x800u + 4u * g.R(0x10)) : Raw(g), PS(2, outCtrO), avail});
        if (g.P(45)) {
            const u32 id = g.Pk({kErrFk + 5, kErrFk + 5, kErrFk + 6, kErrFk + 7});
            const u32 call = g.R(3);
            const u32 m = g.R(9);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x24, 0, g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu}));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x114, 0, g.Pk({1u, 2u, 3u, 5u, 0u}));
            else if (m == 2) x.Poke(id, call, 1, upO + 0x2C, 0, g.R(6));
            else if (m == 3) x.Poke(id, call, 1, upO + 0x30, 0, g.Pk({0u, 1u, 2u, 8u}));
            else if (m == 4) x.Poke(id, call, 0, x.o + 0x1A4, 1, PS(1, cc2O));
            else if (m == 5) x.Poke(id, call, 0, x.o + 0x1A0, 1, PS(1, up2O));
            else if (m == 6) x.Poke(id, call, 2, ctrO, 0, g.R(6));
            else if (m == 7) x.Poke(id, call, 2, outCtrO, 0, g.R(6));
            else x.Poke(id, call, 1, ibO + 4 * g.R(4), 1, PS(3, 0x600u + 4u * g.R(0x10)));
        }
        break;
    }
    case kUpInit: {
        MkErr(); MkMemFns();
        const u32 compBase = 0x100u + 4u * g.R(0x40), compBase2 = 0x500u + 4u * g.R(0x40);
        A32(0xC4, PS(2, compBase));
        const u32 maxh = g.Pk({1u, 2u, 2u, 3u, 4u, 4u, 6u, 8u}), maxv = g.Pk({1u, 2u, 2u, 3u, 4u, 4u, 8u});
        const u32 mind = g.Pk({1u, 1u, 2u, 4u, 8u, 8u});
        A32(0x110, maxh); A32(0x114, maxv); A32(0x118, mind);
        A8(0x48, g.Pk({0u, 1u, 1u, 0xFFu, 0x80u}));
        A8(0x10A, g.Pk({0u, 0u, 0u, 0u, 1u, 0xFFu}));
        A32(0x5C, g.Pk({0u, 1u, 100u, 640u, 641u, 0x7FFFFFFFu, g.R(2000)}));
        const u32 nc = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 4u, 0xFFFFFFFFu, 0x80000000u});
        A32(0x24, nc);
        auto Comp = [&](u32 e) {
            const u32 sc = g.R(8);
            u32 tH = maxh, tV = maxv;   // objetivo de h_in_group y v_in_group
            if (sc == 2) tH = maxh / 2 ? maxh / 2 : 1; else if (sc == 3) { tH = maxh / 2 ? maxh / 2 : 1; tV = maxv / 2 ? maxv / 2 : 1; }
            else if (sc == 4) { tH = g.Pk({1u, 2u, 1u, 4u}); tV = g.Pk({1u, 2u, 1u, 4u}); }
            else if (sc == 5) { tH = g.Pk({3u, 5u, 6u, 7u}); tV = g.Pk({3u, 1u, 5u}); }
            else if (sc == 6) { tH = g.Pk({1u, 2u, 3u, 4u}); tV = g.Pk({1u, 2u, 3u, 4u}); }
            u32 k = g.Pk({1u, 1u, 2u});
            if (tH % k || tV % k) k = 1;
            x.P32(2, e + 0x08, tH / k); x.P32(2, e + 0x0C, tV / k);
            x.P32(2, e + 0x24, mind * k);
            x.P32(2, e + 0x28, g.Pk({0u, 1u, 2u, 3u, 4u, 100u, 0xFFFFFFFFu, 0x80000000u, g.R(50)}));
            x.P8(2, e + 0x30, sc == 0 ? 0u : g.Pk({1u, 1u, 1u, 0xFFu, 0x80u}));
        };
        for (u32 i = 0; i < 6; ++i) { Comp(compBase + 0x54u * i); Comp(compBase2 + 0x54u * i); }
        x.Ret(kErrFk + 3, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        for (u32 k = 0; k < 6; ++k) x.Ret(kErrFk + 4, k, g.P(50) ? PS(3, 0x400u + 0x40u * k) : Raw(g));
        for (u32 k = 0; k < 6; ++k) x.Ret(kRoundUpId, k, g.Pk({g.W(), g.R(2000), 640u, 0u, g.R(100)}));
        x.Args({thisA});
        if (g.P(45)) {
            const u32 id = g.Pk({kErrFk + 3, kErrFk + 3, kErrFk + 4, kErrFk + 0, kRoundUpId});
            const u32 call = g.R(3);
            const u32 m = g.R(9);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x24, 0, g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu}));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x114, 0, g.Pk({1u, 2u, 4u}));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x110, 0, g.Pk({1u, 2u, 4u}));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0x118, 0, 1u);
            else if (m == 4) x.Poke(id, call, 0, x.o + 0xC4, 1, PS(2, compBase2));
            else if (m == 5) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, mem2O));
            else if (m == 6) x.Poke(id, call, 0, x.o + 0x48, 0, g.Pk({0u, 1u}));
            else if (m == 7) x.Poke(id, call, 0, x.o + 0x5C, 0, g.Pk({0u, 100u, 641u}));
            else x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, err2O));
        }
        break;
    }
    case kErrExit: {
        MkErr();
        A32(0x04, g.W()); A32(0x10, g.W());
        x.Args({thisA});
        if (g.P(25)) x.Throw(kErrFk + g.Pk({1u, 1u, 0u, 3u, 5u}), 0);   // el metodo output_message (o el falso al que apunte) no vuelve
        if (g.P(35)) {
            const u32 m = g.R(3);
            const u32 id = kErrFk + g.Pk({1u, 1u, 0u});
            if (m == 0) x.Poke(id, 0, 0, x.o + 0x00, 1, PS(1, err2O));
            else if (m == 1) x.Poke(id, 0, 1, errO + 4, 0, g.W());
            else x.Poke(id, 0, 0, x.o + 0x04, 0, g.W());
        }
        if (g.P(15)) x.P32(1, errO + 8, FS(g.Pk({0u, 8u, 9u, 10u, 12u, 13u})));
        break;
    }
    case kOutMsg: {
        MkErr();
        x.P32(1, errO + 0xC, FS(2));
        x.Ret(kErrFk + 2, 0, g.W());
        x.Args({thisA});
        if (g.P(30)) x.Poke(kErrFk + 2, 0, 0, x.o + g.Pk({0x00u, 0x04u, 0x10u}), 0, g.W());   // escritura en el cinfo despues de format_message: no debe influir
        break;
    }
    case kStdErr: {
        x.Args({thisA});
        break;
    }
    case kAllocS: case kAllocL: {
        const bool isSmall = fn == kAllocS;
        MkErr();
        A32(0x04, PS(1, memO));
        // ranuras de lista de mem: 0x28..0x48 (pool -3..5): cadenas de cabeceras bien formadas (o nulas); total en +0x4C
        const bool badPool = g.P(14);
        i32 pool = i32(badPool ? g.Pk({2u, 3u, 0xFFFFFFFEu, 4u, 5u, 0xFFFFFFFDu}) : g.Pk({0u, 1u, 1u}));
        u32 hdrIdx = 0;
        for (u32 slot = 0x28; slot <= 0x48; slot += 4) {
            const u32 len = g.Pk({0u, 1u, 1u, 2u, 3u});
            const u32 first = 0x100u + 0x20u * hdrIdx;
            for (u32 k = 0; k < len; ++k) {
                const u32 h = 0x100u + 0x20u * hdrIdx++;
                x.P32(2, h + 0, k + 1 < len ? PS(2, h + 0x20u) : 0u);
                x.P32(2, h + 4, g.Pk({0u, 8u, 0x10u, g.R(0x400)})); x.P32(2, h + 8, g.Pk({0u, 8u, 0x10u, 0x20u, 0x100u, 0x1000u, g.R(0x2000), 0xFFFFFFFFu}));
            }
            x.P32(1, memO + slot, len ? PS(2, first) : 0u);
        }
        x.P32(1, memO + 0x4C, g.Pk({0x54u, 0u, 0x1000u, 0xFFFFFFF0u, g.W()}));
        x.P32(1, memO + 0x50, g.W());
        const u32 size = g.Pk({0u, 1u, 7u, 8u, 9u, 0x10u, 0x28u, 0x100u, 0x1000u, 0x3B9AC9F0u, 0x3B9AC9F1u, 0x3B9AC9E9u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF9u, g.R(0x300), g.R(0x3000), g.W()});
        x.Args({thisA, u32(pool), size});
        const bool tooBig = size > 0x3B9AC9F0u;
        const bool poolErr = !(pool >= 0 && pool < 2);
        u32 errsBefore = (tooBig ? 1u : 0u) + (poolErr ? 1u : 0u);
        const u32 getId = isSmall ? kGetSmall : kGetLarge;
        if (isSmall) {
            const u32 fails = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u});
            for (u32 k = 0; k < 12; ++k) x.Ret(getId, k, k < fails ? 0u : PS(3, 0x100u + 0x40u * k));
            if (g.P(8)) x.Throw(kErrFk + 0, g.R(3));
        } else {
            const bool nullRet = g.P(10);
            x.Ret(getId, 0, nullRet ? 0u : PS(3, 0x100u + 4u * g.R(0x20)));
            if (nullRet) x.Throw(kErrFk + 0, errsBefore);   // el EXE sigue escribiendo sobre el nulo: solo se prueba hasta el error
            else if (g.P(6)) x.Throw(kErrFk + 0, g.R(2));
        }
        if (g.P(40)) {
            const u32 id = isSmall ? g.Pk({kGetSmall, kGetSmall, kErrFk + 0}) : g.Pk({kGetLarge, kErrFk + 0});
            const u32 m = g.R(6);
            if (m == 0) x.Poke(id, 0, 0, x.o + 0x04, 1, PS(1, mem2O));
            else if (m == 1) x.Poke(id, 0, 1, memO + 0x4C, 0, g.W());
            else if (m == 2) x.Poke(id, 0, 1, memO + 0x34 + 4 * (u32(pool) & 7), 0, 0u);
            else if (m == 3) x.Poke(id, 0, 0, x.o + 0x00, 1, PS(1, err2O));
            else if (m == 4) x.Poke(id, 0, 1, memO + 0x28 + 4 * g.R(8), 0, 0u);
            else x.Poke(id, 0, 2, 0x100u + 0x20u * g.R(4) + 8u, 0, g.Pk({0u, 8u, 0xFFFFFFFFu}));
        }
        break;
    }
    case kAllocSa: case kAllocBa: {
        const bool bar = fn == kAllocBa;
        MkErr();
        A32(0x04, PS(1, memO));
        x.P32(1, memO + 0x50, g.W());
        const bool badPool = g.P(10);
        const u32 pool = badPool ? g.Pk({2u, 0xFFFFFFFFu, 7u}) : g.Pk({0u, 1u, 1u});
        // samplesperrow elegido para dar rowsperchunk 1..7, 0 (error) o enorme
        const u32 maxChunk = 0x3B9AC9F0u;
        const u32 want = g.Pk({1u, 2u, 3u, 4u, 5u, 7u, 100u, 0x10000u, 0u});
        u32 spr;
        const u32 sc = g.R(10);
        if (sc < 6) spr = want ? maxChunk / want : 0x3B9AC9F1u;
        else if (sc == 6) spr = g.Pk({1u, 2u, 3u, 0x10u, 0x100u, 0x1000u, 0x100000u});
        else if (sc == 7) spr = g.Pk({0x3B9AC9F0u, 0x3B9AC9F1u, 0xFFFFFFFFu, 0x80000000u, 0x1DCD64F8u, 0x1DCD64F9u});
        else spr = 1u + g.R(0x1000);
        if (bar) {   // la fila mide spr << 7 bytes: el divisor no puede ser 0
            if ((spr << 7) == 0) spr = 1u;
        } else if (spr == 0) spr = 1u;
        const u32 rowBytes = bar ? (spr << 7) : spr;
        const u32 rpc = rowBytes ? maxChunk / rowBytes : 0u;
        u32 numrows = g.Pk({0u, 1u, 2u, 3u, 5u, 8u, 16u, 24u, 1u, 4u});
        const bool err0 = (rpc == 0 || rpc > 0x7FFFFFFFu);
        bool throwHere = false;
        if (err0 && numrows) { x.Throw(kErrFk + 0, 0); throwHere = true; }   // con rowsperchunk <= 0 y filas el bucle no termina: el error_exit no vuelve
        (void)throwHere;
        x.Args({thisA, pool, spr, numrows});
        // la matriz de punteros (numrows * 4 bytes) debe caber en la arena D desde el puntero devuelto
        x.Ret(kAllocSmallId, 0, PS(3, 0x100u + 4u * g.R(0x40)));
        for (u32 k = 0; k < 28; ++k) x.Ret(kAllocLargeId, k, g.P(55) ? PS(3, 0x800u + 0x40u * (k % 16)) : Raw(g));
        if (g.P(40)) {
            const u32 id = g.Pk({kAllocSmallId, kAllocSmallId, kAllocLargeId, kErrFk + 0});
            const u32 call = id == kAllocLargeId ? g.R(2) : 0u;
            const u32 m = g.R(4);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, mem2O));
            else if (m == 1) x.Poke(id, call, 1, memO + 0x50, 0, g.W());
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, err2O));
            else x.Poke(id, call, 3, 0x100u + 4u * g.R(8), 0, g.W());
        }
        break;
    }
    case kReqS: case kReqB: {
        MkErr();
        A32(0x04, PS(1, memO));
        x.P32(1, memO + 0x44, g.P(50) ? 0u : PS(2, 0x100u + 0x80u * g.R(4)));
        x.P32(1, memO + 0x48, g.P(50) ? 0u : PS(2, 0x500u + 0x80u * g.R(4)));
        x.P32(1, mem2O + 0x44, PS(2, 0x900u)); x.P32(1, mem2O + 0x48, PS(2, 0x980u));
        const u32 pool = g.P(85) ? 1u : g.Pk({0u, 2u, 0xFFFFFFFFu, 0x100u, 5u});
        const u32 pre = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x01010100u, 0x80u, g.W()});
        const u32 w = g.Pk({0u, 1u, 100u, 0x10000u, 0xFFFFFFFFu, g.W()}), rows = g.Pk({0u, 1u, 10u, 0x7FFFFFFFu, g.W()}), ma = g.Pk({0u, 1u, 4u, 0x10u, g.W()});
        x.Ret(kAllocSmallId, 0, PS(3, 0x100u + 4u * g.R(0x40)));
        x.Args({thisA, pool, pre, w, rows, ma});
        if (g.P(40)) {
            const u32 id = g.Pk({kAllocSmallId, kErrFk + 0});
            const u32 call = 0;
            const u32 m = g.R(4);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, mem2O));
            else if (m == 1) x.Poke(id, call, 1, memO + (fn == kReqS ? 0x44u : 0x48u), 0, g.Pk({0u, 0x1000u, g.W()}));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, err2O));
            else x.Poke(id, call, 3, 0x100u + 4u * g.R(0x40), 0, g.W());
        }
        break;
    }
    case kRealize: {
        A32(0x04, PS(1, memO));
        x.P32(1, memO + 0x4C, g.W()); x.P32(1, memO + 0x50, g.W());
        // listas de matrices virtuales: sarray en C (0x100 + 0x80 k), barray en D (0x100 + 0x80 k); next en +0x24; las que ya tienen mem_buffer no se procesan
        auto List = [&](u32 arena, u32 headOff) {
            const u32 len = g.Pk({0u, 1u, 1u, 2u, 3u});
            u32 first = 0;
            for (u32 k = 0; k < len; ++k) {
                const u32 s = 0x100u + 0x80u * k;
                x.P32(arena, s + 0x00, g.P(85) ? 0u : PS(3, 0x1800u));
                x.P32(arena, s + 0x04, g.Pk({0u, 1u, 2u, 3u, 10u, 100u, 1000u, 0x7FFFFFFFu, 0x80000000u, g.R(50), g.W()}));
                x.P32(arena, s + 0x08, g.Pk({0u, 1u, 2u, 5u, 16u, 64u, g.R(64), 0x1000u, g.W()}));
                x.P32(arena, s + 0x0C, g.Pk({1u, 1u, 2u, 3u, 4u, 8u, 16u, 0xFFFFFFFFu, g.R(20) + 1u}));
                x.P32(arena, s + 0x24, k + 1 < len ? PS(arena, 0x100u + 0x80u * (k + 1)) : 0u);
                if (k == 0) first = s;
            }
            x.P32(1, memO + headOff, len ? PS(arena, first) : 0u);
        };
        List(2, 0x44); List(3, 0x48);
        for (u32 k = 0; k < 4; ++k) {
            x.Ret(kAllocSarrayId, k, g.P(50) ? PS(3, 0x1000u + 0x40u * k) : Raw(g));
            x.Ret(kAllocBarrayId, k, g.P(50) ? PS(3, 0x1200u + 0x40u * k) : Raw(g));
        }
        x.Ret(kMemAvailId, 0, g.Pk({0xFFFFFFFFu, 0u, 1u, 16u, 100u, 1000u, 0x7FFFFFFFu, 0x80000000u, g.W(), 0x40000000u, g.R(5000), g.R(300)}));
        x.Args({thisA});
        if (g.P(40)) {
            const u32 id = g.Pk({kAllocSarrayId, kAllocBarrayId, kOpenBsId, kMemAvailId});
            const u32 call = id == kMemAvailId ? 0u : g.R(2);
            const u32 m = g.R(5);
            if (m == 0) x.Poke(id, call, 1, memO + 0x50, 0, g.Pk({1u, 5u, g.W()}));
            else if (m == 1) x.Poke(id, call, 1, memO + 0x44, 0, 0u);
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, mem2O));
            else if (m == 3) x.Poke(id, call, 2, 0x100u + 0x10u, 0, g.Pk({1u, 5u, 77u}));
            else x.Poke(id, call, 3, 0x100u + 0x08u, 0, g.Pk({0u, 1u, 8u}));
        }
        break;
    }
    case kFreePool: {
        MkErr();
        A32(0x04, PS(1, memO));
        const bool bad = g.P(10);
        const u32 pool = bad ? g.Pk({2u, 0xFFFFFFFFu, 0x10u, 3u}) : g.Pk({0u, 1u, 1u});
        if (bad) x.Throw(kErrFk + 0, 0);   // con un pool invalido el EXE seguiria sobre memoria que no es una lista: solo se prueba hasta el error
        x.P32(1, memO + 0x4C, g.Pk({0x10000u, g.W(), 0u, 0x54u}));
        // listas virtuales: en C (sarray) y D (barray), 0..3 cada una, con b_s_open, next (+0x24), b_s_info (+0x28) y close_backing_store (+0x30)
        auto Virt = [&](u32 arena, u32 headOff) {
            const u32 len = g.Pk({0u, 1u, 2u, 3u, 1u});
            for (u32 k = 0; k < len; ++k) {
                const u32 s = 0x100u + 0x80u * k;
                x.P8(arena, s + 0x22, g.Pk({0u, 1u, 1u, 0xFFu, 2u}));
                x.P32(arena, s + 0x24, k + 1 < len ? PS(arena, 0x100u + 0x80u * (k + 1)) : 0u);
                x.P32(arena, s + 0x30, g.P(85) ? FS(7) : FS(g.Pk({1u, 7u, 9u})));
            }
            x.P32(1, memO + headOff, len ? PS(arena, 0x100u) : 0u);
        };
        Virt(2, 0x44); Virt(3, 0x48);
        // cadenas grandes y pequenas de cabeceras: en el area D desde 0x1000 (grandes) y 0x1400 (pequenas); next en +0
        auto Chain = [&](u32 headOff, u32 base) {
            const u32 len = g.Pk({0u, 1u, 2u, 3u, 4u});
            for (u32 k = 0; k < len; ++k) {
                const u32 h = base + 0x20u * k;
                x.P32(3, h + 0, k + 1 < len ? PS(3, base + 0x20u * (k + 1)) : 0u);
                x.P32(3, h + 4, g.Pk({0u, 0x10u, 0x100u, g.R(0x1000), g.W()}));
                x.P32(3, h + 8, g.Pk({0u, 0x10u, 0x200u, g.R(0x1000), g.W()}));
            }
            x.P32(1, memO + headOff, len ? PS(3, base) : 0u);
        };
        Chain(0x3C + 4u * (pool & 1), 0x1000u); Chain(0x34 + 4u * (pool & 1), 0x1400u);
        if (!(pool & ~1u)) {   // pool valido: las listas del otro pool tambien existen pero no se tocan
            Chain(0x3C + 4u * (1 - (pool & 1)), 0x1800u); Chain(0x34 + 4u * (1 - (pool & 1)), 0x1C00u);
        }
        x.Args({thisA, pool});
        if (g.P(35)) {
            const u32 id = g.Pk({kErrFk + 7, kErrFk + 7, kFreeLarge, kFreeSmall});
            const u32 call = g.R(3);
            const u32 m = g.R(4);
            if (m == 0) x.Poke(id, call, 1, memO + 0x4C, 0, g.W());
            else if (m == 1) x.Poke(id, call, 1, memO + 0x48, 0, 0u);
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, mem2O));
            else x.Poke(id, call, 2, 0x100u + 0x24u, 0, 0u);
        }
        break;
    }
    case kInitMem: {
        MkErr();
        A32(0x04, g.W());
        const bool nullMem = g.P(8);
        x.Ret(kGetSmall, 0, nullMem ? 0u : PS(3, 0x100u + 4u * g.R(0x40)));
        if (nullMem) x.Throw(kErrFk + 0, 0);   // el EXE sigue escribiendo sobre el nulo: solo se prueba hasta el error
        x.Ret(kMemInitId, 0, g.Pk({0u, 0u, 1u, 0x7FFFFFFFu, 0x80000000u, 5000000u, g.W()}));
        const bool env = g.P(60);
        x.Ret(kGetenv, 0, env ? PS(2, 0x100u + 4u * g.R(0x20)) : 0u);
        x.Ret(kSscanf, 0, g.Pk({0xFFFFFFFFu, 0u, 1u, 1u, 2u, 2u, 2u, 3u}));
        x.Ret(kSscanf, 1, g.Pk({0u, 1u, 5u, 64u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 2147483u, g.W(), g.R(4000)}));
        x.Ret(kSscanf, 2, g.Pk({'m', 'M', 'x', 'k', 0u, 'a', 0x6Du + 0x100u, 'z'}));
        x.Args({thisA});
        if (g.P(30)) {
            const u32 id = g.Pk({kGetSmall, kMemInitId, kErrFk + 0, kGetenv});
            const u32 m = g.R(3);
            if (m == 0) x.Poke(id, 0, 0, x.o + 0x00, 1, PS(1, err2O));
            else if (m == 1) x.Poke(id, 0, 3, 0x100u + 0x2Cu, 0, g.W());
            else x.Poke(id, 0, 0, x.o + 0x04, 0, g.W());
        }
        break;
    }
    case kGetS: case kGetL: {
        const u32 id = kMalloc;
        x.Ret(id, 0, g.Pk({0u, PS(3, 0x100u + 4u * g.R(0x40)), PS(2, 0x100u + 4u * g.R(0x40)), Raw(g) | 1u, 0xFFFFFFFFu, 1u}));
        x.Args({g.P(70) ? thisA : Raw(g), g.Pk({0u, 1u, 0x10u, 0x1000u, 0x3B9ACA00u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(0x100)})});
        break;
    }
    case kFreeS: case kFreeL: {
        x.Args({g.P(70) ? thisA : Raw(g), g.P(60) ? PS(g.Pk({1u, 2u, 3u}), 4u * g.R(0x100)) : Raw(g), g.Pk({0u, 1u, 0x54u, 0x1000u, 0xFFFFFFFFu, g.W(), g.R(0x400)})});
        break;
    }
    case kMemAvail: {
        x.Args({g.P(70) ? thisA : Raw(g), g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.R(100)}), g.Pk({0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W(), g.R(1000), 0x3B9ACA00u}), g.Pk({0u, 0x54u, g.W(), g.R(5000)})});
        break;
    }
    case kOpenBs: {
        MkErr();
        x.Args({thisA, g.P(50) ? PS(2, 4u * g.R(0x100)) : Raw(g), g.Pk({0u, 1u, 0x1000u, 0xFFFFFFFFu, g.W(), 0x7FFFFFFFu})});
        if (g.P(15)) x.Throw(kErrFk + 0, 0);
        if (g.P(25)) x.Poke(kErrFk + 0, 0, 0, x.o + g.Pk({0x00u, 0x04u}), 0, g.W());
        if (g.P(10)) x.P32(1, errO + 0, FS(g.Pk({1u, 8u, 13u})));
        break;
    }
    case kMemInit: case kMemTerm: {
        x.Args({g.P(50) ? thisA : Raw(g)});
        break;
    }
    case kDivUp: case kRoundUp: {
        auto V = [&]() { return g.Pk({0u, 1u, 2u, 3u, 7u, 8u, 9u, 15u, 16u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFF0u, 0x80000001u, g.W(), g.R(2000), 640u, 0xFFFFFFF0u, g.R(100)}); };
        u32 va = V(), vb = V();
        if (g.P(40)) vb = g.Pk({1u, 2u, 3u, 4u, 8u, 16u, 0xFFFFFFFFu, 0xFFFFFFFEu, 5u, 7u, 100u, 0x7FFFFFFFu, 0x80000000u, 0x10000u});
        if (vb == 0) vb = 1u;
        const i32 s = i32(va + vb - 1u);
        if (i32(vb) == -1 && s == INT32_MIN) va += 1u;   // idiv desbordaria
        x.Args({va, vb});
        break;
    }
    case kCopyRows: {
        // matrices de punteros a fila: entrada en B (0x100 + 4 k), salida en B (0x400 + 4 k); filas en C (entrada, 0x100 cada una) y D (salida)
        const bool alias = g.P(12);
        for (u32 k = 0; k < 16; ++k) {
            x.P32(1, 0x200u + 4 * k, PS(2, 0x100u + 0x100u * (k % 12) + g.R(4) * (alias ? 4u : 1u)));
            x.P32(1, 0x600u + 4 * k, alias ? PS(2, 0x100u + 0x100u * (k % 12) + g.R(6) * 4u) : PS(3, 0x100u + 0x100u * (k % 12) + g.R(4)));
        }
        const u32 nr = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u, 2u, 1u});
        const u32 nrows = nr > 5 ? nr : nr;
        const u32 nc = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 9u, 15u, 16u, 0x20u, 0x21u, 0x7Fu, 0x80u, g.R(0x90)});
        const u32 srow = g.R(4), drow = g.R(4);
        x.Args({PS(1, 0x200u), srow, PS(1, 0x600u), drow, nrows, nc});
        break;
    }
    case kZeroFar: {
        const u32 bytes = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 0x10u, 0x11u, 0x20u, 0x7Fu, 0x80u, 0x100u, g.R(0x100)});
        x.Args({thisA, bytes});
        break;
    }
    case kCreateC: {
        MkErr();
        A32(0x0C, g.W());
        const u32 ver = g.Pk({62u, 62u, 62u, 62u, 61u, 63u, 0u, 0xFFFFFFFFu, g.W()});
        const u32 sz = g.Pk({0x168u, 0x168u, 0x168u, 0x168u, 0x16Cu, 0x164u, 0u, 0xFFFFFFFFu, g.W()});
        x.Args({thisA, ver, sz});
        if (g.P(40)) {
            const bool onErr = (ver != 62u || sz != 0x168u) && g.P(70);
            const u32 id = onErr ? kErrFk + 0 : kInitMm;
            const u32 call = onErr ? g.R(2) : 0u;
            const u32 m = g.R(5);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, err2O));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x0C, 0, g.W());
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x10, 0, g.Pk({0u, 1u, 0x100u}));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0x14, 0, g.W());
            else x.Poke(id, call, 0, x.o + 0x160, 0, g.W());
        }
        if (g.P(8)) x.Throw(kErrFk + 0, g.R(2));
        break;
    }
    case kSuppress: {
        for (u32 k = 0; k < 4; ++k) {
            A32(0x48 + 4 * k, g.P(60) ? PS(2, 0x100u + 0x90u * g.R(6)) : 0u);
            A32(0x58 + 4 * k, g.P(60) ? PS(3, 0x100u + 0x120u * g.R(6)) : 0u);
            A32(0x68 + 4 * k, g.P(60) ? PS(3, 0xA00u + 0x120u * g.R(6)) : 0u);
        }
        x.Args({thisA, g.Pk({0u, 1u, 0xFFu, 0x100u, 0x101u, g.W(), 0x80u, 2u})});
        break;
    }
    case kFinish: {
        MkErr();
        const u32 masterO = 0x300u + 4u * g.R(8), master2O = 0x400u + 4u * g.R(8), progO = 0x500u + 4u * g.R(8), coefO = 0x600u + 4u * g.R(8), markO = 0x700u + 4u * g.R(8), destO = 0x800u + 4u * g.R(8);
        const u32 state = g.Pk({0x64u, 0x65u, 0x65u, 0x65u, 0x66u, 0x66u, 0x67u, 0x67u, 0x67u, 0u, 0xFFFFFFFFu, g.W()});
        A32(0x14, state);
        A32(0x8, g.P(55) ? PS(1, progO) : 0u);
        A32(0x18, PS(1, destO));
        const u32 height = g.Pk({0u, 1u, 5u, 100u, 0xFFFFFFFFu, g.R(50)});
        A32(0x20, height);
        A32(0xD0, g.P(60) ? height : g.Pk({0u, 1u, height + 1u, height - 1u, 0xFFFFFFFFu, g.W(), g.R(60)}));
        const u32 rows = g.Pk({0u, 1u, 2u, 3u, 4u, 1u, 2u});
        A32(0xE0, rows);
        A32(0x13C, PS(1, masterO)); A32(0x148, PS(1, coefO)); A32(0x14C, PS(1, markO));
        x.P32(1, masterO + 0, FS(8)); x.P32(1, masterO + 8, FS(9));
        x.P32(1, master2O + 0, FS(1)); x.P32(1, master2O + 8, FS(1));
        const bool lastNow = g.P(35);
        x.P8(1, masterO + 0xC, 0u); x.P8(1, masterO + 0xD, lastNow ? g.Pk({1u, 0xFFu, 0x80u}) : 0u);
        x.P8(1, master2O + 0xD, 1u);
        x.P32(1, progO + 0, FS(10)); x.P32(1, progO + 4, g.W()); x.P32(1, progO + 8, g.W());
        x.P32(1, coefO + 4, FS(11));
        x.P32(1, markO + 0xC, FS(12));
        x.P32(1, destO + 0x10, FS(13));
        for (u32 k = 0; k < 24; ++k) x.Ret(kErrFk + 11, k, g.Pk({1u, 1u, 1u, 1u, 0xFF01u, 0x100u, 0u, 2u}));
        x.Args({thisA});
        // numero de pasadas del bucle: la ultima la marca el finish_pass (id 0xC109) con un efecto lateral sobre master->is_last_pass
        const bool initial = state == 0x65u || state == 0x66u;
        if (!lastNow) {
            const u32 passes = 1u + g.R(3);
            const u32 call = (initial ? 1u : 0u) + passes - 1u;
            x.Poke(kErrFk + 9, call, 1, masterO + 0xC, 0, 0x100u);
            if (g.P(40)) {
                const u32 m = g.R(5);
                const u32 id = g.Pk({kErrFk + 8, kErrFk + 11, kErrFk + 10, kErrFk + 11});
                const u32 c2 = g.R(4);
                if (m == 0) x.Poke2(id, c2, 0, x.o + 0xE0, 0, g.Pk({0u, 1u, 3u}));
                else if (m == 1) x.Poke2(id, c2, 0, x.o + 0x8, 0, 0u);
                else if (m == 2) x.Poke2(id, c2, 0, x.o + 0x148, 1, PS(1, coefO));
                else if (m == 3) x.Poke2(id, c2, 0, x.o + 0x18, 1, PS(1, destO));
                else x.Poke2(id, c2, 0, x.o + 0x00, 1, PS(1, err2O));
            }
        } else if (g.P(40)) {
            x.Poke(kErrFk + 12, 0, 0, x.o + 0x14C, 1, PS(1, markO));
        }
        if (g.P(6)) x.Throw(kErrFk + 0, g.R(2));
        break;
    }
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.A("p", c.p);
    j.A("r", c.r);
    return j.End();
}
}
