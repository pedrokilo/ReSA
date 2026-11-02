#pragma once
// Fixtures del banco Fn85D (lote 85D): tres metodos de CGarage, 16384 casos cada uno (49152 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CGarage::IsAnyOtherCarTouchingGarage    0x449100  thiscall (garaje en ECX, vehiculo ignorado), `ret 4`; devuelve AL                 (hook CGarage / IsAnyOtherCarTouchingGarage, tipo S)
//   1 CGarage::ThrowCarsNearDoorOutOfGarage   0x449220  thiscall (garaje en ECX, vehiculo ignorado), `ret 4`; sin retorno                (hook CGarage / ThrowCarsNearDoorOutOfGarage, tipo S)
//   2 CGarage::NeatlyLineUpStoredCars         0x448330  thiscall (garaje en ECX, coches guardados), `ret 4`; sin retorno                 (hook CGarage / NeatlyLineUpStoredCars, tipo S)
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

namespace F85DFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 3;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPoolSlots = 8, kSlotStride = 0xA18, kPoolWin = kPoolSlots * kSlotStride, kMapWin = 16, kPoolStructWin = 0x20, kMatPoolWin = kPoolSlots * 0x48;
constexpr std::size_t kGarWin = 0xD8, kCarsWin = 4 * 0x40;
constexpr std::size_t kColWin = kPoolSlots * 0x30, kDataWin = kPoolSlots * 0x10, kMaxSph = 4, kSphWin = kPoolSlots * kMaxSph * 0x14;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kGarArena = Arena(kGarWin), kPoolArena = Arena(kPoolWin), kMapArena = Arena(kMapWin), kPoolStructArena = Arena(kPoolStructWin), kMatPoolArena = Arena(kMatPoolWin);
constexpr std::size_t kColArena = Arena(kColWin), kDataArena = Arena(kDataWin), kSphArena = Arena(kSphWin), kCarsArena = Arena(kCarsWin);

constexpr u32 kPoolAddr = 0xB74494;          // CPools::ms_pVehiclePool
constexpr u32 kTimeStepAddr = 0xB7CB5C;      // CTimer::ms_fTimeStep
constexpr u32 kJunkPtr = 0x13572468u;        // "vehiculo ignorado" que no es de ninguna ranura

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85004u); }
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
// Igual pero sin NaN (la salida de CVector::Normalise del falso: un fchs de un NaN invertiria su signo y el oraculo no lo modela)
inline u32 SpecialNoNan(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 12) return Pick(kTiny, c, k + 1);
    if (r < 20) return Pick(kInf, c, k + 1);
    if (r < 26) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// ------------------------------------------------------------------------------------------------ el caso (los campos de cada funcion son independientes)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    // pool de vehiculos (0 y 1)
    u32 psize{};                                  // tamano del pool (0..8)
    u32 pmap[kPoolSlots]{};                       // byte de ranuras: bit 7 = libre
    u32 pmat[kPoolSlots]{};                       // 1 = el vehiculo tiene matriz (posicion en +0x30 de la matriz); 0 = posicion en +4 (m_placement)
    u32 pstat[kPoolSlots]{};                      // byte +0x36 (tipo y estado) de cada vehiculo
    u32 ppos[kPoolSlots][3]{};                    // posicion del vehiculo (patrones de 32 bits)
    u32 pvel[kPoolSlots][3]{};                    // velocidad del vehiculo (+0x44..+0x4C)
    u32 nsph[kPoolSlots]{};                       // palabra +0 de la CCollisionData de cada vehiculo (int16 con signo; la mitad alta es basura)
    u32 sphR[kPoolSlots][kMaxSph]{};              // radio de cada esfera
    u32 tmask{};                                  // resultado de IsEntityTouching3D en la llamada k (bit k)
    u32 rmask{};                                  // resultado de IsPointInsideGarage(radio) en la llamada k (bit k)
    u32 reOn{}, reK{}, reVal{};                   // tras la llamada k a IsPointInsideGarage(radio) se reescribe el contador de esferas del ultimo vehiculo consultado
    u32 ign{};                                    // vehiculo ignorado: 0..7 = ranura, 8 = nulo, 9 = puntero ajeno
    // 1 ThrowCarsNearDoorOutOfGarage
    u32 gco[4]{};                                 // izquierda +0x28, derecha +0x2C, frente +0x30, fondo +0x34
    u32 ts{};                                     // CTimer::ms_fTimeStep
    u32 nrm[3]{};                                 // salida del falso de CVector::Normalise (1 y 2)
    // 2 NeatlyLineUpStoredCars
    u32 gpos[3]{}, gdA[2]{}, gdB[2]{}, gW{}, gH{};
    u32 cmodel[4]{};                              // palabra +0x12 de cada coche guardado
    u32 gate{};
};

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
    auto normalF = [&](float scale, int range) { return B(float(int(R(u32(2 * range + 1))) - range) / scale); };
    auto normalOut = [&] {
        const u32 w = R(100);
        for (u32 k = 0; k < 3; ++k) c.nrm[k] = w < 80 ? normalF(1000.0f, 1000) : SpecialNoNan(n, nk());
    };
    auto fillPool = [&] {
        c.psize = f < 2 ? R(3) : (f == 2 ? 0u : (f >= 12 ? 8u : 1u + R(8)));
        for (u32 s = 0; s < kPoolSlots; ++s) {
            c.pmap[s] = P(30) ? (0x80u | R(0x80)) : R(0x80);
            c.pmat[s] = P(60) ? 1u : 0u;
            c.pstat[s] = P(20) ? (0x28u | R(8)) : R(256);
            for (u32 k = 0; k < 3; ++k) {
                c.ppos[s][k] = P(85) ? B(float(int(R(20001)) - 10000) / 16.0f) : Special(n, nk());
                c.pvel[s][k] = P(85) ? B(float(int(R(2001)) - 1000) / 100.0f) : Special(n, nk());
            }
        }
    };
    auto spheres = [&] {
        for (u32 s = 0; s < kPoolSlots; ++s) {
            const u32 w = R(100);
            const u32 count = w < 12 ? 0u : (w < 30 ? 1u : (w < 55 ? 2u : (w < 75 ? 3u : (w < 92 ? 4u : 0xFFFFu))));   // 0xFFFF = -1 como int16
            c.nsph[s] = (count == 0xFFFFu ? (P(50) ? 0xFFFFu : 0x8000u + R(0x7FFF)) : count) | (R(0x10000) << 16);
            for (u32 j = 0; j < kMaxSph; ++j) c.sphR[s][j] = P(90) ? B(float(R(2000)) / 100.0f) : Special(n, nk());
        }
        c.tmask = Rand(n, nk());
        c.rmask = Rand(n, nk());
    };
    auto allLive = [&](bool notWrecked) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; if (notWrecked && (c.pstat[s] & 0xF8u) == 0x28u) c.pstat[s] ^= 0x08u; } };
    auto setCount = [&](u32 lo, u32 span) { for (u32 s = 0; s < kPoolSlots; ++s) c.nsph[s] = (c.nsph[s] & 0xFFFF0000u) | (lo + R(span)); };
    {
        const u32 w = R(100);
        c.ign = w < 55 ? 8u : (w < 80 ? R(8) : (w < 90 ? 9u : R(8)));
    }
    switch (fn) {
    case 0: {   // IsAnyOtherCarTouchingGarage
        fillPool(); spheres();
        if (f == 3) { allLive(true); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); }                                                           // todo toca, ninguna esfera dentro: false
        if (f == 4) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] |= 0x80u; }                                                                                   // todo libre
        if (f == 5) { allLive(true); c.tmask = 0u; }                                                                                                                // nada toca
        if (f == 6) { allLive(true); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 1u << R(6); setCount(1, 4); }                                                      // una esfera dentro
        if (f == 7) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmap[s] &= 0x7Fu; c.pstat[s] = 0x28u | R(8); } c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; }          // todos siniestrados
        if (f == 8) { allLive(true); c.ign = R(8); c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; setCount(1, 4); }                                                  // ignorado uno
        if (f == 9) { allLive(true); c.ign = P(50) ? 8u : 9u; c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; setCount(1, 4); }                                      // nada ignorado: dentro en la primera esfera
        if (f == 10) { allLive(true); c.reOn = 1; c.reK = R(6); c.reVal = P(50) ? (R(2) ? 0u : 0xFFFFu) : 1u + R(4); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(2, 3); } // reescritura del contador en vuelo
        if (f == 11) { allLive(true); c.reOn = 1; c.reK = R(3); c.reVal = 1u + R(4); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 2); }
        if (f == 12) { allLive(true); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 1u << (24 + R(8)); setCount(4, 1); }                                              // la esfera dentro esta muy al final
        if (f == 13) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 0; c.pmap[s] &= 0x7Fu; } }
        if (f == 14) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 1; c.pmap[s] &= 0x7Fu; } }
        break;
    }
    case 1: {   // ThrowCarsNearDoorOutOfGarage
        fillPool(); spheres();
        const u32 w = R(100);
        for (u32 k = 0; k < 4; ++k) c.gco[k] = w < 85 ? B(float(int(R(4001)) - 2000) / 8.0f) : Special(n, nk());
        c.ts = P(80) ? B(float(1 + R(100)) / 60.0f) : Special(n, nk());
        normalOut();
        if (f == 3) { allLive(false); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; setCount(1, 4); }                                                    // todo dentro: nadie sale
        if (f == 4) { for (u32 s = 0; s < kPoolSlots; ++s) c.pmap[s] |= 0x80u; }                                                                                    // todo libre
        if (f == 5) { allLive(false); c.tmask = 0xFFFFFFFFu; for (u32 sl = 0; sl < kPoolSlots; ++sl) c.nsph[sl] = (c.nsph[sl] & 0xFFFF0000u) | (R(2) ? 0u : (R(2) ? 0xFFFFu : 0x8000u + R(0x7FFF))); }   // todo toca, sin esferas o con contador negativo
        if (f == 6) { allLive(false); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 0xFFFFFFFFu; c.rmask &= ~(1u << R(4)); setCount(1, 4); }                          // una esfera fuera
        if (f == 7) { allLive(false); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(2, 3); }                                                             // la primera esfera de cada vehiculo fuera: se empuja y se deja de recorrer
        if (f == 8) { allLive(false); c.ign = R(8); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); }                                                          // ignorado uno
        if (f == 9) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 1; c.pmap[s] &= 0x7Fu; } c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); }
        if (f == 10) { for (u32 s = 0; s < kPoolSlots; ++s) { c.pmat[s] = 0; c.pmap[s] &= 0x7Fu; } c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); }
        if (f == 11) { allLive(false); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); for (u32 k = 0; k < 4; ++k) c.gco[k] = Special(n, nk()); c.ts = Special(n, nk()); }   // flotantes especiales en el garaje
        if (f == 12) { allLive(false); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); for (u32 k = 0; k < 3; ++k) c.nrm[k] = SpecialNoNan(n, nk()); }             // salida de Normalise especial
        if (f == 13) { allLive(false); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < 3; ++k) c.pvel[s][k] = Special(n, nk()); }   // velocidades especiales
        if (f == 14) { allLive(false); c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < 3; ++k) c.ppos[s][k] = Special(n, nk()); }   // posiciones especiales
        if (f == 15) { allLive(false); c.ign = 8; c.tmask = 0xFFFFFFFFu; c.rmask = 0u; setCount(1, 4); c.reOn = 1; c.reK = R(4); c.reVal = P(50) ? (R(2) ? 0u : 0xFFFFu) : 1u + R(4); }  // reescritura del contador
        break;
    }
    default: {  // 2 NeatlyLineUpStoredCars
        const u32 w = R(100);
        for (u32 k = 0; k < 3; ++k) c.gpos[k] = w < 85 ? B(float(int(R(20001)) - 10000) / 16.0f) : Special(n, nk());
        for (u32 k = 0; k < 2; ++k) { c.gdA[k] = w < 85 ? normalF(1000.0f, 1000) : Special(n, nk()); c.gdB[k] = w < 85 ? normalF(1000.0f, 1000) : Special(n, nk()); }
        c.gW = w < 85 ? B(float(R(2000)) / 100.0f) : Special(n, nk());
        c.gH = w < 85 ? B(float(R(2000)) / 100.0f) : Special(n, nk());
        for (u32 k = 0; k < 4; ++k) c.cmodel[k] = P(35) ? 0u : (P(50) ? 1u + R(0x100) : R(0x10000));
        normalOut();
        if (f == 0) { c.cmodel[0] = 0u; }                                                                                                    // sin coches
        if (f == 1) { c.cmodel[0] = 1u + R(0xFF); c.cmodel[1] = 0u; }                                                                         // un coche
        if (f == 2) { c.cmodel[0] = 1u + R(0xFF); c.cmodel[1] = 1u + R(0xFF); c.cmodel[2] = 0u; }                                            // dos coches
        if (f == 3) { c.cmodel[0] = 1u + R(0xFF); c.cmodel[1] = 1u + R(0xFF); c.cmodel[2] = 1u + R(0xFF); c.cmodel[3] = 0u; }                // tres coches
        if (f == 4) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); }                                                               // cuatro: el cuarto no se toca
        if (f == 5) { c.cmodel[0] = 1u + R(0xFF); c.cmodel[1] = 0u; c.cmodel[2] = 1u + R(0xFF); }                                            // hueco: se detiene en el primero vacio
        if (f == 6) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); c.nrm[2] = P(50) ? 0u : normalF(1000.0f, 1000); }             // z de la direccion unitaria distinta de cero
        if (f == 7) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); for (u32 k = 0; k < 3; ++k) c.nrm[k] = B(float(int(R(2001)) - 1000) / 10.0f); }   // direccion enorme: bytes empaquetados saturados
        if (f == 8) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); for (u32 k = 0; k < 3; ++k) c.gpos[k] = Special(n, nk()); }   // posicion especial
        if (f == 9) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); c.gW = Special(n, nk()); c.gH = Special(n, nk()); }               // anchura y altura especiales
        if (f == 10) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); for (u32 k = 0; k < 2; ++k) { c.gdA[k] = Special(n, nk()); c.gdB[k] = Special(n, nk()); } }   // direcciones especiales
        if (f == 11) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); for (u32 k = 0; k < 3; ++k) c.nrm[k] = SpecialNoNan(n, nk()); }  // salida de Normalise especial
        if (f == 12) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); c.gW = 0u; c.gH = 0u; }                                          // ejes de longitud cero
        if (f == 13) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); c.nrm[0] = B(float(int(R(2001)) - 1000) / 1000.0f); c.nrm[1] = B(float(int(R(2001)) - 1000) / 1000.0f); }   // unitaria normal
        if (f == 14) { for (u32 k = 0; k < 4; ++k) c.cmodel[k] = 1u + R(0xFF); c.nrm[0] = B(1.0f); c.nrm[1] = 0u; c.nrm[2] = 0u; }                 // direccion A del eje x
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
    case 0: case 1: {
        std::vector<u32> pos, vel, radius;
        for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < 3; ++k) { pos.push_back(c.ppos[s][k]); vel.push_back(c.pvel[s][k]); }
        for (u32 s = 0; s < kPoolSlots; ++s) for (u32 k = 0; k < kMaxSph; ++k) radius.push_back(c.sphR[s][k]);
        j.U("psize", c.psize).A("pmap", std::vector<u32>(c.pmap, c.pmap + kPoolSlots)).A("pmat", std::vector<u32>(c.pmat, c.pmat + kPoolSlots)).A("pstat", std::vector<u32>(c.pstat, c.pstat + kPoolSlots))
         .A("ppos", pos).A("pvel", vel).A("nsph", std::vector<u32>(c.nsph, c.nsph + kPoolSlots)).A("sphr", radius).U("tmask", c.tmask).U("rmask", c.rmask)
         .U("reon", c.reOn).U("rek", c.reK).U("reval", c.reVal).U("ign", c.ign);
        if (c.fn == 1) j.A("gco", std::vector<u32>(c.gco, c.gco + 4)).U("ts", c.ts).A("nrm", std::vector<u32>(c.nrm, c.nrm + 3));
        break;
    }
    default:
        j.A("gpos", std::vector<u32>(c.gpos, c.gpos + 3)).A("gda", std::vector<u32>(c.gdA, c.gdA + 2)).A("gdb", std::vector<u32>(c.gdB, c.gdB + 2)).U("gw", c.gW).U("gh", c.gH)
         .A("cmodel", std::vector<u32>(c.cmodel, c.cmodel + 4)).A("nrm", std::vector<u32>(c.nrm, c.nrm + 3));
        break;
    }
    j.U("gate", c.gate);
    return j.End();
}
}
