#pragma once
// Fixtures del banco LeaveCarCtor71 (lote 71), 3 funciones x 16384 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexLeaveCar::CTaskComplexLeaveCar     0x63B8C0  thiscall, ECX = this, (CVehicle*, door, delay, sensible, force), `ret 14h`, EAX = this   (hook S, RH_ScopedInstall(Constructor))
//   1 CTaskComplexLeaveCar::~CTaskComplexLeaveCar    0x63B970  thiscall, ECX = this, sin argumentos, `ret`                                              (hook S, RH_ScopedInstall(Destructor))
//   2 CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie  0x62F1A0  thiscall, ECX = this, (CVehicle*, door, delay), `ret 0Ch`, EAX = this      (hook S, RH_ScopedInstall(Constructor))
//
// Reparto: familia = n % 16 (1024 casos por familia). Constructores: el objeto (0x34 bytes) se construye sobre arenas con relleno controlado (ruido, 0x00, 0xFF, 0xCD, 0xA5) y a 8 desplazamientos
// y se compara byte a byte (relleno incluido) salvo la palabra de la vtable, que se sustituye por el tipo (GetTaskType). Destructor: objeto construido al azar sobre ruido.
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

namespace Lc71Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 3;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La arena del objeto lleva 8 desplazamientos posibles.
constexpr std::size_t kGuard = 0x40, kShift = 8;
constexpr std::size_t kTaskWin = 0x34, kSubWin = 0x10, kVehWin = 0x680, kUtilWin = 0x1C, kOthWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin) + kShift, kSubArena = Arena(kSubWin), kVehArena = Arena(kVehWin), kUtilArena = Arena(kUtilWin), kOthArena = Arena(kOthWin);

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
// Relleno de una arena: modo 0..3 = ruido (4 semillas), 4 = 0x00, 5 = 0xFF, 6 = 0xCD, 7 = 0xA5
inline void FillMode(u8* dst, std::size_t bytes, u32 mode, u32 seed) {
    switch (mode) {
    case 4: std::memset(dst, 0x00, bytes); break;
    case 5: std::memset(dst, 0xFF, bytes); break;
    case 6: std::memset(dst, 0xCD, bytes); break;
    case 7: std::memset(dst, 0xA5, bytes); break;
    default: FillWords(dst, bytes, seed + mode * 7919u); break;
    }
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
inline constexpr std::array<u32, 14> kDoors{0u, 1u, 2u, 3u, 5u, 7u, 8u, 9u, 10u, 11u, 12u, 0xFFFFFFFFu, 0x80000000u, 0x100u};
inline constexpr std::array<u32, 10> kDelays{0u, 1u, 100u, 1000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFF9Cu, 5000u, 0x10000u};
inline constexpr std::array<u32, 6> kRawVeh{1u, 0xFFFFFFFFu, 0x80000000u, 0x12345678u, 0xCDCDCDCDu, 0x7FFFFFFFu};

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 shift{}, fillMode{}, fillSeed{};
    // constructores
    u32 vehKind{};       // 0 = nulo, 1 = arena del vehiculo, 2 = puntero crudo (no se desreferencia)
    u32 vehRaw{};
    u32 door{}, delay{}, sensible{}, force{};   // dwords de pila; el EXE lee solo el byte bajo de los dos booleanos
    // destructor
    u32 vptrKind{};      // 0 = vtable del EXE, 1 = ruido
    u32 subKind{};       // 0 = nula, 1 = subtarea de la arena (vtable falsa)
    u32 utilKind{};      // 0 = nula, 1 = arena de la utilidad, 2 = puntero crudo
    u32 utilRaw{};
    u32 flagsSet{}, numIn{}, vehFill{};
};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n; c.family = n % kFamilies;
    c.seed = Rand(n, 7 + fn * 1000u);
    c.eax = SeedReg(n + fn * 100000u, 1); c.edx = SeedReg(n + fn * 100000u, 2); c.stack = StackSeed(n + fn);
    c.shift = Rand(n, 3) % kShift;
    c.fillMode = Rand(n, 4) % 8; c.fillSeed = Rand(n, 5);
    if (fn == 1) {
        c.vptrKind = Rand(n, 10) % 2;
        c.subKind = Rand(n, 11) % 2;
        c.vehKind = Rand(n, 12) % 5 == 0 ? 0u : 1u;
        c.utilKind = Rand(n, 13) % 3;
        c.utilRaw = Rand(n, 14) | 1u;
        c.flagsSet = Rand(n, 15) % 4 == 0 ? Rand(n, 16) % 2 * 0xFFu : Rand(n, 17) & 0xFFu;
        c.numIn = Rand(n, 18) % 4 == 0 ? Rand(n, 19) % 2 * 0xFFu : Rand(n, 20) & 0xFFu;
        c.vehFill = Rand(n, 21);
        switch (c.family) {
        case 0:  c.vehKind = 0; break;
        case 1:  c.vehKind = 1; c.subKind = 0; c.utilKind = 0; break;
        case 2:  c.vehKind = 1; c.subKind = 1; c.utilKind = 1; break;
        case 3:  c.vehKind = 0; c.subKind = 1; c.utilKind = 1; break;
        case 4:  c.vehKind = 1; c.flagsSet = 0; c.numIn = 0; break;
        case 5:  c.vehKind = 1; c.flagsSet = 0xFF; c.numIn = 0xFF; break;
        case 6:  c.vehKind = 1; c.numIn = 1; break;
        case 7:  c.vehKind = 1; c.utilKind = 2; break;
        case 8:  c.vehKind = 1; c.vptrKind = 0; break;
        case 9:  c.vehKind = 1; c.vptrKind = 1; break;
        case 10: c.vehKind = 0; c.utilKind = 2; c.subKind = 0; break;
        case 11: c.vehKind = 1; c.vehFill = 0xFFFFFFFFu; c.numIn = 0xFF; break;
        default: break;
        }
        return c;
    }
    // constructores (0x63B8C0 y 0x62F1A0)
    c.vehKind = Rand(n, 30) % 5 == 0 ? 0u : Rand(n, 31) % 6 == 0 ? 2u : 1u;
    c.vehRaw = Pick(kRawVeh, n, 32);
    c.door = Rand(n, 33) % 4 == 0 ? Rand(n, 34) : Pick(kDoors, n, 35);
    c.delay = Rand(n, 36) % 3 == 0 ? Rand(n, 37) : Pick(kDelays, n, 38);
    c.sensible = Rand(n, 39) % 2; c.force = Rand(n, 40) % 2;
    switch (c.family) {
    case 0:  c.vehKind = 0; break;
    case 1:  c.vehKind = 1; c.fillMode = 4; break;
    case 2:  c.vehKind = 1; c.fillMode = 5; break;
    case 3:  c.vehKind = 1; c.fillMode = 6; break;
    case 4:  c.vehKind = 1; c.fillMode = 7; break;
    case 5:  c.vehKind = 2; break;
    case 6:  c.vehKind = 0; c.fillMode = 5; break;
    case 7:  c.vehKind = 0; c.fillMode = 4; break;
    case 8:  c.sensible = Rand(n, 41) % 2 | (Rand(n, 42) << 8); c.force = Rand(n, 43) % 2 | (Rand(n, 44) << 8); break;   // bytes altos basura: el EXE lee el byte bajo
    case 9:  c.sensible = 0; c.force = 1; break;
    case 10: c.sensible = 1; c.force = 0; break;
    case 11: c.sensible = 1; c.force = 1; break;
    case 12: c.sensible = 0; c.force = 0; break;
    case 13: c.door = 0; c.delay = 0; break;
    case 14: c.shift = 7; break;
    default: break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("shift", c.shift).U("fill_mode", c.fillMode).U("fill_seed", c.fillSeed);
    if (c.fn == 1) {
        j.U("vptr_kind", c.vptrKind).U("sub_kind", c.subKind).U("veh_kind", c.vehKind).U("util_kind", c.utilKind).U("util_raw", c.utilRaw).U("flags_set", c.flagsSet).U("num_in", c.numIn).U("veh_fill", c.vehFill);
    } else {
        j.U("veh_kind", c.vehKind).U("veh_raw", c.vehRaw).U("door", c.door).U("delay", c.delay).U("sensible", c.sensible).U("force", c.force);
    }
    return j.End();
}
}
