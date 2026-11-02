#pragma once
// Fixtures del banco Fn85A (lote 85A): cinco funciones de juego pequenas, 16384 casos cada una (81920 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 JoinGroupFlags                            0x609380  cdecl, sin argumentos, `ret`; devuelve AL = (byte 0x96917C | 0x96917D | 0x96917E) != 0   (hook CPlayerPed / JoinGroupFlags, tipo S)
//   1 CGarage::RightModTypeForThisGarage        0x447720  thiscall (garaje en ECX, vehiculo), `ret 4`; devuelve AL                                  (hook CGarage / RightModTypeForThisGarage, tipo S)
//   2 CGarage::CalcDistToGarageRectangleSquared 0x447D80  thiscall (garaje en ECX, x, y), `ret 8`; devuelve el valor x87 sin guardar en ST0       (hook CGarage / CalcDistToGarageRectangleSquared, tipo S)
//   3 CGarage::RestoreCarsForThisHideOut        0x448550  thiscall (garaje en ECX, coches guardados), `ret 4`; devuelve AL                          (hook CGarage / RestoreCarsForThisHideOut, tipo S)
//   4 CGarage::IsPlayerOutsideGarage            0x448E50  thiscall (garaje en ECX, radio), `ret 4`; devuelve el AL de IsEntityEntirelyOutside      (hook CGarage / IsPlayerOutsideGarage, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
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

namespace F85Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 5;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kGarWin = 0xD8, kVehWin = 0x440, kMiWin = 0x60, kCarsWin = 4 * 0x40, kPedWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kGarArena = Arena(kGarWin), kVehArena = Arena(kVehWin), kMiArena = Arena(kMiWin), kCarsArena = Arena(kCarsWin), kPedArena = Arena(kPedWin);

constexpr u32 kJoinAddr = 0x96917C;          // tres bytes de las banderas (y uno mas)
constexpr u32 kModelInfoPtrs = 0xA9B0C8;     // CModelInfo::ms_modelInfoPtrs
constexpr u32 kHandlingTable = 0xC2B9DC;     // cHandlingDataMgr::m_aVehicleHandling (0xE0 bytes por entrada)
constexpr u32 kModelSlots = 8, kHandlingSlots = 16;
inline constexpr std::array<u32, kModelSlots> kModelIdx{1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};   // indices de modelo que el banco sustituye (se restauran)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85001u); }
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

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// ------------------------------------------------------------------------------------------------ el caso (los campos de cada funcion son independientes)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    // 0 JoinGroupFlags
    u32 jb[4]{};                                  // bytes 0x96917C..0x96917F
    // 1 RightModTypeForThisGarage
    u32 gtype{};                                  // byte +0x4C del garaje
    u32 hasVeh{}, modelSlot{};                    // vehiculo (0 = nulo) y ranura del indice de modelo (0..7)
    u32 mhid[kModelSlots]{};                      // byte +0x4A de la informacion del modelo de cada ranura (indice de manejo 0..15)
    u32 hflags[kHandlingSlots]{};                 // dword +0xD0 de cada entrada de manejo 0..15
    // 2 CalcDistToGarageRectangleSquared
    u32 rect[4]{}, cx{}, cy{};                    // izquierda, derecha, frente, fondo (+0x28, +0x2C, +0x30, +0x34) y el punto
    // 3 RestoreCarsForThisHideOut
    u32 cmodel[4]{};                              // palabra de modelo de cada coche guardado (+0x12 de cada entrada de 0x40)
    u32 rret[4]{};                                // RestoreCar devuelve 0 (nulo) o 1 (la arena del vehiculo k)
    u32 vb[4]{};                                  // byte +0x42D inicial de cada vehiculo
    // 4 IsPlayerOutsideGarage
    u32 radius{};
    u32 fv1{}, fv2{}, fp{};                       // FindPlayerVehicle 1.a y 2.a llamada (0 nulo, 1 arena) y FindPlayerPed
    u32 outRet{};                                 // lo que devuelve IsEntityEntirelyOutside (byte bajo)
    u32 gate{};
};

inline constexpr std::array<u32, 14> kGarageTypes{0u, 1u, 35u, 36u, 36u, 37u, 37u, 38u, 38u, 39u, 40u, 0xFFu, 0x24u, 0x26u};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100 + fn * 50;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    switch (fn) {
    case 0: {   // JoinGroupFlags: n < 13824 recorre 24^3 combinaciones de 24 valores por byte; el resto es aleatorio
        static constexpr std::array<u32, 24> vals{0u, 1u, 2u, 3u, 4u, 8u, 0x10u, 0x20u, 0x40u, 0x80u, 0x81u, 0xFEu, 0xFFu, 0x7Fu, 0x55u, 0xAAu, 0x0Fu, 0xF0u, 0x11u, 0x22u, 0x33u, 0x44u, 0x66u, 0x99u};
        if (n < 13824) { c.jb[0] = vals[n % 24]; c.jb[1] = vals[(n / 24) % 24]; c.jb[2] = vals[(n / 576) % 24]; }
        else { for (u32 k = 0; k < 3; ++k) c.jb[k] = P(50) ? 0u : R(256); }
        c.jb[3] = R(256);
        break;
    }
    case 1: {   // RightModTypeForThisGarage
        c.gtype = P(80) ? Pick(kGarageTypes, n, nk()) : R(256);
        c.hasVeh = P(94) ? 1u : 0u;
        c.modelSlot = R(kModelSlots);
        for (u32 s = 0; s < kModelSlots; ++s) c.mhid[s] = R(kHandlingSlots);
        static constexpr std::array<u32, 8> fl{0u, 0x2000000u, 0x4000000u, 0x6000000u, 0x1FFFFFFu, 0xFFFFFFFFu, 0x8000000u, 0x7FFFFFFu};
        for (u32 h = 0; h < kHandlingSlots; ++h) c.hflags[h] = P(70) ? fl[R(8)] : Rand(n, nk());
        if (f >= 8) {   // sesgo: banderas de la ranura elegida con exactamente uno de los dos bits
            const u32 s = c.mhid[c.modelSlot];
            c.hflags[s] = (c.hflags[s] & ~0x6000000u) | (R(4) << 25);
            if (P(50)) c.gtype = 36u + R(3);
            c.hasVeh = 1;
        }
        break;
    }
    case 2: {   // CalcDistToGarageRectangleSquared
        auto Coord = [&]() -> u32 { return B(float(R(6001)) / 10.0f - 300.0f); };
        float l = F(Coord()), r = F(Coord()), fr = F(Coord()), bk = F(Coord());
        if (r < l) std::swap(l, r);
        if (bk < fr) std::swap(fr, bk);
        c.rect[0] = B(l); c.rect[1] = B(r); c.rect[2] = B(fr); c.rect[3] = B(bk);
        c.cx = Coord(); c.cy = Coord();
        if (f < 4) {        // dentro, antes, despues en cada eje: mezcla
            const u32 wx = R(3), wy = R(3);
            c.cx = wx == 0 ? B(l - 1.0f - float(R(500)) / 10.0f) : wx == 1 ? B(l + (r - l) * float(R(1001)) / 1000.0f) : B(r + 1.0f + float(R(500)) / 10.0f);
            c.cy = wy == 0 ? B(fr - 1.0f - float(R(500)) / 10.0f) : wy == 1 ? B(fr + (bk - fr) * float(R(1001)) / 1000.0f) : B(bk + 1.0f + float(R(500)) / 10.0f);
        } else if (f == 4) {  // exactamente en los bordes
            c.cx = P(50) ? c.rect[0] : c.rect[1]; c.cy = P(50) ? c.rect[2] : c.rect[3];
        } else if (f < 8) {   // valores especiales en la entrada
            if (P(60)) c.cx = Special(n, nk());
            if (P(60)) c.cy = Special(n, nk());
        } else if (f < 11) {  // valores especiales en el rectangulo
            for (u32 k = 0; k < 4; ++k) if (P(40)) c.rect[k] = Special(n, nk());
        } else if (f == 11) { // valores enormes y diminutos: productos que se salen de float o se desnormalizan
            const u32 w = R(4);
            c.cx = w == 0 ? B(3.0e19f) : w == 1 ? B(-2.0e19f) : w == 2 ? B(1.0e-20f) : B(1.0e30f);
            c.cy = P(50) ? B(2.5e19f) : B(-1.0e-22f);
            for (u32 k = 0; k < 4; ++k) c.rect[k] = P(50) ? 0u : B(float(R(10)) - 5.0f);
        } else {              // mezcla con especiales dispersos
            if (P(25)) c.cx = Special(n, nk());
            if (P(25)) c.cy = Special(n, nk());
            if (P(25)) c.rect[R(4)] = Special(n, nk());
        }
        break;
    }
    case 3: {   // RestoreCarsForThisHideOut
        for (u32 k = 0; k < 4; ++k) {
            c.cmodel[k] = P(55) ? 0u : (P(50) ? 1u + R(0x20) : (P(50) ? 0x8000u + R(16) : 0xFFFFu));
            c.rret[k] = P(60) ? 1u : 0u;
            c.vb[k] = R(256);
        }
        if (f >= 12) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(100); }          // todos con coche
        else if (f >= 8) { for (u32 k = 0; k < 4; ++k) { c.cmodel[k] = P(50) ? 0u : 1u + R(100); } }
        if (f == 15) { for (u32 k = 0; k < 4; ++k) c.rret[k] = 1u; }
        if (f == 14) { for (u32 k = 0; k < 4; ++k) c.rret[k] = 0u; }
        break;
    }
    default: {  // 4 IsPlayerOutsideGarage
        c.radius = P(70) ? B(float(R(1001)) / 100.0f - 2.0f) : Special(n, nk());
        c.fv1 = R(2); c.fv2 = P(75) ? c.fv1 : R(2);
        c.fp = P(60) ? 1u : 0u;
        c.outRet = R(2);
        if (f >= 8) { c.fv1 = 1; c.fv2 = P(50) ? 0u : 1u; }
        if (f >= 12) { c.fv1 = 0; c.fv2 = 1; c.fp = R(2); }
        break;
    }
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    switch (c.fn) {
    case 0: j.A("jb", std::vector<u32>(c.jb, c.jb + 4)); break;
    case 1:
        j.U("gtype", c.gtype).U("has_veh", c.hasVeh).U("model_slot", c.modelSlot).A("mhid", std::vector<u32>(c.mhid, c.mhid + kModelSlots)).A("hflags", std::vector<u32>(c.hflags, c.hflags + kHandlingSlots));
        break;
    case 2: j.A("rect", std::vector<u32>(c.rect, c.rect + 4)).U("cx", c.cx).U("cy", c.cy); break;
    case 3: j.A("cmodel", std::vector<u32>(c.cmodel, c.cmodel + 4)).A("rret", std::vector<u32>(c.rret, c.rret + 4)).A("vb", std::vector<u32>(c.vb, c.vb + 4)); break;
    default: j.U("radius", c.radius).U("fv1", c.fv1).U("fv2", c.fv2).U("fp", c.fp).U("out_ret", c.outRet); break;
    }
    j.U("gate", c.gate);
    return j.End();
}
}
