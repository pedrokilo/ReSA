#pragma once
// Fixtures del banco SmallHelpers64B (lote 64B): tres funciones pequenas sin clase propia, 4096 casos cada una. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   0  GetPopcycleGroupPedModel          0x406E80  cdecl (int popcycleGroup, int slot, int worldZone), int en EAX (movzx word)
//   1  FlushWaterRenderBuffer            0x6E7680  cdecl, void, sin argumentos
//   2  CTaskComplex::Destructor (cuerpo) 0x61A3D0  thiscall (ECX = this), void, sin argumentos de pila
//
// Constantes de arena: el C++ del banco (Core.hpp) y el oraculo Python (fixture_oracle.py) usan EXACTAMENTE estas.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace Small64Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en el espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: ver README.md)
// 0: tablas del juego. CPopulation::m_TranslationArray (0x8D2540): 33 grupos x 12 bytes = 396; ventana = 64 de guarda + 396 + 64 = 524.
//    CPopulation::m_PedGroups (0xC0F358): 57 grupos x 21 words = 57 x 42 = 2394; ventana = 128 de guarda + 2394 + 128 = 2650.
constexpr u32 kNumGroups = 33, kNumPedGroups = 57;
constexpr u32 kTransBase = 0x8D2540, kTransGuard = 64, kTransMain = 396, kTransBytes = 524;
constexpr u32 kPedBase = 0xC0F358, kPedGuard = 128, kPedMain = 2394, kPedBytes = 2650;
static_assert(kNumGroups * 12 == kTransMain && kTransGuard + kTransMain + kTransGuard == kTransBytes, "ventana de traduccion");
static_assert(kNumPedGroups * 42 == kPedMain && kPedGuard + kPedMain + kPedGuard == kPedBytes, "ventana de grupos de peatones");
// 1: contadores del bufer de agua: 0xC4B950 (vertices), 0xC4B954 (indices) y g_nNumIm3dDrawCalls (0xB73708)
constexpr u32 kAddrVtx = 0xC4B950, kAddrIdx = 0xC4B954, kAddrDraw = 0xB73708, kAddrVerts = 0xC4D958, kAddrIndices = 0xC4B958;
// 2: arena del objeto de tarea: 0x20 de guarda + 0x10 de objeto (se usan 0xC) + 0x20 de guarda = 0x50
constexpr u32 kTaskObj = 0x20, kTaskObjBytes = 0x10, kTaskArena = 0x20 + 0x10 + 0x20;
static_assert(kTaskObj + kTaskObjBytes + 0x20 == kTaskArena && kTaskArena == 0x50, "arena de tarea");

// ------------------------------------------------------------------------------------------------ 0: GetPopcycleGroupPedModel
// Contenido de las ventanas: traduccion = (Rand(seedT, i) % 60) - 1 por dword (grupo de peatones en [-1, 58]); grupos de peatones = FillWords(seedP).
struct A0 {
    u32 family, group, slot, zone, seedT, seedP, eax, ecx, edx, stack;
};
inline A0 MakeA0(unsigned n) {
    A0 c{};
    c.family = n % 4;                                   // 0..2 validos, 3 extremos
    c.seedT = Rand(n, 1); c.seedP = Rand(n, 2);
    c.eax = Rand(n, 3); c.ecx = Rand(n, 4); c.edx = Rand(n, 5); c.stack = StackSeed(n);
    if (c.family != 3) {
        c.group = Rand(n, 6) % kNumGroups; c.slot = Rand(n, 7) % 21; c.zone = Rand(n, 8) % 3;
    } else {
        c.group = u32(i32(Rand(n, 6) % 41) - 4);       // [-4, 36]
        c.slot = u32(i32(Rand(n, 7) % 32) - 5);        // [-5, 26]
        c.zone = u32(i32(Rand(n, 8) % 7) - 2);         // [-2, 4]
    }
    // casos fijos en los bordes del arreglo
    switch (n) {
    case 0: c.group = 0; c.slot = 0; c.zone = 0; break;
    case 1: c.group = kNumGroups - 1; c.slot = 20; c.zone = 2; break;
    case 2: c.group = u32(-4); c.slot = u32(-5); c.zone = u32(-2); break;
    case 3: c.group = 36; c.slot = 26; c.zone = 4; break;
    case 4: c.group = 18; c.slot = 0; c.zone = 0; break;   // pandilla Ballas, como CreateAttackWave
    default: break;
    }
    return c;
}
inline std::string JsonA0(const A0& c) {
    Js j; j.U("family", c.family).U("group", c.group).U("slot", c.slot).U("zone", c.zone).U("seed_t", c.seedT).U("seed_p", c.seedP)
        .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ------------------------------------------------------------------------------------------------ 1: FlushWaterRenderBuffer
struct B1 {
    u32 family, nVtx, nIdx, draw, retT, retR, eax, ecx, edx, stack;
};
inline B1 MakeB1(unsigned n) {
    B1 c{};
    c.family = n % 8;
    c.draw = Rand(n, 11); c.eax = Rand(n, 3); c.ecx = Rand(n, 4); c.edx = Rand(n, 5); c.stack = StackSeed(n);
    c.retR = Rand(n, 12);
    switch (Rand(n, 13) % 8) {                          // resultado de RwIm3DTransform: AL = 0 con el resto no nulo cuenta (test eax, eax completo)
    case 0: c.retT = 0; break;
    case 1: c.retT = 1; break;
    case 2: c.retT = 0x100; break;
    case 3: c.retT = 0xFFFFFF00u; break;
    case 4: c.retT = 0x80000000u; break;
    case 5: c.retT = 0xC4D958u; break;
    default: c.retT = Rand(n, 14); break;
    }
    switch (c.family) {
    case 0: c.nVtx = 0; c.nIdx = Rand(n, 15) % 4096; break;                                   // sin vertices: no hay llamadas
    case 1: c.nVtx = 0; c.nIdx = Rand(n, 15); break;
    case 2: c.nVtx = 1 + Rand(n, 16) % 2048; c.nIdx = Rand(n, 15) % 4096; break;
    case 3: c.nVtx = 1 + Rand(n, 16) % 2048; c.nIdx = 0; break;
    case 4: c.nVtx = u32(-1 - i32(Rand(n, 16) % 100)); c.nIdx = Rand(n, 15) % 4096; break;   // negativos: distintos de cero
    case 5: c.nVtx = 0x80000000u + Rand(n, 16) % 4; c.nIdx = Rand(n, 15); break;
    case 6: c.nVtx = Rand(n, 16); c.nIdx = u32(-1 - i32(Rand(n, 15) % 100)); break;
    default: c.nVtx = 0x10000u + Rand(n, 16) % 4; c.nIdx = 0x10000u + Rand(n, 15) % 4; break;  // el bit 16: no es un contador de 16 bits
    }
    if (n % 16 == 13) c.draw = 0xFFFFFFFFu;              // la cuenta de dibujados da la vuelta
    if (n % 16 == 10) c.draw = 0x7FFFFFFFu;
    return c;
}
inline std::string JsonB1(const B1& c) {
    Js j; j.U("family", c.family).U("n_vtx", c.nVtx).U("n_idx", c.nIdx).U("draw", c.draw).U("ret_t", c.retT).U("ret_r", c.retR)
        .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ------------------------------------------------------------------------------------------------ 2: CTaskComplex destructor (cuerpo)
struct C2 {
    u32 family, sub, vtable, parent, tail, guardSeed, eax, edx, stack;   // sub: 0 nulo, 1 objeto A, 2 objeto B
};
inline C2 MakeC2(unsigned n) {
    C2 c{};
    c.family = n % 4;
    c.sub = c.family == 0 ? 0 : (c.family == 1 ? 1 : (c.family == 2 ? 2 : Rand(n, 21) % 3));
    c.vtable = Rand(n, 22) % 4 == 0 ? 0x86D4CCu : (Rand(n, 22) % 4 == 1 ? 0u : Rand(n, 23));
    c.parent = Rand(n, 24) % 3 == 0 ? 0u : Rand(n, 25);
    c.tail = Rand(n, 26);                               // dword +0xC (fuera del objeto de 0xC bytes)
    c.guardSeed = Rand(n, 27);
    c.eax = Rand(n, 3); c.edx = Rand(n, 5); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j; j.U("family", c.family).U("sub", c.sub).U("vtable", c.vtable).U("parent", c.parent).U("tail", c.tail).U("guard_seed", c.guardSeed)
        .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}
