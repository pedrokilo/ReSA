#pragma once
// Fixtures del banco PartnerShoveSeq28: CTaskComplexPartnerShove::GetPartnerSequence (0x683120, thiscall sin argumentos, `ret`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion lee DOS campos de `this` (m_partner en +0x38, byte m_leadSpeaker en +0x58) y reserva tres tareas del pool de tareas (CTask::operator new 0x61A5A0):
//   1. CTaskComplexSequence (0x40 bytes) -> constructor 0x632BD0
//   2. CTaskComplexTurnToFaceEntityOrCoord (0x28) -> constructor 0x66B890 (partner, [0x86FC7C]=0.5f, 0x3A83126F=0.001f); registra una referencia en el partner si no es nulo
//   3. CTaskSimpleRunAnim (0x20) -> constructor 0x61A8B0 (grupo 0, anim 0xA4 si m_leadSpeaker != 0, si no 0x8E, mezcla 4.0f, hold false)
// y tras cada una llama a CTaskComplexSequence::AddTask (0x632D10). Devuelve la secuencia (EAX).
//
// Cada caso fija: el pool de tareas sintetico (estados + m_LastFreeSlot; las ranuras "libres" reales del pool sintetico se restauran despues), el partner (nulo, o una
// entidad falsa de tipo/flags/IPL dados, con 0..2 referencias previas y, a veces, la referencia ya registrada), y el byte m_leadSpeaker.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace PartnerShoveSeq28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 6144;

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

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kThisBytes = 0x74;         // sizeof(CTaskComplexPartnerShove)
constexpr u32 kThisOff = 0x40, kThisArena = 0x100;
constexpr u32 kPartnerAt = 0x38, kLeadAt = 0x58;   // campos que lee la funcion
constexpr u32 kEntBytes = 0x38;          // sizeof(CEntity)
constexpr u32 kEntOff = 0x40, kEntArena = 0x100;
constexpr u32 kNodeSize = 8;             // CReference { m_pNext, m_ppEntity }
constexpr u32 kNodeOff = 0x20, kNodeArena = 0x80;
constexpr u32 kSlot = 0x80;              // sizeof del almacenamiento de una ranura del pool de tareas
constexpr u32 kWindowMax = 4;            // primeras ranuras libres, en orden de busqueda, que se registran
constexpr u32 kMinCap = 64;
constexpr u32 kTaskSeq = 0xF4, kTaskTurn = 0x398, kTaskRun = 0x190;   // GetTaskType (0x632C60, 0x66B900, m_nTaskType 0x190)

// ------------------------------------------------------------------------------------------------ caso
struct C {
    u32 family, lead, hasPartner, etype, temp, proc, ipl, refsLen, dup, poolMode, poolK;
    u32 eax, edx, stack;
};

inline constexpr u8 kLeadValues[]{0, 1, 2, 3, 0x10, 0x7F, 0x80, 0x81, 0xFE, 0xFF};

// Familias (n % 16, 384 casos cada una):
//  0 sin partner, lead 0        1 sin partner, lead 1         2 peaton sin referencias previas          3 peaton con 1 referencia previa (a veces ya registrada)
//  4 peaton con 2 referencias (a veces ya registrada)         5 vehiculo         6 objeto          7 edificio sin IPL, no temporal, no procedural (NO registra)
//  8 edificio con IPL (registra)  9 edificio temporal (registra)  10 edificio procedural (registra)  11 tipos 0,5,6,7
// 12..15: pool casi agotado / con vuelta: 12 una sola ranura libre, 13 dos, 14 tres o cuatro, 15 vuelta del cursor y ranuras libres solo antes del cursor.
inline C MakeC(u32 n) {
    C c{};
    const u32 f = n % 16;
    c.family = f;
    const u32 r = Rand(n, 2) % 12;
    c.lead = r < 3 ? 0u : r < 6 ? 1u : r < 10 ? u32(kLeadValues[Rand(n, 3) % std::size(kLeadValues)]) : (Rand(n, 4) & 0xFFu);
    if (f == 0) c.lead = 0;
    if (f == 1) c.lead = 1;
    c.hasPartner = f >= 2;
    if (f >= 12) c.hasPartner = Rand(n, 5) % 2;
    switch (f) {
    case 2: c.etype = 3; break;
    case 3: c.etype = 3; c.refsLen = 1; c.dup = Rand(n, 6) % 3 == 0; break;
    case 4: c.etype = 3; c.refsLen = 2; c.dup = Rand(n, 6) % 3 == 0; break;
    case 5: c.etype = 2; c.refsLen = Rand(n, 7) % 3; break;
    case 6: c.etype = 4; c.refsLen = Rand(n, 7) % 3; break;
    case 7: c.etype = 1; c.refsLen = Rand(n, 7) % 3; break;
    case 8: c.etype = 1; c.ipl = 1 + Rand(n, 8) % 255; c.refsLen = Rand(n, 7) % 3; break;
    case 9: c.etype = 1; c.temp = 1; c.ipl = Rand(n, 8) % 2 ? 0u : 1 + Rand(n, 9) % 255; c.refsLen = Rand(n, 7) % 3; break;
    case 10: c.etype = 1; c.proc = 1; c.ipl = Rand(n, 8) % 2 ? 0u : 1 + Rand(n, 9) % 255; c.refsLen = Rand(n, 7) % 3; break;
    case 11: { constexpr u32 t[4]{0, 5, 6, 7}; c.etype = t[Rand(n, 6) % 4]; c.refsLen = Rand(n, 7) % 3; c.temp = Rand(n, 10) % 2; c.proc = Rand(n, 11) % 2; c.ipl = Rand(n, 12) % 2 ? 0u : (Rand(n, 13) & 0xFFu); break; }
    default:
        if (f >= 12 && c.hasPartner) {
            constexpr u32 t[5]{1, 2, 3, 4, 3};
            c.etype = t[Rand(n, 6) % 5];
            c.refsLen = Rand(n, 7) % 3;
            if (c.etype == 1) { c.temp = Rand(n, 10) % 2; c.ipl = Rand(n, 8) % 2 ? 0u : 1 + Rand(n, 9) % 255; }
        }
        break;
    }
    // Pool: 0 natural (10 % libres), 1 todo libre, 2 k libres al azar, 3 k libres justo detras del cursor, 4 vuelta (cursor al final, libres al principio),
    // 5 libres solo en/antes del cursor (se encuentran tras dar la vuelta), 6 mitad libres.
    const u32 pr = Rand(n, 14);
    if (f <= 11) {
        const u32 modes[7]{0, 0, 1, 6, 2, 3, 4};
        c.poolMode = modes[pr % 7];
        c.poolK = 3 + (pr >> 8) % 4;
    } else if (f == 12) { constexpr u32 m[3]{2, 3, 5}; c.poolMode = m[pr % 3]; c.poolK = 1; }
    else if (f == 13) { constexpr u32 m[4]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 2; }
    else if (f == 14) { constexpr u32 m[4]{2, 3, 4, 5}; c.poolMode = m[pr % 4]; c.poolK = 3 + (pr >> 8) % 2; }
    else { constexpr u32 m[4]{4, 5, 1, 6}; c.poolMode = m[pr % 4]; c.poolK = 1 + (pr >> 8) % 3; }
    c.eax = Rand(n, 20);
    c.edx = Rand(n, 21);
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
        switch (Rand(n, 30) % 7) {
        case 0: return 0l;
        case 1: return long(cap) - 1;
        case 2: return long(cap) - 2;
        case 3: return 1l;
        default: return long(Rand(n, 31) % cap);
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
        p.last = lasts[Rand(n, 32) % 3];
        for (u32 j = 0; j < c.poolK; ++j) isFree[j] = 1;
        break;
    }
    case 5: {   // libres solo en el cursor y antes de el: se encuentran tras dar la vuelta
        p.last = long(1 + Rand(n, 33) % (cap - 2));
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
        const u32 from = pass == 0 ? start : 0, to = pass == 0 ? cap : start;   // segunda pasada: 0..start-1 (lo demas ya se vio)
        for (u32 i = from; i < to && p.win.size() < kWindowMax; ++i)
            if (isFree[i]) p.win.push_back({i, p.states[i]});
    }
    return p;
}
}  // namespace PartnerShoveSeq28Fix
