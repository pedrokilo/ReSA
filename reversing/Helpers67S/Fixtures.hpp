#pragma once
// Fixtures del banco Helpers67S: el constructor de CDecisionMakerTypes (0x4650F0), que seguia como envoltura al EXE (lote 67).
//   CDecisionMakerTypes::CDecisionMakerTypes()  0x4650F0  thiscall, ECX = this, sin argumentos en pila, `ret`. EAX = this, ECX = fs:[0] de entrada, EDX = 0.
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
// Reparto: 16384 casos; familia = n % 16 (1024 casos por familia).
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>

namespace H67Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ arena
// [guarda 0x40][relleno 0x10][objeto 0xF1C0][guarda 0x40]; el objeto esta en uno de ocho desplazamientos (cuatro desalineados).
constexpr u32 kGuard = 0x40;
constexpr u32 kObjBytes = 0xF1C0;                       // sizeof(CDecisionMakerTypes)
constexpr u32 kArena = kGuard + 0x10 + kObjBytes + kGuard;
static_assert(kArena == 0xF250, "arena: 0x40 + 0x10 + 0xF1C0 + 0x40");
constexpr u32 kObjOffsets[8]{0x40, 0x41, 0x42, 0x43, 0x44, 0x48, 0x4C, 0x50};
// Disposicion del objeto (desensamblado de 0x4650F0)
constexpr u32 kCountOff = 0;                            // m_NoOfDecisionMakers
constexpr u32 kMakersOff = 4;                           // 20 CDecisionMaker de 0x99C bytes
constexpr u32 kEventsOff = 0xC034;                      // m_EventIndices: 96 dwords que el constructor NO toca
constexpr u32 kEventsBytes = 0x180;
constexpr u32 kDefaultsOff = 0xC1B4;                    // 5 CDecisionMaker por defecto
constexpr u32 kDecisionBytes = 0x3C;
constexpr u32 kMakerBytes = 0x99C;
static_assert(kMakersOff + 20 * kMakerBytes == kEventsOff);
static_assert(kEventsOff + kEventsBytes == kDefaultsOff);
static_assert(kDefaultsOff + 5 * kMakerBytes == kObjBytes);
// Globales que escribe (region vigilada = estos tres rangos contiguos mas 0x10 bytes antes y 0x10 despues; la instancia del singleton, 0xC0B030, conserva su valor real)
constexpr u32 kRegionAddr = 0xC0AFD0;
constexpr u32 kRegionBytes = 0x70;
constexpr u32 kFlagsOff = 0xC0AFE0 - kRegionAddr;       // 0x10: 20 bytes = 0 (banderas de grupo)
constexpr u32 kIndexOff = 0xC0AFF4 - kRegionAddr;       // 0x24: 40 bytes = 20 words de valor 1 (ScriptReferenceIndex)
constexpr u32 kActiveOff = 0xC0B01C - kRegionAddr;      // 0x4C: 20 bytes = 0 (m_IsActive)
constexpr u32 kInstanceOff = 0xC0B030 - kRegionAddr;    // 0x60: puntero de la instancia: no se toca
static_assert(kFlagsOff == 0x10 && kIndexOff == 0x24 && kActiveOff == 0x4C && kInstanceOff == 0x60 && kInstanceOff + 4 < kRegionBytes);

// El ruido de la arena es una baldosa de 127 palabras repetida (127 es primo: no se alinea con las decisiones de 15 palabras): el oraculo lo reproduce rapido.
constexpr u32 kTileWords = 127;
// Relleno previo de la arena: 0 y 5 ruido (semillas distintas), 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5
inline void FillArena(u8* arena, u32 size, u32 mode, u32 seed) {
    switch (mode) {
    case 1: std::memset(arena, 0x00, size); break;
    case 2: std::memset(arena, 0xFF, size); break;
    case 3: std::memset(arena, 0xCD, size); break;
    case 4: std::memset(arena, 0xA5, size); break;
    default: {
        const u32 s = mode == 5 ? seed ^ 0x5A5A5A5Au : seed;
        for (u32 i = 0; i < size / 4; ++i) { const u32 w = Rand(s, i % kTileWords); std::memcpy(arena + 4 * i, &w, 4); }
        break;
    }
    }
}

struct Case {
    u32 n, family;
    u32 objOff;         // desplazamiento del objeto en la arena
    u32 mode;           // relleno previo de la arena (0..5)
    u32 seedArena;      // semilla del ruido (entre 61 valores)
    u32 gmode;          // relleno previo de la region de globales (0..5)
    u32 gseed;
    u32 eax, edx, stack;
};
constexpr u32 kSeedPool = 61;
inline Case Make(unsigned n) {
    Case f{};
    const u32 c = n * 8u + 0x10001u;
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    f.objOff = kObjOffsets[Rand(c, 1) % 8];
    if (fam == 0) f.objOff = 0x40;
    else if (fam == 1) f.objOff = 0x41 + Rand(c, 2) % 4;            // desalineado (0x41..0x44)
    else if (fam == 13) f.objOff = 0x48 + 4 * (Rand(c, 2) % 3);     // el objeto llega hasta la guarda final
    f.mode = (Rand(c, 3) % 3 == 0) ? 5u : 0u;
    if (fam >= 2 && fam <= 5) f.mode = fam - 1;                     // arena constante: 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5
    else if (fam >= 14) f.mode = Rand(c, 4) % 6;
    f.seedArena = 1u + Rand(c, 5) % kSeedPool;
    f.gmode = 0;
    if (fam >= 6 && fam <= 10) f.gmode = fam - 5;                   // globales: 1 ceros, 2 0xFF, 3 ya finales, 4 invertidos, 5 0x80
    else if (fam >= 14) f.gmode = Rand(c, 6) % 6;
    f.gseed = Rand(c, 7);
    static constexpr u32 extremes[]{0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, 0x00FF00FFu};
    f.eax = Rand(c, 8) % 5 == 0 ? 0u : Rand(c, 9);
    f.edx = Rand(c, 10) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 11);
    if (fam == 11) { f.eax = extremes[Rand(c, 12) % std::size(extremes)]; f.edx = extremes[Rand(c, 13) % std::size(extremes)]; }
    f.stack = fam == 12 ? StackSeed((n / 16) % 12) : StackSeed(Rand(c, 14) % 12);
    return f;
}

// Imagen previa de la arena
inline void BuildRef(const Case& f, u8* ref) { FillArena(ref, kArena, f.mode, f.seedArena); }
// Imagen previa de la region de globales; `instance` = valor real de 0xC0B030 (se conserva)
inline void BuildRegionPre(const Case& f, u32 instance, u8* region) {
    switch (f.gmode) {
    case 1: std::memset(region, 0x00, kRegionBytes); break;
    case 2: std::memset(region, 0xFF, kRegionBytes); break;
    case 5: std::memset(region, 0x80, kRegionBytes); break;
    default:
        for (u32 i = 0; i < kRegionBytes / 4; ++i) { const u32 w = Rand(f.gseed, i); std::memcpy(region + 4 * i, &w, 4); }
        break;
    }
    if (f.gmode == 3) {                                             // ya finales: banderas 0, indices 1, activos 0
        std::memset(region + kFlagsOff, 0, 20);
        for (u32 i = 0; i < 20; ++i) { region[kIndexOff + 2 * i] = 1; region[kIndexOff + 2 * i + 1] = 0; }
        std::memset(region + kActiveOff, 0, 20);
    } else if (f.gmode == 4) {                                      // lo contrario: banderas 1, indices 0, activos 1
        std::memset(region + kFlagsOff, 1, 20);
        std::memset(region + kIndexOff, 0, 40);
        std::memset(region + kActiveOff, 1, 20);
    }
    std::memcpy(region + kInstanceOff, &instance, 4);
}
}
