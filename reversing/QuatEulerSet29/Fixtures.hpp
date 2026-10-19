#pragma once
// Fixtures del banco QuatEulerSet29: CQuaternion::Set(float x, float y, float z) en 0x59C530 (thiscall, ECX = cuaternion {x,y,z,w}, 3 floats por valor en pila, `ret 0Ch`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco; fixture_mirror.py es el espejo
// en Python de este generador (misma aritmetica de 32 bits) y calibra la cobertura sin juego.
//
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total). Una sola arena: [guarda 0x40][ventana 0x40][guarda 0x40]; el cuaternion (16 bytes) vive en la ventana en qoff
// (multiplo de 4 o desalineado segun la familia).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatEulerSet29Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 16;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

constexpr u32 kWinOffset = 0x40, kWinBytes = 0x40, kArena = kWinOffset + kWinBytes + 0x40;

// Nunca sNaN: el x87 los silencia al cargar con reglas distintas segun el operando que el oraculo no modela; ver README.md.
inline u32 Quiet(u32 b) {
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
// Angulos notables (los 24 primeros son los de la familia 3): 0, +-1, +-pi/4, +-pi/2, +-pi, +-3pi/4(aprox), +-2pi, +-4pi, 2, maximo float, subnormales, 2^-127, ...; y, mas alla de 24,
// la frontera de fsin/fcos (|medio angulo| = 2^63 <=> |angulo| = 2^64 = 0x5F800000) y valores grandes.
inline constexpr std::array<u32, 36> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F490FDBu, 0xBF490FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x40490FDBu, 0xC0490FDBu, 0x4096CBE4u, 0xC096CBE4u,
    0x40C90FDBu, 0xC0C90FDBu, 0x41490FDBu, 0xC1490FDBu, 0x40000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x33800000u, 0x4B800000u, 0x5F7FFFFFu, 0x5F800000u, 0x5F800001u, 0xDF800000u, 0x4C000000u, 0x4CFFFFFFu, 0x3A83126Fu, 0x42B40000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00002u, 0x7FE00000u, 0xFFE00000u, 0x7FC7FFFEu};
inline constexpr std::array<u32, 6> kZeroOne{0x00000000u, 0x80000000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u};
inline constexpr std::array<u32, 8> kBoundary{0x5F7FFFFFu, 0x5F800000u, 0x5F800001u, 0xDF7FFFFFu, 0xDF800000u, 0x5F000000u, 0x4C000000u, 0x4BFFFFFFu};

inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Angulo "con mezcla": NaN, infinito, ceros, subnormal, enorme, especial o normal.
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 7) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Pick(kZeroOne, c, k + 100) & 0x80000000u;
    case 3: return (Rand(c, k + 100) & 0x807FFFFFu);
    case 4: return RandFloat(c, k + 100, 20, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    default: return RandFloat(c, k + 100, -6, 2);
    }
}

struct F {
    u32 family;
    u32 a[3];                 // bits de los angulos x, y, z
    u32 qoff;                 // desplazamiento (bytes) del cuaternion dentro de la ventana (0..0x30)
    u32 eax, edx, stack;
    u8 win[kWinBytes];        // contenido previo de la ventana
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.qoff = 0x10;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.win, kWinBytes, Rand(c, 25));
    switch ((n / 16) % 4) {
    case 1: std::memset(f.win, 0x00, kWinBytes); break;
    case 2: std::memset(f.win, 0xFF, kWinBytes); break;
    case 3: for (unsigned i = 0; i < kWinBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.win + i, &w, 4); } break;
    default: break;
    }
    u32 a[3]{};
    for (u32 i = 0; i < 3; ++i) a[i] = RandFloat(c, 41 + i, -6, 2);                       // tipicos: |angulo| hasta ~8
    switch (f.family) {
    case 0: break;                                                                        // tipicos
    case 1: for (u32 i = 0; i < 3; ++i) a[i] = RandFloat(c, 41 + i, -40, -6); break;      // pequenos
    case 2: for (u32 i = 0; i < 3; ++i) a[i] = (Rand(c, 50 + i) % 2) ? ((Rand(c, 60 + i) & 0x80000000u) | (Rand(c, 70 + i) & 0x7FFFFFu)) : (Pick(kZeroOne, c, 80 + i) & 0x80000000u); break;   // subnormales / +-0
    case 3: for (u32 i = 0; i < 3; ++i) a[i] = kSpecial[Rand(c, 41 + i) % 24]; break;     // multiplos de pi/4 y valores notables
    case 4: for (u32 i = 0; i < 3; ++i) a[i] = RandFloat(c, 41 + i, 3, 24); break;        // grandes pero con reduccion fiable
    case 5: for (u32 i = 0; i < 3; ++i) a[i] = RandFloat(c, 41 + i, 25, 127); break;      // enormes (hasta 3,4e38): |medio angulo| >= 2^63 deja fsin/fcos el operando intacto
    case 6: for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 2) a[i] = Pick(kNan, c, 60 + i); break;   // NaN en algunos angulos
    case 7: for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 2) a[i] = Pick(kInf, c, 60 + i); break;   // infinitos en algunos angulos
    case 8: for (u32 i = 0; i < 3; ++i) a[i] = MixComponent(c, 60 + i * 7); break;                      // mezcla
    case 9: for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 3) a[i] = Pick(kZeroOne, c, 60 + i) & 0x80000000u; break;   // casi todos los angulos nulos
    case 10: for (u32 i = 0; i < 3; ++i) a[i] = Quiet(Rand(c, 40 + i)); break;            // bits crudos (nunca sNaN)
    case 11: for (u32 i = 0; i < 3; ++i) a[i] = (Rand(c, 50 + i) % 4) ? Pick(kNan, c, 60 + i) : RandFloat(c, 41 + i, -6, 2); break;   // empates y cargas utiles de NaN
    case 12: f.qoff = Rand(c, 80) % 49; break;                                             // cuaternion desalineado (0..0x30), angulos tipicos
    case 13: f.qoff = (Rand(c, 80) % 13) * 4; for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 4 == 0) a[i] = MixComponent(c, 60 + i * 7); break;
    case 14: for (u32 i = 0; i < 3; ++i) a[i] = Pick(kBoundary, c, 60 + i); break;       // frontera de fsin/fcos (2^64) y de la reduccion fiable (2^26)
    default: for (u32 i = 0; i < 3; ++i) a[i] = RandFloat(c, 41 + i, -10, 30); break;     // 15: exponentes -10..30
    }
    for (u32 i = 0; i < 3; ++i) f.a[i] = a[i];
    return f;
}
}
