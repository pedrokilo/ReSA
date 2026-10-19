#pragma once
// Fixtures del banco QuatGet28: CQuaternion::Get(RwMatrix*) const en 0x59C080 (thiscall, ECX = cuaternion {x,y,z,w}, 1 puntero en pila, `ret 4`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco,
// que lleva todo lo necesario; Mix/Rand/FillWords solo hacen falta para regenerar las fixtures.
//
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total). Los umbrales de cobertura de run_local.py se apoyan en ese reparto (ver README.md).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace QuatGet28Fix {
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
// Arena de la matriz: [guarda 0x40][RwMatrix 0x40][guarda 0x40]. Arena del cuaternion: [guarda 0x10][cuaternion 0x10][guarda 0x10].
constexpr u32 kMatOffset = 0x40, kMatBytes = 0x40, kMatArena = kMatOffset + kMatBytes + 0x40;
constexpr u32 kQOffset = 0x10, kQBytes = 0x10, kQArena = kQOffset + kQBytes + 0x10;

// ------------------------------------------------------------------------------------------------ valores notables
// Nunca sNaN: el x87 los silencia al cargar con reglas distintas segun el operando (memoria / registro) que el oraculo no modela; ver README.md.
inline u32 Quiet(u32 b) {
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F3504F3u, 0xBF3504F3u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x3FC00000u, 0xC0400000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
// NaN silenciosos con cargas utiles distintas (la carga util y el signo de los dos primeros se usan para comprobar el criterio «mayor significando»).
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00002u, 0x7FE00000u, 0xFFE00000u, 0x7FC7FFFEu};
inline constexpr std::array<u32, 6> kZeroOne{0x00000000u, 0x80000000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u};
inline constexpr std::array<u32, 4> kEdgeMant{0x7FFFFFu, 0x000001u, 0x400000u, 0x7FFFFEu};
inline constexpr std::array<u32, 13> kAlias{0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30};   // desplazamiento (bytes) del cuaternion dentro de la matriz

// float con exponente sin sesgo en [lo, hi], mantisa aleatoria y signo aleatorio
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }

struct F {
    u32 family;
    u32 q[4];                 // bits de x, y, z, w
    u32 alias;                // 0xFFFFFFFF = cuaternion en su propia arena; si no, desplazamiento en bytes dentro de la matriz
    u32 eax, edx, stack;
    u8 mat[kMatBytes];        // contenido previo de la matriz (el cuaternion alias se superpone aqui)
};

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.alias = 0xFFFFFFFFu;
    f.eax = Rand(c, 23); f.edx = Rand(c, 24); f.stack = StackSeed(n);
    // contenido previo de la matriz: cuatro estilos (aleatorio, 0x00, 0xFF, NaN silenciosos)
    FillWords(f.mat, kMatBytes, Rand(c, 25));
    switch ((n / 16) % 4) {
    case 1: std::memset(f.mat, 0x00, kMatBytes); break;
    case 2: std::memset(f.mat, 0xFF, kMatBytes); break;
    case 3: for (unsigned i = 0; i < kMatBytes; i += 4) { const u32 w = 0x7FC00000u + i; std::memcpy(f.mat + i, &w, 4); } break;
    default: break;
    }
    u32 q[4]{};
    switch (f.family) {
    case 0: for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 40 + i, -6, 1); break;                                     // tipicos
    case 1: for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 40 + i, -30, 20); break;                                   // rango amplio
    case 2: for (u32 i = 0; i < 4; ++i) q[i] = kSpecial[Rand(c, 40 + i) % 12]; break;                                  // ejes / valores simples
    case 3: for (u32 i = 0; i < 4; ++i) q[i] = Pick(kZeroOne, c, 40 + i); break;                                       // ceros con signo, +-1
    case 4:                                                                                                            // diminutos: productos en el borde de los subnormales
        for (u32 i = 0; i < 4; ++i) q[i] = (n & 256) ? RandFloat(c, 40 + i, -75, -62) : ((Rand(c, 50 + i) & 1) ? RandFloat(c, 40 + i, -126, -70) : (Rand(c, 40 + i) & 0x807FFFFFu));
        break;
    case 5: for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 40 + i, 70, 127); break;                                   // enormes: desbordan a inf
    case 6: {                                                                                                          // infinitos
        for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 3 == 0) ? Pick(kInf, c, 60 + i) : ((Rand(c, 50 + i) % 3 == 1) ? Pick(kZeroOne, c, 60 + i) : RandFloat(c, 40 + i, -6, 1));
        q[Rand(c, 70) & 3] = Pick(kInf, c, 71);
        break;
    }
    case 7: for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 40 + i, -6, 1); q[Rand(c, 70) & 3] = Pick(kNan, c, 71); break;   // un NaN
    case 8: {                                                                                                          // varios NaN con cargas utiles distintas
        for (u32 i = 0; i < 4; ++i) q[i] = (Rand(c, 50 + i) % 5 == 0) ? Pick(kInf, c, 60 + i) : RandFloat(c, 40 + i, -6, 1);
        const u32 a = Rand(c, 70) & 3, b = (a + 1 + Rand(c, 71) % 3) & 3;
        const u32 pa = 1 + Rand(c, 72) % 0x3FFFFE;
        u32 pb = 1 + Rand(c, 73) % 0x3FFFFE;
        if (pb == pa) pb = pa + 1;                                          // significandos distintos: el criterio «mayor significando» es unico
        q[a] = 0x7FC00000u | pa | ((Rand(c, 74) & 1) << 31);
        q[b] = 0x7FC00000u | pb | ((Rand(c, 75) & 1) << 31);
        if (Rand(c, 76) % 4 == 0) { q[(b + 1) & 3] = 0x7FC00000u | (1 + Rand(c, 77) % 0x3FFFFE) | ((Rand(c, 78) & 1) << 31); }
        break;
    }
    case 9: for (u32 i = 0; i < 4; ++i) { const u32 r = Rand(c, 50 + i) % 5; q[i] = r == 0 ? Pick(kNan, c, 60 + i) : r == 1 ? Pick(kInf, c, 60 + i) : r == 2 ? (Rand(c, 61 + i) & 0x80000000u) : r == 3 ? Pick(kZeroOne, c, 60 + i) : RandFloat(c, 40 + i, -6, 1); } break;
    case 10: for (u32 i = 0; i < 4; ++i) q[i] = Quiet(Rand(c, 40 + i)); break;                                         // bits crudos (nunca sNaN)
    case 11: for (u32 i = 0; i < 4; ++i) { const u32 e = u32(127 - 10 + int(Rand(c, 50 + i) % 14)); q[i] = ((Rand(c, 40 + i) & 1) << 31) | (e << 23) | kEdgeMant[Rand(c, 60 + i) % 4]; } break;   // mantisas extremas
    case 12: {                                                                                                         // dos NaN con la misma carga util y signo opuesto (empate: el oraculo no lo resuelve)
        for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 40 + i, -6, 1);
        const u32 a = Rand(c, 70) & 3, b = (a + 1 + Rand(c, 71) % 3) & 3;
        const u32 pay = 0x400000u | (Rand(c, 72) % 0x3FFFFF);
        q[a] = 0x7F800000u | pay; q[b] = 0xFF800000u | pay;
        break;
    }
    case 13: for (u32 i = 0; i < 4; ++i) q[i] = RandFloat(c, 40 + i, -6, 1); f.alias = Pick(kAlias, c, 80); break;       // cuaternion dentro de la matriz
    case 14: {                                                                                                         // idem con valores especiales
        for (u32 i = 0; i < 4; ++i) { const u32 r = Rand(c, 50 + i) % 6; q[i] = r == 0 ? Pick(kNan, c, 60 + i) : r == 1 ? Pick(kInf, c, 60 + i) : r == 2 ? Pick(kZeroOne, c, 60 + i) : r == 3 ? kSpecial[Rand(c, 60 + i) % kSpecial.size()] : RandFloat(c, 40 + i, -6, 1); }
        f.alias = Pick(kAlias, c, 80);
        break;
    }
    default: {                                                                                                         // 15: cancelaciones exactas (B == Z2W, A == D...) para el signo del cero
        const u32 a = RandFloat(c, 40, -3, 1), b = RandFloat(c, 41, -3, 1);
        const u32 s = (Rand(c, 42) & 1) ? 0x80000000u : 0u, t = (Rand(c, 43) & 1) ? 0x80000000u : 0u;
        switch (Rand(c, 44) % 4) {
        case 0: q[0] = a; q[1] = b; q[2] = b ^ s; q[3] = a ^ t; break;      // x = w, y = z (salvo signos)
        case 1: q[0] = a; q[1] = a ^ s; q[2] = b; q[3] = b ^ t; break;      // x2x == y2y
        case 2: q[0] = a; q[1] = a; q[2] = a; q[3] = a ^ s; break;
        default: q[0] = a ^ s; q[1] = b; q[2] = a ^ t; q[3] = b; break;
        }
        break;
    }
    }
    for (u32 i = 0; i < 4; ++i) f.q[i] = q[i];
    return f;
}
}
