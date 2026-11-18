#pragma once
// Fixtures del banco Fn231A (lote 231A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 18 funciones del EXE enganchadas SIN validar: la parte de compresion de la libjpeg 6b embebida (jcmarker.c, jcmaster.c, jcapimin.c, jcomapi.c). Cada una recibe una jpeg_compress_struct sintetica (en la arena A, con todos
// sus punteros a estructuras propias) y se observan: bytes cambiados de las cuatro arenas, eventos de los falsos (todos se llaman por punteros de la propia estructura, asi que son los mismos en el EXE y en la DLL), el valor
// devuelto y las puertas de ABI. Los tres ayudantes sin hook (InitialSetup 0x5C6430, SelectScanParameters 0x5C6600 y PerScanSetup 0x5C6680, ESI = cinfo) se ejercitan por JinitCMasterControl y PrepareForPass.
//   idx  direccion  funcion                  ABI del EXE
//    0   0x5C55D0   EmitDqt                  usercall: EAX = cinfo, indice en pila, `ret`; devuelve la precision en EAX
//    1   0x5C5780   EmitDht                  usercall: ECX = cinfo, AL = es AC, indice en pila, `ret`
//    2   0x5C5910   EmitDri                  usercall: EAX = cinfo
//    3   0x5C5940   EmitSof                  usercall: ECX = cinfo, AL = marcador
//    4   0x5C5AC0   EmitSos                  usercall: EAX = cinfo
//    5   0x5C5CA0   EmitJfifApp0             usercall: EAX = cinfo
//    6   0x5C5EC0   EmitAdobeApp14           usercall: EAX = cinfo
//    7   0x5C60B0   WriteFileHeader          cdecl(cinfo)
//    8   0x5C6100   WriteFrameHeader         cdecl(cinfo)
//    9   0x5C62D0   WriteFileTrailer         cdecl(cinfo)
//   10   0x5C63D0   JinitMarkerWriter        cdecl(cinfo)
//   11   0x5C6850   PrepareForPass           cdecl(cinfo)
//   12   0x5C69E0   PassStartup              cdecl(cinfo)
//   13   0x5C6A90   JinitCMasterControl      cdecl(cinfo, transcode_only)
//   14   0x5C6B50   JpegAbort                cdecl(cinfo)
//   15   0x5C6B90   JpegDestroy              cdecl(cinfo)
//   16   0x5C6BB0   JpegAllocQuantTable      cdecl(cinfo) -> tabla
//   17   0x5C6BD0   JpegAllocHuffTable       cdecl(cinfo) -> tabla
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

namespace F231AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 18;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
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

// ------------------------------------------------------------------------------------------------ disposicion de la escena (desplazamientos desde la base de la escena en la arena A)
constexpr u32 kCi = 0x000, kErr = 0x170, kMem = 0x1B0, kPrg = 0x1E0, kDst = 0x1F8, kMst = 0x210, kMrk = 0x230, kSubs = 0x250, kComps = 0x290, kSceneSpan = 0x530;
// tablas en la arena D: cuantificacion i en D0 + 0x90*i; Huffman j (dc 0..3, ac 4..7) en D0 + 0x240 + 0x120*j
constexpr u32 kQStride = 0x90, kHBase = 0x240, kHStride = 0x120;

// ------------------------------------------------------------------------------------------------ parametros del caso (a[])
enum P : unsigned {
    P_OFF = 0, P_KIND, P_FILL, P_ARG, P_ARG2, P_EAXHI, P_COFF, P_DSIZE, P_DFREE, P_EOBRET, P_ERRMODE, P_BOFF, P_DOFF, P_QMASK, P_DCMASK, P_ACMASK, P_NOMEM, P_NOPROG,
    F_WIDTH, F_HEIGHT, F_INCOMP, F_PREC, F_NCOMP, F_CSPACE, F_NSCANS, F_SCANINFO, F_RAWIN, F_ARITH, F_OPT, F_RINT, F_RROWS, F_JFIF, F_JMAJ, F_JMIN, F_DUNIT, F_XD, F_YD, F_ADOBE, F_PROG,
    F_MAXH, F_MAXV, F_CINSCAN, F_SS, F_SE, F_AH, F_AL, F_STATE, F_ISDEC,
    M_CALLST, M_ISLAST, M_PTYPE, M_PNUM, M_TOTAL, M_SCANNO, K_LASTRI,
    CUR0, CUR1, CUR2, CUR3,
    QDESC0, QDESC1, QDESC2, QDESC3, QSENT0, QSENT1, QSENT2, QSENT3,
    HDESC0, HDESC1, HDESC2, HDESC3, HDESC4, HDESC5, HDESC6, HDESC7, HSENT0, HSENT1, HSENT2, HSENT3, HSENT4, HSENT5, HSENT6, HSENT7,
    COMP0,                                  // 8 componentes x 8 valores: id, h, v, quant_tbl_no, dc_tbl_no, ac_tbl_no, width_in_blocks, height_in_blocks
    SEED0 = COMP0 + 64, SEED1, SEED2, SEED3,
    P_COUNT
};
constexpr unsigned kCompFields = 8;
enum CompField : unsigned { CF_ID = 0, CF_H, CF_V, CF_Q, CF_DC, CF_AC, CF_WIB, CF_HIB };

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, P_COUNT> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
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

// Contenido de las tablas (descriptor = modo (3 bits) | semilla << 3). Palabra k de una tabla de cuantificacion, y bits / huffval de una tabla Huffman. Lo reproduce el oraculo.
inline u32 QuantWord(u32 desc, u32 k) {
    const u32 mode = desc & 7, seed = desc >> 3, r = Rand(seed, k);
    const u32 pos = Rand(seed, 77) % 64;
    switch (mode) {
    case 0: return 1 + r % 255;
    case 1: return r & 0xFFFF;
    case 2: return k == pos ? 256 + (Rand(seed, 78) % 65280) : 1 + r % 255;
    case 3: return 255;
    case 4: return k == pos ? 256 : 255;
    case 5: return 0;
    case 6: return r & 0xFF;
    default: return (r & 1) ? (r >> 8) & 0xFFFF : r & 0xFF;
    }
}
inline u32 HuffBits(u32 desc, u32 i) {   // i = 0..16 (bits[0] no se usa)
    const u32 mode = desc & 7, seed = desc >> 3;
    if (i == 0) return Rand(seed, 99) & 0xFF;
    const u32 r = Rand(seed, i);
    switch (mode) {
    case 0: return r % 2;
    case 1: return r % 3;
    case 2: return r % 9;
    case 3: return r % 17;
    case 4: return 0;
    case 5: return i == 16 ? 255 : (i == 15 ? 1 : 0);
    case 6: return 17;
    default: return i == 1 + seed % 16 ? Rand(seed, 55) % 256 : 0;
    }
}
inline u32 HuffVal(u32 desc, u32 k) { return Rand(desc >> 3, 200 + k) & 0xFF; }

// Un valor "interesante" de 32 bits
inline u32 Wide(Gen& g) { return g.Pk({0u, 1u, 0xFFu, 0x100u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W(), g.W() & 0xFFFFu, g.W() & 0xFFu}); }
inline u32 Byte3(Gen& g) { return g.Pk({0u, 1u, 2u, 0x80u, 0xFFu}); }   // un `boolean` de un byte: 0, 1 y valores no nulos distintos de 1 (el EXE compara contra cero)
inline u32 SmallS(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 15u, 16u, 63u, 255u, Neg(-1), Neg(-2), Neg(-128), 0x7FFFFFFFu, 0x80000000u, g.W() & 0xFFFFu, g.W()}); }
inline u32 Samp(Gen& g) { return g.Pk({1u, 1u, 2u, 2u, 3u, 4u, 4u, 1u, 2u}); }              // factor de muestreo valido (1..4)
inline u32 SampAny(Gen& g) { return g.P(75) ? Samp(g) : g.Pk({1u, Neg(-1), 5u, 6u, 100u, 0x7FFFFFFFu, 16u}); }   // con valores invalidos pero distintos de cero (PerScanSetup divide por ellos)

inline u32 SampB(Gen& g) { return g.P(75) ? Samp(g) : g.Pk({1u, Neg(-1), 5u, 6u, 100u, 16u}); }   // PerScanSetup: acotado (la suma de bloques por MCU no debe desbordar el entero con signo)

inline bool ShortOutput(u32 fn) { return fn == 2 || fn == 3 || fn == 4 || fn == 5 || fn == 6 || fn == 7 || fn == 9; }
// jpeg_compress_struct: ranura pedida de una tabla nula? (indice 0..3 cuantificacion, 4..7 dc, 8..11 ac)
inline bool SlotNull(const std::array<u32, P_COUNT>& a, u32 slot) {
    return slot < 4 ? (a[P_QMASK] >> slot) & 1 : (slot < 8 ? (a[P_DCMASK] >> (slot - 4)) & 1 : (slot < 12 ? (a[P_ACMASK] >> (slot - 8)) & 1 : 0));
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
    a[P_EAXHI] = g.W() & 0xFFFFFF00u;
    a[P_EOBRET] = g.P(88) ? g.Pk({1u, 1u, 1u, 2u, 0x80u, 0xFFu}) : 0u;   // AL de empty_output_buffer (0 = no se puede suspender)
    a[P_DOFF] = g.P(75) ? 4 * g.R(0x101) : 4 * g.R(0x100) + 1 + g.R(3);
    a[P_BOFF] = g.P(80) ? 4 * g.R(0x100) : 4 * g.R(0x100) + 1 + g.R(3);

    // destino
    const bool shortFn = ShortOutput(fn);
    const bool zeroFree = shortFn && g.P(4);
    const u32 size = shortFn ? g.Pk({1u, 2u, 3u, 4u, 5u, 8u, 16u, 17u, 64u, 255u, 256u, 0x200u}) : g.Pk({8u, 9u, 16u, 17u, 31u, 64u, 100u, 255u, 256u, 300u, 0x200u, 0x400u});
    const u32 sm1 = size > 1 ? size - 1 : 1u;
    a[P_DSIZE] = size;
    a[P_DFREE] = zeroFree ? 0u : std::min(size, g.Pk({1u, 1u, 2u, 3u, size, size, sm1, 1 + g.R(size), 1 + g.R(size), 1 + g.R(size)}));
    a[P_COFF] = zeroFree ? 4 * g.R(0x200) : Offset(g, g.R(4), size);

    // valores por omision de la estructura (plausibles y variados; cada funcion fija despues los suyos)
    a[F_WIDTH] = g.Pk({1u, 8u, 16u, 100u, 255u, 256u, 1000u, 0xFFDCu, g.R(70000)});
    a[F_HEIGHT] = g.Pk({1u, 8u, 16u, 100u, 255u, 256u, 1000u, 0xFFDCu, g.R(70000)});
    a[F_INCOMP] = g.Pk({1u, 3u, 3u, 4u});
    a[F_PREC] = g.Pk({8u, 8u, 8u, 12u});
    a[F_NCOMP] = g.Pk({1u, 3u, 3u, 3u, 4u, 2u});
    a[F_CSPACE] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u});
    a[F_NSCANS] = g.Pk({1u, 1u, 2u, 3u, 10u});
    a[F_SCANINFO] = 0;
    a[F_RAWIN] = Byte3(g); a[F_ARITH] = g.P(80) ? 0u : Byte3(g); a[F_OPT] = Byte3(g);
    a[F_RINT] = g.Pk({0u, 0u, 1u, 100u, 0xFFFFu}); a[F_RROWS] = g.Pk({0u, 0u, 1u, 2u, 7u});
    a[F_JFIF] = Byte3(g); a[F_JMAJ] = g.Pk({1u, 1u, 2u, 0u, 0xFFu}); a[F_JMIN] = g.Pk({0u, 1u, 2u, 0xFFu, g.R(256)}); a[F_DUNIT] = g.Pk({0u, 1u, 2u, 0xFFu});
    a[F_XD] = g.Pk({0u, 1u, 72u, 96u, 0xFFFFu, 0x8000u, g.R(65536)}); a[F_YD] = g.Pk({0u, 1u, 72u, 96u, 0xFFFFu, 0x8000u, g.R(65536)});
    a[F_ADOBE] = Byte3(g); a[F_PROG] = g.P(75) ? 0u : Byte3(g);
    a[F_MAXH] = g.Pk({1u, 2u, 2u, 3u, 4u}); a[F_MAXV] = g.Pk({1u, 2u, 2u, 3u, 4u});
    a[F_CINSCAN] = g.Pk({1u, 2u, 3u, 4u}); a[F_SS] = g.Pk({0u, 0u, 1u, 63u}); a[F_SE] = g.Pk({63u, 63u, 0u, 5u}); a[F_AH] = g.Pk({0u, 0u, 1u, 2u}); a[F_AL] = g.Pk({0u, 0u, 1u, 2u, 3u});
    a[F_STATE] = g.Pk({0u, 100u, 200u, 0xDEADBEEFu, g.W()}); a[F_ISDEC] = Byte3(g);
    a[M_CALLST] = Byte3(g); a[M_ISLAST] = Byte3(g); a[M_PTYPE] = g.Pk({0u, 1u, 2u}); a[M_PNUM] = g.Pk({0u, 1u, 2u, 3u}); a[M_TOTAL] = g.Pk({1u, 2u, 3u, 4u, 6u}); a[M_SCANNO] = g.Pk({0u, 0u, 1u, 2u});
    a[K_LASTRI] = g.Pk({0u, 1u, 0xFFFFu, g.W()});
    for (u32 i = 0; i < 4; ++i) a[CUR0 + i] = g.R(8);
    for (u32 i = 0; i < 4; ++i) { a[QDESC0 + i] = g.R(8) | ((g.W() >> 3) << 3); a[QSENT0 + i] = g.P(70) ? 0u : g.Pk({1u, 2u, 0x80u, 0xFFu}); }
    for (u32 j = 0; j < 8; ++j) { a[HDESC0 + j] = g.R(8) | ((g.W() >> 3) << 3); a[HSENT0 + j] = g.P(70) ? 0u : g.Pk({1u, 2u, 0x80u, 0xFFu}); }
    a[P_QMASK] = g.P(20) ? 1u << g.R(4) : 0u; a[P_DCMASK] = g.P(20) ? 1u << g.R(4) : 0u; a[P_ACMASK] = g.P(20) ? 1u << g.R(4) : 0u;
    a[P_NOMEM] = 0; a[P_NOPROG] = g.P(30) ? 1u : 0u;
    for (u32 i = 0; i < 8; ++i) {
        u32* k = &a[COMP0 + kCompFields * i];
        k[CF_ID] = g.Pk({1u, 2u, 3u, 4u, g.R(256), g.W()});
        k[CF_H] = Samp(g); k[CF_V] = Samp(g);
        k[CF_Q] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u});
        k[CF_DC] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u}); k[CF_AC] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u});
        k[CF_WIB] = g.Pk({1u, 2u, 3u, 7u, 8u, 100u, g.R(2000), g.W()}); k[CF_HIB] = g.Pk({1u, 2u, 3u, 7u, 8u, 100u, g.R(2000), g.W()});
    }
    bool mustEscape = false;
    auto compPtr = [&](u32 i) { return &a[COMP0 + kCompFields * i]; };

    switch (fn) {
    case 0: {   // EmitDqt
        const u32 idx = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 4u + g.R(8)});
        a[P_ARG] = idx;
        if (g.P(70)) a[idx < 4 ? QSENT0 + idx : HSENT0 + idx - 4] = 0;
        if (g.P(10)) {   // tabla nula en la ranura pedida
            if (idx < 4) a[P_QMASK] |= 1u << idx; else if (idx < 8) a[P_DCMASK] |= 1u << (idx - 4); else a[P_ACMASK] |= 1u << (idx - 8);
        }
        mustEscape = SlotNull(a, idx);
        break;
    }
    case 1: {   // EmitDht
        const u32 al = g.Pk({0u, 0u, 0u, 1u, 2u, 0x80u, 0xFFu});
        a[P_ARG2] = al;
        const u32 idx = al ? g.R(4) : g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 4u + g.R(4)});
        a[P_ARG] = idx;
        const u32 slot = al ? 4 + idx : idx;   // ranura Huffman pedida: dc 0..3, ac 4..7 (en dc el indice 4..7 lee la tabla ac)
        if (g.P(70)) a[HSENT0 + slot] = 0;
        if (g.P(10)) { if (slot < 4) a[P_DCMASK] |= 1u << slot; else a[P_ACMASK] |= 1u << (slot - 4); }
        mustEscape = SlotNull(a, 4 + slot);
        break;
    }
    case 2: a[F_RINT] = Wide(g); break;   // EmitDri
    case 3: {   // EmitSof
        a[P_ARG2] = g.Pk({0xC0u, 0xC1u, 0xC2u, 0xC9u, g.R(256), 0u, 0xFFu});
        a[F_NCOMP] = g.Pk({1u, 2u, 3u, 3u, 3u, 4u, 0u, Neg(-1), Neg(-3), 5u, 6u, 7u, 8u});
        a[F_PREC] = g.Pk({8u, 8u, 12u, 0u, 16u, 255u, g.W()});
        a[F_WIDTH] = g.P(60) ? g.Pk({1u, 100u, 255u, 256u, 0xFFFFu, g.R(65536)}) : Wide(g);
        a[F_HEIGHT] = g.P(60) ? g.Pk({1u, 100u, 255u, 256u, 0xFFFFu, g.R(65536)}) : Wide(g);
        for (u32 i = 0; i < 8; ++i) {
            u32* k = compPtr(i);
            k[CF_H] = g.Pk({1u, 2u, 3u, 4u, 0u, 15u, 16u, 0x11u, 0xFFu, g.W()}); k[CF_V] = g.Pk({1u, 2u, 3u, 4u, 0u, 15u, 16u, 0x11u, 0xFFu, g.W()});
            k[CF_Q] = SmallS(g);
        }
        break;
    }
    case 4: {   // EmitSos
        a[F_CINSCAN] = g.Pk({1u, 2u, 3u, 4u, 4u, 0u, Neg(-1), Neg(-3), 0x80000000u});
        for (u32 i = 0; i < 8; ++i) { u32* k = compPtr(i); k[CF_DC] = SmallS(g); k[CF_AC] = SmallS(g); }
        a[F_PROG] = Byte3(g); a[F_ARITH] = Byte3(g);
        a[F_SS] = g.Pk({0u, 0u, 1u, 63u, SmallS(g)}); a[F_SE] = SmallS(g); a[F_AH] = g.Pk({0u, 0u, 1u, 2u, 7u, 15u, 16u, SmallS(g)}); a[F_AL] = g.Pk({0u, 1u, 2u, 7u, 15u, 16u, SmallS(g)});
        break;
    }
    case 5: break;   // EmitJfifApp0: los campos por omision cubren los valores
    case 6: a[F_CSPACE] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u, 5u, 6u, Neg(-1), 0x100u, 0x103u, g.W()}); break;   // EmitAdobeApp14
    case 7: a[F_CSPACE] = g.Pk({0u, 1u, 2u, 3u, 5u, 6u}); break;   // WriteFileHeader
    case 8: {   // WriteFrameHeader
        a[F_NCOMP] = g.Pk({1u, 2u, 3u, 3u, 3u, 4u, 4u, 0u, 5u, 8u, Neg(-1)});
        a[F_PREC] = g.Pk({8u, 8u, 8u, 8u, 12u, 0u, 16u, 255u});
        a[F_PROG] = g.P(65) ? 0u : Byte3(g); a[F_ARITH] = g.P(75) ? 0u : Byte3(g);
        const i32 nc = i32(a[F_NCOMP]);
        for (u32 i = 0; i < 8; ++i) {
            u32* k = compPtr(i);
            k[CF_Q] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 4u + g.R(8)});
            k[CF_DC] = g.Pk({0u, 0u, 1u, 1u, 2u, Neg(-1), 3u, g.R(300), 0x80000000u}); k[CF_AC] = g.Pk({0u, 0u, 1u, 1u, 2u, Neg(-1), 3u, g.R(300), 0x80000000u});
            if (g.P(30)) { k[CF_DC] = g.R(2); k[CF_AC] = g.R(2); }   // mas casos de base
        }
        if (g.P(6) && nc > 0) {   // una tabla nula en un componente usado
            const u32 q = compPtr(g.R(u32(std::min(nc, 8))))[CF_Q];
            if (q < 4) a[P_QMASK] |= 1u << q; else if (q < 8) a[P_DCMASK] |= 1u << (q - 4); else if (q < 12) a[P_ACMASK] |= 1u << (q - 8);
        }
        // si algun componente usado apunta a una ranura nula, error_exit no puede volver (se leeria la tabla nula)
        for (i32 i = 0; i < nc && i < 8; ++i) mustEscape |= SlotNull(a, compPtr(u32(i))[CF_Q]);
        break;
    }
    case 9: break;   // WriteFileTrailer
    case 10: break;  // JinitMarkerWriter
    case 11: {       // PrepareForPass
        a[M_PTYPE] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 2u, 2u, 2u, 3u, 4u, Neg(-1), 0x7FFFFFFFu, 0x80000000u});
        a[M_PNUM] = g.Pk({0u, 1u, 2u, 3u, 5u, Neg(-1), 0x7FFFFFFFu, 0xFFFFFFFEu});
        a[M_TOTAL] = g.Pk({0u, 1u, 2u, 3u, 4u, 6u, Neg(-1), 0x80000000u, 0x7FFFFFFFu, 1u});
        a[M_SCANNO] = g.Pk({0u, 0u, 0u, 1u, 2u, Neg(-1)});
        a[F_NCOMP] = g.Pk({1u, 1u, 2u, 3u, 3u, 3u, 4u, 4u, 0u, Neg(-1), Neg(-2), 5u, 8u});
        a[F_OPT] = Byte3(g); a[F_RAWIN] = Byte3(g); a[F_ARITH] = g.P(70) ? 0u : Byte3(g);
        a[F_SS] = g.Pk({0u, 0u, 1u, Neg(-1), 63u}); a[F_AH] = g.Pk({0u, 0u, 1u, 2u, Neg(-1)});
        a[F_RROWS] = g.Pk({0u, 0u, 1u, 2u, 7u, Neg(-1), Neg(-5), 0x7FFFFFFFu, 0xFFFFu, 100000u, 0x80000000u});
        a[F_WIDTH] = g.Pk({1u, 8u, 100u, 256u, 1000u, 0xFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W()});
        a[F_HEIGHT] = g.Pk({1u, 8u, 100u, 256u, 1000u, 0xFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W()});
        a[F_RINT] = g.Pk({0u, 1u, 0xFFFFu, g.W()});
        a[F_MAXH] = g.Pk({1u, 2u, 2u, 3u, 4u, 1u, Neg(-1), 5u, 100u});
        a[F_MAXV] = g.Pk({1u, 2u, 2u, 3u, 4u, 1u, Neg(-1), 5u, 100u});
        for (u32 i = 0; i < 8; ++i) { u32* k = compPtr(i); k[CF_H] = SampB(g); k[CF_V] = SampB(g); }
        const i32 nc = i32(a[F_NCOMP]);
        const bool scans = i32(a[M_PTYPE]) >= 0 && a[M_PTYPE] <= 2;
        if (scans && nc > 4) mustEscape = true;    // SelectScanParameters escribiria cur_comp_info[4..] sobre campos vecinos
        if (scans && nc > 1 && nc <= 4) {          // barrido entrelazado: mas de 10 bloques por MCU no puede volver (MCU_membership desbordaria)
            i32 blocks = 0;
            for (i32 i = 0; i < nc; ++i) {
                const i32 mb = i32(compPtr(u32(i))[CF_V] * compPtr(u32(i))[CF_H]);
                if (blocks + mb > 10) mustEscape = true;
                if (mb > 0) blocks += mb;
            }
        }
        break;
    }
    case 12: break;  // PassStartup
    case 13: {       // JinitCMasterControl
        a[P_ARG] = g.Pk({0u, 0u, 1u, 1u, 2u, 0x80u, 0xFFu}) | (g.W() & 0xFFFFFF00u);   // transcode_only (solo se lee el byte bajo)
        a[F_OPT] = Byte3(g); a[F_PROG] = Byte3(g);
        a[F_SCANINFO] = g.P(15) ? g.Pk({1u, 0xFFFFFFFFu, g.W() | 1u}) : 0u;
        a[F_WIDTH] = g.Pk({1u, 8u, 16u, 100u, 0xFFDCu, 0xFFDDu, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0u, g.R(70000), g.R(2000)});
        a[F_HEIGHT] = g.Pk({1u, 8u, 16u, 100u, 0xFFDCu, 0xFFDDu, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0u, g.R(70000), g.R(2000)});
        a[F_INCOMP] = g.Pk({1u, 3u, 4u, 0u, Neg(-1), 2u});
        a[F_PREC] = g.Pk({8u, 8u, 8u, 8u, 8u, 12u, 0u, 16u, Neg(-8)});
        a[F_NCOMP] = g.Pk({1u, 3u, 3u, 3u, 4u, 2u, 1u, 0u, Neg(-1), 5u, 8u});
        a[F_MAXH] = g.W(); a[F_MAXV] = g.W();
        for (u32 i = 0; i < 8; ++i) { u32* k = compPtr(i); k[CF_H] = SampAny(g); k[CF_V] = SampAny(g); }
        break;
    }
    case 14: a[P_NOMEM] = g.P(25) ? 1u : 0u; break;   // JpegAbort
    case 15: a[P_NOMEM] = g.P(25) ? 1u : 0u; break;   // JpegDestroy
    default: break;   // 16, 17: alloc_small
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
