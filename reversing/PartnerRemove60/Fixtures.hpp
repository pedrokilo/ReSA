#pragma once
// Fixtures del banco PartnerRemove60: CTaskComplexPartner::RemoveStreamedAnims (0x682370, thiscall sin argumentos, `ret`, void).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this` el BYTE m_requiredAnimsStreamedIn (+0x6E) y el primer byte de m_animBlockName (+0x5E); escribe solo +0x6E.
//   byte +0x6E == 0 (nombre cualquiera): fin sin llamadas.
//   byte +0x6E != 0 y m_animBlockName[0] == 0: byte +0x6E = 0 sin llamadas.
//   byte +0x6E != 0 y nombre no vacio: idx = GetAnimationBlockIndex(&this[0x5E]) (0x4D3990, cdecl); RemoveAnimBlockRef(idx) (0x4D3FD0, sin hook en HEAD) y byte +0x6E = 0.
// Los dos colaboradores se sustituyen por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL) que registran sus argumentos EN ORDEN.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerRemove60Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;   // 16 familias x 256

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kThisBytes = 0x70;                        // sizeof(CTaskComplexPartner)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;      // 0x40 + 0x70 + 0x50 = 0x100
constexpr u32 kNameAt = 0x5E, kFlagAt = 0x6E;           // m_animBlockName, m_requiredAnimsStreamedIn
constexpr u32 kBlockSize = 0x20, kLoadedAt = 0x10;      // CAnimBlock: sizeof y desplazamiento de IsLoaded
constexpr u32 kBlocksBase = 0xB5D4A0, kNumBlocksAt = 0xB4EA30;
constexpr u32 kModelBase = 0x63E7, kKeepInMemory = 8;
constexpr u32 kMaxEvents = 8;

// ------------------------------------------------------------------------------------------------ caso
// idxKind: 0 bloque 0, 1 ultimo bloque (numAnimBlocks-1), 2 la mitad, 3 al azar
struct C {
    u32 family, flag, nameFirst, idxKind, idxRaw, loaded;
    u32 eax, edx, stack;
};

inline constexpr u8 kByteEdges[]{2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF, 0x10};
inline u32 IdxFor(const C& c, u32 numBlocks) {
    switch (c.idxKind) {
    case 0: return 0;
    case 1: return numBlocks - 1;
    case 2: return numBlocks / 2;
    default: return c.idxRaw % numBlocks;
    }
}

// Familias (n % 16, 256 casos cada una):
//  0 flag 0 (nombre no vacio): nada                         1 flag 0 (nombre vacio): nada
//  2 flag 1, nombre no vacio: GetAnimationBlockIndex + RemoveAnimBlockRef y flag = 0           3 flag no canonico (2..0xFF), nombre no vacio: igual
//  4 flag 1, nombre vacio: flag = 0 sin llamadas                                                5 flag no canonico, nombre vacio: igual
//  6, 7 como 2 y 3 con IsLoaded arbitrario (la funcion no lo lee)
//  8..11 indices de borde (0, ultimo, mitad) con las cuatro rutas
//  12..15 mezcla al azar
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    const u32 nz = Rand(n, 7) % 3 == 0 ? u32(kByteEdges[Rand(n, 8) % std::size(kByteEdges)]) : (2u + Rand(n, 9) % 254u);              // flag no canonico (>= 2)
    const u32 nameNz = Rand(n, 24) % 4 == 0 ? u32(kByteEdges[Rand(n, 25) % std::size(kByteEdges)]) : (1u + Rand(n, 26) % 255u);   // primer byte del nombre != 0
    const u32 loadedEdge = Rand(n, 10) % 2 ? u32(kByteEdges[Rand(n, 11) % std::size(kByteEdges)]) : (Rand(n, 12) & 0xFFu);
    c.idxKind = 3;
    c.idxRaw = Rand(n, 14);
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    const u32 loadedNon = loadedEdge <= 1 ? 2u : loadedEdge;                                  // IsLoaded no canonico: ni 0 ni 1
    const u32 loadedAny = Rand(n, 22) % 3 == 0 ? 1u : Rand(n, 23) % 2 ? 0u : loadedEdge;       // IsLoaded cualquiera
    switch (f) {
    case 0: c.flag = 0; c.nameFirst = nameNz; break;
    case 1: c.flag = 0; c.nameFirst = 0; break;
    case 2: c.flag = 1; c.nameFirst = nameNz; break;
    case 3: c.flag = nz; c.nameFirst = nameNz; break;
    case 4: c.flag = 1; c.nameFirst = 0; break;
    case 5: c.flag = nz; c.nameFirst = 0; break;
    case 6: c.flag = 1; c.nameFirst = nameNz; break;
    case 7: c.flag = nz; c.nameFirst = nameNz; break;
    case 8: case 9: case 10: case 11: {
        c.idxKind = (f - 8) % 3;   // 0, 1, 2, 0
        const u32 path = Rand(n, 16) % 4;
        c.flag = path == 0 ? 0u : path == 1 ? 1u : nz;
        c.nameFirst = path == 3 ? 0u : nameNz;
        break;
    }
    default:
        c.flag = Rand(n, 16) % 3 == 0 ? 0u : Rand(n, 17) % 2 ? 1u : nz;
        c.nameFirst = Rand(n, 18) % 4 == 0 ? 0u : nameNz;
        c.idxKind = Rand(n, 21) % 4;
        break;
    }
    return c;
}
}  // namespace PartnerRemove60Fix
