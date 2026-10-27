#pragma once
// Fixtures del banco LeaveCarCore71 (lote 71): CTaskComplexLeaveCar::ControlSubTask (virtual, vtable 0x86E828 ranura 10, EXE 0x6421B0), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexLeaveCar::ControlSubTask  0x6421B0  thiscall, ECX = this (tarea), (CPed*), `ret 4`, EAX = subtarea (o nueva, o nulo)   (hook CTaskComplexLeaveCar / ControlSubTask, tipo V)
//
// El tipo de la subtarea se pide por la vtable falsa de la subtarea EN CADA comprobacion (el EXE llama a GetTaskType hasta cinco veces): la secuencia de tipos del caso devuelve un valor
// distinto por llamada, asi que una version que guardara el tipo en una variable difiere. Reparto: familia = n % 16 (1024 casos por familia).
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
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTaskWin = 0x34, kSubWin = 0x10, kPedWin = 0x5A0, kVehWin = 0x680, kOthWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kSubArena = Arena(kSubWin), kPedArena = Arena(kPedWin), kVehArena = Arena(kVehWin), kOthArena = Arena(kOthWin);

// Tipos de subtarea que distingue el EXE
constexpr u32 kWaitSlow = 0x329, kJumpOut = 0x32E, kSetOut = 0x330, kGetUp = 0xCE, kDie = 0xD4;
inline constexpr std::array<u32, 5> kKey{kSetOut, kJumpOut, kGetUp, kDie, kWaitSlow};
inline constexpr std::array<u32, 4> kFour{kSetOut, kJumpOut, kGetUp, kDie};
inline constexpr std::array<u32, 14> kOther{0u, 1u, 0xCAu, 0xCFu, 0x2CFu, 0x326u, 0x32Au, 0x32Du, 0x32Fu, 0x337u, 0x516u, 0xFFFFFFFFu, 0x80000000u, 0x32Cu};
inline constexpr std::array<u32, 3> kThree{kWaitSlow, 0x32Du, 0x516u};
inline constexpr std::array<u32, 6> kRawVeh{1u, 0xFFFFFFFFu, 0x80000000u, 0x12345678u, 0xCDCDCDCDu, 0x7FFFFFFFu};

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
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

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
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 vehKind{};          // m_pTargetVehicle: 0 = nulo, 1 = arena del vehiculo, 2 = puntero crudo (no se desreferencia)
    u32 vehRaw{};
    u32 sensible{}, force{}, die{};   // bytes de la tarea (+0x18, +0x19, +0x1A); el EXE solo lee +0x18
    u32 inVeh{};            // bit 8 de +0x46C del peaton (bInVehicle)
    u32 types[6]{};         // tipo que devuelve la subtarea en la llamada i a GetTaskType (la ultima se repite)
    u32 abortRet{};         // MakeAbortable de la subtarea
    u32 createNull{};       // CreateSubTask devuelve nulo
    u32 pedBits{};          // palabra +0x46C del peaton (se fuerza el bit 8 segun inVeh)
    u32 pedBits2{};         // palabra +0x470
};

inline u32 KeyOrOther(u32 n, u32 k) { return Rand(n, k) % 4 == 0 ? Pick(kOther, n, k + 1) : Pick(kKey, n, k + 2); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.vehKind = Rand(n, 10) % 8 == 0 ? 2u : 1u;
    c.vehRaw = Rand(n, 11) | 1u;
    c.sensible = Rand(n, 12) % 2; c.force = Rand(n, 13) % 2; c.die = Rand(n, 14) % 2;
    c.inVeh = Rand(n, 15) % 2;
    for (int i = 0; i < 6; ++i) c.types[i] = KeyOrOther(n, 20 + 3 * i);
    c.abortRet = Rand(n, 40) % 2; c.createNull = Rand(n, 41) % 4 == 0;
    c.pedBits = Rand(n, 42); c.pedBits2 = Rand(n, 43);

    switch (c.family) {
    case 0:   // sin vehiculo: devuelve nulo sin llamar a nadie
        c.vehKind = 0; break;
    case 1:   // en el vehiculo, salida sensata: devuelve la subtarea sin llamar a nadie
        c.vehKind = Rand(n, 50) % 8 ? 1u : 2u; c.inVeh = 1; c.sensible = 1; break;
    case 2:   // en el vehiculo, no sensata, espera a frenar: MakeAbortable(ped, 0, 0)
        c.inVeh = 1; c.sensible = 0; c.types[0] = kWaitSlow; break;
    case 3:   // en el vehiculo, no sensata, otro tipo: nada
        c.inVeh = 1; c.sensible = 0; c.types[0] = Rand(n, 51) % 2 ? Pick(kOther, n, 52) : Pick(kFour, n, 53); break;
    case 4:   // fuera del vehiculo, primer tipo 0x330: pasa a la comprobacion sensata
        c.inVeh = 0; c.types[0] = kSetOut; break;
    case 5:   // fuera, 0x32E en la segunda comprobacion
        c.inVeh = 0; c.types[0] = Pick(kOther, n, 54); c.types[1] = kJumpOut; break;
    case 6:   // fuera, 0xCE en la tercera
        c.inVeh = 0; c.types[0] = Pick(kOther, n, 55); c.types[1] = Pick(kOther, n, 56); c.types[2] = kGetUp; break;
    case 7:   // fuera, 0xD4 en la cuarta
        c.inVeh = 0; c.types[0] = Pick(kOther, n, 57); c.types[1] = Pick(kOther, n, 58); c.types[2] = Pick(kOther, n, 59); c.types[3] = kDie; break;
    case 8:   // fuera, ninguno de los cuatro: CreateSubTask(0x330)
        c.inVeh = 0; c.createNull = 0;
        for (int i = 0; i < 4; ++i) c.types[i] = Rand(n, 60 + i) % 3 == 0 ? Pick(kThree, n, 64 + i) : Pick(kOther, n, 68 + i);
        break;
    case 9:   // igual que 8 con CreateSubTask nulo (agotado el pool)
        c.inVeh = 0; c.createNull = 1;
        for (int i = 0; i < 4; ++i) c.types[i] = Pick(kOther, n, 72 + i);
        break;
    case 10:  // fuera, tipo 0x330 o 0x32E y luego 0x329 con salida no sensata
        c.inVeh = 0; c.sensible = 0; c.types[0] = Rand(n, 76) % 2 ? kSetOut : kJumpOut; c.types[1] = kWaitSlow; c.types[2] = kWaitSlow; break;
    case 11:  // fuera, la secuencia importa: un tipo guardado en una variable difiere
        c.inVeh = 0; c.sensible = 0; c.types[0] = kSetOut; c.types[1] = kWaitSlow; c.types[2] = kWaitSlow; c.types[3] = kWaitSlow; break;
    case 12:  // fuera, dos tipos clave y despues 0x329 con salida no sensata (el EXE vuelve a preguntar)
        c.inVeh = 0; c.sensible = 0; c.types[0] = Pick(kFour, n, 77); c.types[1] = Pick(kFour, n, 78);
        c.types[2] = kWaitSlow; c.types[3] = kWaitSlow; c.types[4] = kWaitSlow; break;
    case 13:  // palabras +0x46C y +0x470 con todos los bits (solo el bit 8 cuenta)
        c.pedBits = Rand(n, 79) % 2 ? 0xFFFFFFFFu : 0u; c.pedBits2 = Rand(n, 80) % 2 ? 0xFFFFFFFFu : 0u; break;
    case 14:  // mezcla, a veces sin vehiculo
        c.vehKind = Rand(n, 81) % 6 == 0 ? 0u : c.vehKind; break;
    default:  // 15: mezcla con vehiculo crudo
        c.vehKind = Rand(n, 82) % 2 ? 2u : 1u; c.vehRaw = Pick(kRawVeh, n, 83); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("veh_kind", c.vehKind).U("veh_raw", c.vehRaw).U("sensible", c.sensible).U("force", c.force).U("die", c.die).U("in_veh", c.inVeh);
    for (int i = 0; i < 6; ++i) j.U((std::string("type") + std::to_string(i)).c_str(), c.types[i]);
    j.U("abort_ret", c.abortRet).U("create_null", c.createNull).U("ped_bits", c.pedBits).U("ped_bits2", c.pedBits2);
    return j.End();
}
}
