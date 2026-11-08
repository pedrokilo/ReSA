#pragma once
// Fixtures del banco Fn118A (lote 118A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CMemoryMgr::Malloc(size, hint)                 0x72F420  cdecl; reenvia el tamano a malloc del CRT del EXE (0x824257)                                    (hook S)
//   1 CMemoryMgr::Realloc(ptr, size, hint)           0x72F440  cdecl; reenvia (ptr, tamano) a realloc del CRT del EXE (0x824269)                               (hook S)
//   2 CMemoryMgr::Calloc(num, size, hint)            0x72F460  cdecl; reenvia (num, size) a calloc del CRT del EXE (0x824416)                                  (hook S)
//   3 CMemoryMgr::MallocAlign(size, align, hint)     0x72F4C0  cdecl; malloc(size + align); r = (base + align) & ~(align - 1); [r - 4] = base               (hook S)
//   4 CMemoryMgr::FreeAlign(ptr)                     0x72F4F0  cdecl; free([ptr - 4]) (tail call a free del CRT, 0x82413F)                                    (hook S)
//   5 CDecision::CDecision()                         0x6040C0  thiscall; llama a CDecision::SetDefault (0x600530) y devuelve this                              (hook S)
//   6 CTaskComplexLeaveCar::ComputeTargetDoor(ped)   0x63BAB0  thiscall, `ret 4`; si +0x10 == 0 -> +0x10 = CCarEnterExit::ComputeTargetDoorToExit(+0xC, ped) (0x64F110)  (hook S)
//   7 CWaterLevel::CalculateWavesOnlyForCoordinate2  0x6E8550  cdecl (x, y, altura*, grandes, pequenas); llama a ...2Native (0x6E7210) con (x, y, grandes, pequenas, altura)  (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
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

namespace F118AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 8;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x800, kAWin = 0x40, kBWin = 0x40;   // objeto / bloque principal, arena secundaria (ped, vehiculo, altura) y arena de retornos de los falsos
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

// ------------------------------------------------------------------------------------------------ el caso
// Cada funcion interpreta `a` como se documenta en su generador; todos los valores viajan como u32 (los floats, como su patron de bits).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 48> a{};
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
// NaN con cargas distintas (silenciosos y senalizantes)
inline constexpr std::array<u32, 10> kNans{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FA00000u, 0x7FC12345u, 0xFFA5A5A5u, 0x7F812345u};
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
// Un float de magnitud aleatoria (de 1e-30 a 1e30) con signo
inline u32 Wide(Gen& g) {
    const double e = double(int(g.R(61)) - 30);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, e)));
}
// Una coordenada de juego en metros, con signo
inline u32 Coord(Gen& g, float range = 3000.0f) { return B(g.Fl(-range, range)); }

// ------------------------------------------------------------------------------------------------ utilidades de los generadores
inline constexpr std::array<u32, 10> kRawPtrs{1u, 0xFFFFFFFFu, 0x80000000u, 0xCDCDCDCDu, 0x00C0FFEEu, 0x7FFFFFFFu, 0x00010000u, 0xFDFDFDFDu, 0x00000004u, 0x0BADF00Du};
inline u32 RawPtr(Gen& g) { return g.P(60) ? kRawPtrs[g.R(10)] : g.W() | 1u; }
inline u32 NanBits(Gen& g) { return g.P(50) ? kNans[g.R(10)] : (0x7F800000u | (g.W() & 0x807FFFFFu) | 1u); }


// ------------------------------------------------------------------------------------------------ utilidades de los generadores del lote 118A
// Clase de puntero: 0 = arena, 1 = nulo, 2 = crudo.
inline u32 PtrClass(Gen& g, u32 f, unsigned mod = 3) { return f % mod == 0 ? 0u : (f % mod == 1 ? g.Pk({0u, 1u}) : g.R(3)); }
// Tamano (de malloc / calloc / realloc): categorias por familia
inline u32 SizeFor(Gen& g, u32 f) {
    switch (f) {
    case 0: return 0;
    case 1: return 1 + g.R(16);
    case 2: return 1u << g.R(32);
    case 3: { const u32 p = 1u << g.R(32); return g.P(50) ? p - 1 : p + 1; }
    case 4: return g.Pk({0x1000u, 0xFFFu, 0x1001u, 0x10000u, 0xFFFFu, 0x10001u, 0x100000u});
    case 5: return g.Pk({0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0x7FFFFFFEu});
    case 6: return g.Pk({0xFFFFFFFFu, 0xFFFFFFE0u, 0xFFFFFFFEu, 0xFFFFFFF0u, 0xFFFFFFE1u});
    case 7: return g.R(0x10000);
    case 8: return g.W();
    case 9: return g.Pk({0x44u, 0x19Cu, 0xA18u, 0x14u, 0x3Cu, 0x98u, 0x24u, 0x800u});
    default: return g.P(50) ? g.R(0x1000) : g.W();
    }
}
inline u32 HintFor(Gen& g) { return g.P(40) ? g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u}) : g.W(); }

// ------------------------------------------------------------------------------------------------ funcion 0: CMemoryMgr::Malloc (0x72F420)
//   a[0] = tamano, a[1] = hint (ignorado), a[2] = clase del valor que devuelve el falso (0 arena B, 1 nulo, 2 crudo), a[3] = valor crudo, a[4] = desplazamiento en la arena B (0..0x20)
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100);
    c.a[0] = SizeFor(g, f);
    c.a[1] = HintFor(g);
    c.a[2] = PtrClass(g, f, 4);
    c.a[3] = RawPtr(g);
    c.a[4] = g.R(0x21);
}

// ------------------------------------------------------------------------------------------------ funcion 1: CMemoryMgr::Realloc (0x72F440)
//   a[0] = clase del puntero (0 arena A, 1 nulo, 2 crudo), a[1] = crudo, a[2] = desplazamiento (0..0x20), a[3] = tamano, a[4] = hint,
//   a[5] = clase del valor devuelto por el falso (0 arena B, 1 nulo, 2 crudo), a[6] = crudo, a[7] = desplazamiento
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 200);
    c.a[0] = PtrClass(g, f / 2, 3);
    c.a[1] = RawPtr(g);
    c.a[2] = g.R(0x21);
    c.a[3] = SizeFor(g, (f * 7 + n / 16) % 16);
    c.a[4] = HintFor(g);
    c.a[5] = PtrClass(g, f, 4);
    c.a[6] = RawPtr(g);
    c.a[7] = g.R(0x21);
}

// ------------------------------------------------------------------------------------------------ funcion 2: CMemoryMgr::Calloc (0x72F460)
//   a[0] = num, a[1] = size, a[2] = hint, a[3] = clase del valor devuelto (0 arena B, 1 nulo, 2 crudo), a[4] = crudo, a[5] = desplazamiento
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 300);
    c.a[0] = SizeFor(g, f);
    c.a[1] = SizeFor(g, (f * 5 + n / 16) % 16);
    c.a[2] = HintFor(g);
    c.a[3] = PtrClass(g, f, 4);
    c.a[4] = RawPtr(g);
    c.a[5] = g.R(0x21);
}

// ------------------------------------------------------------------------------------------------ funcion 3: CMemoryMgr::MallocAlign (0x72F4C0)
//   a[0] = tamano, a[1] = alineamiento (1..0x400, potencia de dos o no), a[2] = hint, a[3] = desplazamiento de la base que devuelve el falso (0..0x100) respecto al objeto de la arena principal
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 400);
    static constexpr std::array<u32, 11> pow2{1u, 2u, 4u, 8u, 0x10u, 0x20u, 0x40u, 0x80u, 0x100u, 0x200u, 0x400u};
    static constexpr std::array<u32, 14> odd{3u, 5u, 6u, 7u, 9u, 12u, 24u, 0x30u, 0x60u, 0xC0u, 0x110u, 0x3FFu, 0x1FFu, 0x18u};
    u32 align;
    switch (f % 4) {
    case 0: align = pow2[g.R(11)]; break;
    case 1: align = odd[g.R(14)]; break;
    case 2: align = g.Pk({4u, 8u, 0x10u, 0x20u, 0x40u}); break;      // los alineamientos habituales
    default: align = 1 + g.R(0x400); break;
    }
    c.a[0] = SizeFor(g, (f + n / 16) % 16);
    c.a[1] = align;
    c.a[2] = HintFor(g);
    c.a[3] = f < 8 ? g.R(0x101) : (g.P(50) ? 0u : g.R(0x101));
}

// ------------------------------------------------------------------------------------------------ funcion 4: CMemoryMgr::FreeAlign (0x72F4F0)
//   a[0] = desplazamiento del puntero (4..0x100) respecto al objeto de la arena principal, a[1] = clase del valor guardado en [ptr - 4] (0 arena A, 1 nulo, 2 crudo), a[2] = crudo, a[3] = desplazamiento (0..0x20)
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 500);
    c.a[0] = 4 + (f < 8 ? g.R(0xFD) : (g.P(50) ? 0u : g.R(0xFD)));
    c.a[1] = PtrClass(g, f, 4);
    c.a[2] = RawPtr(g);
    c.a[3] = g.R(0x21);
}

// ------------------------------------------------------------------------------------------------ funcion 5: CDecision::CDecision (0x6040C0)
//   a[0] = desplazamiento del objeto (0..0x23, cualquier alineamiento), a[1] = relleno de la arena (0 ruido, 1 ceros, 2 0xFF, 3 0xCD), a[2..16] = los 15 dwords iniciales del objeto (0x3C bytes)
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 600);
    c.a[0] = f < 8 ? 4 * g.R(9) : g.R(0x24);
    c.a[1] = g.R(4);
    for (u32 i = 0; i < 15; ++i) {
        u32 v;
        switch (f % 4) {
        case 0: v = g.W(); break;
        case 1: v = g.Pk({0u, 0xFFFFFFFFu}); break;                          // ya en su valor final o en cero
        case 2: v = g.P(30) ? (i < 6 ? 0xFFFFFFFFu : 0u) : g.W(); break;    // parcialmente en su valor final
        default: v = g.Pk({0u, 0xFFFFFFFFu, 0xCDCDCDCDu, 0x01010101u, 0x80000000u, 0xFFFFFF00u, 0x000000FFu}); break;
        }
        c.a[2 + i] = v;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 6: CTaskComplexLeaveCar::ComputeTargetDoor (0x63BAB0)
//   a[0] = desplazamiento de this (0..0x1C), a[1] = clase del vehiculo (+0xC; 0 arena A, 1 nulo, 2 crudo), a[2] = crudo, a[3] = clase del ped (argumento; 0 arena B, 1 nulo, 2 crudo), a[4] = crudo,
//   a[5] = puerta inicial (+0x10), a[6] = valor que devuelve el falso de ComputeTargetDoorToExit (0x64F110)
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 700);
    c.a[0] = 4 * g.R(8);
    c.a[1] = PtrClass(g, f, 3);
    c.a[2] = RawPtr(g);
    c.a[3] = PtrClass(g, f / 3 + f, 3);
    c.a[4] = RawPtr(g);
    u32 door;
    switch (f % 4) {
    case 0: door = 0; break;
    case 1: door = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u}); break;
    case 2: door = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0x10000u, 0xFFFF0000u, 0xFFu, 0x80u}); break;
    default: door = g.P(50) ? 0u : g.W(); break;
    }
    c.a[5] = door;
    u32 r;
    switch ((f / 2) % 4) {
    case 0: r = g.R(9); break;
    case 1: r = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0x10000u, 0xFFFF0000u}); break;
    case 2: r = g.P(30) ? 0u : g.W(); break;
    default: r = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 0xFFFFFFFFu}); break;
    }
    c.a[6] = r;
}

// ------------------------------------------------------------------------------------------------ funcion 7: CWaterLevel::CalculateWavesOnlyForCoordinate2 (0x6E8550)
//   a[0] = x, a[1] = y (int32), a[2] = clase del puntero de altura (0 arena A, 1 nulo, 2 crudo), a[3] = crudo, a[4] = desplazamiento (0..0x20), a[5] = grandes (float), a[6] = pequenas (float)
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 800);
    auto coord = [&]() -> u32 {
        switch (g.R(8)) {
        case 0: return g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 2u, 0xFFFFFFFEu});
        case 1: return g.R(100);
        case 2: return u32(-int(g.R(100)));
        case 3: return g.R(6000) - 3000u;
        case 4: return g.R(0x20000) - 0x10000u;
        default: return g.W();
        }
    };
    c.a[0] = coord();
    c.a[1] = coord();
    c.a[2] = PtrClass(g, f, 3);
    c.a[3] = RawPtr(g);
    c.a[4] = g.R(0x21);
    switch (f % 4) {
    case 0: c.a[5] = B(g.Fl(0.0f, 2.0f)); c.a[6] = B(g.Fl(0.0f, 2.0f)); break;
    case 1: c.a[5] = Special(g); c.a[6] = Special(g); break;
    case 2: c.a[5] = Wide(g); c.a[6] = Wide(g); break;
    default: c.a[5] = NanBits(g); c.a[6] = g.P(50) ? NanBits(g) : g.W(); break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    switch (fn) {
    case 0: Make0(c, n); break;
    case 1: Make1(c, n); break;
    case 2: Make2(c, n); break;
    case 3: Make3(c, n); break;
    case 4: Make4(c, n); break;
    case 5: Make5(c, n); break;
    case 6: Make6(c, n); break;
    default: Make7(c, n); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}
