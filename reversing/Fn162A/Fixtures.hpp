#pragma once
// Fixtures del banco Fn162A (lote 162A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE con hook en la DLL y SIN banco par a par antes del lote. Las 7 siguientes tenian RH_ScopedInstall dentro de CMatrix::InjectHooks / CVector2D::InjectHooks, que InjectHooksMain
// tiene DESACTIVADOS (rendimiento): el hook NO estaba activo. El lote los activa (CMatrix::InjectHooksLote162A, CVector2D::InjectHooksLote162A) y los valida.
//   idx  direccion  nombre real                                                                    forma
//    0   0x59BB60   CMatrix::Update                                                                thiscall() `ret`: 12 palabras de [this+40h] (RwMatrix) a this (dwords 0,1,2,4,5,6,8,9,10,12,13,14)
//    1   0x59AD20   CMatrix::UpdateMatrix(RwMatrix*)                                               thiscall(rw) `ret 4`: igual con la RwMatrix del argumento
//    2   0x59ACF0   CMatrix::Detach                                                                thiscall() `ret`: si [+40h] y dword [+44h]: RwMatrixDestroy 7F2A20; [+40h] = 0
//    3   0x59ADD0   CMatrix::CopyOnlyMatrix(const CMatrix&)                                         thiscall(src) `ret 4`: rep movsd de 16 dwords (src a this)
//    4   0x59ADF0   CMatrix::operator+=(const CMatrix&)                                             thiscall(rv) `ret 4`: x87, cada una de las 12 palabras = rv + this (fld rv; fadd this; fstp)
//    5   0x459350   CMatrix::Scale(float)                                                          thiscall(f) `ret 4`: x87, las 9 palabras de right/forward/up = f * palabra (fld f; fmul palabra; fstp)
//    6   0x43E110   CVector2D::operator=(const CVector2D&)                                         thiscall(src) `ret 4`: x, y como dwords; eax = this
//   (0x59A840 ConvertToEulerAngles y 0x59AA40 ConvertFromEulerAngles se saltan con evidencia: README.md. 0x638A40 y 0x4F21C0 ya tienen fila V.)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40). La matriz `this` vive en A; el operando origen en B, en C, en A sin solape con this, o es la propia
// matriz (origen == this). No se cubre el solape parcial (el orden de los accesos y la copia de 64 bytes no estan definidos entre el EXE y el C++).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F162AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 7;

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

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
    std::vector<u8> s;          // 128 bytes: [0,64) origen (16 dwords), [64,128) destino inicial de this (16 dwords)
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
// Desplazamiento de un objeto auxiliar de `span` bytes en B o C: alineado (75 %) o desalineado, sin invadir la guarda mas de 3 bytes
inline u32 AuxOffset(Gen& g, u32 span) { const u32 base = 4 * g.R((u32(kWin) - span) / 4 - 1); return g.P(75) ? base : base + 1 + g.R(3); }
// Desplazamiento de un origen de `srcSpan` bytes en A SIN solape con `this` (que ocupa [thisOff, thisOff + thisSpan)), con 4 bytes de margen
inline u32 DisjointOffset(Gen& g, u32 thisOff, u32 thisSpan, u32 srcSpan) {
    const u32 win = u32(kWin);
    const bool low = thisOff >= srcSpan + 8;
    const u32 highMin = thisOff + thisSpan + 4, highMax = win - srcSpan - 8;
    const bool high = thisOff + thisSpan < win && highMin <= highMax;
    u32 s;
    if (low && (!high || g.P(50))) s = 4 * g.R((thisOff - srcSpan - 8) / 4 + 1);
    else s = highMin + 4 * g.R((highMax - highMin) / 4 + 1);
    if (g.P(25)) s += 1 + g.R(3);
    return s;
}

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Un float (como dword) de una clase interesante para la x87: normales de distintas magnitudes, enteros, +-0, subnormales, FLT_MAX, +-inf, NaN silenciosos y senalizantes con carga, exponentes
// altos (desbordan) y bajos (subdesbordan: doble redondeo del producto a 24 bits y luego a float), valores junto a 1.0 y a 2^-24 (empates del redondeo), potencias de dos, mantisa de unos.
inline u32 FloatWord(Gen& g) {
    const u32 sign = g.R(2) << 31;
    const u32 mant = g.W() & 0x7FFFFFu;
    switch (g.R(20)) {
    case 0: case 1: case 2: case 3: case 4: return sign | ((100u + g.R(55)) << 23) | mant;
    case 5: return std::bit_cast<u32>(float(int(g.R(2001)) - 1000));
    case 6: return sign;
    case 7: return sign | (mant ? mant : 1u);
    case 8: return sign | (0x7F7FFFFFu - g.R(3));
    case 9: return sign | 0x7F800000u;
    case 10: return sign | 0x7FC00000u | mant;
    case 11: return sign | 0x7F800000u | ((mant & 0x3FFFFFu) ? (mant & 0x3FFFFFu) : 1u);
    case 12: return sign | ((200u + g.R(55)) << 23) | mant;
    case 13: return sign | ((1u + g.R(60)) << 23) | mant;
    case 14: return sign | ((0x3F800000u + g.R(8)) - g.R(3));
    case 15: return sign | (0x33800000u + g.R(3) - 1u);
    case 16: return sign | ((g.R(254) + 1u) << 23);
    case 17: return sign | ((g.R(254) + 1u) << 23) | 0x7FFFFFu;
    default: return sign | ((70u + g.R(120)) << 23) | mant;
    }
}
inline bool IsNaNBits(u32 w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x7FFFFFu); }
inline bool IsQNaNBits(u32 w) { return IsNaNBits(w) && (w & 0x400000u); }
// Dos operandos de una operacion x87: el oraculo solo contempla, con los dos NaN, NaN silenciosos de carga distinta (gana el mayor significando) o el mismo valor; si no, `second` se sustituye por un normal
inline u32 FixPair(Gen& g, u32 first, u32 second) {
    if (IsNaNBits(first) && IsNaNBits(second) && first != second) {
        const bool ok = IsQNaNBits(first) && IsQNaNBits(second) && (first & 0x7FFFFFu) != (second & 0x7FFFFFu);
        if (!ok) return (second & 0x80000000u) | ((100u + g.R(55)) << 23) | (g.W() & 0x7FFFFFu);
    }
    return second;
}

// Clases de funcion (forma)
enum Kind : u32 { kUpd, kUpdArg, kDetach, kCopy, kAdd, kScale, kV2 };
inline constexpr std::array<Kind, kFunctions> kKind{kUpd, kUpdArg, kDetach, kCopy, kAdd, kScale, kV2};
inline constexpr std::array<u32, kFunctions> kEntry{0x59BB60, 0x59AD20, 0x59ACF0, 0x59ADD0, 0x59ADF0, 0x459350, 0x43E110};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x48, 0x40, 0x48, 0x40, 0x40, 0x40, 8};
// Tamano del origen (RwMatrix / matriz de 0x40 / vector2d); 0: sin origen
inline constexpr std::array<u32, kFunctions> kSrcSpan{0x40, 0x40, 0, 0x40, 0x40, 0, 8};
// Indices (en dwords) de las 12 palabras de la RwMatrix que copian Update y UpdateMatrix y que suman Add; Scale usa las 9 primeras (right, forward, up)
inline constexpr std::array<u32, 12> kRw{0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14};
inline constexpr std::array<u32, 9> kRw9{0, 1, 2, 4, 5, 6, 8, 9, 10};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   funciones con origen (kUpd, kUpdArg, kCopy, kAdd, kV2): a[3] = modo del origen (0 en B, 1 en C, 2 en A sin solape, 3 = this); a[4] = desplazamiento del origen en la arena del modo
//   kUpd: ademas a[5] = dword en +0x44 (no se toca)
//   kDetach: a[3] = modo de la matriz enlazada (0 nula, 1 en B a[4], 2 cruda a[5]); a[6] = dword +0x44
//   kScale: a[3] = el factor (dword)
//   s: origen (16 dwords, los 12 de la RwMatrix de Add como floats de clase) y destino inicial de this (16 dwords); en el modo 3 el origen es el propio destino
inline u32 GetDw(const std::vector<u8>& s, u32 index) { u32 v; std::memcpy(&v, s.data() + 4 * index, 4); return v; }
inline void PutDw(std::vector<u8>& s, u32 index, u32 v) { std::memcpy(s.data() + 4 * index, &v, 4); }
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.s.assign(128, 0);
    const Kind k = kKind[c.fn];
    for (u32 i = 0; i < 32; ++i) PutDw(c.s, i, g.P(30) ? Raw(g) : g.W());
    if (kSrcSpan[c.fn]) {
        c.a[3] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 2u, 2u, 3u, 3u});
        if (c.a[3] == 2) c.a[4] = DisjointOffset(g, c.a[0], kSpan[c.fn], kSrcSpan[c.fn]);
        else if (c.a[3] < 2) c.a[4] = AuxOffset(g, kSrcSpan[c.fn]);
        else c.a[4] = 0;
    }
    switch (k) {
    case kUpd:
        c.a[5] = g.P(30) ? Raw(g) : g.W();
        break;
    case kAdd:
        for (u32 j : kRw) {
            const u32 dst = FloatWord(g);
            PutDw(c.s, 16 + j, dst);
            if (c.a[3] == 3) continue;   // origen == this: el origen es el destino
            const u32 src = g.P(12) ? (dst ^ 0x80000000u) : FloatWord(g);   // 12 %: origen = -destino (cancelacion exacta)
            PutDw(c.s, j, FixPair(g, dst, src));
        }
        break;
    case kDetach:
        c.a[3] = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 2u});
        c.a[4] = AuxOffset(g, 0x40);
        c.a[5] = Raw(g);
        {
            const u32 w1 = g.W(), w2 = g.W() | 1u, w3 = g.W() & ~0xFFu, w4 = (g.W() & ~0xFFu) | 1u;
            c.a[6] = g.Pk({0u, 0u, 1u, 1u, 2u, 0xFFu, 0x100u, 0x10000u, 0x1000000u, 0x01010100u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, w1, w2, w3, w3, w4});
        }
        break;
    case kScale: {
        const u32 sc = FloatWord(g);
        c.a[3] = sc;
        for (u32 j : kRw9) {
            const u32 d = FloatWord(g);
            PutDw(c.s, 16 + j, FixPair(g, sc, d));
        }
        break;
    }
    default: break;
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
    j.H("s", c.s);
    return j.End();
}
}
