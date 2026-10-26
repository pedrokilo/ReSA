#pragma once
// Fixtures del banco LeaveCarSub70 (lote 70): CTaskComplexLeaveCar::CreateSubTask(eTaskType, CPed*) (NO virtual, EXE 0x641530), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexLeaveCar::CreateSubTask  0x641530  thiscall, ECX = this (tarea), (eTaskType, CPed*) en la pila, `ret 8`, EAX = tarea nueva o nulo   (hook CTaskComplexLeaveCar / CreateSubTask, tipo S)
//
// Reparto: familia = n % 16 (1024 casos por familia). Los floats viajan como patrones de 32 bits sin signo.
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

namespace Ls70Fix {
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
constexpr std::size_t kTaskWin = 0x34, kSmallWin = 0x10, kPedWin = 0x10, kAllocWin = 0x70;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kSmallArena = Arena(kSmallWin), kPedArena = Arena(kPedWin), kAllocArena = Arena(kAllocWin);
// Tamanos que reserva el EXE con CTask::operator new (0x61A5A0), uno por arena
constexpr std::array<u32, 9> kAllocSizes{0xCu, 0x10u, 0x14u, 0x18u, 0x1Cu, 0x20u, 0x24u, 0x28u, 0x70u};
constexpr unsigned kAllocArenas = 9;

// Tipos de tarea que decide el EXE (eTaskType)
constexpr u32 kPause = 202, kGetUp = 206, kDie = 212, kLeaveBoat = 716, kDriveTimed = 719, kCloseDoorOut = 806, kWaitSlow = 809, kWaitDoor = 810, kGetOut = 813, kJumpOut = 814, kForcePedOut = 815,
              kSetPedOut = 816, kSlowDragged = 823;
constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds (region vigilada)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 70070u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

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

inline constexpr std::array<u32, 13> kTypesMain{kPause, kGetUp, kDie, kLeaveBoat, kDriveTimed, kCloseDoorOut, kWaitSlow, kWaitDoor, kGetOut, kJumpOut, kForcePedOut, kSetPedOut, kSlowDragged};
// Vecinos de los tipos que decide el EXE y valores extremos (el EXE compara con signo: 0x32A, 0x2CC, 0x330 son los puntos de corte del arbol)
inline constexpr std::array<u32, 52> kTypesOther{
    0u, 1u, 100u, 201u, 203u, 204u, 205u, 207u, 208u, 211u, 213u, 214u, 215u, 715u, 717u, 718u, 720u, 721u, 805u, 807u, 808u, 811u, 812u, 814u + 3u, 817u, 818u, 822u, 824u, 825u, 0x2CBu, 0x2CDu, 0x329u + 3u,
    0x32Bu, 0x32Cu, 0x331u, 0x336u, 0x338u, 0x1387u, 0x100000u, 0x800000CAu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x7FFFFFFEu, 0x100000CAu, 0xFFFFFF36u, 0x000102CCu, 0xFFFFFCD6u, 0x1000032Au, 0x00010330u};
inline constexpr std::array<u32, 10> kBytes{0u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x55u, 0x10u, 0xFEu, 0x01u};
inline constexpr std::array<u32, 14> kDoors{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0xFFFFFF00u};
inline constexpr std::array<u32, 12> kDelays{0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 1000u, 123456u, 0x12345678u, 0xFFFFFF00u, 0xDEADBEEFu, 60000u, 0x01000000u};
// Patrones de float que se copian como dword (NaN con carga util, infinitos, subnormales, cero con signo...): si la DLL los pasa por la x87 o con conversiones, se alteran
inline constexpr std::array<u32, 20> kFloatBits{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u,
    0x7FFFFFFFu, 0x3F800000u, 0xBF800000u, 0x40800000u, 0x3F19999Au, 0x00800000u, 0x7F7FFFFFu};
// pointer: 0 = nulo, 1 = arena, 2 = crudo
inline u32 PtrKind(u32 c, u32 k) { const u32 r = Rand(c, k) % 8; return r < 2 ? 0 : r < 6 ? 1 : 2; }

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 type{}, allocNull{}, isPlayerRet{};
    u32 vehKind{}, vehRaw{}, lineKind{}, lineRaw{};
    u32 door{}, delay{};
    u32 sensible{}, force{}, die{};   // bytes (+0x18, +0x19, +0x1A), cualquier valor de 8 bits
    u32 doorFlags{}, numGetting{}, dieAnim{}, blend{}, speed{}, inAir{};
    u32 parentRaw{}, subRaw{};
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.allocNull = Rand(n, 3) % 7 == 0;
    c.isPlayerRet = Rand(n, 4) % 3 == 0 ? Rand(n, 5) : (Rand(n, 6) % 2 ? 1u : 0u) | (Rand(n, 8) % 2 ? 0 : (Rand(n, 9) & 0xFFFFFF00u));
    c.vehKind = PtrKind(n, 10); c.vehRaw = Rand(n, 11) | 1u;
    c.lineKind = PtrKind(n, 12); c.lineRaw = Rand(n, 13) | 1u;
    c.door = Rand(n, 14) % 2 ? Pick(kDoors, n, 15) : Rand(n, 16);
    c.delay = Rand(n, 17) % 2 ? Pick(kDelays, n, 18) : Rand(n, 19);
    c.sensible = Rand(n, 20) % 2 ? (Rand(n, 21) % 2) : Pick(kBytes, n, 22);
    c.force = Rand(n, 23) % 2 ? (Rand(n, 24) % 2) : Pick(kBytes, n, 25);
    c.die = Rand(n, 26) % 2 ? (Rand(n, 27) % 2) : Pick(kBytes, n, 28);
    c.doorFlags = Rand(n, 29) & 0xFF; c.numGetting = Rand(n, 30) & 0xFF;
    c.dieAnim = Rand(n, 31) % 3 == 0 ? Pick(kDoors, n, 32) : Rand(n, 33);
    c.blend = Rand(n, 34) % 2 ? Pick(kFloatBits, n, 35) : Rand(n, 36);
    c.speed = Rand(n, 37) % 2 ? Pick(kFloatBits, n, 38) : Rand(n, 39);
    c.inAir = Rand(n, 40) & 0xFF;
    c.parentRaw = Rand(n, 41); c.subRaw = Rand(n, 42);
    switch (c.family) {
    case 0: c.type = kPause; break;
    case 1: c.type = kGetUp; break;
    case 2: c.type = kDie; break;
    case 3: c.type = kLeaveBoat; break;
    case 4: c.type = kDriveTimed; break;
    case 5: c.type = kCloseDoorOut; break;
    case 6: c.type = kWaitSlow; if (c.die == 0) c.die = Pick(kBytes, n, 50) | 1u; break;   // m_bDie distinto de cero
    case 7: c.type = kWaitSlow; c.die = 0; if (c.sensible == 0) c.sensible = Pick(kBytes, n, 51) | 1u; break;   // sensata: decide IsPlayer
    case 8: c.type = kWaitSlow; c.die = 0; c.sensible = 0; break;
    case 9: c.type = kWaitDoor; break;
    case 10: c.type = kGetOut; break;
    case 11: c.type = kJumpOut; break;
    case 12: c.type = kForcePedOut; break;
    case 13: c.type = kSetPedOut; break;
    case 14: c.type = kSlowDragged; break;
    default: c.type = Rand(n, 52) % 4 == 0 ? Pick(kTypesMain, n, 53) : Pick(kTypesOther, n, 54); if (Rand(n, 55) % 5 == 0) c.type = Rand(n, 56); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("type", c.type).U("alloc_null", c.allocNull).U("is_player_ret", c.isPlayerRet);
    j.U("veh_kind", c.vehKind).U("veh_raw", c.vehRaw).U("line_kind", c.lineKind).U("line_raw", c.lineRaw);
    j.U("door", c.door).U("delay", c.delay).U("sensible", c.sensible).U("force", c.force).U("die", c.die);
    j.U("door_flags", c.doorFlags).U("num_getting", c.numGetting).U("die_anim", c.dieAnim).U("blend", c.blend).U("speed", c.speed).U("in_air", c.inAir);
    j.U("parent_raw", c.parentRaw).U("sub_raw", c.subRaw);
    return j.End();
}
}
