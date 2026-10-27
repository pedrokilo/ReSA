#pragma once
// Fixtures del banco ForcePedOut71 (lote 71): constructor de CTaskSimpleCarForcePedOut (EXE 0x647710), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleCarForcePedOut::Constructor  0x647710  thiscall, ECX = this (0x10 bytes), (vehiculo, puerta) en la pila, `ret 8`, EAX = this   (hook CTaskSimpleCarForcePedOut / Constructor, tipo S)
//
// Reparto: familia = n % 16 (1024 casos por familia). La puerta viaja como dword sin interpretar.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Fp71Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x10, kSmallWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kSmallArena = Arena(kSmallWin);
constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds (region vigilada)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 71071u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

inline constexpr std::array<u32, 20> kDoors{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x80000001u, 0x7FFFFFFFu, 0x100u, 0xFFFFFF00u, 0xFFu, 0x01000000u, 0xDEADBEEFu};
// Vehiculos crudos (no se desreferencian: el registro de referencias esta sustituido): impares, extremos, alineados y no alineados
inline constexpr std::array<u32, 12> kRawVeh{1u, 2u, 0x10u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00400000u, 0x00B74490u, 0xFFFFFFF0u, 0x12345679u, 0xDEADBEEFu, 0x00000004u};
// vehiculo: 0 = nulo, 1 = arena, 2 = crudo
// Relleno previo del objeto (0x10 bytes): 0 = semilla, 1 = 0xCD (como el pool de tareas), 2 = ceros, 3 = 0xFF

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 vehKind{}, vehRaw{}, door{}, prefill{}, vehObjOff{};
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.vehRaw = Rand(n, 11) % 2 ? Pick(kRawVeh, n, 12) : (Rand(n, 13) | 1u);
    { const u32 r = Rand(n, 14) % 8; c.prefill = r >= 4 && r <= 6 ? r - 3u : 0u; }   // 5/8 semilla, 1/8 cada uno de 0xCD / ceros / 0xFF
    c.vehObjOff = Rand(n, 15) % 3;                                                // desplazamiento del vehiculo dentro de la ventana (0, 4, 8 bytes)
    switch (c.family) {
    case 0: case 1: c.vehKind = 0; break;                       // vehiculo nulo: no se registra la referencia
    case 2: case 3: case 4: case 5: case 6: case 7: c.vehKind = 1; break;
    case 8: case 9: case 10: case 11: c.vehKind = 2; break;
    default: c.vehKind = Rand(n, 16) % 3; break;
    }
    switch (c.family % 4) {
    case 0: c.door = Rand(n, 17) % 8; break;                    // puertas validas
    case 1: c.door = Pick(kDoors, n, 18); break;                // extremos
    case 2: c.door = Rand(n, 19); break;                        // aleatoria
    default: c.door = (Rand(n, 20) % 2 ? 0x80000000u : 0xFFFF0000u) | (Rand(n, 21) & 0xFFFFu); break;   // bit alto activo
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("veh_kind", c.vehKind).U("veh_raw", c.vehRaw).U("door", c.door).U("prefill", c.prefill).U("veh_off", c.vehObjOff * 4);
    return j.End();
}
}
