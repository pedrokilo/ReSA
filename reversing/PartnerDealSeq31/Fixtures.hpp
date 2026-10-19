#pragma once
// Fixtures del banco PartnerDealSeq31: CTaskComplexPartnerDeal::GetPartnerSequence (0x682440, thiscall sin argumentos, `ret`, marco SEH propio).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee de `this` SOLO m_partner (+0x38) y el BYTE m_leadSpeaker (+0x58); no escribe nada en `this`. Reserva en el pool de tareas (CTask::operator new 0x61A5A0) TRES tareas:
//   1. CTaskComplexSequence (0x40) -> 0x632BD0     2. CTaskComplexTurnToFaceEntityOrCoord (0x28) -> 0x66B890 (partner, 0.5f, 0.02f = 0x3CA3D70A)
//   3. CTaskSimpleRunAnim (0x20) -> 0x61A8B0 (grupo 0x34 = ANIM_GROUP_GANGS, anim 0x11F si lead != 0 / 0x120 si lead == 0, 4.0f, false)
// y llama a AddTask (0x632D10) tras cada una de las dos ultimas (aunque la reserva fallara: se anade el nulo).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerDealSeq31Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;   // 16 familias x 256

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

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda | objeto | guarda)
constexpr u32 kThisBytes = 0x74;                        // sizeof(CTaskComplexPartnerDeal)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;      // 0x40 + 0x74 + 0x4C = 0x100
constexpr u32 kPartnerAt = 0x38, kLeadAt = 0x58;
constexpr u32 kEntBytes = 0x40;                         // el partner falso: solo se usa como CEntity (m_pReferences +0x24, tipo, IPL)
constexpr u32 kEntOff = 0x40, kEntArena = 0x100;        // 0x40 + 0x40 + 0x80 = 0x100
constexpr u32 kNodeSize = 8;                            // CReference { m_pNext, m_ppEntity }
constexpr u32 kNodeOff = 0x20, kNodeArena = 0x80;       // 3 nodos: 0x20 + 0x18 + 0x48 = 0x80
constexpr u32 kSlot = 0x80;                             // sizeof del almacenamiento de una ranura del pool de tareas
constexpr u32 kWindowMax = 3;                           // primeras ranuras libres, en orden de busqueda, que se registran (= nº de reservas de la funcion)
constexpr u32 kAllocs = 3;
constexpr u32 kMinCap = 64;
constexpr u32 kTypeSeq = 0xF4, kTypeTurn = 0x398, kTypeRun = 0x190;

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, lead, hasPartner, etype, temp, proc, ipl, refsLen, dupKind, poolMode, poolK;
    u32 eax, edx, stack;
};

inline constexpr u8 kLeadValues[]{0, 1, 2, 3, 0x10, 0x7F, 0x80, 0x81, 0xFE, 0xFF};

// Familias (n % 16, 256 casos cada una):
//  0 lead, sin partner          1 NO lead, sin partner        2 lead, peaton           3 NO lead, peaton
//  4 bytes lead de borde (2, 3, 0x10, 0x7F, 0x80, 0x81, 0xFE, 0xFF; a veces 0) con peaton   5 peaton con 1-2 referencias previas (a veces la del Turn ya registrada)
//  6 vehiculo / objeto / tipos 0,5,6,7        7 edificios (sin IPL, con IPL, temporal, procedural)
//  8..11 pool casi agotado: 8 una sola ranura libre, 9 dos, 10 tres, 11 vuelta del cursor o k libres (1..3)
//  12..15 mezcla (lead y partner al azar, pool normal)
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    bool lead = Rand(n, 2) % 2 == 0;
    switch (f) {
    case 0: case 2: lead = true; break;
    case 1: case 3: lead = false; break;
    default: break;
    }
    const u32 lr = Rand(n, 4) % 8;
    const u32 leadRand = Rand(n, 6) & 0xFFu;
    if (!lead) c.lead = 0u;
    else if (f == 4) c.lead = u32(kLeadValues[2 + Rand(n, 5) % (std::size(kLeadValues) - 2)]);
    else c.lead = lr < 4 ? 1u : lr < 6 ? u32(kLeadValues[1 + Rand(n, 5) % (std::size(kLeadValues) - 1)]) : (leadRand ? leadRand : 0x40u);
    if (f == 4 && Rand(n, 7) % 3 == 0) c.lead = 0u;
    // --- partner (nulo es valido tambien con lead == 0: esta funcion no lo desreferencia)
    c.hasPartner = (f == 0 || f == 1) ? 0u : 1u;
    if (f >= 8 && Rand(n, 14) % 4 == 0) c.hasPartner = 0;
    // --- entidad y referencias
    if (c.hasPartner) {
        c.etype = 3;
        switch (f) {
        case 5: c.refsLen = 1 + Rand(n, 15) % 2; c.dupKind = Rand(n, 16) % 3 == 0 ? 1u : 0u; break;
        case 6: {
            constexpr u32 t[8]{2, 4, 0, 5, 6, 7, 2, 4};
            c.etype = t[Rand(n, 18) % 8]; c.refsLen = Rand(n, 19) % 3; c.temp = Rand(n, 20) % 2; c.proc = Rand(n, 21) % 2; c.ipl = Rand(n, 22) % 2 ? 0u : (Rand(n, 23) & 0xFFu);
            break;
        }
        case 7: {
            c.etype = 1;
            const u32 v = Rand(n, 24) % 4;
            if (v == 1) c.ipl = 1 + Rand(n, 25) % 255;                                              // con IPL: registra
            else if (v == 2) { c.temp = 1; c.ipl = Rand(n, 25) % 2 ? 0u : 1 + Rand(n, 26) % 255; }  // temporal: registra
            else if (v == 3) { c.proc = 1; c.ipl = Rand(n, 25) % 2 ? 0u : 1 + Rand(n, 26) % 255; }  // procedural: registra
            c.refsLen = Rand(n, 19) % 3;                                                            // v == 0: sin IPL, ni temporal ni procedural: NO registra
            break;
        }
        default:
            c.refsLen = Rand(n, 19) % 3;
            if (f >= 8) {
                constexpr u32 t[5]{1, 2, 3, 4, 3};
                c.etype = t[Rand(n, 18) % 5];
                if (c.etype == 1) { c.temp = Rand(n, 20) % 2; c.ipl = Rand(n, 22) % 2 ? 0u : 1 + Rand(n, 23) % 255; }
            }
            if (c.etype == 3 && c.refsLen && Rand(n, 16) % 4 == 0) c.dupKind = 1;
            break;
        }
    }
    // --- pool: 0 natural (10 % libres), 1 todo libre, 2 k libres al azar, 3 k libres justo detras del cursor, 4 vuelta (cursor al final), 5 libres solo en/antes del cursor, 6 mitad libres.
    const u32 pr = Rand(n, 64);
    if (f <= 7 || f >= 12) {
        const u32 modes[7]{0, 0, 1, 6, 2, 3, 4};
        c.poolMode = modes[pr % 7];
        c.poolK = 3 + (pr >> 8) % 4;
    } else if (f == 8) { constexpr u32 m[3]{2, 3, 5}; c.poolMode = m[pr % 3]; c.poolK = 1; }
    else if (f == 9) { constexpr u32 m[4]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 2; }
    else if (f == 10) { constexpr u32 m[4]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 3; }
    else {
        constexpr u32 m[4]{2, 3, 4, 5};
        c.poolMode = m[pr % 4];
        c.poolK = 3 + (pr >> 8) % 2;
        if (pr % 5 == 0) { c.poolMode = 4; c.poolK = 1 + (pr >> 12) % 3; }
    }
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
}  // namespace PartnerDealSeq31Fix
