#pragma once
// Fixtures del banco QuatMultiply29: CQuaternion::Multiply(const CQuaternion& a, const CQuaternion& b) en 0x59C270 (thiscall, ECX = salida `this`, [esp+4] = &a, [esp+8] = &b, `ret 8`, sin retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco; fixture_mirror.py es el espejo en
// Python de este generador (misma aritmetica de 32 bits) y calibra la cobertura sin juego.
//
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total). Una sola arena: [guarda 0x40][ventana 0x60][guarda 0x40]. La salida (16 bytes), a (16) y b (16) viven DENTRO de la ventana en
// los desplazamientos oo/ao/bo del caso (por defecto disjuntos 0x00/0x10/0x20; las familias 9-14 los superponen: out == a, out == b, los tres iguales, a == b, parciales y arbitrarios).
// Orden de preparacion (identico en el oraculo): contenido previo de la ventana, luego a en ao y luego b en bo (si se superponen gana b).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatMultiply29Fix {
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
constexpr u32 kWinOffset = 0x40, kWinBytes = 0x60, kArena = kWinOffset + kWinBytes + 0x40;

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
inline void FillQ(u32* q, u32 c, u32 k, int lo, int hi) { for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, k + i, lo, hi); }
// Con probabilidad 1/m cada componente pasa a ser "con mezcla".
inline void Sprinkle(u32* q, u32 c, u32 k, u32 m) { for (u32 i = 0; i < 4; ++i) if (Rand(c, k + i) % m == 0) q[i] = MixComponent(c, k + 10 + i * 7); }

struct F {
    u32 family;
    u32 qa[4], qb[4];         // bits de x, y, z, w de a y de b
    u32 oo, ao, bo;           // desplazamientos (bytes) de la salida, a y b dentro de la ventana
    u32 eax, edx, stack;
    u8 win[kWinBytes];        // contenido previo de la ventana (a y b se escriben encima)
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.oo = 0x00; f.ao = 0x10; f.bo = 0x20;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.win, kWinBytes, Rand(c, 25));
    switch ((n / 16) % 4) {
    case 1: std::memset(f.win, 0x00, kWinBytes); break;
    case 2: std::memset(f.win, 0xFF, kWinBytes); break;
    case 3: for (unsigned i = 0; i < kWinBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.win + i, &w, 4); } break;
    default: break;
    }
    u32 a[4]{}, b[4]{};
    FillQ(a, c, 40, -6, 1); FillQ(b, c, 44, -6, 1);
    switch (f.family) {
    case 0: break;                                                                        // tipicos
    case 1: FillQ(a, c, 40, -4, 0); FillQ(b, c, 44, -4, 0); break;                        // casi unitarios
    case 2: FillQ(a, c, 40, -30, 30); FillQ(b, c, 44, -30, 30); break;                    // exponentes anchos
    case 3: {                                                                             // cancelacion: b1/b2 proporcionales a a1/a2 (y alguno con un ulp de desvio)
        FillQ(a, c, 40, -3, 1); FillQ(b, c, 44, -3, 1);
        const u32 sh = u32(int(Rand(c, 48) % 5) - 2) << 23;
        b[1] = a[1] + sh; b[2] = a[2] + sh;
        if (Rand(c, 49) % 3 == 0) b[2] += u32(int(Rand(c, 50) % 3) - 1);
        if (Rand(c, 51) % 3 == 0) { b[0] = a[0] + sh; b[2] = a[2] + sh; }
        if (Rand(c, 52) % 3 == 0) { b[0] = a[0] + sh; b[1] = a[1] + sh; }
        break;
    }
    case 4: {                                                                             // extremos: enormes, diminutos, subnormales
        for (u32 i = 0; i < 4; ++i) {
            a[i] = (Rand(c, 60 + i) & 1) ? RandFloat(c, 41 + i, 60, 127) : ((Rand(c, 64 + i) & 1) ? RandFloat(c, 41 + i, -126, -60) : (Rand(c, 68 + i) & 0x807FFFFFu));
            b[i] = (Rand(c, 72 + i) & 1) ? RandFloat(c, 45 + i, 60, 127) : ((Rand(c, 76 + i) & 1) ? RandFloat(c, 45 + i, -126, -60) : (Rand(c, 80 + i) & 0x807FFFFFu));
        }
        break;
    }
    case 5: for (u32 i = 0; i < 4; ++i) { a[i] = Pick(kSpecial, c, 41 + i); b[i] = Pick(kSpecial, c, 45 + i); } break;
    case 6: Sprinkle(a, c, 90, 2); Sprinkle(b, c, 100, 2); break;                         // NaN, infinitos, ceros, ... en la mitad de los componentes
    case 7: for (u32 i = 0; i < 4; ++i) { if (Rand(c, 90 + i) % 2) a[i] = Pick(kZeroOne, c, 94 + i); if (Rand(c, 98 + i) % 2) b[i] = Pick(kZeroOne, c, 102 + i); } break;   // ceros con signo y +-1
    case 8: for (u32 i = 0; i < 4; ++i) { a[i] = Quiet(Rand(c, 40 + i)); b[i] = Quiet(Rand(c, 44 + i)); } break;   // bits crudos (nunca sNaN)
    case 9: f.oo = 0x00; f.ao = 0x00; f.bo = 0x20; Sprinkle(a, c, 90, 8); Sprinkle(b, c, 100, 8); break;       // salida == a
    case 10: f.oo = 0x00; f.ao = 0x20; f.bo = 0x00; Sprinkle(a, c, 90, 8); Sprinkle(b, c, 100, 8); break;      // salida == b
    case 11: f.oo = 0x10; f.ao = 0x10; f.bo = 0x10; Sprinkle(a, c, 90, 8); Sprinkle(b, c, 100, 8); break;      // salida == a == b (b sobreescribe a)
    case 12: f.oo = 0x30; f.ao = 0x10; f.bo = 0x10; Sprinkle(a, c, 90, 8); Sprinkle(b, c, 100, 8); break;      // a == b, salida aparte
    case 13: {                                                                            // solapamientos parciales (multiplos de 4)
        f.ao = 0x10; f.oo = u32(int(f.ao) + (int(Rand(c, 80) % 7) - 3) * 4); f.bo = u32(int(f.ao) + 0x10 + (int(Rand(c, 81) % 7) - 3) * 4);
        if (Rand(c, 82) % 2) { const u32 t = f.ao; f.ao = f.bo; f.bo = t; }
        Sprinkle(a, c, 90, 8); Sprinkle(b, c, 100, 8);
        break;
    }
    case 14: f.oo = (Rand(c, 80) % 21) * 4; f.ao = (Rand(c, 81) % 21) * 4; f.bo = (Rand(c, 82) % 21) * 4; Sprinkle(a, c, 90, 8); Sprinkle(b, c, 100, 8); break;   // los tres arbitrarios
    default:                                                                              // 15: empates y cargas utiles de NaN
        for (u32 i = 0; i < 4; ++i) {
            if (Rand(c, 90 + i) % 4) a[i] = Pick(kNan, c, 94 + i);
            if (Rand(c, 98 + i) % 4) b[i] = Pick(kNan, c, 102 + i);
        }
        break;
    }
    for (u32 i = 0; i < 4; ++i) { f.qa[i] = a[i]; f.qb[i] = b[i]; }
    return f;
}
}
