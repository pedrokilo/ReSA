#pragma once
// Fixtures del banco WalkRoundNext31: CTaskComplexWalkRoundObject::CreateNextSubTask (0x657220, thiscall, 1 argumento (ped), `ret 4`, marco SEH propio).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this`: m_pSubTask (+8, tarea falsa con vtable propia: GetTaskType = ranura 4), m_targetPoint (+0x10), m_object (+0x1C), m_pointRoute (+0x20, solo el contador +0 y,
// con contador 0, se le anaden hasta dos puntos). Llama (todos SUSTITUIDOS por falsos identicos en ambos lados): CPedGeometryAnalyser::ComputeClosestSurfacePoint(ped) 0x5F3B70 y (posn) 0x5F36F0,
// ComputeEntityHitSide(punto, entidad) 0x5F3AC0 y CTaskComplexWalkRoundObject::CreateRouteTask 0x655140. Reserva en el pool de tareas (CTask::operator new 0x61A5A0) UNA tarea:
// CTaskComplexGoToPointAndStandStill (0x28) -> 0x668120 (4 = PEDMOVE_WALK, punto, 0.5f, 2.0f, false, false), solo si los lados del punto y del destino difieren.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace WalkRoundNext31Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4608;   // 16 familias x 288

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda); sumas a mano
constexpr u32 kThisBytes = 0x54;                        // sizeof(CTaskComplexWalkRoundObject)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;      // 0x40 + 0x54 = 0x94 <= 0x100
constexpr u32 kSubBytes = 0x20;                         // tarea falsa (vtable propia en +0, tipo en +0x10)
constexpr u32 kSubOff = 0x20, kSubArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kObjBytes = 0x20;                         // CEntity falsa (solo se pasa por puntero a los falsos)
constexpr u32 kObjOff = 0x20, kObjArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kPedBytes = 0x20;                         // CPed falso (solo se pasa por puntero a los falsos)
constexpr u32 kPedOff = 0x20, kPedArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kRouteBytes = 0x64;                       // CPointRoute: contador (+0) y 8 vectores (+4)
constexpr u32 kRouteOff = 0x20, kRouteArena = 0x100;    // 0x20 + 0x64 = 0x84 <= 0x100
constexpr u32 kRouteWords = kRouteBytes / 4;            // 25
constexpr u32 kSlot = 0x80;                             // almacenamiento de una ranura del pool de tareas
constexpr u32 kWindowMax = 1;                           // primeras ranuras libres que se registran (= nº de reservas de la funcion)
constexpr u32 kAllocs = 1;
constexpr u32 kSubTaskAt = 0x8, kMoveAt = 0xC, kTargetAt = 0x10, kObjectAt = 0x1C, kRouteAt = 0x20;
constexpr u32 kSubTypeAt = 0x10;
constexpr u32 kTypeGoToStand = 0x387, kTypeAchieve = 0x386;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, objNull, subType, routeCount, routeRet;
    u32 routeInit[kRouteWords - 1];   // las 24 palabras de los ocho vectores; la palabra 0 es el contador
    u32 target[3], p1[3], p2[3];
    i32 side1, side2;
    u32 moveState;
    u32 poolMode, poolK;
    u32 eax, edx, stack;
};

inline constexpr u32 kFloatEdges[]{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x4B800000u, 0x4B800001u, 0x4B000001u, 0x4B7FFFFFu, 0x4B000000u, 0x4EFFFFFFu, 0x4F000000u, 0xCF000000u,
    0x5F000000u, 0x5F7FFFFFu, 0xDF000000u, 0x00000001u, 0x007FFFFFu, 0x00800000u, 0x3F000000u, 0x3FC00000u, 0x40400000u, 0xC0400000u,
    0xCDCDCDCDu, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x40000000u, 0x3F000001u, 0x3EFFFFFFu, 0x7E967699u, 0xFE967699u, 0x20000000u, 0x1F800000u};
inline constexpr u32 kTypes[]{0x387, 0x386, 0xCB, 0x389, 0x3F, 0x0, 0x1F4, 0x516, 0xFFFFFFFFu, 0x385, 0x388, 0x38A};
inline constexpr i32 kSideEdges[]{0, 1, 2, 3, -1, 4, 7, 8, 255, INT32_MIN, INT32_MAX, 0x10000, -2};
inline constexpr u32 kCounts[]{1, 2, 3, 7, 8, 9, 100, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x100u, 0x01000000u};

inline u32 FloatBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// extremeP: probabilidad (sobre 100) de elegir un valor de borde
inline u32 PtBits(u32 n, u32 k, u32 extremeP) {
    const u32 r = Rand(n, k);
    if ((r >> 8) % 100 < extremeP) return kFloatEdges[Rand(n, k + 700) % std::size(kFloatEdges)];
    return FloatBits(float(i32(Rand(n, k + 300) % 80001u) - 40000) * 0.125f);   // multiplos exactos de 1/8 en [-5000, 5000]
}
inline void Triple(u32 n, u32 k, u32 extremeP, u32* out) { for (u32 j = 0; j < 3; ++j) out[j] = PtBits(n, k + j, extremeP); }

// Familias (n % 16, 288 casos cada una):
//  0 sin objeto                                  1 subtarea GoToPointAndStandStill (0x387): crea la ruta           2 AchieveHeading con ruta ya cargada (contador != 0)
//  3 AchieveHeading, ruta vacia, mismo lado       4 AchieveHeading, ruta vacia, lados distintos (reserva)            5 otros tipos (reposo 0xCB, ruta 0x389, desconocidos): nulo
//  6 contadores de ruta extremos (distintos de 0) 7 lados en el borde (INT_MIN, INT_MAX, -1 ...)                      8 puntos con flotantes extremos
//  9 CreateRouteTask devuelve nulo o valores raros 10 pool agotado en lados distintos                                 11 pool con pocas libres
//  12..15 mezclas
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    // --- subtarea
    const u32 tr = Rand(n, 2) % 100;
    c.subType = tr < 15 ? kTypeGoToStand : tr < 70 ? kTypeAchieve : kTypes[Rand(n, 3) % std::size(kTypes)];
    if (f == 1) c.subType = kTypeGoToStand;
    if (f == 2 || f == 3 || f == 4 || f == 6 || f == 7 || f == 8 || f == 9 || f == 10 || f == 11) c.subType = Rand(n, 3) % 20 == 0 ? kTypes[Rand(n, 4) % std::size(kTypes)] : kTypeAchieve;
    if (f == 5) { constexpr u32 t[]{0xCB, 0x389, 0x3F, 0x0, 0x1F4, 0x516, 0xFFFFFFFFu, 0x385, 0x388, 0x38A}; c.subType = t[Rand(n, 3) % std::size(t)]; }
    c.objNull = f == 0 ? 1u : (f >= 12 && Rand(n, 5) % 14 == 0) ? 1u : 0u;
    // --- ruta
    const bool emptyRoute = (f == 3 || f == 4 || f == 7 || f == 8 || f == 10 || f == 11) ? true : (f == 2 || f == 6 || f == 1 || f == 5 || f == 9) ? Rand(n, 6) % 5 == 0 : Rand(n, 6) % 2 == 0;
    c.routeCount = emptyRoute ? 0u : (f == 6 ? kCounts[Rand(n, 7) % std::size(kCounts)] : (Rand(n, 7) % 3 == 0 ? kCounts[Rand(n, 8) % std::size(kCounts)] : 1u + Rand(n, 9) % 8));
    for (u32 j = 0; j < kRouteWords - 1; ++j) c.routeInit[j] = Rand(n, 100 + j);
    c.routeRet = f == 9 ? (Rand(n, 10) % 3 == 0 ? 0u : Rand(n, 11) % 3 == 0 ? 0xFFFFFFFFu : Rand(n, 12)) : (Rand(n, 10) % 10 == 0 ? 0u : 0x77000000u | (Rand(n, 12) & 0xFFFFFFu));
    // --- puntos
    const u32 extremeP = f == 8 ? 55 : 6;
    Triple(n, 20, extremeP, c.target);
    Triple(n, 23, extremeP, c.p1);
    Triple(n, 26, extremeP, c.p2);
    // --- lados (ComputeEntityHitSide): iguales con ~50 %
    const bool same = (f == 3) ? true : (f == 4 || f == 10) ? false : Rand(n, 30) % 2 == 0;
    {
        const u32 r = Rand(n, 31) % 10;
        c.side1 = (f == 7 || r < 2) ? kSideEdges[Rand(n, 32) % std::size(kSideEdges)] : i32(Rand(n, 33) % 5u) - 0;
        c.side2 = same ? c.side1 : ((f == 7 || r < 2) ? kSideEdges[Rand(n, 34) % std::size(kSideEdges)] : i32(Rand(n, 35) % 5u));
        if (!same && c.side2 == c.side1) c.side2 = c.side1 + 1;
        if (same && f == 7 && Rand(n, 36) % 4 == 0) c.side1 = c.side2 = INT32_MIN;
    }
    c.moveState = Rand(n, 37) % 3 == 0 ? Rand(n, 38) : Rand(n, 38) % 9;
    // --- pool (una reserva)
    const u32 pr = Rand(n, 90);
    if (f == 10) { constexpr u32 m[]{7, 7, 7, 2, 3, 4}; c.poolMode = m[pr % 6]; c.poolK = 1; }
    else if (f == 11) { constexpr u32 m[]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 1; }
    else { constexpr u32 modes[8]{0, 0, 1, 6, 2, 3, 4, 7}; c.poolMode = modes[pr % 8]; c.poolK = 1 + (pr >> 8) % 4; }
    c.eax = Rand(n, 70);
    c.edx = Rand(n, 71);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    return c;
}

// ------------------------------------------------------------------------------------------------ pool sintetico
struct Pool {
    std::vector<u8> states;   // bit 7 = libre, bits 0..6 = id
    long last{};              // m_LastFreeSlot
    u32 freeTotal{};
    struct W { u32 idx, state; };
    std::vector<W> win;       // hasta kWindowMax primeras ranuras libres en orden de busqueda del EXE (last+1..cap-1, luego 0..cap-1)
};

inline u8 IdOf(u32 n, u32 i) {
    const u32 r = Rand(n, 40000 + i);
    constexpr u8 edge[4]{0, 1, 0x7E, 0x7F};
    return r % 8 == 0 ? edge[(r >> 8) % 4] : u8((r >> 8) & 0x7F);
}

// poolMode: 0 natural (10 % libres), 1 todo libre, 2 k libres al azar, 3 k libres justo detras del cursor, 4 vuelta (cursor al final), 5 libres solo en/antes del cursor, 6 mitad libres, 7 ninguna libre.
inline Pool MakePool(const C& c, u32 cap, u32 n) {
    Pool p;
    p.states.assign(cap, 0);
    std::vector<u8> isFree(cap, 0);
    auto pickLast = [&]() -> long {
        switch (Rand(n, 80) % 7) {
        case 0: return 0l;
        case 1: return long(cap) - 1;
        case 2: return long(cap) - 2;
        case 3: return 1l;
        default: return long(Rand(n, 81) % cap);
        }
    };
    switch (c.poolMode) {
    case 0: for (u32 i = 0; i < cap; ++i) isFree[i] = Rand(n, 20000 + i) % 10 == 0; p.last = pickLast(); break;
    case 1: std::fill(isFree.begin(), isFree.end(), u8(1)); p.last = pickLast(); break;
    case 6: for (u32 i = 0; i < cap; ++i) isFree[i] = Rand(n, 20000 + i) % 2 == 0; p.last = pickLast(); break;
    case 2: {
        p.last = pickLast();
        for (u32 j = 0, placed = 0; placed < c.poolK && j < 4096; ++j) {
            const u32 i = Rand(n, 300 + j) % cap;
            if (!isFree[i]) { isFree[i] = 1; ++placed; }
        }
        break;
    }
    case 3: {   // k libres consecutivas justo detras del cursor
        p.last = pickLast();
        for (u32 j = 0; j < c.poolK; ++j) isFree[(u32(p.last) + 1 + j) % cap] = 1;
        break;
    }
    case 4: {   // el cursor esta al final: libres al principio del pool
        const long lasts[3]{long(cap) - 1, long(cap) - 2, long(cap) - 3};
        p.last = lasts[Rand(n, 82) % 3];
        for (u32 j = 0; j < c.poolK; ++j) isFree[j] = 1;
        break;
    }
    case 5: {   // libres solo en el cursor y antes de el: se encuentran tras dar la vuelta
        p.last = long(kAllocs + Rand(n, 83) % (cap - 2 * kAllocs));
        for (u32 j = 0; j < c.poolK && long(j) <= p.last; ++j) isFree[u32(p.last) - j] = 1;
        break;
    }
    case 7: p.last = pickLast(); break;   // ninguna libre
    default: break;
    }
    for (u32 i = 0; i < cap; ++i) {
        p.states[i] = u8((isFree[i] ? 0x80u : 0u) | IdOf(n, i));
        p.freeTotal += isFree[i];
    }
    u32 start = u32(p.last) + 1;
    if (start == cap) start = 0;
    for (u32 pass = 0; pass < 2 && p.win.size() < kWindowMax; ++pass) {
        const u32 from = pass == 0 ? start : 0, to = pass == 0 ? cap : start;
        for (u32 i = from; i < to && p.win.size() < kWindowMax; ++i)
            if (isFree[i]) p.win.push_back({i, p.states[i]});
    }
    return p;
}
}  // namespace WalkRoundNext31Fix
