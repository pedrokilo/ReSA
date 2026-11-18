#pragma once
// Fixtures del banco Fn233A (lote 233A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 12 funciones del EXE enganchadas SIN validar: la parte de DESCOMPRESION de la libjpeg 6b embebida (jdhuff.c, jdinput.c, jdmainct.c). Cada una recibe una jpeg_decompress_struct sintetica (en la arena A, con todos
// sus punteros a estructuras propias) y se observan: bytes cambiados de las cuatro arenas, eventos de los falsos (todos se llaman por punteros de la propia estructura, asi que son los mismos en el EXE y en la DLL),
// el valor devuelto y las puertas de ABI. Los ayudantes sin hook (initial_setup 0x5CA2A0, per_scan_setup 0x5CA490, latch_quant_tables 0x5CA630 y alloc_funny_pointers 0x5CA890, con el cinfo en ESI o EBX) se
// ejercitan por sus llamadores (consume_markers, start_input_pass y jinit_d_main_controller).
//   idx  direccion  funcion                    ABI del EXE
//    0   0x5C9790   jpeg_make_d_derived_tbl    cdecl(cinfo, isDC BYTE, tblno, pdtbl)
//    1   0x5C9A60   jpeg_fill_bit_buffer       cdecl(state, get_buffer, bits_left, nbits) -> AL
//    2   0x5C9B80   jpeg_huff_decode           cdecl(state, get_buffer, bits_left, dtbl, min_bits) -> EAX
//    3   0x5CA260   jinit_huff_decoder         cdecl(cinfo)
//    4   0x5CA6E0   start_input_pass           cdecl(cinfo)
//    5   0x5CA720   consume_markers            cdecl(cinfo) -> EAX
//    6   0x5CA7E0   reset_input_controller     cdecl(cinfo)
//    7   0x5CA840   jinit_input_controller     cdecl(cinfo)
//    8   0x5CA940   make_funny_pointers        cdecl(cinfo)
//    9   0x5CAA90   set_wraparound_pointers    cdecl(cinfo)
//   10   0x5CAB70   set_bottom_pointers        cdecl(cinfo)
//   11   0x5CAE30   jinit_d_main_controller    cdecl(cinfo, need_full_buffer BYTE)
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

namespace F233AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 12;
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
// Arena A (desplazamientos desde la base S de la escena): jpeg_decompress_struct 0x000 (0x1B0), err 0x1C0, mem 0x200, src 0x240, inputctl 0x260, marker 0x280 (0xB0), entropy 0x330 (0xB0), coef 0x3E0,
// upsample 0x3F0, 6 jpeg_component_info (0x54) en 0x400.
constexpr u32 kCi = 0x000, kErr = 0x1C0, kMem = 0x200, kSrc = 0x240, kInp = 0x260, kMrk = 0x280, kEnt = 0x330, kCoef = 0x3E0, kUp = 0x3F0, kComps = 0x400, kSceneSpan = 0x600;
constexpr u32 kCiSize = 0x1B0, kMaxComps = 6;
// Arena B (desde B0): main 0x00 (0x38), xbuffer[0] 0x40, xbuffer[1] 0x50, state 0x60 (0x14), ranura de pdtbl 0x80, zona del asignador (bump) 0x400..0x1C00, dtbl existente 0x1000
constexpr u32 kBMain = 0x00, kBX0 = 0x40, kBX1 = 0x50, kBState = 0x60, kBSlot = 0x80, kBAlloc = 0x400, kBAllocEnd = 0x1C00, kBDtbl = 0x1000;
// Arena D (desde D0): tablas Huffman j (0..7) en 0x120*j; tablas de cuantificacion k (0..3) en 0x900 + 0x90*k; zona "pub" de jpeg_huff_decode en 0x1400
constexpr u32 kHStride = 0x120, kQBase = 0x900, kQStride = 0x90, kPub = 0x1400;
// Arena C (desde C0): flujo de entrada 0x000..0x540
constexpr u32 kStreamSize = 0x540;
constexpr u32 kDtblSize = 0x590;

// ------------------------------------------------------------------------------------------------ parametros del caso (a[])
enum P : unsigned {
    P_OFF = 0, P_KIND, P_FILL, P_ERRMODE, P_BOFF, P_DOFF, P_COFF, P_NOISE,
    A0, A1, A2, A3, A4,
    S_MODE, S_SEED, S_MASK, IN_OFF, IN_LEN,
    CH_OFF0, CH_OFF1, CH_OFF2, CH_OFF3, CH_OFF4, CH_OFF5, CH_OFF6, CH_OFF7,
    CH_LEN0, CH_LEN1, CH_LEN2, CH_LEN3, CH_LEN4, CH_LEN5, CH_LEN6, CH_LEN7,
    F_WIDTH, F_HEIGHT, F_NCOMP, F_INSCAN, F_OUTSCAN, F_PREC, F_PROG, F_MAXH, F_MAXV, F_MINDCT, F_TOTROWS, F_CINS, F_UNREAD, F_COEFB,
    F_CUR0, F_CUR1, F_CUR2, F_CUR3,
    IC_MULTI, IC_EOI, IC_INHDR, MK_SAWSOF, MK_RET, EN_WARN, UP_CTX, CO_CONSUME, MN_WHICH, MN_ROWS,
    QMASK, DCMASK, ACMASK,
    QDESC0, QDESC1, QDESC2, QDESC3,
    HDESC0, HDESC1, HDESC2, HDESC3, HDESC4, HDESC5, HDESC6, HDESC7,
    DT_EXIST, DT_DESC,
    COMP0,                                  // 6 componentes x 10 valores (CompField)
    SEED0 = COMP0 + 60, SEED1, SEED2, SEED3,
    P_COUNT
};
constexpr unsigned kCompFields = 10;
enum CompField : unsigned { CF_H = 0, CF_V, CF_Q, CF_WIB, CF_HIB, CF_DCT, CF_DSW, CF_DSH, CF_NEED, CF_QT };

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

// ------------------------------------------------------------------------------------------------ contenido de las tablas (lo reproduce el oraculo)
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
    case 5: bits[16] = 255; bits[15] = 2; break;   // 257 simbolos: avisa con ERREXIT(8) en l = 16
    case 6: bits[1 + seed % 16] = 1; break;        // un simbolo
    case 3: {        // sobresuscrita en un nivel 1..8 (el codigo desborda 1 << l): ERREXIT(8) y sigue
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
// Simbolo k de una tabla Huffman: el bit 2 de la semilla pide simbolos 0..15 (validos como DC) con un 1/40 de simbolos > 15
inline u32 HuffVal(u32 desc, u32 k) {
    const u32 r = Rand(desc >> 3, 200 + k);
    if (((desc >> 3) & 4) == 0) return r & 0xFF;
    return r % 40 == 0 ? 16 + (r >> 8) % 240 : (r >> 8) & 15;
}
inline u32 QuantWord(u32 desc, u32 k) { return Rand(desc >> 3, 300 + k) & 0xFFFF; }   // palabra k (0..64 = 0x82 bytes) de una tabla de cuantificacion
// jpeg_huff_decode: tabla derivada falsa
inline i32 MaxCodeVal(u32 desc, u32 l) {   // l = 1..16
    const u32 r = Rand(desc >> 3, 400 + l);
    switch (desc & 3) {
    case 0: { i32 m = std::min<i32>(0x3FF, (1 << l) - 1) - i32(r % 4); return m < -1 ? -1 : m; }
    case 1: return r % 4 == 0 ? -1 : i32(r % 0x400);
    case 2: return l < 9 ? -1 : i32(r % 0x400);
    default: return i32(r % 0x401) - 1;
    }
}
inline i32 ValOffVal(u32 desc, u32 l) { return i32(Rand(desc >> 3, 500 + l) % 0x600) - 0x300; }
// Flujo de entrada (arena C): byte k segun el modo
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
inline u32 BufWord(u32 seed, u32 ci, u32 k) { return Rand(seed + 77 * (ci + 1), 600 + k) | 1u; }   // contenido de buffer[ci] (fn 8..10)
inline u32 BlockWord(u32 seed, u32 ci, u32 k) { return Rand(seed + 99 * (ci + 1), 700 + k) | 2u; } // contenido inicial de los arrays xbuf (fn 8..10)

// ------------------------------------------------------------------------------------------------ disposicion de fn 8..10 en la arena B
constexpr i32 IDiv(i32 a, i32 b) { return a / b; }
struct LayoutB {
    u32 buf[4]{}, xb0[4]{}, xb1[4]{}, bufWords[4]{}, blockWords[4]{}, blockOff[4]{};
    bool ok{true};
};
inline LayoutB MakeLayout(const std::array<u32, P_COUNT>& a) {
    LayoutB L;
    const i32 M = i32(a[F_MINDCT]);
    u32 cur = kBAlloc;
    for (u32 ci = 0; ci < 4; ++ci) {
        const u32* k = &a[COMP0 + kCompFields * ci];
        const i32 rg = M != 0 ? IDiv(i32(k[CF_V] * k[CF_DCT]), M) : 0;
        if (rg > 0) {
            L.bufWords[ci] = u32(rg * (M + 2)); L.buf[ci] = cur; cur += 4 * L.bufWords[ci];
            L.blockWords[ci] = u32(2 * rg * (M + 4)); L.blockOff[ci] = cur;
            L.xb0[ci] = cur + 4 * u32(rg); L.xb1[ci] = L.xb0[ci] + 4 * u32(rg * (M + 4)); cur += 4 * L.blockWords[ci];
        } else { L.buf[ci] = cur; L.blockOff[ci] = cur; L.xb0[ci] = cur; L.xb1[ci] = cur; }
    }
    L.ok = cur <= kBAllocEnd;
    return L;
}

// ------------------------------------------------------------------------------------------------ generacion
inline u32 Byte3(Gen& g) { return g.Pk({0u, 1u, 2u, 0x80u, 0xFFu}); }   // un `boolean` de un byte: 0, 1 y valores no nulos distintos de 1
inline u32 SmallS(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 15u, 16u, 63u, 255u, Neg(-1), Neg(-2), Neg(-128), 0x7FFFFFFFu, 0x80000000u, g.W() & 0xFFFFu, g.W()}); }
inline u32 Samp(Gen& g) { return g.Pk({1u, 1u, 2u, 2u, 3u, 4u, 4u, 1u, 2u}); }
inline u32 SampAny(Gen& g) { return g.P(80) ? Samp(g) : g.Pk({Neg(-1), Neg(-2), 5u, 6u, 100u}); }   // sin ceros (division)
inline u32 BitsLeft(Gen& g) { return g.Pk({0u, 1u, 7u, 8u, 9u, 15u, 16u, 17u, 23u, 24u, 25u, 26u, 31u, 32u, Neg(-1), Neg(-5), g.R(41)}); }
inline u32 NBits(Gen& g) { return g.Pk({0u, 1u, 2u, 8u, 9u, 16u, 17u, 24u, 25u, 26u, 30u, 32u, Neg(-1), g.R(41)}); }

// Escena de un barrido (start_input_pass / consume_markers): true si seguir tras un error_exit que vuelve no es seguro (se leeria una tabla nula o se escribiria fuera de MCU_membership / cur_comp_info)
inline bool ScanUnsafe(const std::array<u32, P_COUNT>& a) {
    const i32 cs = i32(a[F_CINS]);
    if (cs > 4) return true;
    u32 blocks = 0;
    for (i32 i = 0; i < cs; ++i) {
        const u32* k = &a[COMP0 + kCompFields * std::min<u32>(a[F_CUR0 + u32(i)], kMaxComps - 1)];
        if (cs != 1) {
            const u32 mb = k[CF_V] * k[CF_H];
            if (i32(blocks + mb) > 10) return true;
            if (i32(mb) > 0) blocks += mb;
        }
        if (k[CF_QT] == 0) {
            const i32 q = i32(k[CF_Q]);
            if (q < 0 || q >= 4 || ((a[QMASK] >> q) & 1)) return true;
        }
    }
    return false;
}

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
    // flujo de entrada
    a[S_MODE] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u}); a[S_SEED] = g.W();
    a[S_MASK] = g.P(80) ? 0xFFu : (g.W() & 0xFFu);
    a[IN_OFF] = g.R(0x400); a[IN_LEN] = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 6u, 10u, 50u});
    for (u32 i = 0; i < 8; ++i) { a[CH_OFF0 + i] = g.R(0x400); a[CH_LEN0 + i] = g.Pk({1u, 1u, 2u, 3u, 5u, 8u, 16u, 40u, 100u}); }
    // valores por omision de la estructura
    a[F_WIDTH] = g.Pk({1u, 8u, 16u, 100u, 255u, 256u, 1000u, 0xFFDCu, 0xFFDDu, 0x10000u, 0u, g.R(70000), 0x7FFFFFFFu, 0x80000000u});
    a[F_HEIGHT] = g.Pk({1u, 8u, 16u, 100u, 255u, 256u, 1000u, 0xFFDCu, 0xFFDDu, 0x10000u, 0u, g.R(70000), 0x7FFFFFFFu, 0x80000000u});
    a[F_NCOMP] = g.Pk({1u, 3u, 3u, 3u, 4u, 2u});
    a[F_INSCAN] = g.Pk({0u, 1u, 5u, Neg(-1), 0x7FFFFFFFu, g.W()}); a[F_OUTSCAN] = g.Pk({0u, 1u, 5u, 10u, Neg(-1), 0x7FFFFFFFu, g.W()});
    a[F_PREC] = g.Pk({8u, 8u, 8u, 12u, 0u, Neg(-8), 16u});
    a[F_PROG] = g.P(75) ? 0u : Byte3(g);
    a[F_MAXH] = g.Pk({1u, 2u, 2u, 3u, 4u, 8u}); a[F_MAXV] = g.Pk({1u, 2u, 2u, 3u, 4u, 8u});
    a[F_MINDCT] = g.Pk({1u, 2u, 3u, 4u, 8u, 8u});
    a[F_TOTROWS] = g.W(); a[F_CINS] = g.Pk({1u, 1u, 2u, 3u, 4u, 0u});
    a[F_UNREAD] = g.P(60) ? 0u : g.Pk({1u, 0xD9u, 0xFFu, g.R(256), g.W()});
    a[F_COEFB] = g.W();
    for (u32 i = 0; i < 4; ++i) a[F_CUR0 + i] = g.R(kMaxComps);
    a[IC_MULTI] = Byte3(g); a[IC_EOI] = g.P(25) ? Byte3(g) : 0u; a[IC_INHDR] = Byte3(g);
    a[MK_SAWSOF] = Byte3(g); a[MK_RET] = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 2u, 3u, Neg(-1), 0x80000000u, 0x7FFFFFFFu, 4u});
    a[EN_WARN] = g.Pk({0u, 0u, 1u, 2u, 0x80u}); a[UP_CTX] = Byte3(g); a[CO_CONSUME] = g.W(); a[MN_WHICH] = g.Pk({0u, 1u, 0u, 1u}); a[MN_ROWS] = g.W();
    a[QMASK] = g.P(15) ? 1u << g.R(4) : 0u; a[DCMASK] = g.P(15) ? 1u << g.R(4) : 0u; a[ACMASK] = g.P(15) ? 1u << g.R(4) : 0u;
    for (u32 i = 0; i < 4; ++i) a[QDESC0 + i] = g.W();
    for (u32 j = 0; j < 8; ++j) a[HDESC0 + j] = g.R(8) | ((g.W() >> 3) << 3);
    a[DT_EXIST] = g.P(50) ? 1u : 0u; a[DT_DESC] = g.W();
    for (u32 i = 0; i < kMaxComps; ++i) {
        u32* k = &a[COMP0 + kCompFields * i];
        k[CF_H] = Samp(g); k[CF_V] = Samp(g);
        k[CF_Q] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, Neg(-1), 4u, 7u});
        k[CF_WIB] = g.Pk({1u, 2u, 3u, 7u, 8u, 100u, g.R(2000), g.W()}); k[CF_HIB] = g.Pk({1u, 2u, 3u, 7u, 8u, 100u, g.R(2000), g.W()});
        k[CF_DCT] = g.Pk({8u, 8u, 8u, 4u, 2u, 1u, 16u}); k[CF_DSW] = g.Pk({1u, 8u, 100u, g.W()}); k[CF_DSH] = g.Pk({1u, 7u, 8u, 9u, 16u, 17u, 100u, g.R(300), g.W()});
        k[CF_NEED] = Byte3(g); k[CF_QT] = g.P(70) ? 0u : (g.W() | 1u);
    }
    bool mustEscape = false;
    auto comp = [&](u32 i) { return &a[COMP0 + kCompFields * i]; };

    switch (fn) {
    case 0: {   // jpeg_make_d_derived_tbl
        const u32 isdc = g.Pk({0u, 1u, 1u, 0u, 2u, 0x80u, 0xFFu});
        a[A0] = isdc;
        const bool dc = (isdc & 0xFF) != 0;
        const u32 tbl = g.P(88) ? g.R(4) : g.Pk({Neg(-1), 4u, 5u, 6u, 7u, 100u});
        a[A1] = tbl;
        if (tbl < 4) {
            if (dc ? ((a[DCMASK] >> tbl) & 1) : ((a[ACMASK] >> tbl) & 1)) mustEscape = true;   // tabla nula
        } else {
            const bool viaAc = dc && tbl <= 7;      // dc con tbl 4..7 lee ac_huff_tbl_ptrs[tbl - 4]
            if (!(viaAc && !((a[ACMASK] >> (tbl - 4)) & 1))) mustEscape = true;
        }
        break;
    }
    case 1: {   // jpeg_fill_bit_buffer
        a[A0] = g.P(30) ? 0u : g.W(); a[A1] = BitsLeft(g); a[A2] = NBits(g);
        break;
    }
    case 2: {   // jpeg_huff_decode
        a[A0] = g.P(30) ? 0u : g.W();
        a[A1] = g.P(60) ? g.Pk({0u, 1u, 3u, 8u, 9u, 12u, 16u, 25u, 32u, g.R(33)}) : BitsLeft(g);
        a[A2] = g.Pk({1u, 1u, 2u, 3u, 8u, 9u, 9u, 9u, 12u, 16u, 17u, 1u + g.R(17)});
        break;
    }
    case 3: case 6: case 7: break;
    case 4: {   // start_input_pass
        a[F_CINS] = g.Pk({1u, 1u, 1u, 2u, 2u, 2u, 3u, 3u, 3u, 4u, 4u, 0u, Neg(-1), 5u});
        for (u32 i = 0; i < kMaxComps; ++i) { u32* k = comp(i); k[CF_H] = g.P(88) ? g.Pk({1u, 1u, 1u, 2u, 2u, 3u}) : SampAny(g); k[CF_V] = g.P(88) ? g.Pk({1u, 1u, 1u, 2u, 2u, 3u}) : SampAny(g); }
        mustEscape = ScanUnsafe(a);
        break;
    }
    case 5: {   // consume_markers
        for (u32 i = 0; i < kMaxComps; ++i) { u32* k = comp(i); k[CF_H] = g.P(85) ? Samp(g) : g.Pk({Neg(-1), 5u, 0u, 100u, Neg(-3)}); k[CF_V] = g.P(85) ? Samp(g) : g.Pk({Neg(-1), 5u, 0u, 100u, Neg(-3)}); }
        a[F_NCOMP] = g.Pk({1u, 3u, 3u, 3u, 4u, 2u, 0u, Neg(-1), 5u, 6u});
        a[F_CINS] = g.Pk({1u, 1u, 2u, 3u, 3u, 4u, 4u, 0u, Neg(-1)});
        // start_input_pass solo corre con val == 1, sin eoi y sin inheaders: sin ceros en h/v (dividen)
        const bool eoi = (a[IC_EOI] & 0xFF) != 0, inh = (a[IC_INHDR] & 0xFF) != 0;
        if (!eoi && a[MK_RET] == 1 && !inh) {
            for (u32 i = 0; i < kMaxComps; ++i) { u32* k = comp(i); if (k[CF_H] == 0) k[CF_H] = 1; if (k[CF_V] == 0) k[CF_V] = 1; }
            mustEscape = ScanUnsafe(a);
        }
        break;
    }
    case 8: case 9: case 10: {
        a[F_NCOMP] = g.Pk({1u, 2u, 3u, 3u, 4u, 4u, 0u, Neg(-1)});
        a[F_MINDCT] = fn == 10 ? g.Pk({1u, 2u, 3u, 4u, 4u}) : g.Pk({1u, 2u, 3u, 4u, 6u, 8u});
        for (u32 i = 0; i < 4; ++i) {
            u32* k = comp(i);
            if (fn == 10) { k[CF_V] = g.Pk({1u, 2u, 3u, 4u}); k[CF_DCT] = g.Pk({4u, 8u, 8u}); }
            else { k[CF_V] = g.P(5) ? g.Pk({Neg(-1), Neg(-2)}) : g.Pk({1u, 2u, 3u, 4u, 1u, 2u}); k[CF_DCT] = g.Pk({8u, 8u, 4u, 2u, 1u}); if (i32(k[CF_V]) < 0) k[CF_DCT] = 8; }
        }
        for (int tries = 0; tries < 8 && !MakeLayout(a).ok; ++tries) for (u32 i = 0; i < 4; ++i) { comp(i)[CF_V] = 1; if (tries > 3) comp(i)[CF_DCT] = std::min<u32>(comp(i)[CF_DCT], 4u); }
        break;
    }
    case 11: {  // jinit_d_main_controller
        a[A0] = g.Pk({0u, 0u, 0u, 1u, 2u, 0x80u, 0xFFu});
        a[F_NCOMP] = g.Pk({1u, 3u, 3u, 4u, 2u, 0u, Neg(-1), 4u});
        a[F_MINDCT] = g.Pk({1u, 2u, 3u, 4u, 8u, 8u, 8u});
        for (u32 i = 0; i < 4; ++i) { u32* k = comp(i); k[CF_V] = g.P(5) ? g.Pk({Neg(-1), 0u, Neg(-3)}) : Samp(g); k[CF_DCT] = g.Pk({8u, 8u, 4u, 2u, 1u, 16u}); }
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
