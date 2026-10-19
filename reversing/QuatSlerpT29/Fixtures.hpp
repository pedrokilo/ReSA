#pragma once
// Fixtures del banco QuatSlerpT29: CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float t) en 0x59C630 (thiscall, ECX = destino `this`, tres argumentos de pila:
// from, to y t; `ret 0Ch`, sin valor de retorno). Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que
// registra el banco; fixture_mirror.py es el espejo en Python de este generador (misma aritmetica de 32 bits) y calibra la cobertura sin juego.
//
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total). Una sola arena de ventana: [guarda 0x40][ventana 0x60][guarda 0x40]; from (16 bytes), to (16) y el destino (16) viven DENTRO
// de la ventana en los desplazamientos fo/to/oo del caso (por defecto disjuntos; las familias 10-13 los superponen para comprobar el orden lectura/escritura). Primero se copia `a` en fo y luego
// `b` en to (si se solapan gana `b`); el oraculo lee siempre la memoria resultante.
// Los cuaterniones se construyen sin aritmetica de coma flotante (solo bits): tabla de cuaterniones unitarios exactos, perturbaciones de +-k ulp y componentes aleatorias.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatSlerpT29Fix {
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
constexpr u32 kDefFrom = 0x00, kDefTo = 0x10, kDefOut = 0x20;

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
// valores de t: 0, -0, 1, 0.5, -1, 2, 1-ulp, 1+ulp, 0.8, subnormal, 1e-30, 8, -8, 100, NaN, NaN negativo con carga, 1.19e-7, 1/3, -0.25, 1e5
inline constexpr std::array<u32, 20> kT{0x00000000u, 0x80000000u, 0x3F800000u, 0x3F000000u, 0xBF800000u, 0x40000000u, 0x3F7FFFFFu, 0x3F800001u, 0x3F4CCCCDu, 0x00000001u,
    0x0DA24260u, 0x41000000u, 0xC1000000u, 0x42C80000u, 0x7FC00000u, 0xFFC00012u, 0x34000000u, 0x3EAAAAABu, 0xBE800000u, 0x47C35000u};
// cuaterniones unitarios exactos (x, y, z, w en bits)
inline constexpr std::array<std::array<u32, 4>, 10> kUnit{{
    {0x00000000u, 0x00000000u, 0x00000000u, 0x3F800000u}, {0x3F800000u, 0x00000000u, 0x00000000u, 0x00000000u}, {0x00000000u, 0x3F800000u, 0x00000000u, 0x00000000u},
    {0x00000000u, 0x00000000u, 0x3F800000u, 0x00000000u}, {0x3F000000u, 0x3F000000u, 0x3F000000u, 0x3F000000u}, {0x3F000000u, 0x3F000000u, 0x3F000000u, 0xBF000000u},
    {0x3F000000u, 0xBF000000u, 0x3F000000u, 0xBF000000u}, {0x3F19999Au, 0x3F4CCCCDu, 0x00000000u, 0x00000000u}, {0x3F3504F3u, 0x00000000u, 0x00000000u, 0x3F3504F3u},
    {0x00000000u, 0x3F3504F3u, 0x3F3504F3u, 0x00000000u}}};
// pares ortogonales de kUnit
inline constexpr std::array<std::array<u32, 2>, 6> kOrth{{{0, 1}, {0, 2}, {1, 2}, {1, 3}, {2, 3}, {4, 6}}};
inline constexpr std::array<u32, 11> kZ{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x20000000u, 0x5F800000u, 0x3F000000u};

inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Componente con mezcla: NaN, infinito, ceros, subnormal, enorme, especial o normal.
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
// +-d ulp sobre la magnitud (los ceros no cambian; la magnitud se limita a [1, 0x7F7FFFFF])
inline u32 Perturb(u32 bits, int d) {
    if ((bits & 0x7FFFFFFFu) == 0) return bits;
    int mag = int(bits & 0x7FFFFFFFu) + d;
    if (mag < 1) mag = 1;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return (bits & 0x80000000u) | u32(mag);
}
inline int Delta(u32 c, u32 k, int m) { return int(Rand(c, k) % u32(2 * m + 1)) - m; }

struct F {
    u32 family;
    u32 a[4], b[4];           // bits de x, y, z, w de from y to
    u32 t;                    // bits de t
    u32 fo, to, oo;           // desplazamientos (bytes) de from, to y destino dentro de la ventana
    u32 eax, edx, stack;
    u8 win[kWinBytes];        // contenido previo de la ventana (from y to se escriben encima)
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.fo = kDefFrom; f.to = kDefTo; f.oo = kDefOut;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    FillWords(f.win, kWinBytes, Rand(c, 25));
    switch ((n / 16) % 4) {
    case 1: std::memset(f.win, 0x00, kWinBytes); break;
    case 2: std::memset(f.win, 0xFF, kWinBytes); break;
    case 3: for (unsigned i = 0; i < kWinBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.win + i, &w, 4); } break;
    default: break;
    }
    u32 a[4]{}, b[4]{};
    for (u32 i = 0; i < 4; ++i) { a[i] = RandFloat(c, 41 + i, -4, -1); b[i] = RandFloat(c, 45 + i, -4, -1); }
    u32 t = RandFloat(c, 49, -8, -1) & 0x7FFFFFFFu;                                       // [0.004, 1)
    const u32 ui = Rand(c, 70) % 10;
    const u32 sa = (Rand(c, 71) & 1) ? 0x80000000u : 0u;
    u32 unit[4]{};
    for (u32 i = 0; i < 4; ++i) unit[i] = kUnit[ui][i] ^ sa;
    switch (f.family) {
    case 0: break;                                                                        // tipicos (|dot| casi siempre < 1)
    case 1:                                                                               // casi paralelos: dot = 1 - pocos ulp (o algo mayor que 1)
        for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = Perturb(unit[i], Delta(c, 72 + i, 4)); }
        break;
    case 2:                                                                               // from == to (dot = |q|^2; unitario exacto o aleatorio)
        if (Rand(c, 72) & 1) { for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = unit[i]; } }
        else { for (u32 i = 0; i < 4; ++i) { a[i] = RandFloat(c, 41 + i, -3, 0); b[i] = a[i]; } }
        break;
    case 3:                                                                               // opuestos con ruido: dot = -1 +- pocos ulp (dominio, pi de 64 bits o theta cerca de pi)
        for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = Perturb(unit[i], Delta(c, 72 + i, 4)) ^ 0x80000000u; }
        break;
    case 4: {                                                                             // ortogonales (theta = pi/2, frontera de la rama theta > pi/2) con ruido
        const u32 pr = Rand(c, 73) % 6;
        const u32 sb = (Rand(c, 74) & 1) ? 0x80000000u : 0u;
        for (u32 i = 0; i < 4; ++i) { a[i] = kUnit[kOrth[pr][0]][i] ^ sa; b[i] = kUnit[kOrth[pr][1]][i] ^ sb; }
        if (Rand(c, 75) & 1) { const u32 k = Rand(c, 76) % 4; a[k] = Perturb(a[k], Delta(c, 77, 3)); }
        break;
    }
    case 5: for (u32 i = 0; i < 4; ++i) { a[i] = RandFloat(c, 41 + i, -2, 2); b[i] = RandFloat(c, 45 + i, -2, 2); } break;   // sin normalizar: dot mayor que 1 (recorte) o menor que -1 (dominio)
    case 6:                                                                               // mezcla de NaN, inf, ceros, subnormales y enormes
        for (u32 i = 0; i < 4; ++i) {
            if (Rand(c, 50 + i) % 2) a[i] = MixComponent(c, 60 + i * 7);
            if (Rand(c, 54 + i) % 2) b[i] = MixComponent(c, 110 + i * 7);
        }
        break;
    case 7: for (u32 i = 0; i < 4; ++i) { a[i] = Pick(kZ, c, 41 + i); b[i] = Pick(kZ, c, 45 + i); } break;   // ceros, subnormales, +-1 y extremos (dot 0, theta = pi/2)
    case 8:                                                                               // valores especiales de t; la mitad con to negado (rama theta > pi/2)
        if (Rand(c, 72) & 1) { for (u32 i = 0; i < 4; ++i) b[i] ^= 0x80000000u; }
        t = Pick(kT, c, 49);
        break;
    case 9:                                                                               // dot = 1 - hasta 64 ulp por componente: theta pequeno, 1/sin grande
        for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = Perturb(unit[i], Delta(c, 72 + i, 64)); }
        break;
    case 10:                                                                              // destino == from
        f.fo = 0x10; f.oo = 0x10; f.to = 0x30;
        if (Rand(c, 80) & 1) { for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = Perturb(unit[i], Delta(c, 72 + i, 3)); } }
        break;
    case 11:                                                                              // destino == to
        f.to = 0x10; f.oo = 0x10; f.fo = 0x30;
        if (Rand(c, 80) & 1) { for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = Perturb(unit[i], Delta(c, 72 + i, 3)); } }
        break;
    case 12:                                                                              // from y to son el mismo objeto; destino igual o aparte
        f.fo = 0x10; f.to = 0x10; f.oo = (Rand(c, 80) & 1) ? 0x10u : 0x30u;
        for (u32 i = 0; i < 4; ++i) b[i] = a[i];
        break;
    case 13:                                                                              // desplazamientos arbitrarios (solapes parciales)
        f.fo = (Rand(c, 80) % 21) * 4; f.to = (Rand(c, 81) % 21) * 4; f.oo = (Rand(c, 82) % 21) * 4;
        if (Rand(c, 83) % 3 == 0) { for (u32 i = 0; i < 4; ++i) { a[i] = unit[i]; b[i] = Perturb(unit[i], Delta(c, 72 + i, 3)); } }
        break;
    case 14: for (u32 i = 0; i < 4; ++i) { a[i] = Quiet(Rand(c, 40 + i)); b[i] = Quiet(Rand(c, 44 + i)); } t = Quiet(Rand(c, 48)); break;   // bits crudos (nunca sNaN)
    default:                                                                              // 15: empates y cargas utiles de NaN
        for (u32 i = 0; i < 4; ++i) {
            if (Rand(c, 50 + i) % 4) a[i] = Pick(kNan, c, 60 + i);
            if (Rand(c, 54 + i) % 4) b[i] = Pick(kNan, c, 70 + i);
        }
        if (Rand(c, 58) & 1) t = Pick(kNan, c, 59);
        break;
    }
    for (u32 i = 0; i < 4; ++i) { f.a[i] = a[i]; f.b[i] = b[i]; }
    f.t = t;
    return f;
}
}
