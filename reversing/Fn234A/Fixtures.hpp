#pragma once
// Fixtures del banco Fn234A (lote 234A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 10 funciones del EXE enganchadas SIN validar: la parte de jdmarker (lectura de marcadores) y jdmaster (seleccion de modulos) de la libjpeg 6b de DESCOMPRESION embebida. Cada una recibe una
// jpeg_decompress_struct sintetica (en la arena A, con todos sus punteros a estructuras propias) y se observan: bytes cambiados de las cuatro arenas, eventos de los falsos (todos se llaman por punteros de
// la propia estructura o son colaboradores por direccion fija sustituidos por falsos identicos en ambos lados), el valor devuelto (AL) y las puertas de ABI.
//   idx  direccion  funcion                       ABI del EXE
//    0   0x5CB5B0   get_dht                       cdecl(cinfo) -> AL
//    1   0x5CBB20   get_dri                       cdecl(cinfo) -> AL
//    2   0x5CC050   skip_variable                 cdecl(cinfo) -> AL
//    3   0x5CC0F0   next_marker                   cdecl(cinfo) -> AL
//    4   0x5CC1F0   first_marker                  cdecl(cinfo) -> AL
//    5   0x5CC610   read_restart_marker           cdecl(cinfo) -> AL
//    6   0x5CC7D0   jinit_marker_reader           cdecl(cinfo)
//    7   0x5CC8C0   jpeg_calc_output_dimensions   cdecl(cinfo)
//    8   0x5CCB50   prepare_for_output_pass       cdecl(cinfo)
//    9   0x5CCCC0   jinit_master_decompress       cdecl(cinfo)
//
// Familia = n % 16: tipo = familia % 4 (posicion de la escena: 0 alineada, 1 desalineada, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F234AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 10;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ disposicion de la escena
// Arena A (desplazamientos desde la base S de la escena): jpeg_decompress_struct 0x000 (0x1B0), err 0x1C0 (0x40), mem 0x200 (0x20), src 0x240 (0x20), inputctl 0x260 (0x18), marker 0x280 (0xB0), entropy 0x330 (0xB0),
// coef 0x3E0 (0x10), upsample 0x3F0 (0x10), 6 jpeg_component_info (0x54) en 0x400, master 0x600 (0x1C), progress 0x620 (0x14), idct 0x640, cconvert 0x650, cquantize (cinfo, inicial) 0x660, quantizer_1pass 0x670,
// quantizer_2pass 0x680, post 0x690.
constexpr u32 kCi = 0x000, kErr = 0x1C0, kMem = 0x200, kSrc = 0x240, kInp = 0x260, kMrk = 0x280, kEnt = 0x330, kCoef = 0x3E0, kUp = 0x3F0, kComps = 0x400;
constexpr u32 kMaster = 0x600, kProgress = 0x620, kIdct = 0x640, kCconv = 0x650, kQ0 = 0x660, kQ1 = 0x670, kQ2 = 0x680, kPost = 0x690, kSceneSpan = 0x700;
constexpr u32 kCiSize = 0x1B0, kMaxComps = 6;
// Arena B (desde B0): main 0x00 (0x38), zona del asignador (bump) 0x400..0x1C00
constexpr u32 kBMain = 0x00, kBAlloc = 0x400, kBAllocEnd = 0x1C00;
// Arena D (desde D0): tablas Huffman j (0..7) en 0x120*j
constexpr u32 kHStride = 0x120;
// Arena C (desde C0): flujo de entrada 0x000..0x540
constexpr u32 kStreamSize = 0x540;

// ------------------------------------------------------------------------------------------------ parametros del caso (a[])
enum P : unsigned {
    P_OFF = 0, P_KIND, P_FILL, P_ERRMODE, P_BOFF, P_DOFF, P_COFF, P_NOISE,
    A0, A1, A2, A3,
    S_MODE, S_SEED, S_MASK, IN_OFF, IN_LEN,
    CH_OFF0, CH_OFF1, CH_OFF2, CH_OFF3, CH_OFF4, CH_OFF5, CH_OFF6, CH_OFF7,
    CH_LEN0, CH_LEN1, CH_LEN2, CH_LEN3, CH_LEN4, CH_LEN5, CH_LEN6, CH_LEN7,
    F_GSTATE, F_WIDTH, F_HEIGHT, F_NCOMP, F_JCS, F_OUTCS, F_BUFIMG, F_RAW, F_QUANT, F_TWOPASS, F_EN1, F_ENX, F_EN2, F_COLORMAP, F_PROG, F_ARITH, F_MINDCT, F_UNREAD, F_PROGRESS, F_PRCOMP, F_PRTOTAL,
    IC_MULTI, IC_EOI, MK_NEXT, MK_DISC, UP_CTX, RS_RET,
    MS_DUMMY, MS_PASS, MS_MERGED,
    DCMASK, ACMASK,
    DHT_NT, DHT_IDX0, DHT_IDX1, DHT_IDX2, DHT_IDX3, DHT_DESC0, DHT_DESC1, DHT_DESC2, DHT_DESC3, DHT_LENADJ,
    HDESC0, HDESC1, HDESC2, HDESC3, HDESC4, HDESC5, HDESC6, HDESC7,
    COMP0,                                  // 6 componentes x 4 valores (CompField)
    SEED0 = COMP0 + 24, SEED1, SEED2, SEED3,
    P_COUNT
};
constexpr unsigned kCompFields = 4;
enum CompField : unsigned { CF_H = 0, CF_V, CF_WIB, CF_DCT };

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, P_COUNT> a{};
    std::vector<u8> s;          // sin uso en este lote
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};
inline u32 Neg(i32 v) { return u32(v); }

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde, 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// ------------------------------------------------------------------------------------------------ contenido de las tablas y del flujo (lo reproduce el oraculo)
// Cuentas de una tabla Huffman (bits[1..16]); descriptor = modo (3 bits) | semilla << 3. bits[0] es basura (no se usa).
inline void MakeBits(u32 desc, std::array<u32, 17>& bits) {
    const u32 mode = desc & 7, seed = desc >> 3;
    bits.fill(0);
    bits[0] = Rand(seed, 99) & 0xFF;
    switch (mode) {
    case 2: {   // tablas estandar de JPEG (DC y AC de luminancia)
        static constexpr u32 dc[17]{0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
        static constexpr u32 ac[17]{0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D};
        for (u32 l = 1; l <= 16; ++l) bits[l] = (seed & 1) ? dc[l] : ac[l];
        break;
    }
    case 4: break;   // vacia
    case 5: bits[16] = 255; bits[15] = 2; break;   // 257 simbolos
    case 6: bits[1 + seed % 16] = 1; break;        // un simbolo
    case 3: {        // sobresuscrita en un nivel 1..8
        const u32 lv = 1 + Rand(seed, 50) % 8;
        i32 freeCodes = 1;
        for (u32 l = 1; l <= 16; ++l) {
            freeCodes *= 2;
            if (l < lv) { const u32 c = std::min<u32>(Rand(seed, l) % 3, u32(freeCodes)); bits[l] = c; freeCodes -= i32(c); }
            else if (l == lv) { bits[l] = std::min<u32>(255, u32(freeCodes) + 1 + Rand(seed, 51) % 2); freeCodes -= i32(bits[l]); }
            else bits[l] = 0;
        }
        break;
    }
    default: {       // validas: 0 ralas, 1 densas, 7 variables
        const u32 maxc = mode == 0 ? 3 : (mode == 1 ? 12 : 1 + Rand(seed, 60) % 40);
        i32 freeCodes = 1;
        u32 sum = 0;
        for (u32 l = 1; l <= 16; ++l) {
            freeCodes = std::min(freeCodes * 2, 100000);
            const u32 cap = std::min<u32>(maxc, u32(freeCodes));
            u32 c = Rand(seed, l) % (cap + 1);
            if (sum + c > 256) c = 256 - sum;
            bits[l] = c; sum += c; freeCodes -= i32(c);
        }
        break;
    }
    }
}
inline u32 CountOf(u32 desc) { std::array<u32, 17> b; MakeBits(desc, b); u32 s = 0; for (u32 l = 1; l <= 16; ++l) s += b[l]; return s; }
// Simbolo k de una tabla Huffman
inline u32 HuffVal(u32 desc, u32 k) {
    const u32 r = Rand(desc >> 3, 200 + k);
    if (((desc >> 3) & 4) == 0) return r & 0xFF;
    return r % 40 == 0 ? 16 + (r >> 8) % 240 : (r >> 8) & 15;
}
// Flujo de entrada aleatorio (arena C): byte k segun el modo
inline u32 StreamByte(u32 mode, u32 seed, u32 k) {
    const u32 r = Rand(seed, k), b = (r >> 8) & 0xFF;
    switch (mode) {
    case 0: return r & 0xFF;
    case 1: switch (r % 16) { case 0: case 1: case 2: case 3: return 0xFF; case 4: return 0x00; case 5: return 0xD9; case 6: return 0xD0 + (b & 7); default: return b; }
    case 2: return r % 4 == 0 ? 0xFF : (r % 8 == 1 ? 0x00 : b);
    case 3: return (r & 0xFF) == 0xFF ? 0xFE : (r & 0xFF);
    default: return 0xFF;
    }
}
// Cabecera estructurada del flujo (los primeros bytes) segun la funcion; el resto es StreamByte.
inline std::vector<u8> StreamHead(u32 fn, const std::array<u32, P_COUNT>& a) {
    std::vector<u8> h;
    auto b2 = [&](u32 v) { h.push_back(u8(v >> 8)); h.push_back(u8(v)); };
    switch (fn) {
    case 0: {   // un segmento DHT: longitud, y por tabla indice, bits[1..16] y simbolos
        u32 total = 2;
        for (u32 t = 0; t < a[DHT_NT] && t < 4; ++t) total += 17 + CountOf(a[DHT_DESC0 + t]);
        b2(total + a[DHT_LENADJ]);
        for (u32 t = 0; t < a[DHT_NT] && t < 4; ++t) {
            std::array<u32, 17> bits; MakeBits(a[DHT_DESC0 + t], bits);
            h.push_back(u8(a[DHT_IDX0 + t]));
            u32 count = 0;
            for (u32 l = 1; l <= 16; ++l) { h.push_back(u8(bits[l])); count += bits[l]; }
            for (u32 k = 0; k < count; ++k) h.push_back(u8(HuffVal(a[DHT_DESC0 + t], k)));
        }
        break;
    }
    case 1: b2(a[A0]); b2(a[A1]); break;      // get_dri: longitud y valor
    case 2: b2(a[A0]); break;                 // skip_variable: longitud
    case 4: h.push_back(u8(a[A0])); h.push_back(u8(a[A1])); break;   // first_marker: dos bytes
    default: break;
    }
    if (h.size() > kStreamSize) h.resize(kStreamSize);
    return h;
}
inline u32 StreamAt(const std::array<u32, P_COUNT>& a, const std::vector<u8>& head, u32 k) {
    return k < head.size() ? head[k] : StreamByte(a[S_MODE], a[S_SEED], k);
}

// ------------------------------------------------------------------------------------------------ generacion
inline u32 Byte3(Gen& g) { return g.Pk({0u, 1u, 2u, 0x80u, 0xFFu}); }   // un `boolean` de un byte: 0, 1 y valores no nulos distintos de 1
inline u32 Samp(Gen& g) { return g.Pk({1u, 1u, 2u, 2u, 3u, 4u, 4u, 1u, 2u}); }

inline void MakeAll(C& c, unsigned n) {
    auto& a = c.a;
    const u32 f = c.family, fn = c.fn;
    Gen g(n, 100 + fn * 100);
    const u32 kind = f % 4;
    a[P_KIND] = kind;
    a[P_FILL] = f / 4;
    for (u32 i = 0; i < 4; ++i) a[SEED0 + i] = c.seed * 13u + 50u + i;
    a[P_OFF] = Offset(g, kind, kSceneSpan);
    a[P_ERRMODE] = g.R(2);      // 0: error_exit no vuelve (se sale de la llamada como la libjpeg real); 1: vuelve y el cuerpo sigue ejecutando como el EXE
    a[P_NOISE] = g.W() & 0xFFFFFF00u;
    a[P_BOFF] = g.P(75) ? 4 * g.R(0x100) : 4 * g.R(0x100) + 1 + g.R(3);
    a[P_DOFF] = g.P(75) ? 4 * g.R(0x100) : 4 * g.R(0x100) + 1 + g.R(3);
    a[P_COFF] = g.P(75) ? 4 * g.R(0x100) : 4 * g.R(0x100) + 1 + g.R(3);
    // flujo de entrada por omision
    a[S_MODE] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u}); a[S_SEED] = g.W();
    a[S_MASK] = g.P(80) ? 0xFFu : (g.W() & 0xFFu);
    a[IN_OFF] = g.R(0x400); a[IN_LEN] = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 6u, 10u, 50u});
    for (u32 i = 0; i < 8; ++i) { a[CH_OFF0 + i] = g.R(0x400); a[CH_LEN0 + i] = g.Pk({1u, 1u, 2u, 3u, 5u, 8u, 16u, 40u, 100u}); }
    // valores por omision de la estructura
    a[F_GSTATE] = g.P(75) ? 0xCAu : g.Pk({0u, 0xC9u, 0xCBu, 0x64u, Neg(-1), g.W()});
    a[F_WIDTH] = g.Pk({1u, 8u, 16u, 100u, 255u, 256u, 1000u, 0xFFDCu, 0x10000u, 0u, g.R(70000), 0x7FFFFFFFu, 0x80000000u});
    a[F_HEIGHT] = g.Pk({1u, 8u, 16u, 100u, 255u, 256u, 1000u, 0xFFDCu, 0x10000u, 0u, g.R(70000), 0x7FFFFFFFu, 0x80000000u});
    a[F_NCOMP] = g.Pk({1u, 3u, 3u, 3u, 4u, 2u});
    a[F_JCS] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, g.W()});
    a[F_OUTCS] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 1u, 2u, 3u, Neg(-1), 0x80000000u, g.W()});
    a[F_BUFIMG] = Byte3(g); a[F_RAW] = g.P(70) ? 0u : Byte3(g); a[F_QUANT] = Byte3(g); a[F_TWOPASS] = Byte3(g);
    a[F_EN1] = Byte3(g); a[F_ENX] = Byte3(g); a[F_EN2] = Byte3(g);
    a[F_COLORMAP] = g.P(55) ? 0u : (g.W() | 1u);
    a[F_PROG] = g.P(85) ? 0u : Byte3(g); a[F_ARITH] = g.P(90) ? 0u : Byte3(g);
    a[F_MINDCT] = g.Pk({1u, 2u, 3u, 4u, 8u, 8u});
    a[F_UNREAD] = g.P(60) ? 0u : g.Pk({1u, 0xD9u, 0xFFu, g.R(256), g.W()});
    a[F_PROGRESS] = g.P(70) ? 1u : 0u; a[F_PRCOMP] = g.W(); a[F_PRTOTAL] = g.W();
    a[IC_MULTI] = Byte3(g); a[IC_EOI] = g.P(25) ? Byte3(g) : 0u;
    a[MK_NEXT] = g.W(); a[MK_DISC] = g.W(); a[UP_CTX] = Byte3(g); a[RS_RET] = Byte3(g);
    a[MS_DUMMY] = g.P(30) ? Byte3(g) : 0u; a[MS_PASS] = g.Pk({0u, 1u, 2u, 7u, Neg(-1), g.R(100), g.W()}); a[MS_MERGED] = g.P(85) ? 0u : Byte3(g);
    a[DCMASK] = g.P(35) ? 1u << g.R(4) : 0u; a[ACMASK] = g.P(35) ? 1u << g.R(4) : 0u;
    for (u32 j = 0; j < 8; ++j) a[HDESC0 + j] = g.R(8) | ((g.W() >> 3) << 3);
    for (u32 i = 0; i < kMaxComps; ++i) {
        u32* k = &a[COMP0 + kCompFields * i];
        k[CF_H] = Samp(g); k[CF_V] = g.P(6) ? g.Pk({Neg(-1), 0u, Neg(-3)}) : Samp(g);
        k[CF_WIB] = g.Pk({1u, 2u, 3u, 7u, 8u, 100u, g.R(2000), g.W()});
        k[CF_DCT] = g.Pk({8u, 8u, 8u, 4u, 2u, 1u, 16u, 0u});
    }
    bool mustEscape = false;
    // flujo contiguo a partir de IN_OFF = 0 (con saltos aleatorios de vez en cuando) para los flujos estructurados
    auto chunks = [&](u32 total) {
        u32 cursor = a[IN_OFF] + a[IN_LEN];
        for (u32 i = 0; i < 8; ++i) {
            const bool jump = !g.P(85);
            const u32 rest = total > cursor ? total - cursor : 1;
            a[CH_OFF0 + i] = jump ? g.R(0x400) : cursor;
            a[CH_LEN0 + i] = g.Pk({rest, rest, 1u, 2u, 3u, 5u, 8u, 16u, 40u, 100u});
            cursor = a[CH_OFF0 + i] + a[CH_LEN0 + i];
        }
    };

    switch (fn) {
    case 0: {   // get_dht
        a[DHT_NT] = g.Pk({1u, 1u, 1u, 2u, 2u, 3u, 4u, 0u});
        u32 total = 2;
        for (u32 t = 0; t < 4; ++t) {
            const u32 r = g.R(100);
            u32 idx;
            if (r < 88) idx = g.Pk({0u, 1u, 2u, 3u, 0x10u, 0x11u, 0x12u, 0x13u});
            else if (r < 94) idx = 4 + g.R(4);
            else idx = g.Pk({0x14u, 0x1Fu, 0x20u, 0x80u, 0xFFu, 0x18u});
            a[DHT_IDX0 + t] = idx;
            a[DHT_DESC0 + t] = g.R(8) | ((g.W() >> 3) << 3);
            if (t < a[DHT_NT]) {
                total += 17 + CountOf(a[DHT_DESC0 + t]);
                if (CountOf(a[DHT_DESC0 + t]) > 256 || (idx >= 8 && !(idx >= 0x10 && idx <= 0x13))) mustEscape = true;   // count > 256 desborda la pila del EXE; indice fuera de las ranuras validas lee punteros basura
            }
        }
        a[DHT_LENADJ] = g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u, 5u, Neg(-1), Neg(-3), Neg(-20), 17u, 0x7FFFu});
        if (i32((total + a[DHT_LENADJ]) & 0xFFFFu) - i32(total) > 16) mustEscape = true;   // la longitud declarada sobra: lee otra tabla de los bytes de relleno (count > 256 desbordaria la pila del EXE)
        a[DCMASK] = g.P(30) ? (g.W() & 15) : 0u; a[ACMASK] = g.P(30) ? (g.W() & 15) : 0u;
        a[IN_OFF] = 0; a[IN_LEN] = g.Pk({total, total, total, total, 0u, 1u, 2u, 3u, 4u, 19u, 20u, 21u, g.R(total + 1)});
        if (a[IN_LEN] < total) mustEscape = true;   // entrada truncada: los rellenos del fill dan bytes de flujo como indices/cuentas; error_exit sale por longjmp antes de usar el puntero
        chunks(total);
        break;
    }
    case 1: case 2: case 4: {
        u32 total = 4;
        if (fn == 1) {
            a[A0] = g.P(75) ? 4u : g.Pk({0u, 1u, 2u, 3u, 5u, 6u, 0x100u, 0xFFFFu, g.W() & 0xFFFFu});
            a[A1] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, g.W() & 0xFFFFu});
            a[IN_LEN] = g.Pk({4u, 4u, 4u, 0u, 1u, 2u, 3u, 5u, 6u});
        } else if (fn == 2) {
            a[A0] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0x10u, 0x100u, 0x7FFFu, 0x8000u, 0xFFFFu, g.W() & 0xFFFFu});
            a[IN_LEN] = g.Pk({2u, 2u, 2u, 0u, 1u, 3u, 10u});
            total = 2;
        } else {
            if (g.P(70)) { a[A0] = 0xFF; a[A1] = 0xD8; }
            else { const u32 k = g.R(5); a[A0] = k == 0 ? 0xFFu : (k == 1 ? 0xD8u : (k == 2 ? 0u : (k == 3 ? 0xFFu : g.W() & 0xFF))); a[A1] = k == 0 ? 0xD9u : (k == 1 ? 0xFFu : (k == 2 ? 0u : (k == 3 ? 0xFFu : g.W() & 0xFF))); }
            a[IN_LEN] = g.Pk({2u, 2u, 2u, 0u, 1u, 3u});
            total = 2;
        }
        a[IN_OFF] = 0;
        chunks(total + 8);
        break;
    }
    case 3: {
        a[MK_DISC] = g.Pk({0u, 0u, 0u, 1u, 5u, 0xFFFFu, Neg(-1), Neg(-1000), g.W() & 0xFFFFFFu});
        break;
    }
    case 5: {
        const u32 nn = g.P(88) ? g.R(8) : g.Pk({8u, 9u, Neg(-1), 0x100u, g.W() & 0xFFFFu});
        a[MK_NEXT] = nn;
        const u32 r = g.R(100);
        a[F_UNREAD] = r < 33 ? 0u : (r < 66 ? 0xD0u + nn : g.Pk({0xD0u + ((nn + 1) & 7), 0xD0u + g.R(8), 0xFFu, g.R(256), g.W()}));
        break;
    }
    case 6: case 7: case 8: break;
    case 9: {
        a[F_NCOMP] = g.Pk({1u, 3u, 3u, 4u, 2u, 0u, Neg(-1), 4u});
        a[F_MINDCT] = g.Pk({1u, 2u, 3u, 4u, 8u, 8u, 8u});
        a[F_GSTATE] = g.P(88) ? 0xCAu : g.W();
        break;
    }
    default: break;
    }
    if (mustEscape) a[P_ERRMODE] = 0;
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
    j.H("s", c.s);
    return j.End();
}
}
