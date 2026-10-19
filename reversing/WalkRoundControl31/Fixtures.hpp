#pragma once
// Fixtures del banco WalkRoundControl31: CTaskComplexWalkRoundObject::ControlSubTask (0x6575F0, thiscall, 1 argumento (ped), `ret 4`, sin marco SEH).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this`: m_pSubTask (+8, tarea falsa con vtable propia: GetTaskType = ranura 4, MakeAbortable = ranura 6), m_object (+0x1C), field_24/28 (+0x24/+0x28), field_2C/2D (bytes +0x2C/+0x2D),
// la posicion/frente/derecha iniciales del objeto (+0x30/+0x3C/+0x48); del peaton: m_pIntelligence (+0x47C) -> contador (+0x274, con signo, se compara con la variable estatica 0x86C938 = 30);
// del objeto: m_matrix (+0x14, si falta AllocateMatrix 0x54F560 + CSimpleTransform::UpdateMatrix 0x54F1B0, sustituidos por falsos) y la posicion/frente/derecha de esa matriz; del reloj: CTimer::m_snTimeInMilliseconds (0xB7CB84).
// Escribe: field_24 y field_2D (reinicio del plazo). Reserva en el pool de tareas (CTask::operator new 0x61A5A0) UNA tarea: CTaskSimpleStandStill (0x20) -> 0x62F310 (500, 0, 0, 8.0f), solo en la rama de aborto con MakeAbortable verdadero.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace WalkRoundControl31Fix {
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
constexpr u32 kSubBytes = 0x20;                         // tarea falsa (vtable propia en +0, tipo en +0x10, resultado de MakeAbortable en +0x14)
constexpr u32 kSubOff = 0x20, kSubArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kObjBytes = 0x20;                         // CEntity falsa hasta m_matrix (+0x14) inclusive: placement en +4 (posicion +4, rumbo +0x10)
constexpr u32 kObjOff = 0x20, kObjArena = 0x80;         // 0x20 + 0x20 = 0x40 <= 0x80
constexpr u32 kMatBytes = 0x40;                         // CMatrix falsa: derecha +0, frente +0x10, posicion +0x30
constexpr u32 kMatOff = 0x20, kMatArena = 0x100;        // 0x20 + 0x40 = 0x60 <= 0x100
constexpr u32 kPedBytes = 0x480;                        // CPed falso hasta m_pIntelligence (+0x47C) inclusive
constexpr u32 kPedOff = 0x40, kPedArena = 0x600;        // 0x40 + 0x480 = 0x4C0 <= 0x600
constexpr u32 kIntBytes = 0x278;                        // CPedIntelligence falsa hasta m_AnotherStaticCounter (+0x274) inclusive
constexpr u32 kIntOff = 0x20, kIntArena = 0x400;        // 0x20 + 0x278 = 0x298 <= 0x400
constexpr u32 kSlot = 0x80;                             // almacenamiento de una ranura del pool de tareas
constexpr u32 kWindowMax = 1;                           // primeras ranuras libres que se registran (= nº de reservas de la funcion)
constexpr u32 kAllocs = 1;
constexpr u32 kSubTaskAt = 0x8, kObjectAt = 0x1C, kF24At = 0x24, kF28At = 0x28, kF2CAt = 0x2C, kF2DAt = 0x2D, kStartPosAt = 0x30, kStartFwdAt = 0x3C, kStartRightAt = 0x48;
constexpr u32 kSubTypeAt = 0x10, kSubAbortAt = 0x14;
constexpr u32 kPlaceAt = 0x4, kMatrixAt = 0x14;         // CEntity: m_placement.m_vPosn y m_matrix
constexpr u32 kMatRightAt = 0x0, kMatFwdAt = 0x10, kMatPosAt = 0x30;
constexpr u32 kIntPtrAt = 0x47C, kCounterAt = 0x274;
constexpr i32 kCounterLimit = 30;                       // [0x86C938]
constexpr u32 kTypeFollow = 0x389, kTypeStand = 0xCB;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, objNull, subType, abortRes, f2c, f2d, f24, f28, time;
    i32 counter;
    u32 matPresent;
    u32 startPos[3], startFwd[3], startRight[3], pos[3], fwd[3], right[3], place[3];
    u32 poolMode, poolK;
    u32 eax, edx, stack;
};

inline constexpr u8 kByteEdges[]{1, 2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF};
inline constexpr u32 kFloatEdges[]{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x4B800000u, 0x4B800001u, 0x4B000001u, 0x4B7FFFFFu, 0x4B000000u, 0x4EFFFFFFu, 0x4F000000u, 0xCF000000u,
    0x5F000000u, 0x5F7FFFFFu, 0xDF000000u, 0x00000001u, 0x007FFFFFu, 0x00800000u, 0x3F000000u, 0x3FC00000u, 0x40400000u, 0xC0400000u,
    0x3E800000u, 0x3E800001u, 0x3E7FFFFFu, 0x3F666666u, 0x3F666667u, 0x3F666665u, 0x7E967699u, 0xFE967699u, 0x20000000u, 0x1F800000u};
inline constexpr u32 kUnitVectors[][3]{   // bits de floats
    {0x3F800000u, 0, 0}, {0, 0x3F800000u, 0}, {0, 0, 0x3F800000u}, {0xBF800000u, 0, 0}, {0, 0xBF800000u, 0}, {0, 0, 0xBF800000u},
    {0x3F19999Au, 0x3F4CCCCDu, 0}, {0x3F4CCCCDu, 0, 0x3F19999Au}, {0, 0x3F19999Au, 0x3F4CCCCDu}, {0x3F13CD3Au, 0x3F13CD3Au, 0x3F13CD3Au}};
inline constexpr u32 kTypes[]{0x389, 0xCB, 0x386, 0x387, 0x3F, 0x0, 0x1F4, 0x516, 0xFFFFFFFFu};

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
inline u32 AddF(u32 a, float d) { return FloatBits(BitsFloat(a) + d); }
inline u32 ByteOf(u32 n, u32 k) { return Rand(n, k) % 3 == 0 ? u32(kByteEdges[Rand(n, k + 1) % std::size(kByteEdges)]) : (Rand(n, k + 2) % 2 ? 1u : (Rand(n, k + 3) & 0xFFu)); }

// Frente (o derecha) inicial = eje con signo; actual = componente en el borde de 0.9 (0x86F160) sobre el mismo eje (a veces con un pequeño componente transversal).
inline void DotBoundary(u32 n, u32 salt, u32* cur, u32* start) {
    constexpr u32 b[]{0x3F666666u, 0x3F666667u, 0x3F666665u, 0x3F666668u, 0x3F666664u, 0x3F800000u, 0x3F4CCCCDu, 0x3F7FFFFFu};
    const u32 axis = Rand(n, salt) % 3;
    const bool neg = Rand(n, salt + 1) % 4 == 0;
    const u32 sgn = neg ? 0x80000000u : 0u;
    start[0] = start[1] = start[2] = 0;
    start[axis] = 0x3F800000u | sgn;
    cur[0] = cur[1] = cur[2] = 0;
    cur[axis] = b[Rand(n, salt + 2) % std::size(b)] | sgn;
    if (Rand(n, salt + 3) % 3 == 0) cur[(axis + 1) % 3] = 0x3E800000u;
}

// Familias (n % 16, 288 casos cada una):
//  0 sin objeto            1 subtarea de ruta con field_2C = 0     2 ruta con plazo (reinicio de field_2D)   3 ruta con plazo en el borde (envoltura sin signo)
//  4 contador en el borde  5 reposo (retorno anticipado)           6 reposo con contador alto               7 sin matriz (AllocateMatrix + UpdateMatrix falsos)
//  8 posicion en el borde  9 frente en el borde                    10 derecha en el borde                   11 todo en orden (devuelve la subtarea)
//  12 flotantes extremos   13 abortos con MakeAbortable variado     14 pool casi agotado en abortos          15 mezcla
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    // --- subtarea y banderas
    const u32 tr = Rand(n, 2) % 100;
    c.subType = tr < 25 ? kTypeFollow : tr < 40 ? kTypeStand : tr < 58 ? 0x386u : tr < 72 ? 0x387u : kTypes[Rand(n, 3) % std::size(kTypes)];
    if (f == 1 || f == 2 || f == 3) c.subType = kTypeFollow;
    if (f == 5 || f == 6) c.subType = kTypeStand;
    if (f == 7 || f == 8 || f == 9 || f == 10 || f == 11) { constexpr u32 t[]{0x386, 0x387, 0x3F, 0x0, kTypeFollow}; c.subType = t[Rand(n, 3) % 5]; }
    c.objNull = f == 0 ? 1u : (f == 15 && Rand(n, 4) % 12 == 0) ? 1u : 0u;
    c.abortRes = Rand(n, 5) % 2 ? (Rand(n, 6) | 1u) : (Rand(n, 6) & 0xFFFFFF00u);   // al = 0 o != 0, bytes altos al azar
    if (f == 13) { const u32 v = Rand(n, 7) % 6; c.abortRes = v == 0 ? 0u : v == 1 ? 1u : v == 2 ? 0x100u : v == 3 ? 0xFFFFFF00u : v == 4 ? 0x80u : Rand(n, 8); }
    c.f2c = (f == 1) ? 0u : (f == 2 || f == 3) ? ByteOf(n, 9) : (Rand(n, 9) % 3 == 0 ? 0u : ByteOf(n, 10));
    c.f2d = (f == 2) ? ByteOf(n, 11) : (f == 3) ? 0u : (Rand(n, 11) % 3 == 0 ? 0u : ByteOf(n, 12));
    // --- plazo: field_24 + field_28 contra el reloj (suma y comparacion SIN signo, con envoltura a 32 bits)
    const u32 big = Rand(n, 13) % 4;
    c.f24 = big == 0 ? Rand(n, 14) : big == 1 ? 0xFFFFFFF0u + Rand(n, 14) % 16 : Rand(n, 14) % 100000u;
    c.f28 = Rand(n, 15) % 3 == 0 ? (Rand(n, 16) % 5 == 0 ? Rand(n, 17) : (Rand(n, 17) % 3 == 0 ? 0xFFFFFFFFu - Rand(n, 18) % 8 : 0u)) : Rand(n, 17) % 20000u;
    c.time = Rand(n, 19);
    if (Rand(n, 19) % 4 == 0) c.time = Rand(n, 20) % 100000u;
    if (f == 3 || (c.subType == kTypeFollow && Rand(n, 21) % 3 == 0)) {
        constexpr int d[]{-2, -1, 0, 1, 2, -3, 3};
        c.time = c.f24 + c.f28 + u32(d[Rand(n, 22) % 7]);
    }
    if (f == 2) c.time = Rand(n, 23) % 5 == 0 ? Rand(n, 24) : (Rand(n, 24) % 100000u);   // el plazo se reinicia a `time`: field_24 = time, se compara con time + field_28
    // --- contador del peaton
    {
        constexpr i32 e[]{-1, 0, 29, 30, 31, 32, INT32_MIN, INT32_MAX, 1000, -1000, 255, 256};
        const u32 v = Rand(n, 25) % 10;
        c.counter = (f == 4 || v < 3) ? e[Rand(n, 26) % std::size(e)] : i32(Rand(n, 27) % 41u) - 5;
        if (f == 5 || f == 7 || f == 8 || f == 9 || f == 10 || f == 11 || f == 1 || f == 2 || f == 3) c.counter = Rand(n, 27) % 4 == 0 ? e[Rand(n, 26) % 4 + 1] : i32(Rand(n, 27) % 26u);   // <= 30 casi siempre
        if (f == 6) c.counter = 31 + i32(Rand(n, 27) % 5u);
    }
    // --- vectores
    const u32 extremeP = f == 12 ? 55 : 6;
    c.matPresent = f == 7 ? 0u : (Rand(n, 28) % 8 == 0 ? 0u : 1u);
    Triple(n, 30, extremeP, c.startPos);
    Triple(n, 33, extremeP, c.place);
    Unit(n, 36, c.startFwd);
    Unit(n, 37, c.startRight);
    // por omision: todo en orden (posicion igual, frente y derecha iguales a las iniciales)
    for (u32 j = 0; j < 3; ++j) { c.pos[j] = c.startPos[j]; c.fwd[j] = c.startFwd[j]; c.right[j] = c.startRight[j]; }
    const u32 tweak = Rand(n, 40) % 10;
    auto offsetPos = [&](float mag) {   // desplaza un eje (o dos) de la posicion actual
        const u32 ax = Rand(n, 41) % 3;
        const float s = Rand(n, 42) % 2 ? mag : -mag;
        c.pos[ax] = AddF(c.startPos[ax], s);
        if (Rand(n, 43) % 3 == 0) c.pos[(ax + 1) % 3] = AddF(c.startPos[(ax + 1) % 3], Rand(n, 44) % 2 ? mag * 0.5f : -mag * 0.5f);
    };
    switch (f) {
    case 8: {   // posicion en el borde: |delta|^2 vs 0.0625
        constexpr float m[]{0.25f, 0.2500001f, 0.24999999f, 0.1767767f, 0.17677669f, 0.125f, 0.3f, 0.1f, 0.0f, 0.35355339f};
        offsetPos(m[Rand(n, 45) % std::size(m)]);
        if (tweak < 2) c.pos[Rand(n, 46) % 3] = PtBits(n, 47, 40);
        break;
    }
    case 9: DotBoundary(n, 50, c.fwd, c.startFwd); break;
    case 10: DotBoundary(n, 55, c.right, c.startRight); break;
    case 11: if (tweak < 3) offsetPos(0.0625f * float(Rand(n, 46) % 3)); if (tweak == 3) Unit(n, 58, c.fwd); break;
    case 12:
        for (u32 j = 0; j < 3; ++j) {
            if (tweak < 5) c.pos[j] = PtBits(n, 60 + j, 60);
            if (Rand(n, 64) % 2) c.fwd[j] = PtBits(n, 65 + j, 60);
            if (Rand(n, 68) % 2) c.right[j] = PtBits(n, 69 + j, 60);
            if (Rand(n, 72) % 3 == 0) c.startFwd[j] = PtBits(n, 73 + j, 60);
            if (Rand(n, 76) % 3 == 0) c.startRight[j] = PtBits(n, 77 + j, 60);
        }
        break;
    case 15:
        Triple(n, 80, 10, c.pos);
        Unit(n, 83, c.fwd);
        Unit(n, 84, c.right);
        if (Rand(n, 85) % 2) for (u32 j = 0; j < 3; ++j) c.pos[j] = c.startPos[j];
        break;
    default:
        if (tweak < 3) offsetPos(Rand(n, 45) % 2 ? 1.0f : 0.125f);
        else if (tweak < 5) Unit(n, 58, c.fwd);
        else if (tweak < 7) Unit(n, 59, c.right);
        break;
    }
    // --- pool (una reserva)
    const u32 pr = Rand(n, 90);
    if (f == 14) { constexpr u32 m[]{2, 3, 4, 5, 7, 7}; c.poolMode = m[pr % 6]; c.poolK = 1; }
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
}  // namespace WalkRoundControl31Fix
