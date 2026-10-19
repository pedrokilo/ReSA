#pragma once
// Fixtures del banco CameraShake30: CCamera::ProcessShake(float) en 0x516560 (thiscall, ECX = this, 1 float en la pila que el EXE IGNORA, `ret 4`, devuelve un puntero a un temporal de su pila que nadie lee).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total).
//
// Arena: [guarda 0x40][CCamera 0xD78][guarda 0x40] = 0xDF8 bytes. La tabla de CHandShaker del juego (6 x 0x94 bytes en 0xB6ECA0) y la bandera 0xB70048 son memoria global: el banco las
// escribe antes de cada lado y las restaura al final del caso.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace CameraShake30Fix {
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
constexpr u32 kGuard = 0x40;
constexpr u32 kObjOffset = kGuard;                                   // 0x040
constexpr u32 kObjBytes = 0xD78;                                     // sizeof(CCamera)
constexpr u32 kArena = kObjOffset + kObjBytes + kGuard;              // 0xDF8
static_assert(kArena == 0xDF8, "arena: guarda 0x40 + camara 0xD78 + guarda 0x40");
constexpr u32 kTableAddress = 0xB6ECA0, kEntryBytes = 0x94, kEntries = 6, kTableBytes = kEntryBytes * kEntries;   // 0x378
static_assert(kTableBytes == 0x378, "6 entradas de 0x94 bytes");
constexpr u32 kFlagAddress = 0xB70048;
constexpr u32 kOffActive = 0x59, kOffCams = 0x174, kCamStride = 0x238, kOffIntensity = 0xC90, kOffType = 0xCA0;
constexpr u32 kOffFront = 0x190, kOffUp = 0x1B4;                     // dentro de cada CCam
constexpr u32 kEntryAngZ = 0x08, kEntryMatrix = 0x3C;                // dentro de cada entrada de la tabla
// palabras de la matriz que lee 0x59C810: right (0..2), forward (4..6), up (8..10); los desplazamientos son los de float dentro de CMatrix
constexpr std::array<u32, 9> kMatrixWordIndex{0, 1, 2, 4, 5, 6, 8, 9, 10};

// ------------------------------------------------------------------------------------------------ valores notables
inline u32 Quiet(u32 b) {   // nunca sNaN: el x87 los silencia al cargar con reglas que el oraculo no modela
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F3504F3u, 0xBF3504F3u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x3FC90FDBu, 0xC0490FDBu};
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
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }

struct F {
    u32 family;
    u32 type;           // m_nShakeType (0..5)
    u32 intensity;      // m_fShakeIntensity (+0xC90)
    u32 active;         // m_nActiveCam
    u32 front[3], up[3];// vectores iniciales de la camara activa
    u32 angZ;           // m_ang.z de la entrada tras CHandShaker::Process (lo escribe el falso)
    u32 mat[9];         // m_resultMat tras Process: right(3), forward(3), up(3) (lo escribe el falso)
    u32 flag;           // bandera 0xB70048 al entrar (0 = el EXE rellena la tabla)
    u32 tableSeed, headSeed;
    u32 eax, edx, stack;
};

inline void Identity(u32* m) {
    const u32 one = 0x3F800000u;
    const u32 id[9]{one, 0, 0, 0, one, 0, 0, 0, one};
    for (u32 i = 0; i < 9; ++i) m[i] = id[i];
}

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.type = Rand(c, 1) % kEntries;
    f.active = Rand(c, 2) % 3;
    f.intensity = RandFloat(c, 3, -3, 1) & 0x7FFFFFFFu;
    f.angZ = RandFloat(c, 4, -8, -4);
    for (u32 i = 0; i < 9; ++i) f.mat[i] = RandFloat(c, 10 + i, -8, -3);
    f.mat[0] = 0x3F800000u | (Rand(c, 30) & 0x3FFFFu); f.mat[4] = 0x3F800000u | (Rand(c, 31) & 0x3FFFFu); f.mat[8] = 0x3F800000u | (Rand(c, 32) & 0x3FFFFu);
    for (u32 i = 0; i < 3; ++i) { f.front[i] = RandFloat(c, 40 + i, -3, 0); f.up[i] = RandFloat(c, 44 + i, -3, 0); }
    f.flag = (Rand(c, 5) % 8 == 0) ? 0u : 1u;
    f.tableSeed = Rand(c, 6); f.headSeed = Rand(c, 7);
    f.eax = Rand(c, 25); f.edx = Rand(c, 26); f.stack = StackSeed(n);
    const u32 mixed = Rand(c, 8) % 3;   // 0, 1, 2: cuantos componentes se mezclan en las familias de especiales
    switch (f.family) {
    case 0: break;                                                                             // tipico, tabla ya rellena
    case 1: f.flag = 0; break;                                                                 // tipico, el EXE rellena la tabla
    case 2: {                                                                                  // intensidad con especiales
        const u32 pick = Rand(c, 50) % 8;
        f.intensity = pick == 0 ? 0u : pick == 1 ? 0x80000000u : pick == 2 ? RandFloat(c, 51, -126, -100) & 0x7FFFFFFFu : pick == 3 ? MixComponent(c, 52) : pick == 4 ? RandFloat(c, 53, 20, 100)
                    : pick == 5 ? (RandFloat(c, 53, -3, 1) | 0x80000000u) : pick == 6 ? Pick(kInf, c, 54) : Pick(kNan, c, 54);
        break;
    }
    case 3: f.angZ = (Rand(c, 50) % 3) ? MixComponent(c, 52) : (Rand(c, 51) & 0x807FFFFFu); break;       // m_ang.z con especiales
    case 4: for (u32 i = 0; i < 9; ++i) if (Rand(c, 50 + i) % 3 == 0) f.mat[i] = MixComponent(c, 60 + i * 7); break;   // matriz con especiales
    case 5: for (u32 i = 0; i < 3; ++i) if (Rand(c, 50 + i) % 2 == 0 || i < mixed) f.front[i] = MixComponent(c, 60 + i * 7); break;   // front con especiales
    case 6: {                                                                                  // front = (0, 0, z) con la identidad: front.x == front.y == 0 tras normalizar
        Identity(f.mat);
        f.front[0] = (Rand(c, 50) & 1) ? 0u : 0x80000000u; f.front[1] = (Rand(c, 51) & 1) ? 0u : 0x80000000u;
        f.front[2] = (Rand(c, 52) % 8 == 0) ? 0u : RandFloat(c, 53, -4, 4);
        if (Rand(c, 54) % 4 == 0) f.front[0] = RandFloat(c, 55, -20, -10);        // solo x distinto de 0: no se aplica
        if (Rand(c, 56) % 4 == 0) f.front[1] = RandFloat(c, 57, -20, -10);        // solo y distinto de 0: no se aplica
        if (Rand(c, 58) % 3 == 0) f.angZ = (Rand(c, 59) & 1) ? 0u : 0x80000000u;
        break;
    }
    case 7: f.angZ = RandFloat(c, 50, 8, 15); f.intensity = RandFloat(c, 51, 0, 3) & 0x7FFFFFFFu; break;   // angulo mediano (hasta 2^18 aprox.)
    case 8: {                                                                                  // angulo enorme, infinito o en el rango impreciso del fsin
        switch (Rand(c, 50) % 4) {
        case 0: f.angZ = RandFloat(c, 51, 20, 62); f.intensity = 0x3F800000u; break;                   // 2^20..2^62: resultado impreciso (comodin)
        case 1: f.angZ = RandFloat(c, 51, 63, 100); f.intensity = 0x3F800000u; break;                  // >= 2^63: fsin deja el operando
        case 2: f.angZ = Pick(kInf, c, 51); break;
        default: f.angZ = RandFloat(c, 51, 30, 60); f.intensity = RandFloat(c, 52, 20, 40) & 0x7FFFFFFFu; break;   // producto >= 2^63
        }
        break;
    }
    case 9:                                                                                    // vector girado nulo: matriz con filas nulas o front nulo
        if (Rand(c, 50) & 1) { for (u32 i = 0; i < 9; ++i) if (Rand(c, 60 + i) % 2) f.mat[i] = (Rand(c, 70 + i) & 1) ? 0u : 0x80000000u; }
        else { for (u32 i = 0; i < 3; ++i) f.front[i] = (Rand(c, 60 + i) & 1) ? 0u : 0x80000000u; }
        if (Rand(c, 51) % 3 == 0) for (u32 i = 0; i < 9; ++i) f.mat[i] = 0u;
        break;
    case 10: {                                                                                 // front paralelo a up = (sin A, 0, cos A) con A = 0: lado nulo
        Identity(f.mat);
        f.angZ = (Rand(c, 50) & 1) ? 0u : 0x80000000u;
        f.front[0] = (Rand(c, 51) & 1) ? 0u : 0x80000000u; f.front[1] = (Rand(c, 52) & 1) ? 0u : 0x80000000u;
        f.front[2] = (Rand(c, 53) % 8 == 0) ? 0u : (RandFloat(c, 54, -6, 6));
        if (Rand(c, 55) % 4 == 0) f.front[0] = RandFloat(c, 56, -70, -30);        // casi paralelo
        break;
    }
    case 11: {                                                                                 // casi paralelo: angulo y componentes diminutos
        Identity(f.mat);
        f.angZ = RandFloat(c, 50, -20, -8); f.intensity = 0x3F800000u | (Rand(c, 51) & 0x7FFFFu);
        f.front[0] = RandFloat(c, 52, -20, -8); f.front[1] = RandFloat(c, 53, -20, -8); f.front[2] = RandFloat(c, 54, -1, 1);
        break;
    }
    case 12:                                                                                   // exponente extendido: sumas de cuadrados fuera del rango de float
        for (u32 i = 0; i < 3; ++i) f.front[i] = RandFloat(c, 50 + i, 62, 120);
        if (Rand(c, 53) & 1) for (u32 i = 0; i < 9; ++i) f.mat[i] = RandFloat(c, 60 + i, -30, 30);
        break;
    case 13:                                                                                   // exponente extendido por debajo: cuadrados menores que el minimo de float
        for (u32 i = 0; i < 3; ++i) f.front[i] = RandFloat(c, 50 + i, -90, -50);
        if (Rand(c, 53) & 1) for (u32 i = 0; i < 9; ++i) f.mat[i] = RandFloat(c, 60 + i, -60, 0);
        break;
    case 14:                                                                                   // bits crudos (nunca sNaN)
        f.intensity = Quiet(Rand(c, 50)); f.angZ = Quiet(Rand(c, 51));
        for (u32 i = 0; i < 9; ++i) f.mat[i] = Quiet(Rand(c, 60 + i));
        for (u32 i = 0; i < 3; ++i) { f.front[i] = Quiet(Rand(c, 70 + i)); f.up[i] = Quiet(Rand(c, 74 + i)); }
        break;
    default:                                                                                   // 15: empates y cargas utiles de NaN
        f.intensity = (Rand(c, 50) % 3) ? RandFloat(c, 51, -3, 1) & 0x7FFFFFFFu : Pick(kNan, c, 52);
        f.angZ = (Rand(c, 53) % 3 == 0) ? Pick(kNan, c, 54) : RandFloat(c, 55, -8, -4);
        for (u32 i = 0; i < 9; ++i) f.mat[i] = (Rand(c, 60 + i) % 4 == 0) ? Pick(kNan, c, 70 + i) : f.mat[i];
        for (u32 i = 0; i < 3; ++i) f.front[i] = (Rand(c, 80 + i) % 3 == 0) ? Pick(kNan, c, 90 + i) : f.front[i];
        break;
    }
    return f;
}
}
