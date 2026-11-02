#pragma once
// Fixtures del banco Fn85B (lote 85B): cuatro metodos de CGarage, 16384 casos cada uno (65536 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CGarage::RestoreCarsForThisImpoundingGarage      0x4485C0  thiscall (ECX sin uso, coches guardados), `ret 4`; devuelve AL                              (hook CGarage / RestoreCarsForThisImpoundingGarage, tipo S)
//   1 CGarage::CountCarsWithCenterPointWithinGarage    0x4495F0  thiscall (garaje en ECX, vehiculo ignorado), `ret 4`; devuelve EAX = cuenta                (hook CGarage / CountCarsWithCenterPointWithinGarage, tipo S)
//   2 CGarage::SlideDoorOpen                            0x44A660  thiscall (garaje en ECX), `ret`; devuelve AL                                                (hook CGarage / SlideDoorOpen, tipo S)
//   3 CGarage::SlideDoorClosed                          0x44A750  thiscall (garaje en ECX), `ret`; devuelve AL                                                (hook CGarage / SlideDoorClosed, tipo S)
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

namespace F85BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kGarWin = 0xD8, kVehWin = 0x5A0, kCarsWin = 4 * 0x40, kDoorWin = 0x40, kMatWin = 0x48;
constexpr std::size_t kPoolSlots = 8, kSlotStride = 0xA18, kPoolWin = kPoolSlots * kSlotStride, kMapWin = 16, kPoolStructWin = 0x20, kMatPoolWin = kPoolSlots * 0x48;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kGarArena = Arena(kGarWin), kVehArena = Arena(kVehWin), kCarsArena = Arena(kCarsWin), kDoorArena = Arena(kDoorWin), kMatArena = Arena(kMatWin);
constexpr std::size_t kPoolArena = Arena(kPoolWin), kMapArena = Arena(kMapWin), kPoolStructArena = Arena(kPoolStructWin), kMatPoolArena = Arena(kMatPoolWin);

constexpr u32 kTimestepAddr = 0xB7CB5C;      // CTimer::ms_fTimeStep
constexpr u32 kPoolAddr = 0xB74494;          // CPools::ms_pVehiclePool

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85002u); }
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
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}

// ------------------------------------------------------------------------------------------------ el caso (los campos de cada funcion son independientes)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    // 0 RestoreCarsForThisImpoundingGarage
    u32 cmodel[4]{};                              // palabra de modelo de cada coche guardado (+0x12 de cada entrada de 0x40); solo se miran los tres primeros
    u32 rret[4]{};                                // RestoreCar devuelve 0 (nulo) o 1 (la arena del vehiculo k)
    u32 vb[4]{};                                  // byte +0x42D inicial de cada vehiculo
    u32 vtype[4]{};                               // dword +0x594 de cada vehiculo (tipo de vehiculo)
    // 1 CountCarsWithCenterPointWithinGarage
    u32 psize{};                                  // tamano del pool (0..8)
    u32 pmap[kPoolSlots]{};                       // byte de ranuras: bit 7 = libre
    u32 pmat[kPoolSlots]{};                       // 1 = el vehiculo tiene matriz (posicion en +0x30 de la matriz); 0 = posicion en +4
    u32 ppos[kPoolSlots][3]{};                    // posicion del vehiculo (patrones de 32 bits)
    u32 imask{};                                  // resultado de IsPointInsideGarage en la llamada k (bit k)
    u32 ign{};                                    // 0 nulo, 1..8 ranura del pool (-1), 9 arena de vehiculo ajena al pool, 10 puntero crudo
    // 2 SlideDoorOpen / 3 SlideDoorClosed
    u32 gtype{};                                  // byte +0x4C del garaje
    u32 pos{};                                    // m_fDoorPosition (+0x38)
    u32 ts{};                                     // CTimer::ms_fTimeStep
    u32 door1{}, door2{};                         // primera puerta: 0 nula, 1 con matriz, 2 sin matriz; segunda: 0 nula, 1 presente
    u32 gate{};
};

inline constexpr std::array<u32, 12> kDoorTypes{44u, 45u, 44u, 45u, 0u, 1u, 43u, 46u, 0xFFu, 20u, 15u, 5u};
inline constexpr std::array<u32, 16> kVTypes{0u, 9u, 0u, 9u, 1u, 2u, 3u, 5u, 8u, 10u, 11u, 0x100u, 0x109u, 0x80000000u, 0xFFFFFFFFu, 7u};
inline constexpr float kStepOpen = 0.011f, kStepOpenHangar = 0.0011f, kStepClose = 0.013f, kStepCloseHangar = 0.0013f;

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
    case 0: {   // RestoreCarsForThisImpoundingGarage
        for (u32 k = 0; k < 4; ++k) {
            c.cmodel[k] = P(55) ? 0u : (P(50) ? 1u + R(0x20) : (P(50) ? 0x8000u + R(16) : 0xFFFFu));
            c.rret[k] = P(60) ? 1u : 0u;
            c.vb[k] = R(256);
            c.vtype[k] = P(70) ? (R(2) ? 0u : 9u) : kVTypes[R(16)];
        }
        if (f >= 12) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(100); }          // todos con coche
        else if (f >= 8) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = P(50) ? 0u : 1u + R(100); }
        if (f == 15) { for (u32 k = 0; k < 4; ++k) c.rret[k] = 1u; }
        if (f == 14) { for (u32 k = 0; k < 4; ++k) c.rret[k] = 0u; }
        if (f == 13) { c.cmodel[0] = c.cmodel[1] = c.cmodel[2] = 0u; c.cmodel[3] = 1u + R(100); }   // solo el cuarto con coche: no se mira
        if (f == 7) { for (u32 k = 0; k < 3; ++k) { c.cmodel[k] = 1u + R(100); c.rret[k] = 1u; c.vtype[k] = R(2) ? 0u : 9u; } }
        if (f == 6) { for (u32 k = 0; k < 3; ++k) { c.cmodel[k] = 1u + R(100); c.rret[k] = 1u; c.vtype[k] = kVTypes[R(16)]; } }
        break;
    }
    case 1: {   // CountCarsWithCenterPointWithinGarage
        c.psize = f < 2 ? R(3) : (f == 2 ? 0u : (f >= 12 ? 8u : 1u + R(8)));
        for (u32 s = 0; s < kPoolSlots; ++s) {
            c.pmap[s] = P(35) ? (0x80u | R(0x80)) : R(0x80);
            c.pmat[s] = P(60) ? 1u : 0u;
            for (u32 k = 0; k < 3; ++k) c.ppos[s][k] = P(85) ? B(float(int(R(20001)) - 10000) / 16.0f) : Special(n, nk());
        }
        c.imask = Rand(n, nk()) & 0xFFu;
        const u32 w = R(100);
        c.ign = w < 25 ? 0u : (w < 70 ? 1u + R(kPoolSlots) : (w < 85 ? 9u : 10u));
        if (f == 3) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] &= 0x7Fu; c.imask = 0xFFu; c.ign = 0; }       // todo ocupado y dentro
        if (f == 4) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] |= 0x80u; }                                  // todo libre
        if (f == 5) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] &= 0x7Fu; c.imask = 0u; }                     // todo ocupado y fuera
        if (f == 6) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] &= 0x7Fu; c.ign = 1u + R(c.psize ? c.psize : 1u); if (!c.psize) c.psize = 1; }  // el ignorado esta en el pool
        if (f == 7) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = R(2); } }
        if (f == 8) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 0; c.pmap[s] &= 0x7Fu; } }                 // sin matrices
        if (f == 9) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 1; c.pmap[s] &= 0x7Fu; } }                 // con matrices
        break;
    }
    default: {  // 2 SlideDoorOpen / 3 SlideDoorClosed
        const bool open = fn == 2;
        c.gtype = P(35) ? kDoorTypes[R(12)] : (P(50) ? R(256) : (R(2) ? 44u : 45u));
        const bool hangar = c.gtype == 44u || c.gtype == 45u;
        const float step = open ? (hangar ? kStepOpenHangar : kStepOpen) : (hangar ? kStepCloseHangar : kStepClose);
        c.ts = P(80) ? B(float(R(2000)) / 1000.0f + 0.1f) : Special(n, nk());
        c.pos = B(float(R(1001)) / 1000.0f);
        const float stepProduct = F(c.ts) * step;
        if (f < 3) {            // avance normal dentro del recorrido
            c.pos = B(float(R(10001)) / 10000.0f);
            c.ts = B(float(R(3000)) / 1000.0f + 0.5f);
        } else if (f < 6) {     // al borde: posicion + paso ~ exactamente 1.0 (abrir) o 0.0 (cerrar), con unos pocos ulps de mas o de menos
            c.ts = B(float(R(2000)) / 1000.0f + 0.5f);
            const float sp = F(c.ts) * step;
            const int ulps = int(R(7)) - 3;
            c.pos = open ? NextUp(B(1.0f - sp), ulps) : NextUp(B(sp), ulps);
        } else if (f == 6) {    // posicion especial
            c.pos = Special(n, nk());
        } else if (f == 7) {    // paso especial (cero, desnormal, infinito, NaN, enorme)
            c.ts = Special(n, nk());
            c.pos = P(50) ? B(float(R(1001)) / 1000.0f) : Special(n, nk());
        } else if (f == 8) {    // ya pasado del final
            c.pos = open ? B(1.0f + float(R(1000)) / 100.0f) : B(-float(R(1000)) / 100.0f);
        } else if (f == 9) {    // exactamente en el limite
            c.pos = open ? (R(2) ? B(1.0f) : B(0.999999f)) : (R(2) ? 0u : 0x80000000u);
            c.ts = B(float(R(3)) * 0.25f);
        } else if (f == 10) {   // negativo / enorme
            c.pos = P(50) ? B(-float(R(100000)) / 3.0f) : B(float(R(100000)) * 1.0e30f);
        } else if (f == 11) {   // hangares
            c.gtype = R(2) ? 44u : 45u;
            c.pos = B(float(R(10001)) / 10000.0f);
        } else {                // mezcla con especiales dispersos
            if (P(25)) c.pos = Special(n, nk());
            if (P(25)) c.ts = Special(n, nk());
        }
        (void)stepProduct;
        c.door1 = f == 12 ? 0u : (f == 13 ? 1u : (f == 14 ? 2u : R(3)));
        c.door2 = R(2);
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
    case 0: j.A("cmodel", std::vector<u32>(c.cmodel, c.cmodel + 4)).A("rret", std::vector<u32>(c.rret, c.rret + 4)).A("vb", std::vector<u32>(c.vb, c.vb + 4)).A("vtype", std::vector<u32>(c.vtype, c.vtype + 4)); break;
    case 1: {
        std::vector<u32> pos;
        for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < 3; ++k) pos.push_back(c.ppos[s][k]);
        j.U("psize", c.psize).A("pmap", std::vector<u32>(c.pmap, c.pmap + kPoolSlots)).A("pmat", std::vector<u32>(c.pmat, c.pmat + kPoolSlots)).A("ppos", pos).U("imask", c.imask).U("ign", c.ign);
        break;
    }
    default: j.U("gtype", c.gtype).U("pos", c.pos).U("ts", c.ts).U("door1", c.door1).U("door2", c.door2); break;
    }
    j.U("gate", c.gate);
    return j.End();
}
}
