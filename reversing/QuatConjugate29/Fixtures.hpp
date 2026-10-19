#pragma once
// Fixtures del banco QuatConjugate29: CQuaternion::Conjugate() en 0x4D37D0 (thiscall, ECX = cuaternion {x,y,z,w}, sin argumentos en pila, `ret`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco; fixture_mirror.py es el espejo
// en Python de este generador (misma aritmetica de 32 bits) y calibra la cobertura sin juego.
//
// Reparto: familia = n % 8 (512 casos por familia, 4096 en total). Una sola arena: [guarda 0x40][ventana 0x40][guarda 0x40]; el cuaternion (16 bytes) vive DENTRO de la ventana en el
// desplazamiento qoff del caso (multiplo de 4 salvo la familia 6, que lo desalinea). A diferencia de QuatGetAxis29, aqui SI se generan sNaN: el EXE niega con fld/fchs/fstp y el fld
// silencia el sNaN (bit 22 a 1), cosa que el oraculo modela.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatConjugate29Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 8;

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

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 24> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u,
    0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80800000u, 0x7F800000u, 0xFF800000u, 0x4B800000u, 0x34000000u, 0x7F000000u, 0x3F7FFFFFu, 0x3F800001u, 0xBF800001u, 0x5F800000u};
inline constexpr std::array<u32, 12> kQNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00002u, 0x7FE00000u, 0xFFE00000u, 0x7FC7FFFEu};
// sNaN: exponente todo unos, bit 22 a 0, mantisa distinta de cero.
inline constexpr std::array<u32, 12> kSNan{0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FBFFFFFu, 0xFFBFFFFFu, 0x7F812345u, 0xFFA54321u, 0x7F800002u, 0xFF800400u, 0x7FB00000u, 0xFF9FFFFEu};

inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Valor "con mezcla": normal, especial, qNaN o sNaN, subnormal, cero con signo, infinito.
inline u32 MixValue(u32 c, u32 k) {
    switch (Rand(c, k) % 8) {
    case 0: return Pick(kQNan, c, k + 100);
    case 1: return Pick(kSNan, c, k + 100);
    case 2: return Pick(kSpecial, c, k + 100);
    case 3: return Rand(c, k + 100) & 0x807FFFFFu;                       // cero o subnormal con signo
    case 4: return ((Rand(c, k + 100) & 0x80000000u) | 0x7F800000u);       // infinito con signo
    case 5: return RandFloat(c, k + 100, 100, 127);
    default: return RandFloat(c, k + 100, -126, 100);
    }
}

struct F {
    u32 family;
    u32 q[4];                 // bits de x, y, z, w
    u32 qoff;                 // desplazamiento (bytes) del cuaternion dentro de la ventana
    u32 eax, edx, stack;
    u8 win[kWinBytes];        // contenido previo de la ventana (el cuaternion se escribe encima en qoff)
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.qoff = (Rand(c, 7) % 13) * 4;                                                       // 0..0x30, alineado a 4 (hasta 0x30 + 16 = 0x40)
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.win, kWinBytes, Rand(c, 25));
    switch ((n / 8) % 4) {
    case 1: std::memset(f.win, 0x00, kWinBytes); break;
    case 2: std::memset(f.win, 0xFF, kWinBytes); break;
    case 3: for (unsigned i = 0; i < kWinBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.win + i, &w, 4); } break;
    default: break;
    }
    u32 q[4]{};
    for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 41 + i, -20, 20);
    switch (f.family) {
    case 0: break;                                                                        // normales
    case 1: for (u32 i = 0; i < 4; ++i) q[i] = Pick(kSpecial, c, 41 + i); break;          // especiales: ceros, subnormales, maximos, infinitos, +-1
    case 2: for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) & 0x80000000u) | (Rand(c, 54 + i) % 3 == 0 ? 0x7F800000u : Rand(c, 58 + i) & 0x7FFFFFu); break;   // ceros/subnormales/inf con signo
    case 3: for (u32 i = 0; i < 4; ++i) q[i] = Pick(kQNan, c, 41 + i); break;             // qNaN con cargas utiles
    case 4: for (u32 i = 0; i < 4; ++i) q[i] = Pick(kSNan, c, 41 + i); break;             // sNaN en las cuatro (w no se toca)
    case 5: for (u32 i = 0; i < 4; ++i) q[i] = MixValue(c, 60 + i * 7); break;            // mezcla
    case 6: for (u32 i = 0; i < 4; ++i) q[i] = Rand(c, 70 + i);                           // bits crudos (incluye sNaN) y cuaternion desalineado
            f.qoff = Rand(c, 7) % 49; break;
    default: for (u32 i = 0; i < 3; ++i) q[i] = (Rand(c, 50 + i) % 2) ? Pick(kSNan, c, 41 + i) : MixValue(c, 60 + i * 7);   // 7: xyz con sNaN frecuentes, w con mezcla
             q[3] = MixValue(c, 90); break;
    }
    for (u32 i = 0; i < 4; ++i) f.q[i] = q[i];
    return f;
}
}
