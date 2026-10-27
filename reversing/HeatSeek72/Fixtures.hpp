#pragma once
// Fixtures del banco HeatSeek72 (lote 72): CVehicle::ScanAndMarkTargetForHeatSeekingMissile (EXE 0x6E0400), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::ScanAndMarkTargetForHeatSeekingMissile  0x6E0400  thiscall, ECX = this (vehiculo), (CEntity* previo), `ret 4`, EAX = objetivo o nulo   (hook CVehicle / ScanAndMarkTargetForHeatSeekingMissile, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
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

namespace Hs72Fix {
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
constexpr std::size_t kVehWin = 0xA00, kTgtWin = 0x40, kOthWin = 0x40, kMatWin = 0x48;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kTgtArena = Arena(kTgtWin), kOthArena = Arena(kOthWin), kMatArena = Arena(kMatWin);

constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds
constexpr u32 kPlane = 4;             // VEHICLE_TYPE_PLANE

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 72072u); }
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

// ------------------------------------------------------------------------------------------------ valores notables (NaN solo canonicos)
inline constexpr std::array<u32, 2> kNan{0x7FC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
// Subtipos de vehiculo que no son avion (-1 = ignorar, 0..11 validos, y valores fuera de rango; 0x80000004 y 0x100000004 truncado comparan distinto de 4)
inline constexpr std::array<u32, 16> kSubOther{0xFFFFFFFFu, 0u, 1u, 2u, 3u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 0x80000004u, 0x00000104u, 0x40000004u};
// Instantes en los alrededores del umbral de 1000 ms (y de los pasos 0/1/2 de la bandera del estado del avion)
inline constexpr std::array<u32, 16> kTimesEdge{0u, 1u, 2u, 3u, 999u, 1000u, 1001u, 1002u, 1003u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFC17u, 0xFFFFFC18u, 0xFFFFFC19u};
inline constexpr std::array<u32, 10> kTimesLarge{1001u, 1002u, 1500u, 5000u, 60000u, 123456u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x12345678u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float "de mundo": mayormente moderado, a veces especial
inline u32 World(u32 c, u32 k, bool special) {
    const u32 r = Rand(c, k) % 16;
    if (special) {
        if (r < 3) return Pick(kTiny, c, k + 1);
        if (r < 5) return Pick(kInf, c, k + 1);
        if (r < 7) return Pick(kBig, c, k + 1);
        if (r < 9) return Pick(kNan, c, k + 1);
        if (r < 10) return B(1.0f);
        if (r < 11) return B(-1.0f);
        return RandFloat(c, k + 2, -20, 100);
    }
    if (r == 0) return B(0.0f);
    return RandFloat(c, k + 2, -4, 11);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 time{};
    u32 subType{};     // m_nVehicleSubType (+0x594)
    u32 pstate{};      // dword +0x9E4 del vehiculo (estado del avion: bit 31 = ultima vision, bits bajos = marca)
    u32 status{};      // byte +0x36 del vehiculo (bits 3..7 = m_nStatus)
    u32 vflags{}, tflags{};   // dword +0x1C del vehiculo y del objetivo
    u32 tgtKind{};     // 0 nulo, 1 entidad objetivo, 2 el propio vehiculo
    u32 entKind{};     // argumento `entity`: 0 nulo, 1 objetivo, 2 otra entidad, 3 puntero crudo
    u32 entRaw{};
    u32 tgtMat{};      // el objetivo tiene matriz (posicion en +0x30) o solo posicion compacta (+4)
    u32 los{}, losHi{};   // resultado de la vision (AL) y bits altos de EAX
    u32 mutSelf{}, mutTgt{};   // xor que aplica el falso de la vision a los dwords +0x1C
    u32 vpos[3]{}, vdir[3]{}, tpos[3]{};
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const bool special = (n / 16) % 4 == 3;
    c.time = Rand(n, 20) % 2 == 0 ? Pick(kTimesEdge, n, 21) : Rand(n, 22) % 2 ? Pick(kTimesLarge, n, 23) : Rand(n, 24);
    c.subType = Rand(n, 25) % 2 ? kPlane : Pick(kSubOther, n, 26);
    c.pstate = Rand(n, 27) % 3 == 0 ? (Rand(n, 28) & 0x80000000u) : Rand(n, 29);
    c.status = Rand(n, 30) % 2 ? (Rand(n, 31) & 7u) : (Rand(n, 32) & 0xFFu);
    c.vflags = Rand(n, 33); c.tflags = Rand(n, 34);
    c.tgtKind = Rand(n, 35) % 10 == 0 ? 0 : 1;
    c.entKind = Rand(n, 36) % 4;
    c.entRaw = Rand(n, 37) | 1u;
    c.tgtMat = Rand(n, 38) % 2;
    c.los = Rand(n, 39) % 2;
    c.losHi = Rand(n, 40) & 0xFFFFFFu;
    c.mutSelf = Rand(n, 41) % 3 == 0 ? Rand(n, 42) : 0u;
    c.mutTgt = Rand(n, 43) % 3 == 0 ? Rand(n, 44) : 0u;
    for (int i = 0; i < 3; ++i) {
        c.vpos[i] = World(n, 50 + i, special);
        c.vdir[i] = World(n, 60 + i, special);
        c.tpos[i] = World(n, 70 + i, special);
    }
    const bool planeFamily = true;
    (void)planeFamily;
    switch (c.family) {
    case 0:   // sin objetivo, no avion: la vision no se calcula; se borra la mira
        c.tgtKind = 0; c.subType = Pick(kSubOther, n, 26);
        break;
    case 1:   // sin objetivo, avion: decide el bit 31 guardado
        c.tgtKind = 0; c.subType = kPlane; c.pstate = Rand(n, 27) % 2 ? (Rand(n, 28) & 0x80000000u) : Rand(n, 29);
        break;
    case 2:   // no avion, objetivo, tiempo grande, vision libre, vehiculo del jugador: se marca
        c.tgtKind = 1; c.subType = Pick(kSubOther, n, 26); c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : 1001u + Rand(n, 24) % 5000u; c.los = 1; c.status &= 7u;
        break;
    case 3:   // no avion, objetivo, tiempo grande, vision tapada
        c.tgtKind = 1; c.subType = Pick(kSubOther, n, 26); c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : 1001u + Rand(n, 24) % 5000u; c.los = 0;
        break;
    case 4:   // no avion, objetivo, tiempo <= 1000: no se calcula la vision (visible = verdadero)
        c.tgtKind = 1; c.subType = Pick(kSubOther, n, 26); c.time = Rand(n, 20) % 3 == 0 ? Pick(kTimesEdge, n, 21) % 1001u : Rand(n, 24) % 1001u;
        break;
    case 5:   // avion, bits bajos del estado distintos de cero (sin holgura), umbral de 1000
        c.tgtKind = 1; c.subType = kPlane; c.pstate = (Rand(n, 27) & 0x80000000u) | ((Rand(n, 28) & 0x7FFFFFFFu) | (Rand(n, 29) % 3 == 0 ? 1u : 0u)); if ((c.pstate & 0x7FFFFFFFu) == 0) c.pstate |= 1u;
        c.time = Pick(kTimesEdge, n, 21);
        break;
    case 6:   // avion, bits bajos a cero (holgura de 1 ms): el umbral se desplaza
        c.tgtKind = 1; c.subType = kPlane; c.pstate = Rand(n, 27) & 0x80000000u; c.time = Pick(kTimesEdge, n, 21);
        break;
    case 7:   // avion, vision calculada (tiempo grande): se guarda el estado (tiempo > 1 y vision en el bit 31)
        c.tgtKind = 1; c.subType = kPlane; c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : (Rand(n, 20) % 3 == 0 ? 0u : 1001u + Rand(n, 24) % 5000u);
        break;
    case 8:   // avion con tiempo <= 1000: no se calcula, manda el bit 31 guardado
        c.tgtKind = 1; c.subType = kPlane; c.time = Rand(n, 20) % 3 == 0 ? Pick(kTimesEdge, n, 21) % 1001u : Rand(n, 24) % 1001u;
        if (c.time == 0 && (c.pstate & 0x7FFFFFFFu) == 0) c.time = 1;   // con tiempo 0 y estado bajo cero la holgura si lo calcula (familia 6)
        break;
    case 9:   // vehiculo que no esta en estado jugador: solo se borra la mira y se devuelve el objetivo
        c.tgtKind = 1; c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : 1001u + Rand(n, 24) % 5000u; c.los = 1; c.status = (Rand(n, 31) & 0xF8u) | (Rand(n, 32) & 7u); if ((c.status & 0xF8u) == 0) c.status |= 8u << (Rand(n, 33) % 5);
        break;
    case 10:  // el objetivo es el propio vehiculo (alias)
        c.tgtKind = 2; c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : 1001u + Rand(n, 24) % 5000u;
        break;
    case 11:  // el falso de la vision cambia los dwords de banderas (incluido el bit 0): solo el bit 0 se restaura
        c.tgtKind = Rand(n, 35) % 8 == 0 ? 2 : 1; c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : 1001u + Rand(n, 24) % 5000u;
        c.mutSelf = Rand(n, 45) | (Rand(n, 46) % 2); c.mutTgt = Rand(n, 47) | (Rand(n, 48) % 2);
        break;
    case 12:  // valores especiales en las posiciones y direcciones, vision calculada
        c.tgtKind = 1; c.time = Rand(n, 20) % 2 ? Pick(kTimesLarge, n, 23) : 1001u + Rand(n, 24) % 5000u; c.los = Rand(n, 39) % 4 != 0; c.status &= 7u;
        for (int i = 0; i < 3; ++i) { c.vpos[i] = World(n, 80 + i, true); c.vdir[i] = World(n, 90 + i, true); c.tpos[i] = World(n, 100 + i, true); }
        break;
    case 13:  // objetivo sin matriz (posicion compacta en +4)
        c.tgtMat = 0; c.tgtKind = 1;
        break;
    default:  // 14 y 15: mezcla libre
        c.tgtKind = Rand(n, 35) % 10 == 0 ? 0 : (Rand(n, 35) % 10 == 1 ? 2 : 1);
        break;
    }
    if (c.family != 13 && c.family != 14 && c.family != 15) { /* tgtMat libre */ }
    if (c.tgtKind == 2) c.tgtMat = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("time", c.time).U("sub_type", c.subType).U("pstate", c.pstate).U("status", c.status).U("vflags", c.vflags).U("tflags", c.tflags);
    j.U("tgt_kind", c.tgtKind).U("ent_kind", c.entKind).U("ent_raw", c.entRaw).U("tgt_mat", c.tgtMat).U("los", c.los).U("los_hi", c.losHi).U("mut_self", c.mutSelf).U("mut_tgt", c.mutTgt);
    j.U("vpos_x", c.vpos[0]).U("vpos_y", c.vpos[1]).U("vpos_z", c.vpos[2]).U("vdir_x", c.vdir[0]).U("vdir_y", c.vdir[1]).U("vdir_z", c.vdir[2]);
    j.U("tpos_x", c.tpos[0]).U("tpos_y", c.tpos[1]).U("tpos_z", c.tpos[2]);
    return j.End();
}
}
