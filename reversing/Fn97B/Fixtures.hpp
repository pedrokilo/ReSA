#pragma once
// Fixtures del banco Fn97B (lote 97A, segunda parte): 16384 casos por funcion (98304 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CMatrix::SetRotateZOnly   0x59B020  thiscall (ECX = matriz, float angulo), `ret 4`; fsin/fcos del x87, sin colaboradores                                   (hook S)
//   1 CMatrix::UpdateRwMatrix   0x59AD70  thiscall (ECX = matriz, RwMatrix*), `ret 4`; copia 12 dwords y llama a RwMatrixUpdate 0x7F18A0 (cdecl)              (hook S)
//   2 CMatrix::UpdateRW         0x59BBB0  thiscall (ECX = matriz), `ret`; si +0x40 != 0 llama a UpdateRwMatrix 0x59AD70 (colaborador con hook)                  (hook S)
//   3 CMatrix::Destructor       0x59ACD0  thiscall (ECX = matriz), `ret`; si dword +0x44 y +0x40 != 0 llama a RwMatrixDestroy 0x7F2A20 (cdecl)                (hook S)
//   4 CMatrix::ConstructCopy    0x59BCF0  thiscall (ECX = matriz, matriz*), `ret 4`; devuelve this en EAX; sin colaboradores                                   (hook S)
//   5 CMatrix::ConstructAttached 0x59C050 thiscall (ECX = matriz, RwMatrix*, dword), `ret 8`; devuelve this; llama a Attach 0x59BD10 (colaborador con hook)    (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Modo = (n / 16) % 8.
// Memoria: una ventana de 48 dwords (0xC0 bytes) con valores explicitos en `mem`; la matriz (0x48 bytes = 18 dwords) esta en `po` y la RwMatrix o la matriz origen (16 dwords) en `pa`.
// Un dword de `mem` con el valor 0xAAA00000 + desplazamiento es un puntero a ese desplazamiento de la ventana (se resuelve al ejecutar; el oraculo lo ve como etiqueta de arena).
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

namespace F97BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 6;
constexpr unsigned kWords = 48;         // ventana de memoria
constexpr u32 kPtrMarker = 0xAAA00000u; // + desplazamiento en bytes dentro de la ventana
constexpr u32 kRawPtr = 0x00ABCDE0u;    // puntero crudo (no apunta a la ventana; ninguna funcion lo desreferencia)

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = kWords * 4;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 97002u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

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

struct C {
    u32 fn{}, n{}, family{}, seed{}, mode{};
    u32 eax{}, edx{}, stack{};
    u32 po{}, pa{};                       // indices de dword: matriz (this) y RwMatrix / matriz origen
    u32 arg{};                            // angulo (SetRotateZOnly), o la bandera de propiedad (ConstructAttached), o puntero (marcador / crudo) de la RwMatrix (UpdateRwMatrix y ConstructAttached)
    u32 owns{};                           // bandera de propiedad (dword completo) de ConstructAttached
    std::array<u32, kWords> mem{};
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
    u32 W() { return Rand(n, nk()); }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
// NaN: todos con un significando distinto una vez silenciados (ver Fn97A/Fixtures.hpp)
inline constexpr std::array<u32, 6> kNanBits{0xFFC00000u, 0x7FC12345u, 0x7F800001u, 0xFF812346u, 0xFFE00001u, 0x7FF55555u};

// Ningun dword aleatorio puede parecer un marcador de puntero (0xAAAxxxxx): la sonda los resuelve a direcciones de la ventana
inline u32 Clean(u32 w) { return (w & 0xFFF00000u) == kPtrMarker ? (w ^ 0x00100000u) : w; }
inline u32 SignedZero(Gen& g) { return g.P(50) ? 0x80000000u : 0u; }
inline u32 NormalBits(Gen& g, int e0, int e1) {
    const u32 e = u32(127 + e0 + int(g.R(u32(e1 - e0 + 1))));
    return (g.W() & 0x80000000u) | (e << 23) | (g.W() & 0x7FFFFFu);
}
inline u32 SpecialNoNan(Gen& g) {
    const u32 r = g.R(20);
    if (r < 8) return kTiny[g.R(12)];
    if (r < 12) return kInf[g.R(4)];
    if (r < 15) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 Special(Gen& g) { return g.P(25) ? kNanBits[g.R(6)] : SpecialNoNan(g); }
inline u32 Coord(Gen& g, float range = 3000.0f) { return B(g.Fl(-range, range)); }

// Un float "de matriz" segun la familia (los contenidos solo se copian: lo que importa es que se conserven los bits, incluidos NaN senalizantes y desnormales)
inline u32 MatWordRaw(Gen& g, u32 f) {
    switch (f % 8) {
    case 0: return Coord(g);
    case 1: return B(g.Fl(-1.0f, 1.0f));
    case 2: return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u});
    case 3: return g.P(50) ? kTiny[g.R(12)] : NormalBits(g, -75, -10);
    case 4: return g.P(40) ? kNanBits[g.R(6)] : Coord(g);
    case 5: return g.W();
    case 6: return Special(g);
    default: return NormalBits(g, -6, 6);
    }
}
inline u32 MatWord(Gen& g, u32 f) { return Clean(MatWordRaw(g, f)); }

// Angulo para SetRotateZOnly: se evitan los modulos entre 2^19 y 2^62 (la reduccion con el pi de 66 bits del x87 no se modela); fuera de ese rango el modelo es seno/coseno exactos
// (|x| < 2^19), el operando sin tocar (|x| >= 2^63, C2 = 1), el indefinido (infinitos) o el propio NaN.
inline u32 AngleBits(Gen& g, u32 f) {
    const double pi = 3.14159265358979323846;
    switch (f) {
    case 0: return B(g.Fl(-4.0f * float(pi), 4.0f * float(pi)));
    case 1: return B(g.Fl(-float(pi), float(pi)));
    case 2: { static const double K[] = {0.0, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 6.0, 8.0, 100.0, 1000.0};
              const double k = K[g.R(11)] * (g.P(50) ? 1.0 : -1.0); return B(float(k * pi / 2.0)) + (g.P(30) ? g.R(3) : 0u); }
    case 3: return g.P(40) ? kTiny[g.R(12)] : NormalBits(g, -75, -10);
    case 4: return NormalBits(g, 4, 12);                                  // 16 .. 8192
    case 5: return NormalBits(g, 13, 18);                                 // 8192 .. 524288
    case 6: { const int k = int(g.R(2001)) - 1000; return B(float(double(k) * pi)); }
    case 7: return kNanBits[g.R(6)];
    case 8: return kInf[g.R(4)];
    case 9: return g.P(60) ? kBig[g.R(6)] : NormalBits(g, 63, 100);
    case 10: { u32 w = g.W(); const u32 e = (w >> 23) & 0xFF; if (e >= 127 + 19 && e <= 127 + 62) w = NormalBits(g, -30, 18); return w; }
    case 11: return B(g.Fl(-1.0f, 1.0f) * 0.01f);
    case 12: return NormalBits(g, 0, 3);
    case 13: return B(g.Fl(-100.0f, 100.0f));
    case 14: return g.P(50) ? SignedZero(g) : kTiny[g.R(12)];
    default: { const u32 r = g.R(6); return r == 0 ? kNanBits[g.R(6)] : r == 1 ? kInf[g.R(4)] : r == 2 ? kBig[g.R(6)] : r == 3 ? SignedZero(g) : B(g.Fl(-10.0f, 10.0f)); }
    }
}

inline void PutWords(C& c, u32 at, const u32* w, unsigned n) { for (unsigned k = 0; k < n; ++k) c.mem[at + k] = w[k]; }

// Valor de un puntero segun el tipo (0 nulo, 1 apunta a la RwMatrix (pa), 2 crudo, 3 apunta a otro sitio de la ventana, 4 crudo impar)
inline u32 PtrValue(const C& c, u32 kind) {
    switch (kind) {
    case 0: return 0u;
    case 1: return kPtrMarker + 4 * c.pa;
    case 2: return kRawPtr;
    case 3: return kPtrMarker + 4 * ((c.po + 20) % 40);
    default: return 0xFFFFFFF1u;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n; c.family = n % 16; c.mode = (n / 16) % 8;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    Gen g(n, 100 + 50 * fn);
    for (unsigned k = 0; k < kWords; ++k) c.mem[k] = Clean(g.W());
    const u32 f = c.family;
    static const u32 poS[2] = {0, 28};
    c.po = poS[g.R(2)];
    c.pa = c.po == 0 ? (g.P(50) ? 20u : 30u) : (g.P(50) ? 2u : 10u);
    // la matriz (16 dwords utiles + puntero + bandera) y la RwMatrix / matriz origen
    u32 mat[18], rw[16];
    for (unsigned k = 0; k < 16; ++k) { mat[k] = MatWord(g, f); rw[k] = MatWord(g, f + g.R(2) * 3); }
    mat[16] = Clean(g.W()); mat[17] = Clean(g.W());
    switch (fn) {
    case 0:
        c.arg = AngleBits(g, f);
        PutWords(c, c.po, mat, 18);
        break;
    case 1:
        PutWords(c, c.po, mat, 18); PutWords(c, c.pa, rw, 16);
        if (c.mode == 3) c.pa = c.po;                                     // la RwMatrix es la propia matriz: la copia no cambia nada
        break;
    case 2:
        mat[16] = PtrValue(c, c.mode % 5);                                // m_pAttachMatrix (+0x40)
        if (c.mode >= 5) mat[16] = c.mode == 5 ? PtrValue(c, 3) : (c.mode == 6 ? PtrValue(c, 2) : PtrValue(c, 0));
        PutWords(c, c.po, mat, 18); PutWords(c, c.pa, rw, 16);
        break;
    case 3: {
        static const u32 flags[8] = {0u, 1u, 2u, 0x100u, 0x01000000u, 0xFFFFFFFFu, 0x00010000u, 0x7FFFFFFFu};
        mat[16] = PtrValue(c, (c.n / 16) % 5);
        mat[17] = g.P(15) ? Clean(g.W()) : flags[(c.n / 64) % 8];
        PutWords(c, c.po, mat, 18); PutWords(c, c.pa, rw, 16);
        break;
    }
    case 4:
        PutWords(c, c.po, mat, 18); PutWords(c, c.pa, rw, 16);
        if (c.mode == 3) c.pa = c.po;                                     // origen == destino
        break;
    default: {
        static const u32 owns[6] = {0u, 1u, 2u, 0x80u, 0xFFu, 1u};
        PutWords(c, c.po, mat, 18); PutWords(c, c.pa, rw, 16);
        c.arg = c.mode == 7 ? PtrValue(c, 4) : PtrValue(c, c.mode % 5);
        c.owns = owns[(c.n / 128) % 6];
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("mode", c.mode).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("pa", c.pa).U("po", c.po).U("arg", c.arg).U("owns", c.owns);
    j.A("mem", std::vector<u32>(c.mem.begin(), c.mem.end()));
    return j.End();
}
}
