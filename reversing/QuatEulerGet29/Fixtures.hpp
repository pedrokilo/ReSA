#pragma once
// Fixtures del banco QuatEulerGet29: CQuaternion::Get(float* x, float* y, float* z) en 0x59C160 (thiscall, ECX = cuaternion {x,y,z,w}, 3 punteros float en pila, `ret 0Ch`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco; fixture_mirror.py es el espejo
// en Python de este generador (misma aritmetica de 32 bits) y calibra la cobertura sin juego. Sin coma flotante: todo son enteros (el FCW del juego al generar no importa).
//
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total). Una sola arena de ventana: [guarda 0x40][ventana 0x40][guarda 0x40]; el cuaternion (16 bytes) y los tres floats de salida
// x (1er puntero), y (2o) y z (3er puntero) viven DENTRO de la ventana en los desplazamientos qoff/xoff/yoff/zoff del caso (multiplos de 4; por defecto disjuntos; las familias 10-12 los
// superponen entre si y con el cuaternion para comprobar el orden de escritura z, x, y).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatEulerGet29Fix {
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

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kWinOffset = 0x40, kWinBytes = 0x40, kArena = kWinOffset + kWinBytes + 0x40;
constexpr u32 kDefQ = 0x00, kDefX = 0x10, kDefY = 0x14, kDefZ = 0x18;

// ------------------------------------------------------------------------------------------------ valores notables
// Nunca sNaN: el x87 los silencia al cargar con reglas distintas segun el operando que el oraculo no modela; ver README.md.
inline u32 Quiet(u32 b) {
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F3504F3u, 0xBF3504F3u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x3FC00000u, 0xC0400000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00002u, 0x7FE00000u, 0xFFE00000u, 0x7FC7FFFEu};
inline constexpr std::array<u32, 6> kZeroOne{0x00000000u, 0x80000000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u};
// Componentes "exactas": ceros con signo, +-1, +-0,5 y +-0,70710677 (rotaciones de 45/90/180 grados con matriz casi exacta)
inline constexpr std::array<u32, 12> kExactComp{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F3504F3u, 0xBF3504F3u, 0x00000000u, 0x80000000u, 0x3F3504F3u, 0xBF3504F3u};
inline constexpr std::array<u32, 8> kAliasOut{0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20};   // salidas respecto a un cuaternion en 0x10 (antes, dentro, despues)

inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Componente "con mezcla": NaN, infinito, ceros, subnormal, enorme, especial o normal.
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 7) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Pick(kZeroOne, c, k + 100) & 0x80000000u;
    case 3: return (Rand(c, k + 100) & 0x807FFFFFu);
    case 4: return RandFloat(c, k + 100, 100, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    default: return RandFloat(c, k + 100, -6, 1);
    }
}

struct F {
    u32 family;
    u32 q[4];                     // bits de x, y, z, w
    u32 qoff, xoff, yoff, zoff;   // desplazamientos (bytes) del cuaternion y de los tres floats de salida (1er, 2o y 3er puntero) dentro de la ventana
    u32 eax, edx, stack;
    u8 win[kWinBytes];            // contenido previo de la ventana (el cuaternion se escribe encima en qoff)
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.qoff = kDefQ; f.xoff = kDefX; f.yoff = kDefY; f.zoff = kDefZ;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.win, kWinBytes, Rand(c, 25));
    switch ((n / 16) % 4) {
    case 1: std::memset(f.win, 0x00, kWinBytes); break;
    case 2: std::memset(f.win, 0xFF, kWinBytes); break;
    case 3: for (unsigned i = 0; i < kWinBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.win + i, &w, 4); } break;
    default: break;
    }
    u32 q[4]{};
    for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 41 + i, -3, 0);                      // tipicos: componentes de modulo en [1/8, 1)
    switch (f.family) {
    case 0: break;
    case 1: for (u32 i = 0; i < 4; ++i) q[i] = Pick(kExactComp, c, 41 + i); break;       // componentes exactas (ceros con signo, +-1, +-0,5, +-0,7071)
    case 2: {                                                                             // cerca del bloqueo de cardan: x = +-0,7071, w = +-0,7071, y y z diminutos o cero
        q[0] = (Rand(c, 45) & 1) ? 0xBF3504F3u : 0x3F3504F3u;
        q[3] = (Rand(c, 46) & 1) ? 0x3F3504F3u : 0xBF3504F3u;
        if (Rand(c, 47) % 4 == 0) { q[1] = Pick(kZeroOne, c, 48) & 0x80000000u; q[2] = Pick(kZeroOne, c, 49) & 0x80000000u; }
        else { q[1] = RandFloat(c, 48, -24, -4); q[2] = RandFloat(c, 49, -24, -4); }
        break;
    }
    case 3: for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 3 == 0) ? (Rand(c, 60 + i) & 0x807FFFFFu) : RandFloat(c, 41 + i, -126, -40); break;   // diminutos y subnormales
    case 4: for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 41 + i, 20, 127); break;     // enormes: los productos desbordan a inf / NaN
    case 5: {                                                                             // NaN (al menos uno)
        for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 3 == 0) ? Pick(kNan, c, 60 + i) : RandFloat(c, 41 + i, -3, 0);
        q[Rand(c, 45) % 4] = Pick(kNan, c, 46);
        break;
    }
    case 6: {                                                                             // infinitos (al menos uno)
        for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 3 == 0) ? Pick(kInf, c, 60 + i) : RandFloat(c, 41 + i, -3, 0);
        q[Rand(c, 45) % 4] = Pick(kInf, c, 46);
        break;
    }
    case 7: for (u32 i = 0; i < 4; ++i) q[i] = MixComponent(c, 60 + i * 7); break;        // mezcla en las cuatro componentes
    case 8: for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 3 == 0) ? RandFloat(c, 70 + i, -30, -20) : (Pick(kZeroOne, c, 41 + i) & 0x80000000u); break;   // ceros con signo y diminutos
    case 9: {                                                                             // rotaciones pequenas: angulos diminutos de ambos signos (ramas < 0 del +2*pi)
        for (u32 i = 0; i < 3; ++i) q[i] = RandFloat(c, 41 + i, -30, -4);
        q[3] = (Rand(c, 45) & 1) ? (0x3F800000u | ((Rand(c, 46) & 1u) << 31)) : RandFloat(c, 40, -1, 0);
        break;
    }
    case 10: f.qoff = 0x10; f.xoff = Pick(kAliasOut, c, 80); f.yoff = Pick(kAliasOut, c, 81); f.zoff = Pick(kAliasOut, c, 82); break;   // salidas sobre el cuaternion y entre si
    case 11: {                                                                            // salidas superpuestas entre si (cuaternion aparte)
        f.qoff = 0x00;
        f.xoff = 0x20 + 4 * (Rand(c, 80) % 4);
        f.yoff = (Rand(c, 81) & 1) ? f.xoff : 0x20 + 4 * (Rand(c, 82) % 4);
        f.zoff = (Rand(c, 83) % 3 == 0) ? f.xoff : ((Rand(c, 84) % 3 == 0) ? f.yoff : 0x20 + 4 * (Rand(c, 85) % 4));
        break;
    }
    case 12: {                                                                            // desplazamientos arbitrarios (multiplos de 4)
        f.qoff = (Rand(c, 80) % 13) * 4; f.xoff = (Rand(c, 81) % 16) * 4; f.yoff = (Rand(c, 82) % 16) * 4; f.zoff = (Rand(c, 83) % 16) * 4;
        for (u32 i = 0; i < 4; ++i) if (Rand(c, 50 + i) % 8 == 0) q[i] = MixComponent(c, 60 + i * 7);
        break;
    }
    case 13: for (u32 i = 0; i < 4; ++i) q[i] = Quiet(Rand(c, 40 + i)); break;            // bits crudos (nunca sNaN)
    case 14: for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 4) ? Pick(kNan, c, 60 + i) : RandFloat(c, 41 + i, -3, 0); break;   // empates y cargas utiles de NaN
    default: {                                                                            // 15: cuaterniones unitarios exactos (1 componente = +-1; 2 = +-0,7071; 4 = +-0,5)
        const u32 mode = Rand(c, 45) % 3;
        const u32 i = Rand(c, 46) % 4, j = (i + 1 + Rand(c, 47) % 3) % 4;
        for (u32 k = 0; k < 4; ++k) q[k] = Rand(c, 50 + k) & 0x80000000u;
        if (mode == 0) q[i] = 0x3F800000u | (Rand(c, 54) & 0x80000000u);
        else if (mode == 1) { q[i] = 0x3F3504F3u | (Rand(c, 55) & 0x80000000u); q[j] = 0x3F3504F3u | (Rand(c, 56) & 0x80000000u); }
        else for (u32 k = 0; k < 4; ++k) q[k] = 0x3F000000u | (Rand(c, 57 + k) & 0x80000000u);
        break;
    }
    }
    for (u32 i = 0; i < 4; ++i) f.q[i] = q[i];
    return f;
}
}
