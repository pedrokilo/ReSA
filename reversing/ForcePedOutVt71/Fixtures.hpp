#pragma once
// Fixtures del banco ForcePedOutVt71 (lote 71, virtuales de CTaskSimpleCarForcePedOut). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 Destructor            0x647790  thiscall, ECX = this, `ret`                         (hook S: RH_ScopedInstall)
//   1 Clone                 0x649EE0  thiscall, ECX = this, `ret`, EAX = tarea nueva o nulo (hook V, ranura 1 de la vtable 0x86EE94)
//   2 MakeAbortable         0x647780  thiscall, (ped, prioridad, evento), `ret 0Ch`, AL = 0  (hook V, ranura 6)
//   3 ProcessPed            0x6477F0  thiscall, (ped), `ret 4`, AL = terminada            (hook V, ranura 7)
//   4 DeletingDestructor    0x64BF50  thiscall, (banderas), `ret 4`, EAX = this           (ranura 0; sin hook: el de la DLL lo genera el compilador)
//
// 16384 casos por funcion; familia = n % 16 (1024 casos por familia).
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

namespace Vt71Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 5;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x10, kVehWin = 0x500, kPedWin = 0x10, kAllocWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kVehArena = Arena(kVehWin), kPedArena = Arena(kPedWin), kAllocArena = Arena(kAllocWin);
constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds (region vigilada)
// Desplazamientos del vehiculo que lee ProcessPed
constexpr std::size_t kOffDriver = 0x460, kOffPassengers = 0x464, kOffGettingOut = 0x487;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 71171u); }
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

inline constexpr std::array<u32, 20> kDoors{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 18u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0xFFFFFF00u, 0xFFu, 0xDEADBEEFu};
inline constexpr std::array<u32, 12> kRaw{1u, 2u, 0x10u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00400000u, 0x00B74490u, 0xFFFFFFF0u, 0x12345679u, 0xDEADBEEFu, 0x00000004u};
// Banderas de ComputeOppositeDoorFlag (solo AL lo lee el EXE): valores reales, nulos y con basura en los bits altos
inline constexpr std::array<u32, 14> kOpp{0u, 1u, 2u, 4u, 5u, 8u, 10u, 0xFFu, 0x100u, 0xFFFFFF00u, 0x80000001u, 0x12345604u, 0xFFFFFFFFu, 0x7Fu};
// Valores de retorno de ComputePassengerIndexFromCarDoor: reales (-1, 0, 1, 2), cero, negativos y hasta 12 asientos
inline constexpr std::array<u32, 14> kSeats{0xFFFFFFFFu, 0u, 1u, 2u, 3u, 5u, 8u, 12u, 0x80000000u, 0xFFFFFFF0u, 4u, 6u, 7u, 9u};
inline constexpr std::array<u32, 12> kFlagsArg{0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000001u, 0x55555555u, 0xAAAAAAAAu};
inline constexpr std::array<u32, 12> kPrio{0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 100u, 0x100u, 0xDEADBEEFu};

// puntero: 0 = nulo, 1 = arena, 2 = crudo
inline u32 PtrKind(u32 c, u32 k) { const u32 r = Rand(c, k) % 8; return r < 2 ? 0 : r < 6 ? 1 : 2; }

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, fn{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 vehKind{}, vehRaw{}, vehOff{}, pedKind{}, pedRaw{}, evtKind{}, evtRaw{};
    u32 door{}, prefill{}, prio{}, delFlags{};
    u32 allocNull{}, isDriverRet{}, oppRet{}, seatsRet{}, posRet{};
    u32 driver{}, passMask{}, passVals{}, gettingOut{};
};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.n = n; c.fn = fn;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7 + fn * 1000u);
    const u32 r = c.seed;   // base de los demas aleatorios
    auto R = [&](u32 k) { return Rand(r, k); };
    c.eax = SeedReg(r, 1); c.edx = SeedReg(r, 2); c.stack = StackSeed(n);
    c.vehKind = PtrKind(r, 10); c.vehRaw = R(11) % 2 ? Pick(kRaw, r, 12) : (R(13) | 1u); c.vehOff = R(15) % 3;
    c.pedKind = PtrKind(r, 16); c.pedRaw = R(17) % 2 ? Pick(kRaw, r, 18) : (R(19) | 1u);
    c.evtKind = PtrKind(r, 20); c.evtRaw = R(21) % 2 ? Pick(kRaw, r, 22) : (R(23) | 1u);
    c.door = R(24) % 2 ? Pick(kDoors, r, 25) : R(26);
    { const u32 q = R(27) % 8; c.prefill = q >= 4 && q <= 6 ? q - 3u : 0u; }   // 5/8 semilla, 1/8 cada uno de 0xCD / ceros / 0xFF
    c.prio = R(28) % 2 ? Pick(kPrio, r, 29) : R(30);
    c.delFlags = R(31) % 2 ? Pick(kFlagsArg, r, 32) : R(33);
    c.delFlags &= ~6u;   // el destructor de la DLL es el «vector deleting destructor» del compilador: el bit 1 lo trata como array y el bit 2 usa ::operator delete global (ningun llamador los pasa: solo 0 o 1; el EXE los ignora)
    c.allocNull = R(34) % 7 == 0;
    c.isDriverRet = R(35) % 3 == 0 ? R(36) : ((R(37) % 2 ? 1u : 0u) | (R(38) % 2 ? 0 : (R(39) & 0xFFFFFF00u)));
    c.oppRet = R(40) % 2 ? Pick(kOpp, r, 41) : R(42);
    c.seatsRet = R(43) % 3 ? Pick(kSeats, r, 44) : (R(45) % 4);
    c.posRet = R(46);
    c.driver = R(47) % 2 ? 0u : (R(48) | 1u);
    c.passMask = R(49) & 0xFFu; c.passVals = R(50);
    c.gettingOut = R(51) % 3 ? Pick(kOpp, r, 52) & 0xFFu : R(53) & 0xFFu;
    if (fn == 3) {
        // ProcessPed: el vehiculo nunca es crudo (el EXE desreferencia el vehiculo tras IsDriver)
        if (c.vehKind == 2) c.vehKind = 1;
        const u32 low = c.oppRet & 0xFFu;
        switch (c.family) {
        case 0: c.vehKind = 0; break;
        case 1: case 2: c.vehKind = 1; c.isDriverRet = (R(60) % 2) ? 1u : (R(61) | 1u); break;
        case 3: case 4: case 5: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.gettingOut = ~low & R(62) & 0xFFu; break;
        case 6: case 7: case 8: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.oppRet = (c.oppRet & ~0xFFu) | (R(63) | 1u) & 0xFFu; c.gettingOut = (c.oppRet & 0xFFu) | (R(64) & 0xFFu); c.driver = R(65) | 1u; break;
        case 9: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.oppRet = (c.oppRet & ~0xFFu) | (R(63) | 1u) & 0xFFu; c.gettingOut = (c.oppRet & 0xFFu) | (R(64) & 0xFFu); c.driver = 0; c.seatsRet = R(66) % 2 ? 0u : (0x80000000u | R(67)); break;
        case 10: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.oppRet = (c.oppRet & ~0xFFu) | (R(63) | 1u) & 0xFFu; c.gettingOut = (c.oppRet & 0xFFu) | (R(64) & 0xFFu); c.driver = 0; c.seatsRet = 1 + R(68) % 2; c.passMask = 0; break;
        case 11: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.oppRet = (c.oppRet & ~0xFFu) | (R(63) | 1u) & 0xFFu; c.gettingOut = (c.oppRet & 0xFFu) | (R(64) & 0xFFu); c.driver = 0; c.seatsRet = 1 + R(68) % 2; c.passMask = 1; break;
        case 12: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.oppRet = (c.oppRet & ~0xFFu) | (R(63) | 1u) & 0xFFu; c.gettingOut = (c.oppRet & 0xFFu) | (R(64) & 0xFFu); c.driver = 0; c.seatsRet = 2; c.passMask = 2; break;
        case 13: c.vehKind = 1; c.isDriverRet &= 0xFFFFFF00u; c.oppRet = (c.oppRet & ~0xFFu) | (R(63) | 1u) & 0xFFu; c.gettingOut = (c.oppRet & 0xFFu) | (R(64) & 0xFFu); c.driver = 0; c.seatsRet = 3 + R(68) % 10; c.passMask = R(69) % 2 ? 0u : (1u << (R(70) % 8)); break;
        default: break;   // 14, 15: todo aleatorio
        }
        if (c.vehKind == 0 && c.family != 0) c.vehKind = 1;
        if (c.family == 0) c.vehKind = 0;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("fn", c.fn).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("veh_kind", c.vehKind).U("veh_raw", c.vehRaw).U("veh_off", c.vehOff * 4).U("ped_kind", c.pedKind).U("ped_raw", c.pedRaw).U("evt_kind", c.evtKind).U("evt_raw", c.evtRaw);
    j.U("door", c.door).U("prefill", c.prefill).U("prio", c.prio).U("del_flags", c.delFlags).U("alloc_null", c.allocNull);
    j.U("is_driver_ret", c.isDriverRet).U("opp_ret", c.oppRet).U("seats_ret", c.seatsRet).U("pos_ret", c.posRet);
    j.U("driver", c.driver).U("pass_mask", c.passMask).U("pass_vals", c.passVals).U("getting_out", c.gettingOut);
    return j.End();
}
}
