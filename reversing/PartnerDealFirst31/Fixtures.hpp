#pragma once
// Fixtures del banco PartnerDealFirst31: CTaskComplexPartnerDeal::CreateFirstSubTask (0x6823B0, thiscall con un argumento de pila (ped), `ret 4`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// El EXE es un unico `jmp 0x681F20` (CTaskComplexPartner::CreateFirstSubTask): no lee ni escribe nada propio. El banco sustituye 0x681F20 por un falso identico en ambos lados que registra
// (this, ped) y devuelve un valor del caso; se comprueba que la funcion reenvia EXACTAMENTE `this` y el argumento (puntero nulo, bordes, valores arbitrarios: nunca se desreferencian),
// que devuelve en EAX el valor del llamado y que no toca el objeto ni rompe la ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerDealFirst31Fix {
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
constexpr u32 kThisBytes = 0x74;                        // sizeof(CTaskComplexPartnerDeal)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;      // 0x40 + 0x74 + 0x4C = 0x100

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, pedKind, ped, retKind, ret;
    u32 eax, edx, stack;
};

inline constexpr u32 kPedEdges[]{0u, 1u, 2u, 3u, 4u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xCDCDCDCDu, 0xDEADBEEFu, 0x00400000u, 0x00682440u};
inline constexpr u32 kRetEdges[]{0u, 1u, 2u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xCDCDCDCDu, 0xDEADBEEFu, 0x00681F20u, 0x006823B0u};

// Familias (n % 16, 256 casos cada una): ped y retorno segun la familia
//  0 ped nulo, retorno nulo   1 ped nulo, retorno arbitrario   2 ped de borde, retorno de borde   3 ped arbitrario, retorno arbitrario
//  4 retorno = this   5 retorno = ped   6 ped = this (puntero al propio objeto; retorno al azar)   7 ped y retorno con bits altos   8..15 mezcla al azar
// pedKind: 0 valor `ped`, 1 el propio `this` (puntero al objeto del caso) ; retKind: 0 valor `ret`, 1 el propio `this`, 2 el propio `ped`
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    const u32 pedRand = Rand(n, 1);
    const u32 retRand = Rand(n, 2);
    auto edgePed = [&] { return kPedEdges[Rand(n, 3) % std::size(kPedEdges)]; };
    auto edgeRet = [&] { return kRetEdges[Rand(n, 4) % std::size(kRetEdges)]; };
    switch (f) {
    case 0: c.ped = 0; c.ret = 0; break;
    case 1: c.ped = 0; c.ret = retRand; break;
    case 2: c.ped = edgePed(); c.ret = edgeRet(); break;
    case 3: c.ped = pedRand; c.ret = retRand; break;
    case 4: c.ped = pedRand; c.retKind = 1; break;
    case 5: c.ped = Rand(n, 5) % 2 ? edgePed() : pedRand; c.retKind = 2; break;
    case 6: c.pedKind = 1; c.ret = retRand; c.retKind = Rand(n, 9) % 3; break;
    case 7: c.ped = pedRand | 0x80000000u; c.ret = retRand | 0x80000000u; break;
    default:
        c.ped = Rand(n, 6) % 3 == 0 ? edgePed() : pedRand;
        c.retKind = Rand(n, 7) % 3;
        c.pedKind = Rand(n, 10) % 8 == 0 ? 1u : 0u;
        c.ret = Rand(n, 8) % 3 == 0 ? edgeRet() : retRand;
        break;
    }
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    return c;
}
}  // namespace PartnerDealFirst31Fix
