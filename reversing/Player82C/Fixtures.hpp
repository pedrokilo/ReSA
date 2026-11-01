#pragma once
// Fixtures del banco Player82C (lote 82C): CPlayerPed::SetRealMoveAnim (EXE 0x60A9C0, thiscall, ECX = jugador, sin argumentos, `ret`, void).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::SetRealMoveAnim  0x60A9C0  (hook CPlayerPed / SetRealMoveAnim, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n % 16 = familia (1024 casos por familia, 16384 casos).
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

namespace Pm82CFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Animaciones que la funcion pide al empezar, EN ESTE ORDEN de indice: ids 0,1,2,5,3,6,7,0x89,0x8A,0x0A
enum : unsigned { aWalk, aRun, aSprint, aWalkStart, aIdle, aRunStop, aRunStopR, aStrafeA, aStrafeB, aTired, kAssocs };
constexpr unsigned kNewAssocs = 4;   // animaciones nuevas que entregan BlendAnimation/AddAnimation (una por llamada)
constexpr u32 kIdOf[kAssocs]{0, 1, 2, 5, 3, 6, 7, 0x89, 0x8A, 0x0A};

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPedWin = 0x800, kPdWin = 0x100, kAssocWin = 0x40, kHierWin = 0x20, kMatWin = 0x80, kSmallWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPedArena = Arena(kPedWin), kPdArena = Arena(kPdWin), kAssocArena = Arena(kAssocWin), kHierArena = Arena(kHierWin), kMatArena = Arena(kMatWin), kSmallArena = Arena(kSmallWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 82103u); }
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
    template<std::size_t N> Js& A(const char* k, const u32 (&v)[N]) { Key(k); s += '['; for (std::size_t i = 0; i < N; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    std::string End() const { return s + "}"; }
};
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// Valores especiales de float (0, -0, desnormales, uno, maximos, infinitos, NaN con carga util)
inline u32 SpecialF(u32 n, u32 k) {
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x7F7FFFFFu, 0xFF7FFFFFu,
                              0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x3F000000u, 0x42C80000u, 0x41200000u, 0x42700000u};
    return sp[Rand(n, k) % std::size(sp)];
}
// Elige entre una lista de floats (como patrones) con un 1/10 de especiales y un 1/10 de valores anchos
inline u32 Pick(u32 n, u32 k, const float* list, std::size_t count) {
    const u32 r = Rand(n, k) % 20;
    if (r == 0) return SpecialF(n, k + 1);
    if (r == 1) { const u32 e = 127u + (Rand(n, k + 2) % 41u) - 20u; return (Rand(n, k + 3) & 0x80000000u) | (e << 23) | (Rand(n, k + 4) & 0x7FFFFFu); }
    return B(list[Rand(n, k + 5) % count]);
}
template<std::size_t N> inline u32 Pick(u32 n, u32 k, const float (&list)[N]) { return Pick(n, k, list, N); }

inline constexpr float kRatios[]{0.0f, -0.0f, 0.1f, 0.3f, 0.39999998f, 0.4f, 0.40000004f, 0.5f, 0.99f, 1.0f, 1.0000001f, 1.01f, 1.5f, 1.9999999f, 2.0f, 2.5f, 7.0f, -1.0f};
inline constexpr float kAmounts[]{0.0f, -0.0f, 0.005f, 0.01f, 0.010000001f, 0.5f, 0.79999995f, 0.8f, 0.80000007f, 0.99f, 1.0f, 1.5f, -1.0f, 0.001f};
inline constexpr float kDeltas[]{0.0f, -0.0f, -8.0f, -1000.0f, -0.001f, 0.001f, 8.0f, 1.0f, -1.0f, 2.0f, -2.0f, -4.0f};
inline constexpr float kTimes[]{0.0f, 0.1f, 0.25f, 0.5f, 0.75f, 0.99f, 1.0f, 1.5f, 2.0f, 5.0f, -0.5f};
inline constexpr float kSteps[]{0.0f, 0.016f, 0.02f, 0.05f, 0.1f, 0.5f, -0.1f};
inline constexpr float kTotals[]{0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 3.0f, 5.0f, 0.3f};
inline constexpr float kTimeCanRun[]{-1.0f, -0.0f, 0.0f, 0.5f, 100.0f, -100.0f, 1.0f, -0.001f};
inline constexpr float kSprintRet[]{0.0f, 0.5f, 0.99999994f, 1.0f, 1.0000001f, 1.5f, 2.0f, -1.0f, 5.0f};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 mask{};                         // bit i: la animacion i existe (orden de enum aWalk..aTired)
    u32 flags[kAssocs]{}, amount[kAssocs]{}, delta[kAssocs]{}, cur[kAssocs]{}, speed[kAssocs]{}, step[kAssocs]{};
    u32 total[3]{};                     // jerarquia de runStop, sprint y walkStart (+0x10)
    u32 newInit[kNewAssocs][3]{};       // indicadores, mezcla y delta iniciales de las animaciones nuevas
    u32 pedFlags{}, moveState{}, group{}, headRate{};
    u32 ratio{}, canRun{}, pdFlags{}, advEnd{};
    u32 timer{}, timeScale0{}, cheat{}, camIdx{}, camMode[3]{};
    u32 forceRet{}, aimRet[3]{};
    u32 taskRet[5]{}, sphereRet{}, motionRet{}, sprintRet{};
    u32 hasMatrix{}, pl[3]{}, pm[3]{};
};

inline u32 Flags16(u32 n, u32 k, bool playing) { return (Rand(n, k) & 0xFFFEu) | (playing ? 1u : 0u); }
inline u32 Coord(u32 n, u32 k) { return B(float(i32(Rand(n, k) % 600001) - 300000) / 100.0f); }

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    // cada animacion existe casi siempre (algunas con mas frecuencia que otras segun el papel)
    c.mask = 0;
    for (unsigned i = 0; i < kAssocs; ++i) {
        static constexpr unsigned presence[kAssocs]{80, 80, 50, 25, 50, 20, 15, 30, 30, 40};   // porcentaje
        if (Rand(n, 10 + i) % 100 < presence[i]) c.mask |= 1u << i;
        c.flags[i] = Flags16(n, 20 + i, Rand(n, 30 + i) % 3 == 0);
        c.amount[i] = Pick(n, 40 + 6 * i, kAmounts); c.delta[i] = Pick(n, 100 + 6 * i, kDeltas);
        c.cur[i] = Pick(n, 160 + 6 * i, kTimes); c.speed[i] = Rand(n, 220 + i); c.step[i] = Pick(n, 230 + 6 * i, kSteps);
    }
    for (unsigned i = 0; i < 3; ++i) c.total[i] = Pick(n, 290 + 6 * i, kTotals);
    for (unsigned i = 0; i < kNewAssocs; ++i) { c.newInit[i][0] = Flags16(n, 320 + 3 * i, false); c.newInit[i][1] = Pick(n, 340 + 6 * i, kAmounts); c.newInit[i][2] = Pick(n, 380 + 6 * i, kDeltas); }
    c.pedFlags = Rand(n, 410) % 3 == 0 ? (Rand(n, 411) | 1u) : (Rand(n, 412) & ~1u);
    c.moveState = Rand(n, 413) % 4 == 0 ? 7u : Rand(n, 414) % 10;
    c.group = Rand(n, 415) % 4 ? Rand(n, 416) % 118 : Rand(n, 417);
    c.headRate = Rand(n, 418);
    c.ratio = Pick(n, 420, kRatios); c.canRun = Pick(n, 430, kTimeCanRun);
    c.pdFlags = Rand(n, 440) % 3 == 0 ? (Rand(n, 441) | 2u) : (Rand(n, 442) & ~2u);
    c.timer = Rand(n, 443); c.advEnd = Rand(n, 444) % 2 ? c.timer - (Rand(n, 445) % 5000) : c.timer + (Rand(n, 446) % 5000);
    if (Rand(n, 447) % 8 == 0) c.advEnd = c.timer - (Rand(n, 448) % 3) + 1;
    c.timeScale0 = Rand(n, 449);
    c.cheat = Rand(n, 450) % 4 == 0 ? (Rand(n, 451) | 1u) & 0xFF : (Rand(n, 452) & 0xFE);
    c.camIdx = Rand(n, 453) % 3;
    for (unsigned k = 0; k < 3; ++k) c.camMode[k] = Rand(n, 454 + k) % 3 == 0 ? (Rand(n, 457 + k) % 2 ? 4u : 15u) : (Rand(n, 460 + k) % 4 == 0 ? (0x10000u | (Rand(n, 463 + k) % 2 ? 4u : 15u)) : Rand(n, 466 + k) % 70);
    c.forceRet = Rand(n, 470) % 3 ? ((Rand(n, 471) & 0xFFFFFF00u) | 1u) : (Rand(n, 472) & 0xFFFFFF00u);
    for (unsigned i = 0; i < 3; ++i) {
        static constexpr u32 sp[]{0u, 1u, 0x8000u, 0x7FFFu, 0xFFFFu, 0xFFFFFFFFu, 0x10000u, 0x20000u, 0x80u, 0xFF80u};
        c.aimRet[i] = Rand(n, 473 + i) % 3 == 0 ? sp[Rand(n, 476 + i) % std::size(sp)] : ((Rand(n, 479 + i) & 0xFFFF0000u) | (Rand(n, 482 + i) % 2 ? (Rand(n, 485 + i) % 256) : u32(0x10000 - Rand(n, 488 + i) % 256)));
    }
    for (unsigned i = 0; i < 5; ++i) c.taskRet[i] = Rand(n, 500 + i) % 4 ? 0u : Rand(n, 505 + i) | 1u;
    c.sphereRet = Rand(n, 510) % 5 ? 0u : Rand(n, 511) | 1u;
    c.motionRet = Rand(n, 512) % 3 == 0 ? 0x37u : (Rand(n, 513) % 6 == 0 ? Rand(n, 514) : Rand(n, 515) % 118);
    c.sprintRet = Pick(n, 516, kSprintRet);
    c.hasMatrix = Rand(n, 530) % 3 != 0;
    for (unsigned i = 0; i < 3; ++i) { c.pl[i] = Coord(n, 531 + i); c.pm[i] = Coord(n, 534 + i); }
    return c;
}

inline void Mark(C& c, unsigned i, bool exists) { if (exists) c.mask |= 1u << i; else c.mask &= ~(1u << i); }
inline void Playing(C& c, unsigned i, bool on) { c.flags[i] = (c.flags[i] & ~1u) | (on ? 1u : 0u); }

inline C MakeC(unsigned n) {
    C c = BaseDefaults(n);
    const auto r = [&](u32 k) { return Rand(n, 600 + k); };
    switch (c.family) {
    case 0:   // runStop o runStopR reproduciendose (estado = correr)
        Mark(c, aRunStop, r(1) % 4 != 0); Mark(c, aRunStopR, r(2) % 3 == 0);
        if (r(3) % 4) { Playing(c, aRunStop, r(4) % 2); Playing(c, aRunStopR, r(5) % 2); if (!((c.flags[aRunStop] | c.flags[aRunStopR]) & 1)) Playing(c, r(6) % 2 ? aRunStop : aRunStopR, true); }
        break;
    case 1:   // ninguno se reproduce: se desvanecen (delta >= 0) con y sin la animacion de reposo
        Mark(c, aRunStop, r(1) % 2); Mark(c, aRunStopR, r(2) % 2); Playing(c, aRunStop, false); Playing(c, aRunStopR, false); Mark(c, aIdle, r(3) % 2);
        break;
    case 2: case 3:   // ratio 0 y sin sprint: apuntado lateral (camara modo 4 y pad con direccion forzada)
        Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aSprint, false); c.ratio = r(1) % 3 ? 0u : 0x80000000u;
        c.forceRet |= 1u; c.camMode[c.camIdx] = (r(2) % 6 == 0 ? 0x10000u : 0u) | 4u; Mark(c, c.family == 2 ? aStrafeA : aStrafeB, r(3) % 2);
        if (r(4) % 4) { c.aimRet[0] = (r(5) % 2 ? 1u + r(6) % 100 : 0x10000u - 1u - r(7) % 100) | (r(13) % 3 == 0 ? 0x30000u : 0u);
                        c.aimRet[1] = c.family == 2 ? (0x10000u - 1u - r(8) % 100) : (r(9) % 100); c.aimRet[2] = (r(10) % 2 ? r(11) % 130 : 0x10000u - r(12) % 130) & 0xFFFFu; }
        else { c.aimRet[0] = 1u + r(14) % 3; c.aimRet[1] = c.family == 2 ? 0xFFFFu - r(15) % 3 : r(16) % 3; c.aimRet[2] = r(17) % 3 ? r(18) % 4 : 0u; }
        break;
    case 4: case 5:   // ratio 0 y sin sprint, sin apuntar: cansancio
        Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aSprint, false); c.ratio = r(1) % 3 ? 0u : 0x80000000u;
        if (r(2) % 2) c.forceRet &= ~0xFFu;
        if (c.family == 4) { c.canRun = B(-float(r(3) % 100 + 1) / 10.0f); for (unsigned i = 0; i < 5; ++i) c.taskRet[i] = 0; c.sphereRet = 0; }
        else { if (r(4) % 3 == 0) c.canRun = B(-1.0f); for (unsigned i = 0; i < 5; ++i) if (r(5 + i) % 3 == 0) c.taskRet[i] = 1u + r(10 + i); if (r(15) % 3 == 0) c.sphereRet = 1u; }
        Mark(c, aTired, r(16) % 2);
        break;
    case 6: case 7:   // ratio distinto de 0 (o sprint): rama principal con y sin reposo
        if (r(1) % 4) c.ratio = Pick(n, 700, kRatios);
        if (c.ratio == 0u || c.ratio == 0x80000000u) c.ratio = B(0.5f + float(r(2) % 100) / 50.0f);
        Mark(c, aIdle, c.family == 6 ? r(3) % 5 != 0 : r(3) % 4 == 0);
        if (r(4) % 2) { Mark(c, aRunStop, false); Mark(c, aRunStopR, false); }
        else { Playing(c, aRunStop, false); Playing(c, aRunStopR, false); c.delta[aRunStop] = B(-1.0f - float(r(5) % 8)); c.delta[aRunStopR] = B(-1.0f); }
        if (c.family == 7) { Mark(c, aWalkStart, true); c.moveState = r(6) % 2 ? 7u : r(7) % 10; if (r(8) % 3 == 0) { const float cur = F(c.cur[aWalkStart]), st = F(c.step[aWalkStart]); c.total[2] = B(cur + st); if (r(9) % 2) c.total[2] = B(F(c.total[2]) * 0.99999994f); } }
        break;
    case 8: case 9:   // con sprint: el bloque del sprint
        Mark(c, aSprint, true); Mark(c, aRun, true); Mark(c, aWalk, true); Mark(c, aWalkStart, false); Mark(c, aIdle, false); Mark(c, aRunStop, false); Mark(c, aRunStopR, false);
        c.moveState = c.family == 8 ? (r(1) % 3 ? 7u : r(2) % 10) : (r(3) % 4 ? r(4) % 10 : 7u);
        if (c.family == 9 && r(5) % 2) c.ratio = B(float(r(6) % 40) / 100.0f);
        if (r(7) % 3 == 0) c.total[1] = B(F(c.cur[aSprint]) * 2.0f);
        if (r(8) % 6 == 0) c.amount[aSprint] = B(0.0f);
        break;
    case 10:  // walkStart presente sin reposo (se cierra el flujo)
        Mark(c, aWalkStart, true); Mark(c, aIdle, false); Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aWalk, true); Mark(c, aRun, true);
        Playing(c, aWalkStart, r(1) % 4 != 0); if (r(2) % 3 == 0) c.total[2] = B(F(c.cur[aWalkStart]) + F(c.step[aWalkStart]));
        if (c.ratio == 0u || c.ratio == 0x80000000u) c.ratio = B(1.0f);
        break;
    case 11:  // sin sprint y estado 7 (correr hacia el sprint)
        Mark(c, aSprint, false); Mark(c, aWalkStart, false); Mark(c, aIdle, false); Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aWalk, true); Mark(c, aRun, true);
        c.moveState = 7u; if (c.ratio == 0u || c.ratio == 0x80000000u) c.ratio = B(1.0f);
        break;
    case 12:  // con sprint, estado 7 y ratio >= 0.4
        Mark(c, aSprint, true); Mark(c, aWalkStart, false); Mark(c, aIdle, false); Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aWalk, true); Mark(c, aRun, true);
        c.moveState = 7u; c.ratio = B(0.4f + float(r(1) % 200) / 100.0f);
        break;
    case 13:  // sin sprint y estado distinto de 7: mezcla andar/correr
        Mark(c, aSprint, false); Mark(c, aWalkStart, false); Mark(c, aIdle, false); Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aWalk, r(1) % 5 != 0); Mark(c, aRun, r(2) % 5 != 0);
        c.moveState = r(3) % 9 == 0 ? 7u : (r(4) % 7 == 0 ? 6u : 1u + r(5) % 5);
        if (c.moveState == 7u) c.moveState = 3u;
        if (r(6) % 6 != 0) c.ratio = Pick(n, 710, kRatios);
        if (c.ratio == 0u || c.ratio == 0x80000000u) c.ratio = B(0.25f);
        break;
    case 14:  // adrenalina, camara fija y velocidad del sprint
        Mark(c, aSprint, r(1) % 4 != 0); c.pdFlags |= r(2) % 4 ? 2u : 0u; c.camMode[c.camIdx] = r(3) % 3 == 0 ? 15u : (r(4) % 2 ? 0x30000u | 15u : c.camMode[c.camIdx]);
        c.ratio = B(0.5f + float(r(5) % 100) / 40.0f); Mark(c, aIdle, false); Mark(c, aWalkStart, false); Mark(c, aRunStop, false); Mark(c, aRunStopR, false); Mark(c, aWalk, true); Mark(c, aRun, true);
        if (r(6) % 3 == 0) c.advEnd = c.timer - 1u - r(7) % 3; else if (r(8) % 3 == 0) c.advEnd = c.timer + r(9) % 3;
        break;
    default:  // 15: mezcla amplia con mas especiales
        for (unsigned i = 0; i < kAssocs; ++i) if (Rand(n, 800 + i) % 12 == 0) c.amount[i] = SpecialF(n, 820 + i);
        for (unsigned i = 0; i < kAssocs; ++i) if (Rand(n, 840 + i) % 12 == 0) c.delta[i] = SpecialF(n, 860 + i);
        if (Rand(n, 880) % 10 == 0) c.ratio = SpecialF(n, 881);
        if (Rand(n, 884) % 10 == 0) c.canRun = SpecialF(n, 885);
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("mask", c.mask);
    j.A("flags", c.flags).A("amount", c.amount).A("delta", c.delta).A("cur", c.cur).A("speed", c.speed).A("step", c.step).A("total", c.total);
    j.U("new_f0", c.newInit[0][0]).U("new_a0", c.newInit[0][1]).U("new_d0", c.newInit[0][2]).U("new_f1", c.newInit[1][0]).U("new_a1", c.newInit[1][1]).U("new_d1", c.newInit[1][2]);
    j.U("new_f2", c.newInit[2][0]).U("new_a2", c.newInit[2][1]).U("new_d2", c.newInit[2][2]).U("new_f3", c.newInit[3][0]).U("new_a3", c.newInit[3][1]).U("new_d3", c.newInit[3][2]);
    j.U("ped_flags", c.pedFlags).U("move_state", c.moveState).U("group", c.group).U("head_rate", c.headRate);
    j.U("ratio", c.ratio).U("can_run", c.canRun).U("pd_flags", c.pdFlags).U("adv_end", c.advEnd);
    j.U("timer", c.timer).U("time_scale0", c.timeScale0).U("cheat", c.cheat).U("cam_idx", c.camIdx).A("cam_mode", c.camMode);
    j.U("force_ret", c.forceRet).A("aim_ret", c.aimRet).A("task_ret", c.taskRet).U("sphere_ret", c.sphereRet).U("motion_ret", c.motionRet).U("sprint_ret", c.sprintRet);
    j.U("has_matrix", c.hasMatrix).A("pl", c.pl).A("pm", c.pm);
    return j.End();
}
}
