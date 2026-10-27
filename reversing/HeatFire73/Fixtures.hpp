#pragma once
// Fixtures del banco HeatFire73 (lote 73): CVehicle::FireHeatSeakingMissile (EXE 0x6E05C0), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::FireHeatSeakingMissile  0x6E05C0  thiscall, ECX = this (vehiculo), (CEntity* objetivo, eOrdnanceType tipo, bool arg2), `ret 0Ch`   (hook CVehicle / FireHeatSeakingMissile, tipo S)
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

namespace Hf73Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del vehiculo mide sizeof(CPlane) = 0xA04 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0xA04, kTgtWin = 0x40, kOthWin = 0x40, kMatWin = 0x48, kPedWin = 0x5B0;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kTgtArena = Arena(kTgtWin), kOthArena = Arena(kOthWin), kMatArena = Arena(kMatWin), kPedArena = Arena(kPedWin);

constexpr u32 kTimeAddr = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds
constexpr u32 kStepAddr = 0xB7CB5C;   // CTimer::ms_fTimeStep
constexpr u32 kPads0 = 0xB73458;      // CPad::Pads[0]

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 73073u); }
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

// ------------------------------------------------------------------------------------------------ valores notables
// NaN: canonicos (quiet con signo +/-), NO canonicos (carga distinta, sNaN con y sin signo, carga maxima, carga minima) e infinitos
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};   // +-0, desnormales y el menor normal
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
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
// Un float: modo 0 moderado, 1 pequeno (productos que caen en el rango desnormal), 2 grande (desbordamiento), 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla de todo
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 11);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 time{};          // CTimer::m_snTimeInMilliseconds
    u32 step{};          // CTimer::ms_fTimeStep (bits)
    u32 type{};          // eOrdnanceType (dword)
    u32 arg2{};          // dword; el EXE lee solo el byte bajo (`mov al,[esp+3Ch]; test al,al`)
    u32 t1{}, t2{};      // +0x4FC, +0x500
    u32 rate{};          // retorno de GetPlaneOrdnanceRateOfFire
    u32 tgtKind{};       // argumento objetivo: 0 nulo, 1 entidad objetivo, 2 otra entidad, 3 puntero crudo
    u32 entRaw{};
    u32 hasMat{};        // el vehiculo tiene matriz (siempre 1 salvo si el intervalo corta el disparo antes de leerla)
    u32 cycle{};         // byte +0x4A9 completo (bits 2-3 = m_nOrdnanceCycleIndex)
    u32 driver{};        // 0 sin conductor, 1 con conductor (ped)
    u32 pedType{};       // m_nPedType (+0x598) del conductor
    u32 projRet{};       // EAX del falso de AddProjectile
    u32 ord[3]{}, mpos[3]{}, ppos[3]{}, fwd[3]{}, ms[3]{};
    u32 gate{};          // 1 = el intervalo del arma debe cortar el disparo; 2 = debe dejarlo pasar (informativo)
};

inline u32 Limit(const C& c) { return c.rate + (c.type == 1 ? c.t1 : c.t2); }   // uint32 con desbordamiento, como el EXE
inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n % kFamilies;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    static constexpr std::array<u32, 8> types{1u, 0u, 2u, 3u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000001u, 0x00010001u};
    c.type = Rand(n, 10) % 3 == 0 ? 1u : Rand(n, 11) % 4 == 0 ? Rand(n, 12) : Pick(types, n, 13);
    c.arg2 = (Rand(n, 14) & 0xFFFFFF00u) | (Rand(n, 15) % 2 ? 1u : Rand(n, 16) % 3 ? 0u : (1u + Rand(n, 17) % 255u));
    c.t1 = Rand(n, 18) % 8 == 0 ? 0xFFFFFFFFu : Rand(n, 19) % 2 ? Rand(n, 20) % 100000u : Rand(n, 21);
    c.t2 = Rand(n, 22) % 8 == 0 ? 0xFFFFFFFFu : Rand(n, 23) % 2 ? Rand(n, 24) % 100000u : Rand(n, 25);
    c.rate = Rand(n, 26) % 8 == 0 ? 0u : Rand(n, 27) % 10 == 0 ? 0xFFFFFFFFu : Rand(n, 28) % 6000u;
    c.step = Rand(n, 29) % 3 == 0 ? Pick(kUnit, n, 30) : FloatOf(n, 31, Rand(n, 32) % 5 == 0 ? 3u : 0u);
    c.tgtKind = Rand(n, 33) % 4;
    c.entRaw = Rand(n, 34) | 1u;
    c.hasMat = 1;
    c.cycle = Rand(n, 35) & 0xFFu;
    c.driver = Rand(n, 36) % 4 != 0;
    c.pedType = Rand(n, 37) % 3 == 0 ? Rand(n, 38) % 3 : Rand(n, 39) % 2 ? Rand(n, 40) % 30 : Rand(n, 41);
    c.projRet = Rand(n, 42);
    // la hora: alrededor del limite (rate + ultimo disparo, con desbordamiento)
    const u32 lim = Limit(c);
    switch (Rand(n, 43) % 9) {
    case 0: c.time = lim; break;
    case 1: c.time = lim + 1; break;
    case 2: c.time = lim - 1; break;
    case 3: c.time = 0; break;
    case 4: c.time = 0xFFFFFFFFu; break;
    case 5: c.time = lim + Rand(n, 44) % 5000u; break;
    case 6: c.time = lim - Rand(n, 44) % 5000u; break;
    case 7: c.time = 0x80000000u + Rand(n, 44) % 3u - 1u; break;
    default: c.time = Rand(n, 45); break;
    }
    const u32 m = Rand(n, 46) % 10 == 0 ? 5u : 0u;   // 10 %: todo con mezcla de rangos
    for (int i = 0; i < 3; ++i) {
        c.ord[i] = FloatOf(n, 50 + i, m ? 5u : 0u);
        c.mpos[i] = FloatOf(n, 60 + i, m ? 5u : 0u);
        c.ppos[i] = FloatOf(n, 70 + i, 0u);
        c.fwd[i] = FloatOf(n, 80 + i, m ? 5u : 0u);
        c.ms[i] = FloatOf(n, 90 + i, m ? 5u : 0u);
    }
    auto all = [&](u32 mode, u32 salt) { for (int i = 0; i < 3; ++i) { c.ord[i] = FloatOf(n, 110 + i + salt, mode); c.mpos[i] = FloatOf(n, 120 + i + salt, mode); c.fwd[i] = FloatOf(n, 130 + i + salt, mode); c.ms[i] = FloatOf(n, 140 + i + salt, mode); } };
    switch (c.family) {
    case 0:   // el intervalo corta el disparo (arg2 verdadero, hora <= limite): no se lee la matriz (puede ser nula)
        c.arg2 |= 1u; c.hasMat = Rand(n, 47) % 3 != 0; c.time = Rand(n, 48) % 3 == 0 ? lim : lim - Rand(n, 49) % 3000u; if (c.time > lim) c.time = lim;
        c.gate = 1;
        break;
    case 1:   // frontera del intervalo: hora = limite (corta), limite + 1 (dispara), con arg2 verdadero (bytes bajos) y basura arriba
        c.arg2 = (c.arg2 & 0xFFFFFF00u) | 1u | (Rand(n, 47) % 2 ? 0x80u : 0u); c.time = Rand(n, 48) % 2 ? lim : lim + 1u + Rand(n, 49) % 2u;
        c.hasMat = c.time <= lim ? Rand(n, 50) % 2 : 1u;
        break;
    case 2:   // sin comprobar el intervalo (byte bajo de arg2 a cero, basura arriba): dispara siempre; la hora no importa
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1;
        break;
    case 3:   // tipo 1 frente a los demas (+0x4FC o +0x500), hora justo en el limite de ese tipo
        c.type = Rand(n, 47) % 2 ? 1u : Pick(types, n, 48); c.t1 = Rand(n, 49); c.t2 = Rand(n, 50) ; c.arg2 = (c.arg2 & 0xFFFFFF00u) | 1u;
        c.time = Limit(c) + 1u + Rand(n, 51) % 3u; c.hasMat = 1;
        break;
    case 4:   // los cuatro valores del indice de ordenanza (bits 2-3 del byte +0x4A9) con basura en el resto del byte
        c.arg2 &= 0xFFFFFF00u; c.cycle = (Rand(n, 47) & 0xF3u) | ((Rand(n, 48) % 4u) << 2); c.hasMat = 1;
        break;
    case 5:   // velocidad hacia delante negativa (se recorta a 0)
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1;
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 80 + i, 4u); c.ms[i] = (Rand(n, 150 + i) % 4 == 0) ? FloatOf(n, 90 + i, 0u) : (c.fwd[i] ^ 0x80000000u); }
        break;
    case 6:   // velocidad positiva moderada: el caso normal
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1; c.step = Rand(n, 47) % 2 ? Pick(kUnit, n, 48) : RandFloat(n, 49, -6, 1);
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 80 + i, 4u); c.ms[i] = (Rand(n, 150 + i) % 4 == 0) ? FloatOf(n, 90 + i, 0u) : (c.fwd[i] & 0x7FFFFFFFu); }
        break;
    case 7:   // valores especiales en velocidad y direccion
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1;
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 80 + i, Rand(n, 160 + i) % 3 == 0 ? 3u : 4u); c.ms[i] = FloatOf(n, 90 + i, Rand(n, 163 + i) % 3 == 0 ? 3u : 0u); }
        break;
    case 8:   // valores especiales en la ordenanza y las posiciones
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1;
        for (int i = 0; i < 3; ++i) { c.ord[i] = FloatOf(n, 50 + i, Rand(n, 160 + i) % 2 ? 3u : 0u); c.mpos[i] = FloatOf(n, 60 + i, Rand(n, 163 + i) % 2 ? 3u : 0u); c.ppos[i] = c.mpos[i] ^ (Rand(n, 166 + i) % 2 ? 0u : 0x80000000u); }
        break;
    case 9:   // paso del temporizador especial
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1; c.step = FloatOf(n, 47, Rand(n, 48) % 2 ? 3u : 4u);
        break;
    case 10:  // magnitudes pequenas: los productos caen en el rango desnormal / de exponente extendido
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1; all(1u, 0u); c.step = Rand(n, 47) % 2 ? FloatOf(n, 48, 1u) : FloatOf(n, 48, 0u);
        for (int i = 0; i < 3; ++i) if (Rand(n, 170 + i) % 3 == 0) c.ppos[i] = FloatOf(n, 71 + i, 1u);
        break;
    case 11:  // magnitudes grandes: desbordamientos intermedios y finales
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1; all(2u, 0u); c.step = Rand(n, 47) % 2 ? FloatOf(n, 48, 2u) : FloatOf(n, 48, 0u);
        break;
    case 12:  // conductor jugador (tipos 0 y 1): temblor del mando
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1; c.driver = 1; c.pedType = Rand(n, 47) % 2;
        break;
    case 13:  // conductor distinto del jugador, o sin conductor
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1; c.driver = Rand(n, 47) % 3 != 0; c.pedType = 2u + Rand(n, 48) % 28u; if (Rand(n, 49) % 4 == 0) c.pedType = Rand(n, 50) | 2u;
        break;
    case 14:  // mezcla de rangos en todo (con y sin intervalo)
        c.hasMat = 1; all(5u, 0u); if (c.time <= lim && (c.arg2 & 0xFFu) != 0) c.hasMat = Rand(n, 47) % 2;
        break;
    default:  // 15: ordenanza, velocidad y direccion anulables entre si (cancelaciones) y pasos extremos
        c.arg2 &= 0xFFFFFF00u; c.hasMat = 1;
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 80 + i, 4u); c.ms[i] = Rand(n, 150 + i) % 2 ? c.fwd[i] : FloatOf(n, 90 + i, 4u); c.mpos[i] = FloatOf(n, 60 + i, 4u); c.ord[i] = Rand(n, 153 + i) % 2 ? (c.mpos[i] ^ 0x80000000u) : FloatOf(n, 50 + i, 4u); }
        c.step = Pick(kUnit, n, 47);
        break;
    }
    // un caso con matriz nula solo es valido si el intervalo corta el disparo (el EXE lee matriz + 0x10 sin comprobar nulo)
    const bool cut = (c.arg2 & 0xFFu) != 0 && c.time <= Limit(c);
    if (!cut) c.hasMat = 1;
    c.gate = cut ? 1 : 2;
    if (c.tgtKind == 3 && c.entRaw == 0) c.entRaw = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("time", c.time).U("step", c.step).U("type", c.type).U("arg2", c.arg2).U("t1", c.t1).U("t2", c.t2).U("rate", c.rate);
    j.U("tgt_kind", c.tgtKind).U("ent_raw", c.entRaw).U("has_mat", c.hasMat).U("cycle", c.cycle).U("driver", c.driver).U("ped_type", c.pedType).U("proj_ret", c.projRet).U("gate", c.gate);
    j.U("ord_x", c.ord[0]).U("ord_y", c.ord[1]).U("ord_z", c.ord[2]).U("mpos_x", c.mpos[0]).U("mpos_y", c.mpos[1]).U("mpos_z", c.mpos[2]);
    j.U("ppos_x", c.ppos[0]).U("ppos_y", c.ppos[1]).U("ppos_z", c.ppos[2]).U("fwd_x", c.fwd[0]).U("fwd_y", c.fwd[1]).U("fwd_z", c.fwd[2]);
    j.U("ms_x", c.ms[0]).U("ms_y", c.ms[1]).U("ms_z", c.ms[2]);
    return j.End();
}
}
