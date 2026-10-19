#pragma once
// Fixtures del banco QuatGetAxis29: CQuaternion::Get(RwV3d* axis, float* angle) en 0x59C230 (thiscall, ECX = cuaternion {x,y,z,w}, 2 punteros en pila, `ret 8`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco; fixture_mirror.py es el espejo
// en Python de este generador (misma aritmetica de 32 bits) y calibra la cobertura sin juego.
//
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total). Una sola arena de ventana: [guarda 0x40][ventana 0x40][guarda 0x40]; el cuaternion (16 bytes), el eje (12 bytes) y el angulo
// (4 bytes) viven DENTRO de la ventana en los desplazamientos qoff/aoff/goff del caso (por defecto disjuntos; las familias 10-13 los superponen para comprobar el orden lectura/escritura).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatGetAxis29Fix {
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
constexpr u32 kDefQ = 0x00, kDefAxis = 0x10, kDefAngle = 0x20;

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
// w en el borde del dominio (|2w| = 1 exacto, un ulp dentro, un ulp fuera)
inline constexpr std::array<u32, 8> kNearHalf{0x3F000000u, 0xBF000000u, 0x3EFFFFFFu, 0xBEFFFFFFu, 0x3F000001u, 0xBF000001u, 0x3EFFFFFEu, 0xBEFFFFFEu};
inline constexpr std::array<u32, 7> kAliasAxis{0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C};    // eje respecto a un cuaternion en 0x10 (antes, igual y despues)
inline constexpr std::array<u32, 4> kAliasAngle{0x10, 0x14, 0x18, 0x1C};                     // angulo sobre x, y, z, w del cuaternion en 0x10
inline constexpr std::array<u32, 3> kAliasAxisAngle{0x10, 0x14, 0x18};                        // angulo sobre axis.x / y / z (eje en 0x10)

inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Componente x/y/z "con mezcla": NaN, infinito, ceros, subnormal, enorme, especial o normal.
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
    u32 q[4];                 // bits de x, y, z, w
    u32 qoff, aoff, goff;     // desplazamientos (bytes) del cuaternion, el eje y el angulo dentro de la ventana
    u32 eax, edx, stack;
    u8 win[kWinBytes];        // contenido previo de la ventana (el cuaternion se escribe encima en qoff)
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.qoff = kDefQ; f.aoff = kDefAxis; f.goff = kDefAngle;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.win, kWinBytes, Rand(c, 25));
    switch ((n / 16) % 4) {
    case 1: std::memset(f.win, 0x00, kWinBytes); break;
    case 2: std::memset(f.win, 0xFF, kWinBytes); break;
    case 3: for (unsigned i = 0; i < kWinBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.win + i, &w, 4); } break;
    default: break;
    }
    u32 q[4]{};
    for (u32 i = 0; i < 3; ++i) q[i] = RandFloat(c, 41 + i, -6, 1);
    q[3] = RandFloat(c, 40, -12, -2);                                                     // w normal: |2w| < 0.5
    switch (f.family) {
    case 0: break;                                                                        // tipicos
    case 1: q[3] = RandFloat(c, 40, -2, -2); break;                                       // |w| en [0,25, 0,5): |2w| en [0,5, 1)
    case 2: q[3] = Pick(kNearHalf, c, 40); for (u32 i = 0; i < 3; ++i) q[i] = RandFloat(c, 41 + i, -3, 1); break;
    case 3: {                                                                             // w diminuto / cero: acos = pi/2, 1/sin = 1
        switch (Rand(c, 45) % 4) {
        case 0: q[3] = Pick(kZeroOne, c, 46) & 0x80000000u; break;
        case 1: q[3] = (Rand(c, 46) & 0x80000000u) | (1 + Rand(c, 47) % 0x7FFFFEu); break;
        case 2: q[3] = RandFloat(c, 46, -126, -100); break;
        default: q[3] = (Rand(c, 46) & 0x80000000u) | 1u; break;
        }
        for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 3 == 0) q[i] = MixComponent(c, 60 + i * 7);
        break;
    }
    case 4: q[3] = RandFloat(c, 40, -1, 127); break;                                      // dominio invalido: |2w| >= 1 (NaN indefinido, errno = 33)
    case 5: q[3] = (n & 256) ? Pick(kNan, c, 40) : Pick(kInf, c, 40); for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 4 == 0) q[i] = MixComponent(c, 60 + i * 7); break;
    case 6: for (u32 i = 0; i < 3; ++i) q[i] = MixComponent(c, 60 + i * 7); break;         // w normal, xyz con mezcla
    case 7: q[3] = 0x3F000000u; for (u32 i = 0; i < 3; ++i) q[i] = MixComponent(c, 60 + i * 7); break;     // 2w = 1: acos = +0, 1/sin = +inf
    case 8: q[3] = 0xBF000000u; for (u32 i = 0; i < 3; ++i) q[i] = (Rand(c, 50 + i) % 3) ? RandFloat(c, 41 + i, -6, 1) : MixComponent(c, 60 + i * 7); break;   // 2w = -1: acos = pi de 64 bits
    case 9: {                                                                             // cerca de |2w| = 1 (|w| = 0,5 - k ulp), signo aleatorio
        const u32 k = 1 + Rand(c, 45) % 64;
        q[3] = (0x3F000000u - k) | (Rand(c, 46) & 0x80000000u);
        for (u32 i = 0; i < 3; ++i) q[i] = RandFloat(c, 41 + i, -3, 1);
        break;
    }
    case 10: f.qoff = 0x10; f.aoff = Pick(kAliasAxis, c, 80); f.goff = 0x30; break;      // el eje se superpone al cuaternion
    case 11: f.qoff = 0x10; f.goff = Pick(kAliasAngle, c, 80); f.aoff = 0x24; break;     // el angulo se superpone al cuaternion
    case 12: f.qoff = 0x00; f.aoff = 0x10; f.goff = Pick(kAliasAxisAngle, c, 80); break; // el angulo se superpone al eje
    case 13: {                                                                            // desplazamientos arbitrarios y w normal o fuera de dominio
        f.qoff = (Rand(c, 80) % 13) * 4; f.aoff = (Rand(c, 81) % 14) * 4; f.goff = (Rand(c, 82) % 16) * 4;
        if (Rand(c, 83) % 3 == 0) q[3] = RandFloat(c, 40, -1, 3);
        for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 8 == 0) q[i] = MixComponent(c, 60 + i * 7);
        break;
    }
    case 14: for (u32 i = 0; i < 4; ++i) q[i] = Quiet(Rand(c, 40 + i)); break;            // bits crudos (nunca sNaN)
    default: {                                                                            // 15: empates y cargas utiles de NaN (indefinido o NaN de w frente a NaN de x/y/z)
        q[3] = (Rand(c, 45) & 1) ? RandFloat(c, 40, 0, 5) : Pick(kNan, c, 40);
        for (u32 i = 0; i < 3; ++i) q[i] = (Rand(c, 50 + i) % 4) ? Pick(kNan, c, 60 + i) : RandFloat(c, 41 + i, -6, 1);
        break;
    }
    }
    for (u32 i = 0; i < 4; ++i) f.q[i] = q[i];
    return f;
}
}
