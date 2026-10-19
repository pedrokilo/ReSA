#pragma once
// Fixtures del banco WalkRoundFirst31: CTaskComplexWalkRoundObject::CreateFirstSubTask (0x657380, thiscall, 1 argumento (ped), `ret 4`, marco SEH propio).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this`: m_moveState (+0xC), m_object (+0x1C), m_pointRoute (+0x20); escribe m_pointRoute->m_NumEntries = 0, field_24 (= reloj 0xB7CB84), field_28 (8000 si moveState == 4, si no 4000),
// field_2C = 1 y la posicion/frente/derecha del objeto en +0x30/+0x3C/+0x48 (del objeto: posicion de la matriz o del placement, m_matrix (+0x14; si falta, AllocateMatrix 0x54F560 + UpdateMatrix 0x54F1B0, falsos)).
// Llama (SUSTITUIDOS por falsos identicos en ambos lados): 0x6551D0 (sin simbolo: rellena la ruta y devuelve un float en ST0) y CreateRouteTask 0x655140. Del peaton: matriz (+0x14: posicion +0x30, frente +0x10).
// REALES en ambos lados: CVector::NormaliseAndMag 0x59C970, CGeneral::GetRadianAngleBetweenPoints 0x53CBE0 y LimitRadianAngle 0x53CB50. Reserva en el pool (0x61A5A0) UNA tarea: CTaskSimpleAchieveHeading (0x18) -> 0x667E20 (angulo, 1.0f, 0.2f).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <iterator>
#include <vector>

namespace WalkRoundFirst31Fix {
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
constexpr u32 kObjBytes = 0x20;                         // CEntity falsa hasta m_matrix (+0x14) inclusive: placement en +4 (posicion +4, rumbo +0x10)
constexpr u32 kObjOff = 0x20, kObjArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kMatBytes = 0x40;                         // CMatrix falsa del objeto: derecha +0, frente +0x10, posicion +0x30
constexpr u32 kMatOff = 0x20, kMatArena = 0x100;        // 0x20 + 0x40 = 0x60 <= 0x100
constexpr u32 kPedBytes = 0x20;                         // CPed falso hasta m_matrix (+0x14) inclusive: placement en +4
constexpr u32 kPedOff = 0x20, kPedArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kPMatBytes = 0x40;                        // CMatrix falsa del peaton: frente +0x10, posicion +0x30
constexpr u32 kPMatOff = 0x20, kPMatArena = 0x100;      // 0x20 + 0x40 = 0x60 <= 0x100
constexpr u32 kRouteBytes = 0x64;                       // CPointRoute: contador (+0) y 8 vectores (+4)
constexpr u32 kRouteOff = 0x20, kRouteArena = 0x100;    // 0x20 + 0x64 = 0x84 <= 0x100
constexpr u32 kRouteWords = kRouteBytes / 4;            // 25
constexpr u32 kSlot = 0x80;                             // almacenamiento de una ranura del pool de tareas
constexpr u32 kWindowMax = 1;                           // primeras ranuras libres que se registran (= nº de reservas de la funcion)
constexpr u32 kAllocs = 1;
constexpr u32 kMoveAt = 0xC, kObjectAt = 0x1C, kRouteAt = 0x20, kF24At = 0x24, kF28At = 0x28, kF2CAt = 0x2C, kF2DAt = 0x2D, kStartPosAt = 0x30, kStartFwdAt = 0x3C, kStartRightAt = 0x48;
constexpr u32 kPlaceAt = 0x4, kMatrixAt = 0x14;         // CEntity / CPed: m_placement.m_vPosn y m_matrix
constexpr u32 kMatRightAt = 0x0, kMatFwdAt = 0x10, kMatPosAt = 0x30;
constexpr u32 kTypeAchieve = 0x386;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, objNull, moveState, matPresent;
    u32 routeCount;
    u32 routeEntries[kRouteWords - 1];
    u32 routeDist, routeRet, time;
    u32 place[3], mpos[3], mfwd[3], mright[3], pedPos[3], pedFwd[3], pedPlace[3];
    u32 poolMode, poolK;
    u32 eax, edx, stack;
};

inline constexpr u32 kFloatEdges[]{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x4B800000u, 0x4B800001u, 0x4B000001u, 0x4B7FFFFFu, 0x4B000000u, 0x4EFFFFFFu, 0x4F000000u, 0xCF000000u,
    0x5F000000u, 0x5F7FFFFFu, 0xDF000000u, 0x00000001u, 0x007FFFFFu, 0x00800000u, 0x3F000000u, 0x3FC00000u, 0x40400000u, 0xC0400000u,
    0x40000000u, 0x40000001u, 0x3FFFFFFFu, 0x40800000u, 0x40800001u, 0x407FFFFFu, 0x40C00000u, 0x40C00001u, 0x40BFFFFFu, 0x20000000u};
inline constexpr u32 kUnitVectors[][3]{   // bits de floats
    {0x3F800000u, 0, 0}, {0, 0x3F800000u, 0}, {0, 0, 0x3F800000u}, {0xBF800000u, 0, 0}, {0, 0xBF800000u, 0}, {0, 0, 0xBF800000u},
    {0x3F19999Au, 0x3F4CCCCDu, 0}, {0x3F4CCCCDu, 0, 0x3F19999Au}, {0, 0x3F19999Au, 0x3F4CCCCDu}, {0x3F13CD3Au, 0x3F13CD3Au, 0x3F13CD3Au}};
inline constexpr u32 kMoveStates[]{4, 6, 5, 7, 0, 1, 2, 3, 8, 9, 0xFFFFFFFFu, 0x80000000u, 0x00000104u};
// distancias de borde: 2.0, 4.0 y 6.0 con sus vecinos (el maximo depende de moveState)
inline constexpr u32 kDistEdges[]{0x40000000u, 0x40000001u, 0x3FFFFFFFu, 0x40800000u, 0x40800001u, 0x407FFFFFu, 0x40C00000u, 0x40C00001u, 0x40BFFFFFu, 0x3F800000u, 0x41200000u, 0x42C80000u, 0x00000000u, 0x3F000000u};
inline constexpr u32 kRouteDistEdges[]{0x00000000u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x7F7FFFFFu, 0xBF800000u, 0x3F800000u, 0x40000000u, 0x40000001u, 0x3FFFFFFFu,
                                       0x40800000u, 0x40800001u, 0x407FFFFFu, 0x40C00000u, 0x40C00001u, 0x40BFFFFFu, 0x42C80000u, 0x00000001u, 0x7F800001u};

inline u32 FloatBits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
inline float BitsFloat(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
// extremeP: probabilidad (sobre 100) de elegir un valor de borde
inline u32 PtBits(u32 n, u32 k, u32 extremeP) {
    const u32 r = Rand(n, k);
    if ((r >> 8) % 100 < extremeP) return kFloatEdges[Rand(n, k + 700) % std::size(kFloatEdges)];
    return FloatBits(float(i32(Rand(n, k + 300) % 80001u) - 40000) * 0.125f);   // multiplos exactos de 1/8 en [-5000, 5000]
}
inline void Triple(u32 n, u32 k, u32 extremeP, u32* out) { for (u32 j = 0; j < 3; ++j) out[j] = PtBits(n, k + j, extremeP); }
inline void Unit(u32 n, u32 k, u32* out) { const auto& u = kUnitVectors[Rand(n, k) % std::size(kUnitVectors)]; out[0] = u[0]; out[1] = u[1]; out[2] = u[2]; }

// Familias (n % 16, 288 casos cada una):
//  0 sin objeto                    1 ruta vacia (0x6551D0 no anade puntos)   2 camino de la ruta (lejos, ruta larga, de frente)   3 distancia al primer punto en el borde del maximo
//  4 distancia de la ruta en el borde (NaN, inf, negativa...)   5 producto escalar en el borde (ortogonal, casi ortogonal, inf)   6 camino del giro, caso tipico (reserva)
//  7 objeto sin matriz (AllocateMatrix + UpdateMatrix falsos)   8 posicion de origen: matriz o placement      9 primer punto igual a la posicion (vector nulo)
//  10 flotantes extremos            11 estados de movimiento raros y de borde   12 pool agotado / casi agotado en el giro   13 cuadrantes del giro (ejes, diagonales)   14..15 mezclas
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    c.objNull = f == 0 ? 1u : (f >= 14 && Rand(n, 4) % 14 == 0) ? 1u : 0u;
    // --- estado de movimiento: 4 (WALK), 6 (RUN), el resto
    {
        const u32 v = Rand(n, 5) % 10;
        c.moveState = v < 4 ? 4u : v < 7 ? 6u : kMoveStates[Rand(n, 6) % std::size(kMoveStates)];
        if (f == 11) c.moveState = kMoveStates[Rand(n, 6) % std::size(kMoveStates)];
    }
    c.matPresent = f == 7 ? 0u : (Rand(n, 7) % 8 == 0 ? 0u : 1u);
    // --- peaton: posicion (matriz +0x30), frente (matriz +0x10) unitario, placement (basura, no se usa)
    const u32 extremeP = f == 10 ? 50 : 4;
    Triple(n, 10, extremeP, c.pedPos);
    Unit(n, 13, c.pedFwd);
    Triple(n, 14, 10, c.pedPlace);
    // --- objeto: placement, matriz
    Triple(n, 17, extremeP, c.place);
    Triple(n, 20, extremeP, c.mpos);
    Unit(n, 23, c.mfwd);
    Unit(n, 24, c.mright);
    if (f == 10) { for (u32 j = 0; j < 3; ++j) { if (Rand(n, 25) % 3 == 0) c.mfwd[j] = PtBits(n, 26 + j, 60); if (Rand(n, 29) % 3 == 0) c.mright[j] = PtBits(n, 30 + j, 60); if (Rand(n, 33) % 3 == 0) c.pedFwd[j] = PtBits(n, 34 + j, 60); } }
    // --- ruta: cuantos puntos pone el falso de 0x6551D0 y cuales; distancia que devuelve
    c.routeCount = (f == 1) ? 0u : (f >= 14 || f == 2 || f == 3 || f == 4 || f == 5) ? 1u + Rand(n, 40) % 8 : (Rand(n, 40) % 8 == 0 ? 0u : 1u + Rand(n, 41) % 8);
    if (f == 1 && Rand(n, 42) % 6 == 0) c.routeCount = 0u;
    if (Rand(n, 43) % 20 == 0 && c.routeCount) c.routeCount = Rand(n, 44) | 1u;   // contadores raros distintos de cero (solo se usa el primer punto)
    for (u32 j = 0; j < kRouteWords - 1; ++j) c.routeEntries[j] = PtBits(n, 100 + j, 6);
    // el primer punto respecto al peaton: d = p0 - ped
    const bool mostlyFar = f == 2 || f == 4 || f == 5 || f == 14 || f == 15;
    const u32 pick = Rand(n, 45) % 100;
    float t;
    if (mostlyFar) t = pick < 50 ? BitsFloat(kDistEdges[Rand(n, 46) % std::size(kDistEdges)]) : 2.0f + float(Rand(n, 47) % 400u) * 0.125f;
    else if (f == 3 || f == 11) t = BitsFloat(kDistEdges[Rand(n, 46) % std::size(kDistEdges)]);
    else t = pick < 30 ? BitsFloat(kDistEdges[Rand(n, 46) % std::size(kDistEdges)]) : float(Rand(n, 47) % 500u) * 0.0625f;
    {   // direccion: la del frente (dot > 0), la opuesta, ortogonal, aleatoria
        const u32 dm = Rand(n, 48) % 8;
        u32 dir[3];
        if (dm < 3) { for (u32 j = 0; j < 3; ++j) dir[j] = c.pedFwd[j]; }
        else if (dm == 3) { for (u32 j = 0; j < 3; ++j) dir[j] = c.pedFwd[j] ^ 0x80000000u; }
        else if (dm == 4) { const u32 ax = Rand(n, 49) % 3; dir[0] = dir[1] = dir[2] = 0; dir[ax] = 0x3F800000u; if ((BitsFloat(c.pedFwd[ax]) != 0.0f)) { dir[ax] = 0; dir[(ax + 1) % 3] = 0x3F800000u; } }
        else Unit(n, 50, dir);
        if (f == 5) {   // casi ortogonal: componente en el frente diminuta o nula
            const u32 ax = Rand(n, 51) % 3;
            dir[0] = dir[1] = dir[2] = 0;
            dir[(ax + 1) % 3] = 0x3F800000u;
            dir[ax] = Rand(n, 52) % 3 == 0 ? 0u : Rand(n, 52) % 3 == 1 ? 0x00000001u : 0x80000001u;
            if (Rand(n, 53) % 4 == 0) { dir[0] = PtBits(n, 54, 70); }
        }
        const float tt = f == 9 ? 0.0f : t;
        for (u32 j = 0; j < 3; ++j) {
            const float p = BitsFloat(c.pedPos[j]) + BitsFloat(dir[j]) * tt;
            c.routeEntries[j] = FloatBits(p);
        }
        if (f == 9) { for (u32 j = 0; j < 3; ++j) c.routeEntries[j] = c.pedPos[j]; if (Rand(n, 55) % 3 == 0) c.routeEntries[Rand(n, 56) % 3] = c.pedPos[Rand(n, 56) % 3] ^ 0x80000000u; }
        if (f == 10 && Rand(n, 57) % 2) c.routeEntries[Rand(n, 58) % 3] = PtBits(n, 59, 70);
        if (f == 13) {   // cuadrantes del giro
            constexpr float q[]{0.f, 1.f, -1.f, 2.f, -2.f, 0.5f, -0.5f, 100.f, -100.f, 1e-6f};
            c.routeEntries[0] = FloatBits(BitsFloat(c.pedPos[0]) + q[Rand(n, 60) % std::size(q)]);
            c.routeEntries[1] = FloatBits(BitsFloat(c.pedPos[1]) + q[Rand(n, 61) % std::size(q)]);
        }
    }
    // La DLL llama a CVector::NormaliseAndMag (C++ en float: los cuadrados desbordan o se anulan fuera de [~1e-19, ~1e19]) y el EXE a su version x87 (exponente extendido): para que el oraculo (x87)
    // y el colaborador coincidan, las componentes de d = p0 - ped finitas y no nulas se mantienen entre 1e-9 y 1e9 (los cuadrados y su suma quedan muy dentro del rango de un float).
    for (u32 j = 0; j < 3; ++j) {
        const float p = BitsFloat(c.routeEntries[j]), q = BitsFloat(c.pedPos[j]);
        if (std::isfinite(p) && std::isfinite(q)) {
            const double d = std::fabs(double(p) - double(q));
            if (d != 0.0 && (d < 1e-9 || d > 1e9)) c.routeEntries[j] = c.pedPos[j];
        }
    }
    {   // distancia de la ruta (float devuelto por 0x6551D0)
        const u32 v = Rand(n, 62) % 100;
        if (f == 4) c.routeDist = v < 80 ? kRouteDistEdges[Rand(n, 63) % std::size(kRouteDistEdges)] : PtBits(n, 64, 30);
        else if (f == 2) c.routeDist = v < 15 ? kRouteDistEdges[Rand(n, 63) % std::size(kRouteDistEdges)] : FloatBits(2.0f + float(Rand(n, 65) % 400u) * 0.125f);
        else c.routeDist = v < 25 ? kRouteDistEdges[Rand(n, 63) % std::size(kRouteDistEdges)] : FloatBits(float(Rand(n, 65) % 800u) * 0.125f);
    }
    c.routeRet = Rand(n, 66) % 10 == 0 ? 0u : (0x77000000u | (Rand(n, 67) & 0xFFFFFFu));
    c.time = Rand(n, 68);
    // --- pool (una reserva)
    const u32 pr = Rand(n, 90);
    if (f == 12) { constexpr u32 m[]{7, 7, 7, 2, 3, 4, 5}; c.poolMode = m[pr % 7]; c.poolK = 1; }
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
}  // namespace WalkRoundFirst31Fix
