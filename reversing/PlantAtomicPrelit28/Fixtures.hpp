#pragma once
// Fixtures del banco PlantAtomicPrelit28: CPlantMgr::AtomicCreatePrelitIfNeeded (0x5DD100, cdecl, `ret` normal, 1 argumento: RpAtomic* atomic).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y la herramienta dump_fixtures.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input`
// que registra el banco y regenera los datos con su propio codigo; este archivo solo decide QUE casos se prueban.
//
// 4096 casos = 8 grupos x 512. Grupo = n % 8; familia = (n / 8) % 16 (modo de relleno de datos = familia % 8; ver FillData).
//   0: geometria CON luces precalculadas (flags & 8): la funcion no hace nada y no llama a nadie.
//   1: 0 vertices y 0 triangulos.            2: 0 triangulos, 1..kMax vertices.        3: 0 vertices, 1..kMax triangulos.
//   4: contadores pequenos 1..8.             5: contadores limite {0,1,3,31,32,33,99,100} x {0,1,3,31,32,33,99,100}.
//   6: contadores aleatorios 0..kMax.        7: 1..kMax vertices y un solo triangulo.
// Los flags de la geometria vieja (sin bit 8 salvo en el grupo 0) y los de la nueva (la que "crea" el falso) son palabras de 32 bits arbitrarias.
// La memoria de los casos son arenas propias del banco con guardas aleatorias; los vertices, coordenadas de textura y triangulos son bytes arbitrarios
// (NaN, infinitos, denormales, matIndex 0xFFFF, indices fuera de rango...) porque la funcion los copia sin interpretarlos.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace PlantPrelitFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kGroups = 8;
constexpr unsigned kPerGroup = 512;
constexpr unsigned kCases = kGroups * kPerGroup;   // 4096
constexpr u32 kMaxV = 100, kMaxT = 100;

// ------------------------------------------------------------------------------------------------ disposicion de las arenas (compartida con fixture_oracle.py)
// Arena de la geometria NUEVA (la que devuelve el falso de RpGeometryCreate): [0x00..0x40) guarda | geometria (0x60) en 0x40 | morph target (0x28) en 0xC0 |
//   vertices (kMaxV*12) en 0x100 | guarda | coordenadas (kMaxV*8) en 0x5D0 | guarda | triangulos (kMaxT*8) en 0x910 | guarda hasta 0xC70.
// Arena de la geometria VIEJA: igual que la nueva, y ademas el RpAtomic (0xA0 bytes) en 0xC80; total 0xD60.
constexpr u32 kGeomAt = 0x40, kMorphAt = 0xC0, kVertsAt = 0x100, kTexAt = 0x5D0, kTriAt = 0x910, kArena = 0xC70, kAtomicAt = 0xC80, kOldArena = 0xD60;
constexpr u32 kGeomPtrTri = 0x2C, kGeomPtrTex = 0x34, kGeomPtrMorph = 0x5C, kMorphPtrParent = 0x00, kMorphPtrVerts = 0x14;   // dwords de puntero (se enmascaran en los hashes)

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
inline u64 Fnv64(const u8* p, std::size_t n) { u64 h = 14695981039346656037ull; for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 1099511628211ull; } return h; }

// kind: 0 vertices (12 bytes c/u), 1 coordenadas (8), 2 triangulos (8: 3 indices u16 + matIndex u16 en +6). Modo = familia & 7.
inline void FillData(u8* dst, std::size_t bytes, u32 seed, u32 mode, u32 kind) {
    switch (mode & 7) {
    case 1: std::memset(dst, 0, bytes); break;
    case 2: std::memset(dst, 0xFF, bytes); break;
    case 6: for (std::size_t i = 0; i < bytes; ++i) dst[i] = u8(u32(i) * 31u + kind * 7u + seed); break;
    default:
        FillWords(dst, bytes, seed + kind * 7919u);
        if ((mode & 7) == 3 && kind == 2) for (std::size_t o = 0; o + 8 <= bytes; o += 8) { dst[o + 6] = 0xFF; dst[o + 7] = 0xFF; }               // matIndex 0xFFFF
        if ((mode & 7) == 4 && kind == 0) for (std::size_t o = 0; o + 4 <= bytes; o += 4) { dst[o + 3] = u8((dst[o + 3] & 0x80) | 0x7F); dst[o + 2] |= 0x80; }   // NaN / infinitos
        if ((mode & 7) == 5 && kind == 1) for (std::size_t o = 0; o + 4 <= bytes; o += 4) { dst[o + 3] &= 0x80; dst[o + 2] &= 0x7F; }                 // denormales
        break;
    }
}

struct Fx {
    unsigned n{};
    int group{}, family{};
    u32 mode{};
    u32 flags{};                   // flags de la geometria vieja
    i32 numV{}, numT{};
    u32 oldSeed{}, dataSeed{}, newSeed{}, newFlags{};
    u32 eax{}, ecx{}, edx{}, stack{};
};

inline Fx Make(unsigned n) {
    static constexpr u32 kClear[8]{0x0u, 0x2u, 0x7u, 0xFFFFFFF7u, 0x80000000u, 0x77u, 0x5u, 0u};
    static constexpr u32 kPrelit[8]{0x8u, 0xFFFFFFFFu, 0xDu, 0xFu, 0x80000008u, 0x1000008u, 0x48u, 0u};
    static constexpr u32 kEdge[8]{0, 1, 3, 31, 32, 33, 99, 100};
    Fx f;
    f.n = n;
    f.group = int(n % kGroups);
    f.family = int((n / kGroups) % 16);
    f.mode = u32(f.family);
    const auto R = [&](u32 k) { return Rand(n, k); };
    const u32 pick = R(3) & 7;
    f.flags = f.group == 0 ? (pick == 7 ? (R(4) | 8u) : kPrelit[pick]) : (pick == 7 ? (R(4) & ~8u) : kClear[pick]);
    switch (f.group) {
    case 0: f.numV = i32(R(1) % (kMaxV + 1)); f.numT = i32(R(2) % (kMaxT + 1)); break;
    case 1: f.numV = 0; f.numT = 0; break;
    case 2: f.numV = i32(1 + R(1) % kMaxV); f.numT = 0; break;
    case 3: f.numV = 0; f.numT = i32(1 + R(2) % kMaxT); break;
    case 4: f.numV = i32(1 + R(1) % 8); f.numT = i32(1 + R(2) % 8); break;
    case 5: f.numV = i32(kEdge[(n / 8) % 8]); f.numT = i32(kEdge[(n / 64) % 8]); break;
    case 6: f.numV = i32(R(1) % (kMaxV + 1)); f.numT = i32(R(2) % (kMaxT + 1)); break;
    default: f.numV = i32(1 + R(1) % kMaxV); f.numT = 1; break;
    }
    f.oldSeed = R(100); f.dataSeed = R(101); f.newSeed = R(102); f.newFlags = R(103);
    f.eax = R(104); f.ecx = R(105); f.edx = R(106); f.stack = kStackSeeds[R(107) & 7];
    return f;
}
}
