#pragma once
// Fixtures del banco Fn236A (lote 236A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 4 funciones del EXE enganchadas SIN validar: modulo jdapimin (API de descompresion) de la libjpeg 6b embebida. Cada una recibe una jpeg_decompress_struct sintetica (arena A, con sus punteros a
// estructuras propias; la tabla de componentes en la arena B) y se observan: bytes cambiados de las cuatro arenas, eventos de los falsos (los que se llaman por punteros de la propia estructura y los
// colaboradores por direccion fija sustituidos por falsos identicos en ambos lados), el valor devuelto y las compuertas de ABI.
//   idx  direccion  funcion                ABI del EXE
//    0   0x5CFD20   jpeg_CreateDecompress  cdecl(cinfo, version, structsize)
//    1   0x5CFFB0   jpeg_consume_input     cdecl(cinfo) -> int en EAX
//    2   0x5D0110   jpeg_finish_decompress cdecl(cinfo) -> bool en AL
//    3   0x5D01D0   jpeg_read_header       cdecl(cinfo, require_image BYTE) -> int en EAX
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

namespace F236AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;
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
// Arena A (desplazamientos desde la base S de la escena): jpeg_decompress_struct 0x000 (0x1B0), err 0x1C0 (0x40), src 0x240 (0x20), inputctl 0x260 (0x20), master 0x280 (0x20).
// Arena B (desde B0): jpeg_component_info[4] (0x54 cada uno) en 0x000. Las arenas C y D no se usan: deben quedar intactas (detectan escrituras ajenas).
constexpr u32 kCi = 0x000, kErr = 0x1C0, kSrc = 0x240, kInp = 0x260, kMaster = 0x280, kSceneSpan = 0x2C0;
constexpr u32 kCiSize = 0x1B0;
constexpr u32 kBComps = 0x000;

// ------------------------------------------------------------------------------------------------ parametros del caso (a[])
enum P : unsigned {
    P_OFF = 0, P_KIND, P_FILL, P_ERRMODE, P_BOFF,
    F_GSTATE, A_VER, A_SIZE, A_REQ, F_CLIENT,
    PRE_NC, PRE_JFIF, PRE_ADOBE, PRE_XF, PRE_ID0, PRE_ID1, PRE_ID2,
    POST_ON, POST_NC, POST_JFIF, POST_ADOBE, POST_XF, POST_ID0, POST_ID1, POST_ID2,
    CR0, CR1, CR2, CR3, CR4, CR5, CR6, CR7, CRDEF, EOI_AT, EOI_INIT,
    F_BUFIMG, F_OUTSCAN, F_OUTH, COMP_BAD,
    SEED0, SEED1, SEED2, SEED3,
    P_COUNT
};

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

// Identificadores de componente: ternas que recorren las ramas de default_decompress_parms (YCbCr 1,2,3; RGB 0x52,0x47,0x42; desconocidas)
inline void IdTriple(Gen& g, u32& i0, u32& i1, u32& i2) {
    switch (g.R(10)) {
    case 0: case 1: i0 = 1; i1 = 2; i2 = 3; break;
    case 2: case 3: i0 = 0x52; i1 = 0x47; i2 = 0x42; break;
    case 4: i0 = 1; i1 = 2; i2 = 4; break;
    case 5: i0 = 1; i1 = 3; i2 = 2; break;
    case 6: i0 = 0x52; i1 = 0x47; i2 = 0x43; break;
    case 7: i0 = 2; i1 = 2; i2 = 3; break;
    case 8: i0 = 0x52; i1 = 0x48; i2 = 0x42; break;
    default: i0 = g.W(); i1 = g.W(); i2 = g.W(); break;
    }
}
inline u32 Byte3(Gen& g) { return g.Pk({0u, 1u, 2u, 0x80u, 0xFFu}); }   // un `boolean` de un byte: 0, 1 y valores no nulos distintos de 1
inline u32 CrVal(Gen& g) { return g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 3u, Neg(-1), 0x100u, 0x101u, 0x80000001u, 0xFFFFFF00u}); }

// Un juego de valores de los campos de color: 14 escenarios que recorren todas las ramas de default_decompress_parms (num_components, JFIF, Adobe, transform, ids de componente)
struct Cs { u32 nc, jfif, adobe, xf, i0, i1, i2; };
inline Cs MakeCs(Gen& g) {
    Cs s{3, 0, 0, g.Pk({0u, 1u, 2u, 3u, 0x80u, 0xFFu}), 1, 2, 3};
    const u32 k = g.R(14);
    const u32 t = g.Pk({1u, 1u, 2u, 0x80u, 0xFFu});   // un `boolean` verdadero
    switch (k) {
    case 0: s.nc = 1; s.jfif = Byte3(g); s.adobe = Byte3(g); IdTriple(g, s.i0, s.i1, s.i2); break;
    case 1: s.jfif = t; s.adobe = Byte3(g); IdTriple(g, s.i0, s.i1, s.i2); break;
    case 2: s.adobe = t; s.xf = 0; IdTriple(g, s.i0, s.i1, s.i2); break;
    case 3: s.adobe = t; s.xf = 1; IdTriple(g, s.i0, s.i1, s.i2); break;
    case 4: s.adobe = t; s.xf = g.Pk({2u, 3u, 0x80u, 0xFFu, 0x7Fu}); IdTriple(g, s.i0, s.i1, s.i2); break;
    case 5: s.i0 = 1; s.i1 = 2; s.i2 = 3; break;
    case 6: s.i0 = 0x52; s.i1 = 0x47; s.i2 = 0x42; break;
    case 7: case 8: do { IdTriple(g, s.i0, s.i1, s.i2); } while ((s.i0 == 1 && s.i1 == 2 && s.i2 == 3) || (s.i0 == 0x52 && s.i1 == 0x47 && s.i2 == 0x42)); break;
    case 9: s.nc = 4; s.jfif = Byte3(g); s.adobe = 0; break;
    case 10: s.nc = 4; s.adobe = t; s.xf = 0; s.jfif = Byte3(g); break;
    case 11: s.nc = 4; s.adobe = t; s.xf = 2; s.jfif = Byte3(g); break;
    case 12: s.nc = 4; s.adobe = t; s.xf = g.Pk({1u, 3u, 0x80u, 0xFFu}); s.jfif = Byte3(g); break;
    default: s.nc = g.Pk({0u, 2u, 5u, Neg(-1), 0x80000000u, g.W()}); s.jfif = Byte3(g); s.adobe = Byte3(g); IdTriple(g, s.i0, s.i1, s.i2); break;
    }
    return s;
}

// ------------------------------------------------------------------------------------------------ generacion
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
    a[P_BOFF] = g.P(75) ? 4 * g.R(0x100) : 4 * g.R(0x100) + 1 + g.R(3);
    a[F_CLIENT] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W()});
    a[F_GSTATE] = 0xC8u + g.R(11);
    a[A_VER] = 0x3Eu; a[A_SIZE] = 0x1B0u;
    a[A_REQ] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu});
    if (g.P(40)) a[A_REQ] |= g.W() & 0xFFFFFF00u;   // la funcion solo mira el byte bajo (el llamador del EXE carga solo AL)
    // valores de los campos que lee default_decompress_parms: antes (escena) y despues (los escribe el falso de consume_input); cada juego recorre una de las ramas de color
    Cs pre = MakeCs(g), post = MakeCs(g);
    a[PRE_NC] = pre.nc; a[PRE_JFIF] = pre.jfif; a[PRE_ADOBE] = pre.adobe; a[PRE_XF] = pre.xf; a[PRE_ID0] = pre.i0; a[PRE_ID1] = pre.i1; a[PRE_ID2] = pre.i2;
    a[POST_ON] = g.P(88) ? 1u : 0u;
    a[POST_NC] = post.nc; a[POST_JFIF] = post.jfif; a[POST_ADOBE] = post.adobe; a[POST_XF] = post.xf; a[POST_ID0] = post.i0; a[POST_ID1] = post.i1; a[POST_ID2] = post.i2;
    // guion de consume_input: valores devueltos por las llamadas 0..7 y despues CRDEF; eoi_reached se pone a 1 tras la llamada EOI_AT (99: nunca)
    for (u32 i = 0; i < 8; ++i) a[CR0 + i] = CrVal(g);
    a[CRDEF] = g.P(70) ? 1u : CrVal(g);
    a[EOI_AT] = g.P(30) ? 99u : g.R(8);
    a[EOI_INIT] = g.P(60) ? 0u : Byte3(g);
    a[F_BUFIMG] = g.P(65) ? 0u : Byte3(g);
    a[F_OUTSCAN] = g.Pk({0u, 1u, 100u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W()});
    a[F_OUTH] = g.P(40) ? a[F_OUTSCAN] : g.Pk({0u, 1u, 100u, 101u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W()});
    if (g.P(25)) a[F_OUTSCAN] = a[F_OUTH] + g.Pk({1u, Neg(-1)});

    switch (fn) {
    case 0:   // jpeg_CreateDecompress: version y tamano de estructura (los dos distintos de los canonicos con cierta frecuencia)
        a[A_VER] = g.P(65) ? 0x3Eu : g.Pk({0x3Du, 0x3Fu, 0u, Neg(-1), 0x3E00u, 0x1B0u, 0x8000003Eu, g.W()});
        a[A_SIZE] = g.P(65) ? 0x1B0u : g.Pk({0x1AFu, 0x1B4u, 0x1B1u, 0u, Neg(-1), 0x1B000u, 0x3Eu, 0x800001B0u, g.W()});
        break;
    case 1:
        if (g.P(55)) a[CR0] = 1u;
        if (g.P(50)) a[F_GSTATE] = 0xC8u + g.R(2);
        else if (g.P(76)) a[F_GSTATE] = 0xC8u + g.R(11);
        else a[F_GSTATE] = g.Pk({0xC7u, 0xD3u, 0xD4u, 0u, 0x64u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x1C8u, 0x100000C8u, 0xC8u - 0x10000u, 0xD1u, g.W()});
        break;
    case 2:
        if (g.P(80)) a[F_GSTATE] = g.Pk({0xCDu, 0xCEu, 0xCFu, 0xD2u, 0xCDu, 0xCEu});
        else a[F_GSTATE] = g.Pk({0xC8u, 0xC9u, 0xCAu, 0xCBu, 0xCCu, 0xD0u, 0xD1u, 0xD3u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x1CDu, g.W()});
        break;
    case 3:
        if (g.P(55)) a[CR0] = 1u;
        if (g.P(82)) a[F_GSTATE] = g.Pk({0xC8u, 0xC9u, 0xC8u, 0xC9u});
        else a[F_GSTATE] = g.Pk({0xCAu, 0xCBu, 0xCDu, 0xD2u, 0xC7u, 0u, 0xFFFFFFFFu, 0x1C8u, g.W()});
        break;
    default: break;
    }
    // la tabla de componentes solo se lee si, al llegar a default_decompress_parms, hay 3 componentes y no hay JFIF ni Adobe; si no se lee, el puntero puede ser basura (no debe tocarse)
    const u32 effNc = a[POST_ON] ? a[POST_NC] : a[PRE_NC], effJ = (a[POST_ON] ? a[POST_JFIF] : a[PRE_JFIF]) & 0xFF, effA = (a[POST_ON] ? a[POST_ADOBE] : a[PRE_ADOBE]) & 0xFF;
    const bool maybeRead = effNc == 3 && !effJ && !effA;
    a[COMP_BAD] = (!maybeRead && g.P(50)) ? 1u : 0u;
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
