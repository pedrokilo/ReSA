#pragma once
// Fixtures del banco Car84I (lote 84I): CCarCtrl::SteerAICarWithPhysics_OnlyMission (EXE 0x436A90), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::SteerAICarWithPhysics_OnlyMission  0x436A90  cdecl, cinco argumentos (vehiculo y cuatro punteros a salidas), `ret` simple, void   (hook CCarCtrl / SteerAICarWithPhysics_OnlyMission, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una):
//   0..2 mision libre (ciclo de 72 valores: 0..0x44, 0x45, 0x80, 0xFF)   3..6 mision 3 (embestir al jugador de cerca)   7, 8 mision 44 (acercarse al jugador)   9, 10 mision 53 (seguir un coche de cerca)
//   11, 12 mision 60 (junto al objetivo)   13 misiones con objetivo (16, 18, 19, 29..32, 55)   14 mision libre   15 y 16 valores especiales (NaN, infinitos, desnormales)   17 barrido de mutaciones.
// El vehiculo es una arena de 0x9A0 bytes con su matriz opcional, un objetivo (arena con matriz opcional), dos vehiculos del jugador alternos (el k-esimo FindPlayerVehicle devuelve el k % 2), seis modelos con su
// caja de colision, un pasajero y una arena de salidas. Reglas de seguridad (el EXE no comprueba nada de esto): el vehiculo propio tiene matriz en la mision 3, los vehiculos del jugador siempre tienen matriz,
// el objetivo existe siempre salvo en la mision 60 (que lo comprueba), los indices de modelo son 1..6, ninguna mutacion anula un puntero y ningun valor flotante lleva un NaN con carga distinta del indefinido.
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

namespace C84IFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 2;    // familias 16..17, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;
constexpr unsigned kModels = 6;           // indices de modelo 1..6

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x9A0, kMatWin = 0x48, kOutWin = 0x10, kTgtWin = 0x60, kPvWin = 0x60, kModelWin = 0x400, kPedWin = 0x20, kMiscWin = 0x80;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kOutArena = Arena(kOutWin), kTgtArena = Arena(kTgtWin), kPvArena = Arena(kPvWin), kModelArena = Arena(kModelWin),
                      kPedArena = Arena(kPedWin), kMiscArena = Arena(kMiscWin);
// Modelos: objeto de informacion en 0x20 * i y caja de colision en 0x100 + 0x30 * i (i = 0..5); el objeto lleva el puntero a la caja en +0x14; la caja, max.x en +0xC y max.y en +0x10
constexpr std::size_t ModelInfoAt(unsigned i) { return 0x20u * i; }
constexpr std::size_t ColModelAt(unsigned i) { return 0x100u + 0x30u * i; }
// Zona de retorno de los falsos (arena "misc"): dos velocidades del jugador (12 bytes cada una) y una entidad ficticia
constexpr std::size_t kSpeedBuf0 = 0x00, kSpeedBuf1 = 0x10, kEntityDummy = 0x20;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84009u); }
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
    Js& A(const char* k, const u32* v, int count) { Key(k); s += '['; for (int i = 0; i < count; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// Un unico patron de NaN (el indefinido de la x87): con dos NaN distintos en una operacion la carga que sale depende del orden de operandos que elija el compilador
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
constexpr u32 kNan = 0xFFC00000u;
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return kNan;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInf(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }

// Misiones del ciclo libre (72 valores): 0..0x44 (69), 0x45 (primera sin destino), 0x80 y 0xFF (negativas con signo)
inline u32 CycleMission(u32 i) {
    const u32 k = i % 72;
    return k < 69 ? k : k == 69 ? 0x45u : k == 70 ? 0x80u : 0xFFu;
}

// Mutaciones (tras el evento numero mutAt el falso muta el estado; el oraculo repite la tabla):
//  1 mision (+0x3BA)   2 accion temporal (+0x3BB)   3 tiempo de la accion (+0x3BC)   4 CTimer::m_snTimeInMilliseconds   5 velocidad.x propia (+0x44)   6 posicion.x propia (matriz si existe, si no +4)
//  7 destino.x (+0x3EC)   8 distancia de seguimiento (+0x3DE)   9 semilla (+0x20, 16 bits)   10 banderas (+0x428)   11 posicion.x del objetivo (matriz si existe, si no +4)   12 velocidad.x del objetivo
//  13 velocidad.x del vehiculo 0 del jugador   14 adelante.x de la matriz propia   15 derecha.x de la matriz del vehiculo 0 del jugador   16 adelante.x de la matriz del vehiculo 1 del jugador
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 16;

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 mission{}, tact{}, ttime{}, follow{}, flags{}, rseed{}, now{};     // +0x3BA, +0x3BB, +0x3BC, +0x3DE (bytes salvo ttime), +0x428, +0x20 (16 bits), CTimer
    u32 oi{}, ti{}, pi[2]{};                                              // indices de modelo (1..6) del vehiculo, del objetivo y de los vehiculos del jugador
    u32 hasMat{}, pp[3]{}, mr[3]{}, mf[3]{}, mu[3]{}, mp[3]{};           // matriz opcional del vehiculo: posicion sin matriz (+4) y matriz (derecha, adelante, arriba, posicion)
    u32 speed[3]{}, dest[3]{};                                            // +0x44, +0x3EC
    u32 tgtNull{}, tmat{}, tpp[3]{}, tmp[3]{}, tspeed[3]{};              // objetivo: puntero nulo (solo mision 60), matriz opcional, posicion sin matriz (+4), posicion de la matriz, velocidad (+0x44)
    u32 pvNull{};                                                         // FindPlayerVehicle devuelve nulo
    u32 pr[2][3]{}, pf[2][3]{}, pu[2][3]{}, pp2[2][3]{}, pspeed[2][3]{};  // vehiculos del jugador: matriz (derecha, adelante, arriba, posicion) y velocidad
    u32 cmx[kModels]{}, cmy[kModels]{};                                   // max.x y max.y de la caja de colision de los modelos 1..6
    u32 pc[2][3]{}, psp[2][3]{}, mt[2][3]{}, nv[3]{};                     // FindPlayerCoors (dos llamadas), FindPlayerSpeed (dos), matriz x punto (dos) y la normalizacion
    u32 passenger{};                                                      // PickRandomPassenger devuelve un pasajero
    u32 o0[4]{}, ov[4]{};                                                 // salidas antes de la llamada y valores que escriben los falsos en ellas
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    const u32 sweep = n < kBaseCases ? n / 16 : (n - kBaseCases) % 1024;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Meters = [&](int r) -> u32 { return B(float(int(R(u32(16 * r + 1))) - 8 * r) / 8.0f); };    // multiplo de 1/8 en [-r, r]
    auto Dir = [&]() -> u32 { return B(float(int(R(33)) - 16) / 16.0f); };                            // multiplo de 1/16 en [-1, 1]
    auto Sp = [&] { return Special(n, nk()); };
    auto Gen = [&](u32 pct, u32 normal) { return P(pct) ? Sp() : normal; };                          // pct % de especiales

    // ---- vehiculo
    c.hasMat = P(75) ? 1u : 0u;
    for (u32 k = 0; k < 3; ++k) {
        c.mp[k] = k < 2 ? Meters(300) : Meters(10);
        c.pp[k] = k < 2 ? Meters(300) : Meters(10);
        c.mr[k] = Dir(); c.mf[k] = Dir(); c.mu[k] = Dir();
        c.speed[k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
        c.dest[k] = Meters(300);
        c.tpp[k] = Meters(300); c.tmp[k] = Meters(300);
        c.tspeed[k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
        c.nv[k] = P(15) ? Sp() : B(float(int(R(33)) - 16) / 16.0f);
        for (u32 j = 0; j < 2; ++j) {
            c.pr[j][k] = Dir(); c.pf[j][k] = Dir(); c.pu[j][k] = Dir(); c.pp2[j][k] = Meters(300);
            c.pspeed[j][k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
            c.pc[j][k] = Meters(300);
            c.psp[j][k] = P(10) ? Sp() : B(float(int(R(193)) - 96) / 32.0f);
            c.mt[j][k] = Meters(300);
        }
    }
    c.mission = CycleMission(sweep + f * 7u);
    c.tact = P(70) ? 0u : R(256);
    c.ttime = Rand(n, nk());
    c.follow = P(40) ? R(40) : R(256);
    c.flags = Rand(n, nk());
    c.rseed = Rand(n, nk()) & 0xFFFFu;
    c.now = P(8) ? R(4) : P(8) ? 0xFFFFFFFFu - R(4) : Rand(n, nk());
    c.oi = 1 + R(kModels); c.ti = 1 + R(kModels); c.pi[0] = 1 + R(kModels); c.pi[1] = 1 + R(kModels);
    for (u32 i = 0; i < kModels; ++i) {
        c.cmx[i] = P(8) ? Sp() : B(float(int(R(41)) + 1) / 16.0f * 2.0f);   // 0.125 .. 5.125
        c.cmy[i] = P(8) ? Sp() : B(float(int(R(81)) + 1) / 16.0f * 2.0f);
    }
    c.tgtNull = 0;
    c.tmat = P(60) ? 1u : 0u;
    c.pvNull = P(10) ? 1u : 0u;
    c.passenger = P(60) ? 1u : 0u;
    for (u32 k = 0; k < 4; ++k) { c.o0[k] = Rand(n, nk()); c.ov[k] = P(20) ? Sp() : Rand(n, nk()); }
    c.ov[3] &= 1u;
    c.mutAt = P(20) ? R(8) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    auto Near = [&](u32 (&target)[3], int spread) {   // el vehiculo cerca de (target.x, target.y): posicion sin matriz y de la matriz
        const float tx = F(target[0]), ty = F(target[1]);
        const float ox = float(int(R(u32(16 * spread + 1))) - 8 * spread) / 8.0f, oy = float(int(R(u32(16 * spread + 1))) - 8 * spread) / 8.0f;
        c.mp[0] = c.pp[0] = B(tx + ox); c.mp[1] = c.pp[1] = B(ty + oy);
        c.mp[2] = c.pp[2] = B(F(target[2]) + float(int(R(9)) - 4) / 4.0f);   // tambien cerca en altura: la distancia de la mision 44 es tridimensional
    };
    auto Align = [&](u32 (&a)[3], u32 (&b)[3], bool opposite) {   // a ~ +-b con una pequena perturbacion (producto escalar cercano a +-1)
        for (u32 k = 0; k < 3; ++k) {
            const float v = F(b[k]) + float(int(R(5)) - 2) / 32.0f;
            a[k] = B(opposite ? -v : v);
        }
    };
    switch (f) {
    case 0: case 1: case 2: case 14:
        break;
    case 3: case 4: case 5: case 6: {   // mision 3
        c.mission = 3; c.hasMat = 1;
        if (f == 3) { c.pvNull = P(8) ? 1u : 0u; }
        else {
            c.pvNull = P(4) ? 1u : 0u;
            c.rseed = (c.rseed & ~3u) | (f == 4 ? 3u : f == 5 ? (R(2) ? 1u : 0u) : (R(2) ? 2u : 0u));
            if (f == 4) { Align(c.pf[0], c.mf, false); Align(c.pf[1], c.mf, false); if (P(60)) { c.tact = 0; } }
            if (f == 5) { Align(c.pf[0], c.mf, P(60)); Align(c.pf[1], c.mf, P(60)); }
            if (f == 6) { for (u32 k = 0; k < 3; ++k) { c.pf[0][k] = Dir(); c.pf[1][k] = Dir(); } }
            if (f == 4 || f == 5) {   // el jugador cerca del vehiculo: la distancia al punto desplazado esta cerca del umbral (velocidad relativa * 12 + 2)
                Near(c.pc[0], P(70) ? 3 : 12); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1];
                c.pspeed[0][0] = c.pspeed[1][0] = c.speed[0];   // velocidad relativa pequena en x
                if (P(50)) { c.pspeed[0][1] = c.pspeed[1][1] = c.speed[1]; }
            }
            if (f == 5 || f == 6) { c.flags = (c.flags & ~1u) | (P(60) ? 1u : 0u); for (u32 j = 0; j < 2; ++j) { c.pspeed[j][0] = B(float(int(R(41)) - 10) / 16.0f); c.pspeed[j][1] = B(float(int(R(41)) - 10) / 16.0f); } }
            if (f == 6 && P(40)) { c.rseed &= ~1u; }
        }
        break;
    }
    case 7: case 8: {   // mision 44
        c.mission = 44;
        c.pvNull = f == 7 ? (P(15) ? 1u : 0u) : (P(5) ? 1u : 0u);
        if (f == 7) { Near(c.pc[0], P(70) ? 12 : 30); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1]; }
        else {
            // geometria: el jugador entre 10 y 40 m; los vectores adelante de los dos coches con signos distintos para que (fy + fx) * rel.x cambie de signo; las dos mitades (4 m delante / detras) a distancias distintas
            Near(c.pc[0], 40); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1];
            for (u32 j = 0; j < 2; ++j) for (u32 k = 0; k < 3; ++k) c.pf[j][k] = Dir();
            for (u32 j = 0; j < 2; ++j) { c.mt[j][0] = B(F(c.pc[0][0]) + float(int(R(65)) - 32) / 4.0f); c.mt[j][1] = B(F(c.pc[0][1]) + float(int(R(65)) - 32) / 4.0f); c.mt[j][2] = Meters(10); }
            if (P(30)) { c.mt[1][0] = c.mt[0][0]; c.mt[1][1] = c.mt[0][1]; c.mt[1][2] = c.mt[0][2]; }   // distancias iguales: gana "atras"
        }
        break;
    }
    case 9: case 10: {   // mision 53
        c.mission = 53;
        // objetivo cerca del vehiculo (a menos de follow + 10 m en la mayoria)
        {
            const float lim = float(c.follow & 0xFFu) + 10.0f;
            const float d = P(75) ? float(R(u32(lim * 8.0f) + 1)) / 8.0f : lim + float(int(R(41)) - 20) / 8.0f;
            const float ang = float(R(628)) / 100.0f;
            c.tpp[0] = c.tmp[0] = B(F(c.pp[0]) + d * std::cos(ang)); c.tpp[1] = c.tmp[1] = B(F(c.pp[1]) + d * std::sin(ang));
            c.mp[0] = c.pp[0]; c.mp[1] = c.pp[1];
            c.tpp[2] = c.tmp[2] = c.pp[2]; c.mp[2] = c.pp[2];
        }
        if (f == 9) { for (u32 k = 0; k < 2; ++k) { c.tspeed[k] = B(float(int(R(81)) - 8) / 32.0f); c.speed[k] = B(float(int(R(41)) - 10) / 64.0f); } }
        else { for (u32 k = 0; k < 2; ++k) { c.tspeed[k] = B(float(int(R(41)) - 10) / 64.0f); c.speed[k] = B(float(int(R(81)) - 8) / 32.0f); } }
        if (P(10)) { c.follow = R(4); }
        break;
    }
    case 11: case 12: {   // mision 60
        c.mission = 60;
        c.tgtNull = f == 11 ? (P(12) ? 1u : 0u) : 0u;
        if (P(30)) { c.speed[0] = c.speed[1] = B(0.0f); }
        break;
    }
    case 13: {
        static constexpr u32 missions[] = {16, 55, 18, 19, 29, 30, 31, 32};
        c.mission = missions[sweep % std::size(missions)];
        break;
    }
    case 15: case 16: {   // especiales
        static constexpr u32 missions[] = {3, 44, 53, 60, 16, 55, 18, 19, 29, 30, 31, 32};
        c.mission = f == 15 ? missions[sweep % std::size(missions)] : CycleMission(sweep);
        if (c.mission == 3) { c.hasMat = 1; c.rseed = (c.rseed & ~3u) | R(4); c.pvNull = P(5) ? 1u : 0u; Align(c.pf[0], c.mf, P(40)); Align(c.pf[1], c.mf, P(40)); Near(c.pc[0], 6); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1]; }
        if (c.mission == 44) { Near(c.pc[0], 20); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1]; }
        if (c.mission == 53) {
            c.tpp[0] = c.tmp[0] = B(F(c.pp[0]) + float(int(R(41)) - 20) / 4.0f); c.tpp[1] = c.tmp[1] = B(F(c.pp[1]) + float(int(R(41)) - 20) / 4.0f); c.mp[0] = c.pp[0]; c.mp[1] = c.pp[1];
        }
        {
            u32* t[] = {&c.speed[0], &c.speed[1], &c.speed[2], &c.mf[0], &c.mf[1], &c.mf[2], &c.mr[0], &c.mr[1], &c.mp[0], &c.mp[1], &c.pp[0], &c.pp[1], &c.dest[0], &c.dest[1],
                        &c.tpp[0], &c.tpp[1], &c.tmp[0], &c.tmp[1], &c.tspeed[0], &c.tspeed[1], &c.pf[0][0], &c.pf[0][1], &c.pf[1][0], &c.pf[1][1], &c.pr[0][0], &c.pr[0][1], &c.pr[1][0], &c.pr[1][1],
                        &c.pspeed[0][0], &c.pspeed[0][1], &c.pspeed[1][0], &c.pspeed[1][1], &c.pc[0][0], &c.pc[0][1], &c.pc[1][0], &c.pc[1][1], &c.psp[0][0], &c.psp[0][1], &c.psp[1][0], &c.psp[1][1],
                        &c.mt[0][0], &c.mt[0][1], &c.mt[1][0], &c.mt[1][1], &c.nv[0], &c.nv[1], &c.cmx[0], &c.cmx[1], &c.cmx[2], &c.cmx[3], &c.cmx[4], &c.cmx[5], &c.cmy[0], &c.cmy[1], &c.cmy[2], &c.cmy[3], &c.cmy[4], &c.cmy[5]};
            for (u32* p : t) if (P(8)) *p = Sp();
        }
        break;
    }
    default: {   // 17: barrido de mutaciones sobre las misiones con calculo propio
        static constexpr u32 missions[] = {3, 44, 53, 60};
        c.mission = missions[(sweep / kKinds) % std::size(missions)];
        c.mutAt = R(9); c.mutKind = 1u + sweep % kKinds;
        if (c.mission == 3) { c.hasMat = 1; c.rseed = (c.rseed & ~3u) | R(4); c.pvNull = 0; Align(c.pf[0], c.mf, P(30)); Align(c.pf[1], c.mf, P(30)); Near(c.pc[0], 4); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1]; c.flags |= 1u; c.passenger = 1; }
        if (c.mission == 44) { c.pvNull = 0; Near(c.pc[0], 20); c.pc[1][0] = c.pc[0][0]; c.pc[1][1] = c.pc[0][1]; }
        if (c.mission == 53) { c.tpp[0] = c.tmp[0] = B(F(c.pp[0]) + float(int(R(41)) - 20) / 4.0f); c.tpp[1] = c.tmp[1] = B(F(c.pp[1]) + float(int(R(41)) - 20) / 4.0f); c.mp[0] = c.pp[0]; c.mp[1] = c.pp[1]; }
        break;
    }
    }
    // mision 3: matriz propia siempre presente; las demas en el ciclo libre quedan como estan
    if (c.mission == 3) c.hasMat = 1;
    if (c.mission != 60) c.tgtNull = 0;

    // ---- reglas de seguridad
    {
        // mutaciones: valores validos
        if (c.mutKind == 1) c.mutVal = P(70) ? CycleMission(R(72)) : R(256);
        if (c.mutKind == 2 || c.mutKind == 8) c.mutVal = R(256);
        if (c.mutKind == 9) c.mutVal &= 0xFFFFu;
    }
    // un NaN con carga distinta del indefinido no se usa (la carga que sale depende del orden de operandos)
    auto Clean = [](u32& v) { if (IsNan(v)) v = kNan; };
    for (u32 k = 0; k < 3; ++k) {
        Clean(c.mp[k]); Clean(c.pp[k]); Clean(c.mr[k]); Clean(c.mf[k]); Clean(c.mu[k]); Clean(c.speed[k]); Clean(c.dest[k]); Clean(c.tpp[k]); Clean(c.tmp[k]); Clean(c.tspeed[k]); Clean(c.nv[k]);
        for (u32 j = 0; j < 2; ++j) { Clean(c.pr[j][k]); Clean(c.pf[j][k]); Clean(c.pu[j][k]); Clean(c.pp2[j][k]); Clean(c.pspeed[j][k]); Clean(c.pc[j][k]); Clean(c.psp[j][k]); Clean(c.mt[j][k]); }
    }
    for (u32 i = 0; i < kModels; ++i) { Clean(c.cmx[i]); Clean(c.cmy[i]); }
    for (u32 k = 0; k < 4; ++k) Clean(c.ov[k]);
    if (c.mutAt != kNoMutation && (c.mutKind == 4 || c.mutKind == 5 || c.mutKind == 6 || c.mutKind == 7 || c.mutKind == 11 || c.mutKind == 12 || c.mutKind == 13 || c.mutKind == 14 || c.mutKind == 15 || c.mutKind == 16)) Clean(c.mutVal);
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("mission", c.mission).U("tact", c.tact).U("ttime", c.ttime).U("follow", c.follow).U("flags", c.flags).U("rseed", c.rseed).U("now", c.now);
    j.U("oi", c.oi).U("ti", c.ti).A("pi", c.pi, 2).U("has_mat", c.hasMat).A("pp", c.pp, 3).A("mr", c.mr, 3).A("mf", c.mf, 3).A("mu", c.mu, 3).A("mp", c.mp, 3);
    j.A("speed", c.speed, 3).A("dest", c.dest, 3).U("tgt_null", c.tgtNull).U("tmat", c.tmat).A("tpp", c.tpp, 3).A("tmp", c.tmp, 3).A("tspeed", c.tspeed, 3).U("pv_null", c.pvNull);
    for (u32 k = 0; k < 2; ++k) {
        const std::string i = std::to_string(k);
        j.A(("pr" + i).c_str(), c.pr[k], 3).A(("pf" + i).c_str(), c.pf[k], 3).A(("pu" + i).c_str(), c.pu[k], 3).A(("pp2_" + i).c_str(), c.pp2[k], 3).A(("pspeed" + i).c_str(), c.pspeed[k], 3);
        j.A(("pc" + i).c_str(), c.pc[k], 3).A(("psp" + i).c_str(), c.psp[k], 3).A(("mt" + i).c_str(), c.mt[k], 3);
    }
    j.A("cmx", c.cmx, kModels).A("cmy", c.cmy, kModels).A("nv", c.nv, 3).U("passenger", c.passenger).A("o0", c.o0, 4).A("ov", c.ov, 4);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}
